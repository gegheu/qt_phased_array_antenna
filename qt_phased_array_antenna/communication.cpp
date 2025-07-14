#include "communication.h"
#include "ui_communication_module.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QTextDecoder>
#include <QTextCodec>
#include <QTime>
#include <QCollator>
#include <QNetworkInterface>
#include <QTextCodec>


communication::communication(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::CommunicationClass)
{
    ui->setupUi(this);

	serialPort = new SerialPort(this);
	tcp = new Tcp(this);

	m_timer = new QTimer(this);
	m_settings = new QSettings("config.ini", QSettings::IniFormat, this);


	m_tx_num = 0;
	m_rx_num = 0;


    initSerial();
	initNetwork();

	//����
	connect(serialPort, &SerialPort::dataReceived, this, &communication::handleRecvData);//��������
    connect(m_timer, &QTimer::timeout, this, &communication::refreshPorts);//��ʱˢ���豸
	connect(ui->serial_switch, &QPushButton::clicked, this, &communication::handleOpenSerial);//���ش���
	connect(ui->send_button, &QPushButton::clicked, this, &communication::handleSendData);//��������
	connect(ui->rxtx_clear, &QPushButton::clicked, this, &communication::handleClearTxRx);//��ռ���
	connect(ui->log_clear, &QPushButton::clicked, this, &communication::handleClearLog);//�����־��
	connect(ui->tx_data_clear, &QPushButton::clicked, this, &communication::handleClearTxData);//��շ���������

	//����
	connect(ui->pushButtonConnect, &QPushButton::clicked, this, &communication::onConnectClicked);
	connect(ui->pushButtonSend, &QPushButton::clicked, this, &communication::onSendClicked);
	connect(ui->pushButtonDisConnect, &QPushButton::clicked, this, &communication::onDisconnectClicked);
	
	connect(tcp, &Tcp::dataReceived, this, &communication::onDataReceived);
	connect(tcp, &Tcp::connectError, this, &communication::onErrorOccurred);
}


communication::~communication()
{
	saveINI();
	delete ui;
}

// ���ӷ�����
void communication::onConnectClicked()
{
	if (!tcp->isConnected()) {
		// ��ȡIP�Ͷ˿�
		QString ip = ui->lineEditIP->text();
		int port = ui->lineEditPort->text().toInt();

		// ��֤����
		if (ip.isEmpty()) {
			QMessageBox::warning(this, QStringLiteral("�������"), QStringLiteral("�����������IP��ַ"));
			return;
		}

		if (port < 1 || port > 65535) {
			QMessageBox::warning(this, QStringLiteral("�˿ڴ���"), QStringLiteral("�˿ںű�����1-65535֮��"));
			return;
		}

		// �������ӷ�����
		tcp->connectToHost(ip, port);
		ui->textEditLog->append(QStringLiteral("�������ӷ�����: ") + ip + ":" + QString::number(port));
		ui->pushButtonConnect->setEnabled(false); // �������Ӱ�ťֱ���������
	}
}

// ��������
void communication::onSendClicked()
{
	if (tcp->isConnected()) {
		QString data = ui->lineEditTextToSend->text();
		if (!data.isEmpty()) {
			// �������ݵ�STM32
			QByteArray byteArray = data.toUtf8();
			tcp->write(byteArray);

			// ��ʾ������־
			ui->textEditLog->append(QStringLiteral("[����] �ı�: ") + data);
			ui->textEditLog->append(QStringLiteral("[����] ʮ������: ") + byteArray.toHex(' ').toUpper());
		}
	}
	else {
		ui->textEditLog->append(QStringLiteral("δ���ӷ��������޷���������"));
	}
}

// �Ͽ�����
void communication::onDisconnectClicked()
{
	if (tcp->isConnected()) {
		tcp->disconnect();
		ui->textEditLog->append(QStringLiteral("�ѶϿ�����������"));
	}
}

// ���ӳɹ�
void communication::onConnected()
{
	// ����UI״̬
	ui->pushButtonConnect->setText(QStringLiteral("������"));
	ui->pushButtonConnect->setEnabled(true);
	ui->pushButtonSend->setEnabled(true);
	ui->pushButtonDisConnect->setEnabled(true);
	ui->lineEditIP->setEnabled(false);
	ui->lineEditPort->setEnabled(false);

	// ��ʾ������Ϣ
	ui->textEditLog->append(QStringLiteral("���ӷ������ɹ�!"));
}

// ���ӶϿ�
void communication::onDisconnected()
{
	// ����UI״̬
	ui->pushButtonConnect->setText(QStringLiteral("���ӷ�����"));
	ui->pushButtonConnect->setEnabled(true);
	ui->pushButtonSend->setEnabled(false);
	ui->pushButtonDisConnect->setEnabled(false);
	ui->lineEditIP->setEnabled(true);
	ui->lineEditPort->setEnabled(true);

	// ��ʾ�Ͽ���Ϣ
	ui->textEditLog->append(QStringLiteral("��������������ѶϿ�"));
}

