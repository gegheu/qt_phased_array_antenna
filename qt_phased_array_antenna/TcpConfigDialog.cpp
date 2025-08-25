#include "TcpConfigDialog.h"
#include "ui_TcpConfigDialog.h"

TcpConfigDialog::TcpConfigDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::TcpConfigDialog)  
{
    ui->setupUi(this);

    // ÉèÖÃÄ¬ÈÏÖµ
    ui->lineEditIP->setText("192.168.1.100");
    ui->lineEditPort->setText("8080");
    ui->tcp_hex_Button->setChecked(true);

    connect(ui->okButton, &QPushButton::clicked, this, &TcpConfigDialog::onAccepted);
}

TcpConfigDialog::~TcpConfigDialog()
{
    delete ui;
}

QString TcpConfigDialog::ipAddress() const
{
    return ui->lineEditIP->text();
}

int TcpConfigDialog::port() const
{
    return ui->lineEditPort->text().toInt();
}

bool TcpConfigDialog::hexDisplay() const
{
    return ui->tcp_hex_Button->isChecked();
}

void TcpConfigDialog::setCurrentConfig(const QString& ip, int port, bool hexDisplay)
{
    ui->lineEditIP->setText(ip);
    ui->lineEditPort->setText(QString::number(port));

    if (hexDisplay) {
        ui->tcp_hex_Button->setChecked(true);
    }
    else {
        ui->tcp_ascii_Button->setChecked(true);
    }
}

void TcpConfigDialog::onAccepted()
{
    accept();
}