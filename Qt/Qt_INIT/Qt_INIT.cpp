#include "Qt_INIT.h"
#include <QLabel>
#include <QPixmap>


Qt_INIT::Qt_INIT(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

	QLabel* label = new QLabel;

	label->setText("asdasdasdadsa");
	//QPixmap pixmap("D:\\CodeSource\\C++\\vs2019\\QListWidgetTest\\Win11.png");
	QPixmap pixmap(":/Win11.png");
	label->setPixmap(pixmap);
	label->show();
	label->setGeometry(this->x() + 100,this->y() + 100,450,500);
}
