#include "MainWindow.h"
#include "ui_mainwindow.h"

// 引入通信架构相关头文件
#include "CommManager.h"
#include "SerialPort.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_variableFreq(nullptr)
    , m_naviUI(nullptr)
    , m_powerCtrl(nullptr)
    , m_anteCtrl(nullptr)
{
    ui->setupUi(this);

    // 1. 获取 UI 中定义的页签控件指针
    // 注意：索引值需与你在 Qt Designer 中排列的顺序严格一致
    m_variableFreq = qobject_cast<variableFreq*>(ui->mainTabWidget->widget(1));
    m_naviUI = qobject_cast<NaviUI*>(ui->mainTabWidget->widget(2));
    m_powerCtrl = qobject_cast<powerCtrl*>(ui->mainTabWidget->widget(3));
    m_anteCtrl = qobject_cast<AnteCtrl*>(ui->mainTabWidget->widget(4));

    // 断言检查，确保 UI 提升（Promote）配置正确
    Q_ASSERT(m_variableFreq != nullptr);
    Q_ASSERT(m_naviUI != nullptr);
    Q_ASSERT(m_powerCtrl != nullptr);
    Q_ASSERT(m_anteCtrl != nullptr);

    // 2. 初始化通信系统
    initCommunicationSystem();
}

void MainWindow::initCommunicationSystem()
{
    // 获取管理器单例
    auto& mgr = CommunicationManager::instance();

    // 3. 为每个业务创建专属的串口硬件实例
    // instanceId 用于在日志和信号中区分数据来源
    SerialPort* vfSerial = new SerialPort("VF_PORT", this);
    SerialPort* naviSerial = new SerialPort("NAVI_PORT", this);
    SerialPort* powerSerial = new SerialPort("POWER_PORT", this);
    SerialPort* anteSerial = new SerialPort("ANTE_PORT", this);

    // 4. 将硬件设备注册到管理器资源池中
    // 这样管理器就能统一监听这些串口的 dataReceived 信号
    mgr.addDevice("VF", vfSerial);
    mgr.addDevice("Navi", naviSerial);
    mgr.addDevice("Power", powerSerial);
    mgr.addDevice("Ante", anteSerial);

    // 5. 将资源注入到页签页面（Dependency Injection）
    // 让页面持有串口指针以控制参数，持有协议指针以订阅解析后的数据

    if (m_variableFreq) {
        m_variableFreq->setDevice(vfSerial);
        m_variableFreq->setProtocol(mgr.getVFProtocol());
    }

    if (m_naviUI) {
        //m_naviUI->setDevice(naviSerial);
        // 如果 Navi 有专属协议，在此处注入
    }

    if (m_powerCtrl) {
        m_powerCtrl->setDevice(powerSerial);
        m_powerCtrl->setProtocol(mgr.getPowerProtocol());
    }

    if (m_anteCtrl) {
        m_anteCtrl->setDevice(anteSerial);
        m_anteCtrl->setProtocol(mgr.getAntennaProtocol());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}