#pragma once

#include <QWidget>
#include "ui_HomePage.h"

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget* parent = nullptr);
    ~HomePage();

private:
    Ui::HomePage* ui;
};
