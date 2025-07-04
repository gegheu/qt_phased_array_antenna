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

	m_timer = new QTimer(this);
	m_serialport = new QSerialPort(this);
	m_settings = new QSettings("config.ini", QSettings::IniFormat, this);

	m_tcpSocket = new QTcpSocket(this);

	m_tx_num = 0;
	m_rx_num = 0;

    serial_init();
	network_init();
    
	//����
    connect(m_timer, &QTimer::timeout, this, &communication::refreshPorts);//��ʱˢ���豸
	connect(ui->serial_switch, &QPushButton::clicked, this, &communication::handleOpenSerial);//���ش���
	connect(ui->send_button, &QPushButton::clicked, this, &communication::handleSendData);//��������
	connect(m_serialport, &QSerialPort::readyRead, this, &communication::handleRecvData);//��������
	connect(ui->rxtx_clear, &QPushButton::clicked, this, &communication::handleClearTxRx);//��ռ���
	connect(ui->log_clear, &QPushButton::clicked, this, &communication::handleClearLog);//�����־��
	connect(ui->tx_data_clear, &QPushButton::clicked, this, &communication::handleClearTxData);//��շ���������

	//����
	connect(ui->pushButtonConnect, &QPushButton::clicked, this, &communication::onConnectClicked);
	connect(ui->pushButtonSend, &QPushButton::clicked, this, &communication::onSendClicked);
	connect(ui->pushButtonDisConnect, &QPushButton::clicked, this, &communication::onDisconnectClicked);
	connect(m_tcpSocket, &QTcpSocket::connected, this, &communication::onConnected);
	connect(m_tcpSocket, &QTcpSocket::disconnected, this, &communication::onDisconnected);
	connect(m_tcpSocket, &QTcpSocket::readyRead, this, &communication::onDataReceived);
	connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
		this, &communication::onErrorOccurred);
}


communication::~communication()
{
	handleSaveINI();
	delete ui;
}

// ���ӷ�����
void communication::onConnectClicked()
{
	if (!m_isConnected) {
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
		m_tcpSocket->connectToHost(ip, port);
		ui->textEditLog->append(QStringLiteral("�������ӷ�����: ") + ip + ":" + QString::number(port));
		ui->pushButtonConnect->setEnabled(false); // �������Ӱ�ťֱ���������
	}
}

