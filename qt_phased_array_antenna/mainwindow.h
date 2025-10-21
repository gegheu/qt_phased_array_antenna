// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

// 前向声明模块类，减少头文件依赖
class Module1;
class Module2;
class Module3;
class Module4;
class Module5;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    Module1* m_module1;
    Module2* m_module2;
    Module3* m_module3;
    Module4* m_module4;
    Module5* m_module5;
};

#endif // MAINWINDOW_H