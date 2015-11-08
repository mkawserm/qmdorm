#include <QCoreApplication>
#include <QMDORM>

#include "userinformation.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QMDORM qmdorm;
    qmdorm.enableDebug();
    qmdorm.registerModel<UserInformation>();
    qmdorm.setDB(QString("one.sqlite"));
    qmdorm.setup();


    UserInformation *u;

    qDebug() << "\n\n //saving data";
    u = qmdorm.model<UserInformation>();
    u->setEmail("one@gmail.com");
    u->setUsername("one");
    u->setPassword("asdgnw");
    qDebug() << u->save();
    qDebug() << "id:"<<u->pk().toInt();
    delete u;

    //USE only for loop to retrieve data
    qDebug() << "\n\n //retrieve data";
    QDjangoQuerySet<UserInformation> query = qmdorm.query<UserInformation>();
    qDebug() << query.size();
    //query = query.limit(0,5);
    for(int i=0;i<query.size();++i){
        qDebug() << query.at(i)->username();
    }

    return a.exec();
}
