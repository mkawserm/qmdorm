#include "userinformation.hpp"

UserInformation::UserInformation(QDjango *dj) : QDjangoModel(dj)
{

}
QString UserInformation::username() const
{
    return m_username;
}

void UserInformation::setUsername(const QString &username)
{
    m_username = username;
}
QString UserInformation::password() const
{
    return m_password;
}

void UserInformation::setPassword(const QString &password)
{
    m_password = password;
}
QString UserInformation::email() const
{
    return m_email;
}

void UserInformation::setEmail(const QString &email)
{
    m_email = email;
}




