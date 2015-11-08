#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QThread>
#include <QStack>

#include "QDjango.h"


QDjangoDatabase::QDjangoDatabase(QObject *parent)
    : QObject(parent), connectionId(0)
{
}

void QDjangoDatabase::threadFinished()
{
    QThread *thread = qobject_cast<QThread*>(sender());
    if (!thread)
        return;

    // cleanup database connection for the thread
    QMutexLocker locker(&mutex);
    disconnect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
    const QString connectionName = copies.value(thread).connectionName();
    copies.remove(thread);
    if (connectionName.startsWith(QLatin1String("__qdjango__")))
        QSqlDatabase::removeDatabase(connectionName);
}


static QDjangoDatabase::DatabaseType getDatabaseType(QSqlDatabase &db)
{
    const QString driverName = db.driverName();
    if (driverName == QLatin1String("QMYSQL") ||
        driverName == QLatin1String("QMYSQL3"))
        return QDjangoDatabase::MySqlServer;
    else if (driverName == QLatin1String("QSQLITE") ||
             driverName == QLatin1String("QSQLITE2"))
        return QDjangoDatabase::SQLite;
    else if (driverName == QLatin1String("QPSQL"))
        return QDjangoDatabase::PostgreSQL;
    else if (driverName == QLatin1String("QODBC")) {
        QSqlQuery query(db);
        if (query.exec("SELECT sqlite_version()"))
            return QDjangoDatabase::SQLite;

        if (query.exec("SELECT @@version") && query.next() &&
            query.value(0).toString().contains("Microsoft SQL Server"))
                return QDjangoDatabase::MSSqlServer;

        if (query.exec("SELECT version()") && query.next()) {
            if (query.value(0).toString().contains("PostgreSQL"))
                return QDjangoDatabase::PostgreSQL;
            else
                return QDjangoDatabase::MySqlServer;
        }
    }
    return QDjangoDatabase::UnknownDB;
}

void QDjango::initDatabase(QSqlDatabase db)
{
    QDjangoDatabase::DatabaseType databaseType = getDatabaseType(db);
    if (databaseType == QDjangoDatabase::SQLite) {
        // enable foreign key constraint handling
        QDjangoQuery query(db);
        query.setDebug(this->isDebugEnabled());
        query.prepare("PRAGMA foreign_keys=on");
        query.exec();
    }
}

QDjangoQuery::QDjangoQuery(QSqlDatabase db)
    : QSqlQuery(db)
{
    if (getDatabaseType(db) == QDjangoDatabase::MSSqlServer) {
        // default to fast-forward cursor
        setForwardOnly(true);
    }
}

void QDjangoQuery::setDebug(const bool &b)
{
    this->m_debug = b;
}

void QDjangoQuery::addBindValue(const QVariant &val, QSql::ParamType paramType)
{
    // this hack is required so that we do not store a mix of local
    // and UTC times
    if (val.type() == QVariant::DateTime)
        QSqlQuery::addBindValue(val.toDateTime().toLocalTime(), paramType);
    else
        QSqlQuery::addBindValue(val, paramType);
}

bool QDjangoQuery::exec()
{
    if (this->m_debug) {
        qDebug() << "SQL query" << lastQuery();
        QMapIterator<QString, QVariant> i(boundValues());
        while (i.hasNext()) {
            i.next();
            qDebug() << "SQL   " << i.key().toLatin1().data() << "="
                     << i.value().toString().toLatin1().data();
        }
    }
    if (!QSqlQuery::exec()) {
        if (this->m_debug)
            qWarning() << "SQL error" << lastError();
        return false;
    }
    return true;
}

bool QDjangoQuery::exec(const QString &query)
{
    if (this->m_debug)
        qDebug() << "SQL query" << query;
    if (!QSqlQuery::exec(query)) {
        if (this->m_debug)
            qWarning() << "SQL error" << lastError();
        return false;
    }
    return true;
}

/// \endcond

/*!
    Returns the database used by QDjango.

    If you call this method from any thread but the application's main thread,
    a new connection to the database will be created. The connection will
    automatically be torn down once the thread finishes.

    \sa setDatabase()
*/
QSqlDatabase QDjango::database()
{
    if (!globalDatabase)
        return QSqlDatabase();

    // if we are in the main thread, return reference connection
    QThread *thread = QThread::currentThread();
    if (thread == globalDatabase->thread())
        return globalDatabase->reference;

    // if we have a connection for this thread, return it
    QMutexLocker locker(&globalDatabase->mutex);
    if (globalDatabase->copies.contains(thread))
        return globalDatabase->copies[thread];

    // create a new connection for this thread
    QObject::connect(thread, SIGNAL(finished()), globalDatabase, SLOT(threadFinished()));
    QSqlDatabase db = QSqlDatabase::cloneDatabase(globalDatabase->reference,
        QLatin1String("__qdjango__") + QString::number(globalDatabase->connectionId++));
    db.open();
    initDatabase(db);
    globalDatabase->copies.insert(thread, db);
    return db;
}

