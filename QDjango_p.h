#ifndef QDJANGO_P_H
#define QDJANGO_P_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

class QDjangoDatabase : public QObject
{
    Q_OBJECT
public:
    QDjangoDatabase(QObject *parent = 0);

    enum DatabaseType {
        UnknownDB,
        MSSqlServer,
        MySqlServer,
        PostgreSQL,
        Oracle,
        Sybase,
        SQLite,
        Interbase,
        DB2
    };

    static DatabaseType databaseType(const QSqlDatabase &db);

    QSqlDatabase reference;
    QMutex mutex;
    QMap<QThread*, QSqlDatabase> copies;
    qint64 connectionId;

private slots:
    void threadFinished();
};


class QDjangoQuery : public QSqlQuery{
    private:
        bool m_debug;
    public:
        QDjangoQuery(QSqlDatabase db);
        void setDebug(const bool &b);
        void addBindValue(const QVariant &val, QSql::ParamType paramType = QSql::In);
        bool exec();
        bool exec(const QString &query);
};

#endif
