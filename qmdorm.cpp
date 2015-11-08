#include "qmdorm.hpp"

QMDORM::QMDORM(QObject *parent) : QObject(parent)
{
    this->m_django = new QDjango();
    this->m_debug = false;
    this->m_django->setDebugEnabled(this->m_debug);
}

QMDORM::~QMDORM()
{
    delete this->m_django;
}

void QMDORM::setDB(const QSqlDatabase &db)
{
    if(this->m_db.databaseName()!=db.databaseName())
    {
        this->m_db = db;
        this->m_django->setDatabase(this->m_db);
        emit databaseChanged(this->m_db.databaseName());
    }
}

void QMDORM::setDB(const QString &path, const QString &connection, const QString &driver)
{
    if((this->m_db.databaseName()==path)
       && (this->m_db.driverName()==driver)
       && (this->m_db.connectionName()==connection)
       ) return;
    this->m_db = QSqlDatabase::addDatabase(driver,connection);
    this->m_db.setDatabaseName(path);
    this->m_db.open();
    this->m_django->setDatabase(this->m_db);
    emit databaseChanged(this->m_db.databaseName());
}

QSqlDatabase QMDORM::db()
{
    return this->m_db;
}

void QMDORM::enableDebug()
{
    if(!this->m_debug)
    {
        this->m_debug = true;
        this->m_django->setDebugEnabled(this->m_debug);
    }
}

void QMDORM::disableDebug()
{
    if(this->m_debug)
    {
        this->m_debug = false;
        this->m_django->setDebugEnabled(this->m_debug);
    }
}

bool QMDORM::setup() const
{
    return this->m_django->createTables();
}

bool QMDORM::hasValidDatabase() const
{
    return this->m_db.isValid();
}

QStringList QMDORM::getExistingTableList()
{
    return this->m_db.tables();
}

QStringList QMDORM::getModelTableList()
{
    return this->m_django->getModelTableList();
}

