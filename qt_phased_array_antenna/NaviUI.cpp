#include "NaviUI.h"


NaviUI::NaviUI(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NaviUI)
{
    ui->setupUi(this);
}

NaviUI::~NaviUI()
{

}