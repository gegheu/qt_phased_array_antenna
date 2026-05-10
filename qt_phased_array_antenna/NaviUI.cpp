#include "NaviUI.h"


NaviUI::NaviUI(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NaviReceiver)
{
    ui->setupUi(this);
}

NaviUI::~NaviUI()
{
    delete ui;
}
