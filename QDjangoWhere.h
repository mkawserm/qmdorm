#ifndef QDJANGO_WHERE_H
#define QDJANGO_WHERE_H

#include <QSharedDataPointer>
#include <QVariant>

#include "QDjango_p.h"

class QDjangoMetaModel;
class QDjangoQuery;
class QDjangoWherePrivate;


class QDjangoWhere
{
public:
    enum Operation
    {
        None,
        Equals,
        NotEquals,
        GreaterThan,
        LessThan,
        GreaterOrEquals,
        LessOrEquals,
        StartsWith,
        EndsWith,
        Contains,
        IsIn,
        IsNull,
        IEquals,
        INotEquals,
        IStartsWith,
        IEndsWith,
        IContains
    };

    QDjangoWhere();
    QDjangoWhere(const QDjangoWhere &other);
    QDjangoWhere(const QString &key, QDjangoWhere::Operation operation, QVariant value);
    ~QDjangoWhere();

    QDjangoWhere& operator=(const QDjangoWhere &other);
    QDjangoWhere operator!() const;
    QDjangoWhere operator&&(const QDjangoWhere &other) const;
    QDjangoWhere operator||(const QDjangoWhere &other) const;

    void bindValues(QDjangoQuery &query) const;
    bool isAll() const;
    bool isNone() const;
    QString sql(const QSqlDatabase &db) const;
    QString toString() const;

private:
    QSharedDataPointer<QDjangoWherePrivate> d;
    friend class QDjangoCompiler;
};

#endif
