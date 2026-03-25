#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "variableFreq.h"
#include "NaviUI.h"
#include "powerCtrl.h"
#include "AnteCtrl.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

// Ç°ÏòÉùÃ÷
class variableFreq;
class NaviUI;
class powerCtrl;
class AnteCtrl;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    variableFreq* m_variableFreq;
    NaviUI* m_naviUI;
    powerCtrl* m_powerCtrl;
    AnteCtrl* m_anteCtrl;
};

#endif // MAINWINDOW_H