// ��������
void communication::onDataReceived(const QByteArray& data)
{
	// ��ʾʮ�����Ƹ�ʽ
	QString hexData = data.toHex(' ').toUpper();
	ui->textEditLog->append(QStringLiteral("[����] ʮ������: ") + hexData);

	// ������ʾ�ı���������ı����ݣ�
	QString textData = QString::fromUtf8(data);
	if (!textData.isEmpty() && textData.toUtf8() == data) {
		ui->textEditLog->append(QStringLiteral("[����] �ı�: ") + textData);
	}
}

// ������
void communication::onErrorOccurred(const QString& errorInfo)
{
	ui->textEditLog->append(QStringLiteral("��������: ") + errorInfo);

	// ��������״̬
	ui->pushButtonConnect->setText(QStringLiteral("���ӷ�����"));
	ui->pushButtonConnect->setEnabled(true);
	ui->pushButtonSend->setEnabled(false);
	ui->pushButtonDisConnect->setEnabled(false);
	ui->lineEditIP->setEnabled(true);
	ui->lineEditPort->setEnabled(true);
}

void communication::initNetwork() 
{
	ui->lineEditIP->setText("192.168.1.100");
	ui->lineEditPort->setText("8080");
}


void communication::handleLoadINI()
{
	QStringList defaultRates = { "1200" , "2400" , "4800" , "9600" , "19200" , "38400" , "57600" , "115200" , "230400" , "345600" , "460800" , "576000" , "921600" , "1382400" };
	QStringList baudRates = m_settings->value("BaudRates/myBaud", defaultRates).toStringList();
	
	ui->baudrate->addItems(baudRates);
}

void communication::saveINI()
{
	QSettings settings("config.ini", QSettings::IniFormat);
	QCollator collator;
	QStringList ratesList;

	for (int i = 0; i < ui->baudrate->count(); i++) {
		ratesList << ui->baudrate->itemText(i);
	}
	collator.setNumericMode(true);
	std::sort(ratesList.begin(),ratesList.end(),collator);
	m_settings->setValue("BaudRates/myBaud", ratesList);
}

void communication::handleClearTxRx()
{
	ui->tx_display->clear();
	ui->rx_display->clear();
	m_rx_num = 0;
	m_tx_num = 0;
}


void communication::handleRecvData(const QByteArray& data)
{
	if (!data.isEmpty()) {
		QString result;
		if (ui->hex_Button->isChecked()) {
			for (quint8 byte : data) {
				result += QString("%1 ")
					.arg(byte, 2, 16, QLatin1Char('0'))
					.toUpper();
			}
		}
		else {
			result = QString(data);
		}
		m_rx_num += data.size();
		ui->rx_display->setText("rx:" + QString::number(m_rx_num));
		ui->rx_display->setAlignment(Qt::AlignCenter);
		appendLog(result, false);
	}
}

void communication::appendLog(const QString& str, bool isSend)
{
	QString append_str;

	append_str += QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");


	if (isSend == true) {
		append_str += "  SEND" ;
		ui->log_display->append(append_str);
		ui->log_display->setTextColor(Qt::blue);
		ui->log_display->append(str);
		ui->log_display->setTextColor(Qt::black);
	}
	else {
		append_str += "  RECV";
		ui->log_display->append(append_str);
		ui->log_display->setTextColor(Qt::darkCyan);
		ui->log_display->append(str);
		ui->log_display->setTextColor(Qt::black);
	}
}

void communication::handleSendData()
{
	qint32 send_byte_size = 0;
	QString displayStr;
	QByteArray write_buff;
	QString data_buff = ui->send_data_display->toPlainText();
	if (!serialPort->isOpen()){
		QMessageBox::information(this, (QString::fromLocal8Bit("��ʾ")), (QString::fromLocal8Bit("���ȴ򿪴���")));
	}
	else {
		if (ui->hex_Button->isChecked()) {
			QStringList hexBytes = data_buff.split(' ', Qt::SkipEmptyParts);
			
			for (auto byteStr : hexBytes) {
				bool ok;
				quint8 byte = byteStr.toUInt(&ok, 16);
				if (ok) {
					write_buff.append(byte);
					displayStr += QString("%1 ").arg(byte, 2, 16, QLatin1Char('0')).toUpper();
				}
			}
		}
		else{
			write_buff.append(data_buff.toUtf8());
			displayStr = data_buff;
		}
		send_byte_size = serialPort->write(write_buff);
		m_tx_num += send_byte_size;
		ui->tx_display->setText("tx:"+ QString::number(m_tx_num));
		ui->tx_display->setAlignment(Qt::AlignCenter);
		appendLog(displayStr, true);
	}
}

