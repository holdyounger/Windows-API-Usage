#pragma once

#include <QtWidgets/QDialog>
#include "ui_Qt_INIT.h"

class Qt_INIT : public QDialog
{
    Q_OBJECT

public:
    Qt_INIT(QWidget *parent = Q_NULLPTR);

private:
    Ui::Qt_INITClass ui;
};
