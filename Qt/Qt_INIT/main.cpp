#include "Qt_INIT.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qt_INIT w;
    w.show();
    return a.exec();
}
