#include "qtextcodec_test.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec_Test w;
	w.show();
	return a.exec();
}
