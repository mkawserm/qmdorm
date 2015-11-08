#ifndef USERINFORMATION_HPP
#define USERINFORMATION_HPP

#include <QObject>
#include <QDjangoModel.h>

class UserInformation : public QDjangoModel
{
        Q_OBJECT

        Q_PROPERTY(QString username READ username WRITE setUsername)
        Q_PROPERTY(QString password READ password WRITE setPassword)
        Q_PROPERTY(QString email READ email WRITE setEmail)

        Q_CLASSINFO("__meta__","db_table=\"UserInformation\"")

    public:
        explicit UserInformation(QDjango *dj=0);

        QString username() const;
        void setUsername(const QString &username);

        QString password() const;
        void setPassword(const QString &password);

        QString email() const;
        void setEmail(const QString &email);

    signals:

    public slots:
    private:
        QString m_username;
        QString m_password;
        QString m_email;
};

#endif // USERINFORMATION_HPP