void communication::handleClearLog()
{
	ui->log_display->clear();
}

void communication::handleClearTxData()
{
	ui->send_data_display->clear();
}

enum QSerialPort::Parity communication::getParity(){
	QString currentParityBit = ui->parity->currentText();
	if (currentParityBit == "None")
		return QSerialPort::NoParity;
	else if (currentParityBit == "Odd")//����
		return QSerialPort::OddParity;
	else if (currentParityBit == "Even")//żУ��
		return QSerialPort::EvenParity;
}

qint32 communication::getBaud()
{
	bool ok;
	return ui->baudrate->currentText().toLong(&ok, 10);
}

enum QSerialPort::StopBits communication::getStopBits()
{
	QString currentStopBits = ui->stopbits->currentText();
	if (currentStopBits == "1")
		return QSerialPort::OneStop;
	else if (currentStopBits == "1.5")
		return QSerialPort::OneAndHalfStop;
	else if (currentStopBits == "2")
		return QSerialPort::TwoStop;
}

enum QSerialPort::DataBits communication::getDataBits()
{
	QString currentDataBits = ui->databits->currentText();
	if (currentDataBits == "5")
		return QSerialPort::Data5;
	else if (currentDataBits == "6")
		return QSerialPort::Data6;
	else if (currentDataBits == "7")
		return QSerialPort::Data7;
	else if (currentDataBits == "8")
		return QSerialPort::Data8;
}

QString communication::getName()
{
	int index = ui->serial_name->currentText().indexOf(' ');
	QString com_num = ui->serial_name->currentText().left(index);
	return com_num;
}


void communication::refreshPorts()
{
    QStringList newPorts;
	QStringList m_currentPorts;
    for (const auto& port : QSerialPortInfo::availablePorts()) {
        newPorts << QString(port.portName() + " " + port.description());
    }

    // �����˿ڱ仯ʱ����UI
    if (newPorts != m_currentPorts) {
        m_currentPorts = newPorts;
        ui->serial_name->clear();
        ui->serial_name->addItems(m_currentPorts);
    }
}

void communication::handleOpenSerial()
{
	if (ui->serial_switch->text() == (QString::fromLocal8Bit("�򿪴���"))){
		if (!serialPort->open(getName(), getBaud(), getParity(), getDataBits(), getStopBits())){
			//��׼�Ի�����ʾ�򿪴���ʧ��
			QMessageBox::warning(this, (QString::fromLocal8Bit("����")), (QString::fromLocal8Bit("�򿪴���ʧ��")));
			return;
		}
		else{
			ui->serial_switch->setText(QString::fromLocal8Bit("�رմ���"));
			serialPortClocked(false);
			isBaudRateExist();
		}
	}
	else{
		serialPort->clear();
		serialPort->close();
		serialPortClocked(true);

		ui->serial_switch->setText(QString::fromLocal8Bit("�򿪴���"));
	}
}

void communication::serialPortClocked(bool flag)
{
	ui->serial_name->setEnabled(flag);
	ui->baudrate->setEnabled(flag);
	ui->parity->setEnabled(flag);
	ui->databits->setEnabled(flag);
	ui->stopbits->setEnabled(flag);
}

void communication::isBaudRateExist()
{
	bool isFound = false;
	QString current_baud = ui->baudrate->currentText();
	for (int i = 0; i < ui->baudrate->count(); i++) {
		if (ui->baudrate->itemText(i) == current_baud) {
			isFound = true;
			break;
		}
	}
	if (isFound == false) {
		ui->baudrate->addItem(current_baud);
	}
}


void communication::initSerial()
{
    //��ʼ����ʱ����ÿ��ɨ��һ�δ����豸
    m_timer = new QTimer(this);
    m_timer->start(1000);

	//��ʼ�����������
	handleLoadINI();
	ui->baudrate->setCurrentText("9600");
	//��ʼ��ֹͣλ Ĭ��1λ
	QStringList mystopBits;
	mystopBits.clear();
	mystopBits << "1" << "1.5" << "2";
	ui->stopbits->addItems(mystopBits);
	ui->stopbits->setCurrentText("1");
	//��ʼ��У��λ Ĭ����У��
	QStringList mycheckBits;
	mycheckBits.clear();
	mycheckBits << "None" << "Even" << "Odd";
	ui->parity->addItems(mycheckBits);
	ui->parity->setCurrentText("None");
	//��ʼ������λ Ĭ��8λ
	QStringList mydataBits;
	mydataBits.clear();
	mydataBits << "5" << "6" << "7" << "8";
	ui->databits->addItems(mydataBits);
	ui->databits->setCurrentText("8");
	//�����ʾ��
	ui->send_data_display->clear();
	ui->log_display->clear();

	ui->hex_Button->setChecked(true);//����Ĭ����16������ʾ
}
