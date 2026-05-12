#pragma once

#include <QWidget>
#include "ui_LEOTrack.h"

class LEOTrackPage : public QWidget
{
    Q_OBJECT

public:
    explicit LEOTrackPage(QWidget* parent = nullptr);
    ~LEOTrackPage();

private:
    Ui::TaskTrajectoryDisplay* ui;
};
