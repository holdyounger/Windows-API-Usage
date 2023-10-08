#ifndef QTEXTCODEC_TEST_H
#define QTEXTCODEC_TEST_H

#include <QtGui/QWidget>
#include "ui_qtextcodec_test.h"

class QTextCodec_Test : public QWidget
{
	Q_OBJECT

public:
	QTextCodec_Test(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QTextCodec_Test();

private:
	Ui::QTextCodec_TestClass ui;
};

#endif // QTEXTCODEC_TEST_H
