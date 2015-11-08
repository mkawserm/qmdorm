#ifndef QDJANGO_H
#define QDJANGO_H

#include <QSharedPointer>
#include "QDjangoMetaModel.h"

class QObject;
class QString;
class QSqlQuery;
class QSqlDatabase;
class QDjangoDatabase;


class QDjango
{
private:
        QMap<QByteArray, QDjangoMetaModel> globalMetaModels;
        QDjangoDatabase *globalDatabase;
        bool globalDebugEnabled;
        QDjangoDatabase::DatabaseType globalDatabaseType;
public:
    QDjango();
    ~QDjango();
    bool createTables();
    bool dropTables();

    QSqlDatabase database();
    void setDatabase(QSqlDatabase database);

    bool isDebugEnabled();
    void setDebugEnabled(bool enabled);

    QDjangoDatabase::DatabaseType databaseType();
    QStringList getModelTableList();

    template <class T>
    QDjangoMetaModel registerModel();

private:
    void qdjango_topsort(const QByteArray &modelName, QHash<QByteArray, bool> &visited,QStack<QDjangoMetaModel> &stack);
    QStack<QDjangoMetaModel> qdjango_sorted_metamodels();
    void initDatabase(QSqlDatabase db);

private:
    QDjangoMetaModel registerModel(const QMetaObject *meta);
    QDjangoMetaModel metaModel(const char *name);

    friend class QDjangoModel;
    friend class QDjangoCompiler;
    friend class QDjangoMetaModel;
    friend class QDjangoQuerySetPrivate;
};

/** Register a QDjangoModel class with QDjango.
 */
template <class T>
QDjangoMetaModel QDjango::registerModel()
{
    return registerModel(&T::staticMetaObject);
}
#endif
