#pragma once

#include <QDialog>

namespace Ui {
    class TcpConfigDialog;
}

class TcpConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TcpConfigDialog(QWidget* parent = nullptr);
    ~TcpConfigDialog();

    QString ipAddress() const;
    int port() const;
    bool hexDisplay() const;

    void setCurrentConfig(const QString& ip, int port, bool hexDisplay);

private slots:
    void onAccepted();

private:
    // 使用完整命名空间限定
    Ui::TcpConfigDialog* ui;
};