/*!
    Sets the database used by QDjango.

    You must call this method from your application's main thread.

    \sa database()
*/
void QDjango::setDatabase(QSqlDatabase database)
{
    this->globalDatabaseType = getDatabaseType(database);
    if (globalDatabaseType == QDjangoDatabase::UnknownDB) {
        qWarning() << "Unsupported database driver" << database.driverName();
    }

    if (!globalDatabase)
    {
        globalDatabase = new QDjangoDatabase();
    }
    initDatabase(database);
    globalDatabase->reference = database;
}

/*!
    Returns whether debugging information should be printed.

    \sa setDebugEnabled()
*/
bool QDjango::isDebugEnabled()
{
    return globalDebugEnabled;
}

/*!
    Sets whether debugging information should be printed.

    \sa isDebugEnabled()
*/
void QDjango::setDebugEnabled(bool enabled)
{
    globalDebugEnabled = enabled;
}

QDjangoDatabase::DatabaseType QDjango::databaseType()
{
    return this->globalDatabaseType;
}

QStringList QDjango::getModelTableList()
{
    QStringList vTableList;
    foreach (QByteArray modelName, globalMetaModels.keys()) {
            vTableList << globalMetaModels.value(modelName).table();
    }
    return vTableList;
}

void QDjango::qdjango_topsort(const QByteArray &modelName, QHash<QByteArray, bool> &visited,
                            QStack<QDjangoMetaModel> &stack)
{
    visited[modelName] = true;
    QDjangoMetaModel model = globalMetaModels[modelName];
    foreach (const QByteArray &foreignModel, model.foreignFields().values()) {
        if (!visited[foreignModel])
            qdjango_topsort(foreignModel, visited, stack);
    }

    stack.push(model);
}

QStack<QDjangoMetaModel> QDjango::qdjango_sorted_metamodels()
{
    QStack<QDjangoMetaModel> stack;
    stack.reserve(globalMetaModels.size());
    QHash<QByteArray, bool> visited;
    visited.reserve(globalMetaModels.size());
    foreach (const QByteArray &model, globalMetaModels.keys())
        visited[model] = false;

    foreach (const QByteArray &model, globalMetaModels.keys()) {
        if (!visited[model])
            qdjango_topsort(model, visited, stack);
    }

    return stack;
}

/*!
    Creates the database tables for all registered models.

    \return true if all the tables were created, false otherwise.
*/
QDjango::QDjango():
    globalMetaModels(QMap<QByteArray, QDjangoMetaModel>()),
    globalDatabase(0),
    globalDebugEnabled(true)
{

}

QDjango::~QDjango()
{
    if(this->globalDatabase!=0) delete this->globalDatabase;
}

bool QDjango::createTables()
{
    bool result = true;
    QStack<QDjangoMetaModel> stack = qdjango_sorted_metamodels();
    foreach (const QDjangoMetaModel &model, stack) {
        if (!model.createTable())
            result = false;
    }

    return result;
}

/*!
    Drops the database tables for all registered models.

    \return true if all the tables were dropped, false otherwise.
*/
bool QDjango::dropTables()
{
    bool result = true;
    QStack<QDjangoMetaModel> stack = this->qdjango_sorted_metamodels();
    for (int i = stack.size() - 1; i >= 0; --i) {
        QDjangoMetaModel model = stack.at(i);
        if (!model.dropTable())
            result = false;
    }

    return result;
}

/*!
    Returns the QDjangoMetaModel with the given \a name.
 */
QDjangoMetaModel QDjango::metaModel(const char *name)
{
    if (globalMetaModels.contains(name))
        return globalMetaModels.value(name);

    // otherwise, try to find a model anyway
    foreach (QByteArray modelName, globalMetaModels.keys()) {
        if (qstricmp(name, modelName.data()) == 0)
            return globalMetaModels.value(modelName);
    }

    return QDjangoMetaModel(this);
}

QDjangoMetaModel QDjango::registerModel(const QMetaObject *meta)
{
    const QByteArray name = meta->className();
    if (!globalMetaModels.contains(name))
        globalMetaModels.insert(name, QDjangoMetaModel(this,meta));
    return globalMetaModels[name];
}

QDjangoDatabase::DatabaseType QDjangoDatabase::databaseType(const QSqlDatabase &db)
{
    QSqlDatabase adb = db;
    return getDatabaseType(adb);
}
