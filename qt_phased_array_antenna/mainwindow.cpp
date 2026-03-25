#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "SerialPortManager.h"



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_variableFreq(nullptr)
    , m_naviUI(nullptr)
    , m_powerCtrl(nullptr)
    , m_anteCtrl(nullptr)
{
    ui->setupUi(this);

    SerialPortManager::instance().startMonitoring(1000);

    // 根据 UI 文件中的页签顺序获取对应的控件
    // 页签顺序: tab(索引0), variableFreq(索引1), NaviUI(索引2), powerCtrl(索引3), AnteCtrl(索引4)
    m_variableFreq = qobject_cast<variableFreq*>(ui->mainTabWidget->widget(1));
    m_naviUI = qobject_cast<NaviUI*>(ui->mainTabWidget->widget(2));
    m_powerCtrl = qobject_cast<powerCtrl*>(ui->mainTabWidget->widget(3));
    m_anteCtrl = qobject_cast<AnteCtrl*>(ui->mainTabWidget->widget(4));

    Q_ASSERT(m_variableFreq != nullptr);
    Q_ASSERT(m_naviUI != nullptr);
    Q_ASSERT(m_powerCtrl != nullptr);
    Q_ASSERT(m_anteCtrl != nullptr);
}

MainWindow::~MainWindow()
{
    delete ui;
}