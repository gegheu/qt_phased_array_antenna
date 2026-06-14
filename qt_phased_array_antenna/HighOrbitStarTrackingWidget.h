#pragma once

#include <QWidget>
#include "ui_HighOrbitStarTracking.h"

class HighOrbitStarTrackingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HighOrbitStarTrackingWidget(QWidget* parent = nullptr);
    ~HighOrbitStarTrackingWidget();

private slots:
    void on_btnIfSet_clicked();

private:
    void setChannelInputsReadOnly();
    void setLedGreen(QWidget* led);

    Ui::HighOrbitStarTrackingWidget* ui;
};
