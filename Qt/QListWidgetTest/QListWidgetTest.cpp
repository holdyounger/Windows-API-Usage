#include "QListWidgetTest.h"

QListWidgetTest::QListWidgetTest(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	QPixmap pixmap("D:\\CodeSource\\C++\\vs2019\\QListWidgetTest\\Win11.png");

	ui.label->setFixedSize(400,250);
	ui.label->setPixmap(pixmap);

	// ui.label->setText(QString("测试文本"));

	initListWidget();
}

void QListWidgetTest::initListWidget()
{
	ui.listWidget->setCurrentRow(2);

	QStringList StrListHeader;
	StrListHeader << QString("选中") << QString("序号") << QString("文件名");
	new QListWidgetItem(tr("【测试】QListWidget添加QListWidgetItem"), ui.listWidget);

	// 设置 List 的显示方式
	ui.listWidget->setViewMode(QListWidget::ListMode);

	QListWidgetItem* ListItem = new QListWidgetItem;
	//    ListItem->setText();

	QListWidgetItem* WContainerItem = new QListWidgetItem(ui.listWidget);
	QWidget* WContainer = new QWidget(ui.listWidget); //新建一个QWidget窗口
	QGridLayout* Glayout = new QGridLayout;

	/*************start*****************/
	/* 在这里添加需要的控件 */
	// 添加文本编辑框
	QLineEdit* lineEdit = new QLineEdit;
	Glayout->addWidget(lineEdit, 1, 0);
	
	// 添加工具按钮
	QToolButton* toolButton = new QToolButton;
	Glayout->addWidget(toolButton, 2, 0);

	// 添加单选按钮
	QRadioButton* radioButton_1 = new QRadioButton;
	Glayout->addWidget(radioButton_1, 3, 0);

	WContainer->setLayout(Glayout);
	WContainer->setMinimumHeight(50);
	/******************end****************/

	// 添加一个Label
	/*************start*****************/
	QWidget* WContainer_1 = new QWidget(ui.listWidget); //新建一个QWidget窗口
	QListWidgetItem* labelContainer = new QListWidgetItem(ui.listWidget);
	QGridLayout* GLayout_1 = new QGridLayout;
	QLabel* label = new QLabel;
	QPixmap pixmap("D:\\CodeSource\\C++\\vs2019\\QListWidgetTest\\Win11.png");
	label->setPixmap(pixmap);
	label->setStyleSheet("QLabel{"
		"background-image:url(D:\\CodeSource\\C++\\vs2019\\QListWidgetTest\\Win11.png);"
		"background-position:top right;"
		"background-origin:content;"
		"background-repeat:none;"
		"}");
	GLayout_1->addWidget(label, 0, 0);
	WContainer_1->setLayout(GLayout_1);
	ui.listWidget->setItemWidget(labelContainer, WContainer_1);
	/******************end****************/

	Glayout->addWidget(label, 3, 0);

	WContainerItem->setSizeHint(QSize(40, 40));
	ui.listWidget->setItemWidget(WContainerItem, WContainer);//将WContainer赋予WContainerItem
}
