#pragma once

#include <QWidget>
#include "ui_SatelliteSignalReceiver.h"

class SatelliteSignalReceiverWidget : public QWidget
{
public:
    explicit SatelliteSignalReceiverWidget(QWidget* parent = nullptr);
    ~SatelliteSignalReceiverWidget();

private:
    Ui::SatelliteSignalReceiverWidget* ui;
};