// ��������
void communication::onSendClicked()
{
	if (m_isConnected && m_tcpSocket->state() == QTcpSocket::ConnectedState) {
		QString data = ui->lineEditTextToSend->text();
		if (!data.isEmpty()) {
			// �������ݵ�STM32
			QByteArray byteArray = data.toUtf8();
			m_tcpSocket->write(byteArray);

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
	if (m_isConnected) {
		m_tcpSocket->disconnectFromHost();
		ui->textEditLog->append(QStringLiteral("�ѶϿ�����������"));
	}
}

// ���ӳɹ�
void communication::onConnected()
{
	m_isConnected = true;

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
	m_isConnected = false;

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
void communication::onDataReceived()
{
	if (m_tcpSocket->bytesAvailable() > 0) {
		// ��ȡ��������
		QByteArray data = m_tcpSocket->readAll();

		// ��ʾʮ�����Ƹ�ʽ
		QString hexData = data.toHex(' ').toUpper();
		ui->textEditLog->append(QStringLiteral("[����] ʮ������: ") + hexData);

		// ������ʾ�ı���������ı����ݣ�
		QString textData = QString::fromUtf8(data);
		if (!textData.isEmpty() && textData.toUtf8() == data) {
			ui->textEditLog->append(QStringLiteral("[����] �ı�: ") + textData);
		}
	}
}

// ������
void communication::onErrorOccurred(QAbstractSocket::SocketError error)
{
	Q_UNUSED(error);

	ui->textEditLog->append(QStringLiteral("��������: ") + m_tcpSocket->errorString());

	// ��������״̬
	m_isConnected = false;
	ui->pushButtonConnect->setText(QStringLiteral("���ӷ�����"));
	ui->pushButtonConnect->setEnabled(true);
	ui->pushButtonSend->setEnabled(false);
	ui->pushButtonDisConnect->setEnabled(false);
	ui->lineEditIP->setEnabled(true);
	ui->lineEditPort->setEnabled(true);
}

void communication::network_init() 
{
	m_isConnected = false;
	ui->lineEditIP->setText("192.168.1.100");
	ui->lineEditPort->setText("8080");
}


void communication::handleLoadINI()
{
	QStringList defaultRates = { "1200" , "2400" , "4800" , "9600" , "19200" , "38400" , "57600" , "115200" , "230400" , "345600" , "460800" , "576000" , "921600" , "1382400" };
	QStringList baudRates = m_settings->value("BaudRates/myBaud", defaultRates).toStringList();
	
	ui->baudrate->addItems(baudRates);
}

void communication::handleSaveINI()
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


void communication::handleRecvData()
{
	QByteArray read_arr = m_serialport->readAll();
	if (!read_arr.isEmpty()) {
		QString result;
		if (ui->hex_Button->isChecked()) {
			for (quint8 byte : read_arr) {
				result += QString("%1 ")
					.arg(byte, 2, 16, QLatin1Char('0'))
					.toUpper();
			}
		}
		else {
			result = QString(read_arr);
		}
		m_rx_num += read_arr.size();
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
	if (!m_serialport->isOpen()){
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
		send_byte_size = m_serialport->write(write_buff);
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

/*   
enum QSerialPort::BaudRate MainWindow::getBaud()
{
	bool ok;
	enum QSerialPort::BaudRate ret;
	switch (ui->baudrate->currentText().toLong(&ok, 10))
	{
	case 1200:		ret = QSerialPort::Baud1200;		break;
	case 2400:		ret = QSerialPort::Baud2400;		break;
	case 4800:		ret = QSerialPort::Baud4800;		break;
	case 9600:		ret = QSerialPort::Baud9600;		break;
	case 19200:		ret = QSerialPort::Baud19200;		break;
	case 38400:		ret = QSerialPort::Baud38400;		break;
	case 57600:		ret = QSerialPort::Baud57600;		break;
	case 115200:	ret = QSerialPort::Baud115200;		break;
	case 230400:	ret = QSerialPort::Baud230400;		break;
	case 345600:	ret = QSerialPort::Baud345600;		break;
	case 460800:	ret = QSerialPort::Baud460800;		break;
	case 576000:	ret = QSerialPort::Baud576000;		break;
	case 921600:	ret = QSerialPort::Baud921600;		break;
	case 1382400:	ret = QSerialPort::Baud1382400;		break;
	default:		ret = QSerialPort::Baud9600;
	}
	return ret;
}
*/
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
		m_serialport->setPortName(getName());
		m_serialport->setBaudRate(getBaud());
		m_serialport->setParity(getParity());
		m_serialport->setDataBits(getDataBits());
		m_serialport->setStopBits(getStopBits());

		if (!(m_serialport->open(QSerialPort::ReadWrite))){
			//��׼�Ի�����ʾ�򿪴���ʧ��
			QMessageBox::warning(this, (QString::fromLocal8Bit("����")), (QString::fromLocal8Bit("�򿪴���ʧ��")));
			return;
		}
		else{
			ui->serial_switch->setText(QString::fromLocal8Bit("�رմ���"));
			ui->serial_name->setEnabled(false);
			ui->baudrate->setEnabled(false);
			ui->parity->setEnabled(false);
			ui->databits->setEnabled(false);
			ui->stopbits->setEnabled(false);

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
	}
	else{
		m_serialport->clear();
		m_serialport->close();
		ui->serial_name->setEnabled(true);
		ui->baudrate->setEnabled(true);
		ui->parity->setEnabled(true);
		ui->databits->setEnabled(true);
		ui->stopbits->setEnabled(true);

		ui->serial_switch->setText(QString::fromLocal8Bit("�򿪴���"));
	}
}

void communication::serial_init()
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
