#include "powerCtrl.h"

powerCtrl::powerCtrl(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::powerCtrlClass)
{
	ui->setupUi(this);
}

powerCtrl::~powerCtrl()
{}
