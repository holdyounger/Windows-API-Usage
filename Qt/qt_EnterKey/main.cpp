#include "qt_enterkey.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qt_EnterKey w;
	w.show();
	return a.exec();
}
