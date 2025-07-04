/********************************************************************************
** Form generated from reading UI file 'communication_module.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMUNICATION_MODULE_H
#define UI_COMMUNICATION_MODULE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CommunicationClass
{
public:
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QLabel *label;
    QComboBox *serial_name;
    QLabel *label_2;
    QComboBox *baudrate;
    QLabel *label_9;
    QComboBox *parity;
    QPushButton *serial_switch;
    QLabel *label_4;
    QLabel *label_3;
    QComboBox *databits;
    QComboBox *stopbits;
    QRadioButton *ascii_Button;
    QRadioButton *hex_Button;
    QGroupBox *groupBox_2;
    QLabel *label_5;
    QLabel *label_6;
    QPushButton *pushButtonConnect;
    QLineEdit *lineEditPort;
    QPushButton *pushButtonDisConnect;
    QLineEdit *lineEditIP;
    QGroupBox *serial_display_group;
    QPushButton *send_button;
    QTextEdit *log_display;
    QPushButton *tx_data_clear;
    QPushButton *log_clear;
    QPushButton *rxtx_clear;
    QTextEdit *tx_display;
    QTextEdit *send_data_display;
    QTextEdit *rx_display;
    QGroupBox *tcp_display_group;
    QPushButton *pushButtonSend;
    QPushButton *log_clear_tcp;
    QTextEdit *tx_display_tcp;
    QPushButton *rxtx_clear_tcp;
    QTextEdit *textEditLog;
    QTextEdit *rx_display_tcp;
    QPushButton *tx_data_clear_tcp;
    QLineEdit *lineEditTextToSend;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CommunicationClass)
    {
        if (CommunicationClass->objectName().isEmpty())
            CommunicationClass->setObjectName(QString::fromUtf8("CommunicationClass"));
        CommunicationClass->resize(1090, 597);
        centralWidget = new QWidget(CommunicationClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 201, 511));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 60, 54, 12));
        serial_name = new QComboBox(groupBox);
        serial_name->setObjectName(QString::fromUtf8("serial_name"));
        serial_name->setGeometry(QRect(70, 50, 101, 31));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 120, 54, 12));
        baudrate = new QComboBox(groupBox);
        baudrate->setObjectName(QString::fromUtf8("baudrate"));
        baudrate->setGeometry(QRect(70, 110, 101, 31));
        baudrate->setEditable(true);
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(20, 180, 54, 12));
        parity = new QComboBox(groupBox);
        parity->setObjectName(QString::fromUtf8("parity"));
        parity->setGeometry(QRect(70, 170, 101, 31));
        serial_switch = new QPushButton(groupBox);
        serial_switch->setObjectName(QString::fromUtf8("serial_switch"));
        serial_switch->setGeometry(QRect(20, 430, 151, 31));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 240, 54, 12));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 300, 54, 12));
        databits = new QComboBox(groupBox);
        databits->setObjectName(QString::fromUtf8("databits"));
        databits->setGeometry(QRect(70, 230, 101, 31));
        stopbits = new QComboBox(groupBox);
        stopbits->setObjectName(QString::fromUtf8("stopbits"));
        stopbits->setGeometry(QRect(70, 290, 101, 31));
        ascii_Button = new QRadioButton(groupBox);
        ascii_Button->setObjectName(QString::fromUtf8("ascii_Button"));
        ascii_Button->setGeometry(QRect(120, 370, 91, 18));
        hex_Button = new QRadioButton(groupBox);
        hex_Button->setObjectName(QString::fromUtf8("hex_Button"));
        hex_Button->setGeometry(QRect(20, 370, 91, 18));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(880, 10, 201, 271));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 50, 54, 12));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 90, 54, 12));
        pushButtonConnect = new QPushButton(groupBox_2);
        pushButtonConnect->setObjectName(QString::fromUtf8("pushButtonConnect"));
        pushButtonConnect->setGeometry(QRect(20, 160, 161, 31));
        lineEditPort = new QLineEdit(groupBox_2);
        lineEditPort->setObjectName(QString::fromUtf8("lineEditPort"));
        lineEditPort->setGeometry(QRect(50, 80, 141, 31));
        lineEditPort->setMaxLength(3273);
        pushButtonDisConnect = new QPushButton(groupBox_2);
        pushButtonDisConnect->setObjectName(QString::fromUtf8("pushButtonDisConnect"));
        pushButtonDisConnect->setGeometry(QRect(20, 210, 161, 31));
        lineEditIP = new QLineEdit(groupBox_2);
        lineEditIP->setObjectName(QString::fromUtf8("lineEditIP"));
        lineEditIP->setGeometry(QRect(50, 40, 141, 31));
        serial_display_group = new QGroupBox(centralWidget);
        serial_display_group->setObjectName(QString::fromUtf8("serial_display_group"));
        serial_display_group->setGeometry(QRect(210, 10, 331, 511));
        send_button = new QPushButton(serial_display_group);
        send_button->setObjectName(QString::fromUtf8("send_button"));
        send_button->setGeometry(QRect(250, 470, 71, 31));
        log_display = new QTextEdit(serial_display_group);
        log_display->setObjectName(QString::fromUtf8("log_display"));
        log_display->setGeometry(QRect(10, 30, 311, 321));
        tx_data_clear = new QPushButton(serial_display_group);
        tx_data_clear->setObjectName(QString::fromUtf8("tx_data_clear"));
        tx_data_clear->setGeometry(QRect(10, 470, 71, 31));
        log_clear = new QPushButton(serial_display_group);
        log_clear->setObjectName(QString::fromUtf8("log_clear"));
        log_clear->setGeometry(QRect(10, 360, 71, 31));
        rxtx_clear = new QPushButton(serial_display_group);
        rxtx_clear->setObjectName(QString::fromUtf8("rxtx_clear"));
        rxtx_clear->setGeometry(QRect(250, 360, 71, 31));
        tx_display = new QTextEdit(serial_display_group);
        tx_display->setObjectName(QString::fromUtf8("tx_display"));
        tx_display->setEnabled(true);
        tx_display->setGeometry(QRect(120, 360, 61, 31));
        send_data_display = new QTextEdit(serial_display_group);
        send_data_display->setObjectName(QString::fromUtf8("send_data_display"));
        send_data_display->setGeometry(QRect(10, 400, 311, 61));
        rx_display = new QTextEdit(serial_display_group);
        rx_display->setObjectName(QString::fromUtf8("rx_display"));
        rx_display->setGeometry(QRect(180, 360, 61, 31));
        tcp_display_group = new QGroupBox(centralWidget);
        tcp_display_group->setObjectName(QString::fromUtf8("tcp_display_group"));
        tcp_display_group->setGeometry(QRect(540, 10, 331, 511));
        pushButtonSend = new QPushButton(tcp_display_group);
        pushButtonSend->setObjectName(QString::fromUtf8("pushButtonSend"));
        pushButtonSend->setGeometry(QRect(250, 470, 71, 31));
        log_clear_tcp = new QPushButton(tcp_display_group);
        log_clear_tcp->setObjectName(QString::fromUtf8("log_clear_tcp"));
        log_clear_tcp->setGeometry(QRect(10, 360, 71, 31));
        tx_display_tcp = new QTextEdit(tcp_display_group);
        tx_display_tcp->setObjectName(QString::fromUtf8("tx_display_tcp"));
        tx_display_tcp->setEnabled(true);
        tx_display_tcp->setGeometry(QRect(120, 360, 61, 31));
        rxtx_clear_tcp = new QPushButton(tcp_display_group);
        rxtx_clear_tcp->setObjectName(QString::fromUtf8("rxtx_clear_tcp"));
        rxtx_clear_tcp->setGeometry(QRect(250, 360, 71, 31));
        textEditLog = new QTextEdit(tcp_display_group);
        textEditLog->setObjectName(QString::fromUtf8("textEditLog"));
        textEditLog->setGeometry(QRect(10, 30, 311, 321));
        rx_display_tcp = new QTextEdit(tcp_display_group);
        rx_display_tcp->setObjectName(QString::fromUtf8("rx_display_tcp"));
        rx_display_tcp->setGeometry(QRect(180, 360, 61, 31));
        tx_data_clear_tcp = new QPushButton(tcp_display_group);
        tx_data_clear_tcp->setObjectName(QString::fromUtf8("tx_data_clear_tcp"));
        tx_data_clear_tcp->setGeometry(QRect(10, 470, 71, 31));
        lineEditTextToSend = new QLineEdit(tcp_display_group);
        lineEditTextToSend->setObjectName(QString::fromUtf8("lineEditTextToSend"));
        lineEditTextToSend->setGeometry(QRect(10, 400, 311, 61));
        CommunicationClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(CommunicationClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1090, 22));
        CommunicationClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CommunicationClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        CommunicationClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(CommunicationClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        CommunicationClass->setStatusBar(statusBar);

        retranslateUi(CommunicationClass);

        QMetaObject::connectSlotsByName(CommunicationClass);
    } // setupUi

    void retranslateUi(QMainWindow *CommunicationClass)
    {
        CommunicationClass->setWindowTitle(QCoreApplication::translate("CommunicationClass", "\351\200\232\344\277\241\346\250\241\345\235\227", nullptr));
        groupBox->setTitle(QCoreApplication::translate("CommunicationClass", "\344\270\262\345\217\243\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("CommunicationClass", "\344\270\262\345\217\243\345\220\215", nullptr));
        label_2->setText(QCoreApplication::translate("CommunicationClass", "\346\263\242\347\211\271\347\216\207", nullptr));
        label_9->setText(QCoreApplication::translate("CommunicationClass", "\345\245\207\345\201\266\346\240\241\351\252\214", nullptr));
        serial_switch->setText(QCoreApplication::translate("CommunicationClass", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        label_4->setText(QCoreApplication::translate("CommunicationClass", "\346\225\260\346\215\256\344\275\215", nullptr));
        label_3->setText(QCoreApplication::translate("CommunicationClass", "\345\201\234\346\255\242\344\275\215", nullptr));
        ascii_Button->setText(QCoreApplication::translate("CommunicationClass", "ASCII", nullptr));
        hex_Button->setText(QCoreApplication::translate("CommunicationClass", "16\350\277\233\345\210\266", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("CommunicationClass", "\347\275\221\345\217\243\350\256\276\347\275\256", nullptr));
        label_5->setText(QCoreApplication::translate("CommunicationClass", "\346\234\254\346\234\272IP", nullptr));
        label_6->setText(QCoreApplication::translate("CommunicationClass", "\347\253\257\345\217\243\345\217\267", nullptr));
        pushButtonConnect->setText(QCoreApplication::translate("CommunicationClass", "\346\211\223\345\274\200\346\234\215\345\212\241\345\231\250", nullptr));
        pushButtonDisConnect->setText(QCoreApplication::translate("CommunicationClass", "\345\205\263\351\227\255\346\234\215\345\212\241\345\231\250", nullptr));
        serial_display_group->setTitle(QString());
        send_button->setText(QCoreApplication::translate("CommunicationClass", "\345\217\221\351\200\201", nullptr));
        tx_data_clear->setText(QCoreApplication::translate("CommunicationClass", "\346\270\205\351\231\244\345\217\221\351\200\201", nullptr));
        log_clear->setText(QCoreApplication::translate("CommunicationClass", "\346\270\205\351\231\244log", nullptr));
        rxtx_clear->setText(QCoreApplication::translate("CommunicationClass", "\346\270\205\351\231\244\350\256\241\346\225\260", nullptr));
        tcp_display_group->setTitle(QString());
        pushButtonSend->setText(QCoreApplication::translate("CommunicationClass", "\345\217\221\351\200\201", nullptr));
        log_clear_tcp->setText(QCoreApplication::translate("CommunicationClass", "\346\270\205\351\231\244log", nullptr));
        rxtx_clear_tcp->setText(QCoreApplication::translate("CommunicationClass", "\346\270\205\351\231\244\350\256\241\346\225\260", nullptr));
        tx_data_clear_tcp->setText(QCoreApplication::translate("CommunicationClass", "\346\270\205\351\231\244\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CommunicationClass: public Ui_CommunicationClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMUNICATION_MODULE_H
