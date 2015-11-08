#ifndef QDJANGO_WHERE_P_H
#define QDJANGO_WHERE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QDjango API.
//

#include <QSharedData>

#include "QDjangoWhere.h"

class QDjangoWherePrivate : public QSharedData
{
public:
    static QString operationToString(QDjangoWhere::Operation operation);

    enum Combine
    {
        NoCombine,
        AndCombine,
        OrCombine
    };
    static QString combineToString(Combine combine);

    QDjangoWherePrivate();

    QString key;
    QDjangoWhere::Operation operation;
    QVariant data;

    QList<QDjangoWhere> children;
    Combine combine;
    bool negate;
};

#endif
