#pragma once
#include <QMainWindow>
#include "variableFreq.h"
#include "NaviUI.h"
#include "powerCtrl.h"
#include "AnteCtrl.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    // 页签页面指针
    variableFreq* m_variableFreq;
    NaviUI* m_naviUI;
    powerCtrl* m_powerCtrl;
    AnteCtrl* m_anteCtrl;

    // 初始化通信系统的私有函数
    void initCommunicationSystem();
};