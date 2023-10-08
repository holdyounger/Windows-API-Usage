#include "qt_enterkey.h"

qt_EnterKey::qt_EnterKey(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	initTreeWidget();
}

qt_EnterKey::~qt_EnterKey()
{

}

void qt_EnterKey::initTreeWidget()
{

	ui.label->setText("hello \r\n world!");
	ui.label_2->setText("hello \n world!");
}
