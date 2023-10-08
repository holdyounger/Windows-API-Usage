#include "QListWidgetTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QListWidgetTest w;
    w.show();
    return a.exec();
}
