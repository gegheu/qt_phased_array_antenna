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
    Ui::NaviUI* ui;
};
