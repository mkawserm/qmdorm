#ifndef QDJANGO_QUERYSET_P_H
#define QDJANGO_QUERYSET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QDjango API.
//

#include <QStringList>

#include "QDjango.h"
#include "QDjango_p.h"
#include "QDjangoWhere.h"

class QDjangoMetaModel;

class QDjangoModelReference
{
public:
    QDjangoModelReference(const QString &tableReference_ = QString(), const QDjangoMetaModel &metaModel_ = QDjangoMetaModel(0), bool nullable_ = false)
        : tableReference(tableReference_)
        , metaModel(metaModel_)
        , nullable(nullable_)
    {
    };

    QString tableReference;
    QDjangoMetaModel metaModel;
    bool nullable;
};

class QDjangoReverseReference
{
public:
    QString leftHandKey;
    QString rightHandKey;
};


class QDjangoCompiler
{
private:
    QDjango *django;
public:
    QDjangoCompiler(QDjango *django,const char *modelName, const QSqlDatabase &db);
    QString fromSql();
    QStringList fieldNames(bool recurse, QDjangoMetaModel *metaModel = 0, const QString &modelPath = QString(), bool nullable = false);
    QString orderLimitSql(const QStringList &orderBy, int lowMark, int highMark);
    void resolve(QDjangoWhere &where);

private:
    QString databaseColumn(const QString &name);
    QString referenceModel(const QString &modelPath, QDjangoMetaModel *metaModel, bool nullable);

    QSqlDriver *driver;
    QDjangoMetaModel baseModel;
    QMap<QString, QDjangoModelReference> modelRefs;
    QMap<QString, QDjangoReverseReference> reverseModelRefs;
    QMap<QString, QString> fieldColumnCache;
};


class QDjangoQuerySetPrivate
{
public:
    QDjango *django;

public:
    QDjangoQuerySetPrivate(QDjango *django, const char *modelName);

    void addFilter(const QDjangoWhere &where);
    QDjangoWhere resolvedWhere(const QSqlDatabase &db) const;
    bool sqlDelete();
    bool sqlFetch();
    bool sqlInsert(const QVariantMap &fields, QVariant *insertId = 0);
    bool sqlLoad(QObject *model, int index);
    int sqlUpdate(const QVariantMap &fields);
    QList<QVariantMap> sqlValues(const QStringList &fields);
    QList<QVariantList> sqlValuesList(const QStringList &fields);

    // SQL queries
    QDjangoQuery countQuery() const;
    QDjangoQuery deleteQuery() const;
    QDjangoQuery insertQuery(const QVariantMap &fields) const;
    QDjangoQuery selectQuery() const;
    QDjangoQuery updateQuery(const QVariantMap &fields) const;

    // reference counter
    QAtomicInt counter;

    bool hasResults;
    int lowMark;
    int highMark;
    QDjangoWhere whereClause;
    QStringList orderBy;
    QList<QVariantList> properties;
    bool selectRelated;

private:
    Q_DISABLE_COPY(QDjangoQuerySetPrivate)

    QByteArray m_modelName;

    friend class QDjangoMetaModel;
};

#endif
