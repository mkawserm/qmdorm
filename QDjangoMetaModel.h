#ifndef QDJANGOMETAMODEL_H
#define QDJANGOMETAMODEL_H

#include <QMap>
#include <QVariant>
#include <QSharedDataPointer>


#include "QDjango.h"
#include "QDjango_p.h"

class QDjango;
class QDjangoMetaFieldPrivate;
class QDjangoMetaModelPrivate;

class QDjangoMetaField
{
public:
    QDjangoMetaField();
    QDjangoMetaField(const QDjangoMetaField &other);
    ~QDjangoMetaField();
    QDjangoMetaField& operator=(const QDjangoMetaField &other);

    QString column() const;
    bool isAutoIncrement() const;
    bool isBlank() const;
    bool isNullable() const;
    bool isUnique() const;
    bool isValid() const;
    bool hasDefault() const;
    QString defaultValue() const;
    QString name() const;
    int maxLength() const;
    QVariant toDatabase(const QVariant &value) const;

private:
    QSharedDataPointer<QDjangoMetaFieldPrivate> d;
    friend class QDjangoMetaModel;
};

class QDjangoMetaModel
{
public:
    QDjangoMetaModel(QDjango *django=0,const QMetaObject *model = 0);
    QDjangoMetaModel(const QDjangoMetaModel &other);
    ~QDjangoMetaModel();
    QDjangoMetaModel& operator=(const QDjangoMetaModel &other);

    bool isValid() const;
    void setContext(QDjango *django);
    bool hasContext() const;

    bool createTable() const;
    QStringList createTableSql() const;
    bool dropTable() const;

    void load(QObject *model, const QVariantList &props, int &pos) const;
    bool remove(QObject *model) const;
    bool save(QObject *model) const;

    QObject *foreignKey(const QObject *model, const char *name) const;
    void setForeignKey(QObject *model, const char *name, QObject *value) const;

    QString className() const;
    QDjangoMetaField localField(const char *name) const;
    QList<QDjangoMetaField> localFields() const;
    QMap<QByteArray, QByteArray> foreignFields() const;
    QByteArray primaryKey() const;
    QString table() const;

private:
    QSharedDataPointer<QDjangoMetaModelPrivate> d;
};

#endif
