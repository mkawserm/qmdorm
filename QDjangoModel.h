#ifndef QDJANGO_MODEL_H
#define QDJANGO_MODEL_H

#include <QObject>
#include <QVariant>

#include "QDjango.h"
#include "QDjango_p.h"

class QDjangoModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant pk READ pk WRITE setPk)
    Q_CLASSINFO("pk", "ignore_field=true")

private:
        QDjango *m_qdjango;

public:
    QDjangoModel(QDjango *qdjango=0);

    QVariant pk() const;
    void setPk(const QVariant &pk);

    bool hasContext() const;
    QDjango *getContext();

public slots:
    bool remove();
    bool save();
    QString toString() const;

protected:
    QObject *foreignKey(const char *name) const;
    void setForeignKey(const char *name, QObject *value);
};

#endif
