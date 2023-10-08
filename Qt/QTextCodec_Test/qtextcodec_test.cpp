#include "qtextcodec_test.h"
#include <QApplication>
#include <QTextCodec>
#include <QDebug>
#include <QFontDatabase>

QTextCodec_Test::QTextCodec_Test(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	ui.setupUi(this);

	QFontDatabase database;
	foreach (const QString &family, database.families()) {
		qDebug()<<family;
	}

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

	ui.label->setText("ÄãºÃ \r\n ÊÀ½ç!");
	ui.label_2->setText("hello \n world!");
}

QTextCodec_Test::~QTextCodec_Test()
{
	
}
