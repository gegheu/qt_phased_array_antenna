#include "module7.h"
#include "ui_module7.h"

Module7::Module7(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Module7)
{
    ui->setupUi(this);
}

Module7::~Module7()
{

}