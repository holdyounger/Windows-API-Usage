#include <QtCore/QCoreApplication>
#include "QUpdateDB.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QUpdateDB::GetInstance()->copyFiletoNew(QString("C:/Users/shimingming/TrustAgent/TrustConf.db"),0);

    QUpdateDB::GetInstance()->createKey("Trust05051025");
    QUpdateDB::GetInstance()->open();


    return a.exec();
}
