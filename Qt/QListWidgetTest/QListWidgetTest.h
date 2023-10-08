#pragma once

#include <QtWidgets/QWidget>
#include "ui_QListWidgetTest.h"
#include <QWidget>
#include <QListWidget>
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QListWidgetItem>
#include <QGridLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QRadioButton>
#include <QLabel>
#include <QPixmap>
#include <QPicture>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #pragma execution_character_set("utf-8")
#endif

class QListWidgetTest : public QWidget
{
    Q_OBJECT

public:
    QListWidgetTest(QWidget *parent = Q_NULLPTR);

private:
    void initListWidget();
private:
    Ui::QListWidgetTestClass ui;
};
