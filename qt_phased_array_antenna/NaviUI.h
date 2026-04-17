#pragma once

#include <QWidget>
#include "ui_navi.h"

class NaviUI : public QWidget
{
    Q_OBJECT

public:
    explicit NaviUI(QWidget* parent = nullptr);
    ~NaviUI();


private:
    //ICommunication* m_device = nullptr;
    //AntennaProtocol* m_protocol = nullptr;
    Ui::NaviUI* ui;
};
