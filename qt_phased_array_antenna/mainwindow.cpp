#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Module1.h"
#include "Module2.h"
#include "Module3.h"
#include "Module4.h"
#include "Module5.h"



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_module1Button_clicked()
{
    Module1* module = new Module1(this);
    module->setAttribute(Qt::WA_DeleteOnClose);
    module->show();
}

void MainWindow::on_module2Button_clicked()
{
    Module2* module = new Module2(this);
    module->setAttribute(Qt::WA_DeleteOnClose);
    module->show();
}

void MainWindow::on_module3Button_clicked()
{
    Module3* module = new Module3(this);
    module->setAttribute(Qt::WA_DeleteOnClose);
    module->show();
}

void MainWindow::on_module4Button_clicked()
{
    Module4* module = new Module4(this);
    module->setAttribute(Qt::WA_DeleteOnClose);
    module->show();
}

void MainWindow::on_module5Button_clicked()
{
    Module5* module = new Module5(this);
    module->setAttribute(Qt::WA_DeleteOnClose);
    module->show();
}