#include "SatelliteSignalReceiverWidget.h"

SatelliteSignalReceiverWidget::SatelliteSignalReceiverWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SatelliteSignalReceiverWidget)
{
    ui->setupUi(this);
}

SatelliteSignalReceiverWidget::~SatelliteSignalReceiverWidget()
{
    delete ui;
}
