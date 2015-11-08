#ifndef QMDORM_HPP
#define QMDORM_HPP

#include <QDebug>
#include <QObject>
#include <QSqlDriver>
#include <QMetaObject>
#include <QSqlDatabase>

#include "QDjango.h"
#include "QDjangoWhere.h"
#include "QDjangoModel.h"
#include "QDjangoQuerySet.h"
#include "QDjangoMetaModel.h"

class QMDORM : public QObject
{
        Q_OBJECT

    private:
        QDjango *m_django;
        QSqlDatabase m_db;
        bool m_debug;

    public:
        explicit QMDORM(QObject *parent = 0);
        ~QMDORM();

        QDjango *qdjango();

        void setDB(const QSqlDatabase &db);
        void setDB(const QString &path,const QString &connection="QMDDORM",const QString &driver="QSQLITE");

        QSqlDatabase db();

        void enableDebug();
        void disableDebug();

        bool setup() const;
        bool hasValidDatabase() const;

        QStringList getExistingTableList();
        QStringList getModelTableList();

        template <class T> T* model()
        {
            return new T(this->m_django);
        }

        template <class T> void registerModel()
        {
            this->m_django->registerModel<T>();
        }

        template <class T> T* one()
        {
            QDjangoQuerySet<T> query(this->m_django);
            query = query.limit(0,1);
            if(query.size()==1) return query.at(0);
            else return new T(this->m_django);
        }

        template <class T> T* one(const QString &sort_by)
        {
            QDjangoQuerySet<T> query(this->m_django);
            query = query.orderBy(QStringList()<<sort_by);
            query = query.limit(0,1);
            if(query.size()==1) return query.at(0);
            else return new T(this->m_django);
        }

        template <class T> QDjangoQuerySet<T> query()
        {
            QDjangoQuerySet<T> vQuery(this->m_django);
            return vQuery;
        }

    signals:
        void databaseChanged(const QString &path);

    public slots:
};

#endif // QMDORM_HPP
