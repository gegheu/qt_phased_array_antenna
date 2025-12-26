#pragma once

#include <QWidget>
#include "ui_powerCtrl.h"

class powerCtrl : public QWidget
{
	Q_OBJECT

public:
	powerCtrl(QWidget *parent = nullptr);
	~powerCtrl();

private:
	Ui::powerCtrlClass* ui;
};
