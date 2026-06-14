#include "HighOrbitStarTrackingWidget.h"

HighOrbitStarTrackingWidget::HighOrbitStarTrackingWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HighOrbitStarTrackingWidget)
{
    ui->setupUi(this);
    setChannelInputsReadOnly();
}

HighOrbitStarTrackingWidget::~HighOrbitStarTrackingWidget()
{
    delete ui;
}

void HighOrbitStarTrackingWidget::on_btnIfSet_clicked()
{
    bool ok = false;
    const int channel = ui->leIfChannel->text().trimmed().toInt(&ok);
    if (!ok || channel < 1 || channel > 8) {
        return;
    }

    QLineEdit* upFreqEdits[] = {
        ui->leChannel1UpFreq, ui->leChannel2UpFreq, ui->leChannel3UpFreq, ui->leChannel4UpFreq,
        ui->leChannel5UpFreq, ui->leChannel6UpFreq, ui->leChannel7UpFreq, ui->leChannel8UpFreq
    };

    QLineEdit* downFreqEdits[] = {
        ui->leChannel1DownFreq, ui->leChannel2DownFreq, ui->leChannel3DownFreq, ui->leChannel4DownFreq,
        ui->leChannel5DownFreq, ui->leChannel6DownFreq, ui->leChannel7DownFreq, ui->leChannel8DownFreq
    };

    QWidget* channelStatusLeds[] = {
        ui->ledChannel1Status, ui->ledChannel2Status, ui->ledChannel3Status, ui->ledChannel4Status,
        ui->ledChannel5Status, ui->ledChannel6Status, ui->ledChannel7Status, ui->ledChannel8Status
    };

    QWidget* rxConverterLeds[] = {
        ui->ledRxBeam1Converter, ui->ledRxBeam2Converter, ui->ledRxBeam3Converter, ui->ledRxBeam4Converter,
        ui->ledRxBeam5Converter, ui->ledRxBeam6Converter, ui->ledRxBeam7Converter, ui->ledRxBeam8Converter
    };

    const int index = channel - 1;
    upFreqEdits[index]->setText(ui->leIfUpFreq->text());
    downFreqEdits[index]->setText(ui->leIfDownFreq->text());
    setLedGreen(channelStatusLeds[index]);
    setLedGreen(rxConverterLeds[index]);
}

void HighOrbitStarTrackingWidget::setChannelInputsReadOnly()
{
    QLineEdit* channelEdits[] = {
        ui->leChannel1UpFreq, ui->leChannel1DownFreq,
        ui->leChannel2UpFreq, ui->leChannel2DownFreq,
        ui->leChannel3UpFreq, ui->leChannel3DownFreq,
        ui->leChannel4UpFreq, ui->leChannel4DownFreq,
        ui->leChannel5UpFreq, ui->leChannel5DownFreq,
        ui->leChannel6UpFreq, ui->leChannel6DownFreq,
        ui->leChannel7UpFreq, ui->leChannel7DownFreq,
        ui->leChannel8UpFreq, ui->leChannel8DownFreq
    };

    for (QLineEdit* edit : channelEdits) {
        edit->setReadOnly(true);
    }
}

void HighOrbitStarTrackingWidget::setLedGreen(QWidget* led)
{
    led->setStyleSheet(QStringLiteral("background:#5CB85C;border:1px solid #4A9C4A;border-radius:6px;"));
}
