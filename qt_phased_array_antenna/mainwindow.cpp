#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "SerialPortManager.h"

#include "Module1.h"
#include "Module2.h"
#include "Module3.h"
#include "Module4.h"
#include "Module5.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_module1(nullptr)  
    , m_module2(nullptr)
    , m_module3(nullptr)
    , m_module4(nullptr)
    , m_module5(nullptr)
{
    ui->setupUi(this);

    SerialPortManager::instance().startMonitoring(1000);

  
    m_module1 = qobject_cast<Module1*>(ui->mainTabWidget->widget(1));
    m_module2 = qobject_cast<Module2*>(ui->mainTabWidget->widget(2));
    m_module3 = qobject_cast<Module3*>(ui->mainTabWidget->widget(3));
    m_module4 = qobject_cast<Module4*>(ui->mainTabWidget->widget(4));
    m_module5 = qobject_cast<Module5*>(ui->mainTabWidget->widget(5));


    Q_ASSERT(m_module1 != nullptr);
    Q_ASSERT(m_module2 != nullptr);
    Q_ASSERT(m_module3 != nullptr);
    Q_ASSERT(m_module4 != nullptr);
    Q_ASSERT(m_module5 != nullptr);
}

MainWindow::~MainWindow()
{
    delete ui;
    
}