#include "communication_ui.h"
#include "ui_communication_module.h"
#include "SerialConfigDialog.h"
#include "TcpConfigDialog.h"
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
	: QMainWindow(parent), ui(new Ui::CommunicationClass), m_tcpConnectionState(Disconnected) // 初始状态为断开
{
	ui->setupUi(this);
	// 初始化默认配置
	m_serialBaudRate = 9600;
	m_serialDataBits = QSerialPort::Data8;
	m_serialParity = QSerialPort::NoParity;
	m_serialStopBits = QSerialPort::OneStop;
	m_serialHexDisplay = true;

	m_tcpIp = "192.168.1.100";
	m_tcpPort = 8080;
	m_tcpHexDisplay = true;


	// 创建配置对话框
	m_serialConfigDialog = new SerialConfigDialog(this);
	m_tcpConfigDialog = new TcpConfigDialog(this);

	initSerial();
	initNetwork();



	m_manager = new CommunicationManager(this);
	connect(m_serialPort, &ICommunication::dataReceived, m_manager, &CommunicationManager::onDataReceived);//接收数据
	connect(m_manager, &CommunicationManager::VFRecieve, this, &communication::test);

	//串口
	//connect(m_timer, &QTimer::timeout, this, &communication::refreshPorts);//定时刷新设备
	connect(ui->serial_switch, &QPushButton::clicked, this, &communication::handleOpenSerial);//开关串口
	connect(ui->send_button, &QPushButton::clicked, this, &communication::handleSendData);//发送数据
	connect(ui->rxtx_clear, &QPushButton::clicked, this, &communication::handleClearTxRx);//清空计数
	connect(ui->log_clear, &QPushButton::clicked, this, &communication::handleClearLog);//清空日志区
	connect(ui->tx_data_clear, &QPushButton::clicked, this, &communication::handleClearTxData);//清空发送数据区
	connect(m_serialPort, &SerialPort::connectStatus, this, &communication::handleOpenSerialResult);

	//网口
	connect(ui->pushButtonConnect, &QPushButton::clicked, this, &communication::onConnectClicked);
	connect(ui->pushButtonSend, &QPushButton::clicked, this, &communication::onSendClicked);
	connect(ui->pushButtonDisConnect, &QPushButton::clicked, this, &communication::onDisconnectClicked);
	connect(m_tcp, &Tcp::connectStatus, this, &communication::onConnectedResult);
	connect(ui->log_clear_tcp, &QPushButton::clicked, this, &communication::handleTcpClearLog);//清空日志区
	connect(ui->tx_data_clear_tcp, &QPushButton::clicked, this, &communication::handleTcpClearTxData);//清空发送数据区

	// 使用中间槽函数而不是lambda表达式
	connect(m_serialPort, &ICommunication::dataReceived, this, &communication::handleSerialDataReceived);
	connect(m_tcp, &Tcp::dataReceived, this, &communication::handleTcpDataReceived);

	//配置按钮的连接
	connect(ui->serialConfigButton, &QPushButton::clicked, this, &communication::onSerialConfigClicked);
	connect(ui->tcpConfigButton, &QPushButton::clicked, this, &communication::onTcpConfigClicked);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}


communication::~communication()
{

	delete ui;
}


void communication::test(VFProtocol::VFModuleFrame data)
{
	ui->log_display->append("RECV:");
	ui->log_display->append("VFStruct.frameHead=" + QString::number(data.frameHead, 16));
	ui->log_display->append("VFStruct.cmd=" + QString::number(data.cmd, 16));
	ui->log_display->append("VFStruct.frameLength=" + QString::number(data.frameLength, 16));
	ui->log_display->append("VFStruct.data=" + data.data.toHex());
}

void communication::onSerialConfigClicked()
{
	m_serialConfigDialog->setCurrentConfig(m_serialPortName, m_serialBaudRate,
		m_serialDataBits, m_serialParity,
		m_serialStopBits, m_serialHexDisplay);

	if (m_serialConfigDialog->exec() == QDialog::Accepted) {
		m_serialPortName = m_serialConfigDialog->portName();
		m_serialBaudRate = m_serialConfigDialog->baudRate();
		m_serialDataBits = m_serialConfigDialog->dataBits();
		m_serialParity = m_serialConfigDialog->parity();
		m_serialStopBits = m_serialConfigDialog->stopBits();
		m_serialHexDisplay = m_serialConfigDialog->hexDisplay();
	}
}

void communication::onTcpConfigClicked()
{
	m_tcpConfigDialog->setCurrentConfig(m_tcpIp, m_tcpPort, m_tcpHexDisplay);

	if (m_tcpConfigDialog->exec() == QDialog::Accepted) {
		m_tcpIp = m_tcpConfigDialog->ipAddress();
		m_tcpPort = m_tcpConfigDialog->port();
		m_tcpHexDisplay = m_tcpConfigDialog->hexDisplay();
	}
}

// 连接服务器
void communication::onConnectClicked()
{
	Tcp* tcp = qobject_cast<Tcp*>(m_tcp);
	if (m_tcp->isConnected() || (tcp && tcp->isConnecting())) {
		return;
	}
	if (!m_tcp->isConnected()) {
		// 验证输入
		if (m_tcpIp.isEmpty()) {
			QMessageBox::warning(this, QStringLiteral("输入错误"), QStringLiteral("请输入服务器IP地址"));
			return;
		}

		if (m_tcpPort < 1 || m_tcpPort > 65535) {
			QMessageBox::warning(this, QStringLiteral("端口错误"), QStringLiteral("端口号必须在1-65535之间"));
			return;
		}

		// 更新连接状态
		m_tcpConnectionState = Connecting;

		// 尝试连接服务器
		m_tcp->portConnect(getTcpParaList());
		ui->textEditLog->append(QStringLiteral("正在连接服务器: ") + m_tcpIp + ":" + QString::number(m_tcpPort));
		ui->pushButtonConnect->setEnabled(false); // 禁用连接按钮直到连接完成
		ui->pushButtonDisConnect->setEnabled(true);
		ui->pushButtonDisConnect->setText(QStringLiteral("取消连接"));
	}
}

// 发送数据
void communication::onSendClicked()
{
	if (m_tcp->isConnected()) {
		QString data = ui->lineEditTextToSend->text();
		if (!data.isEmpty()) {
			QByteArray byteArray;
			if (m_tcpHexDisplay) {
				// 十六进制发送
				QStringList hexList = data.split(' ', Qt::SkipEmptyParts);
				for (const QString& hexStr : hexList) {
					bool ok;
					quint8 byte = hexStr.toUInt(&ok, 16);
					if (ok) {
						byteArray.append(byte);
					}
				}
				m_tcp->write(byteArray);
				ui->textEditLog->append(QStringLiteral("[发送] 十六进制: ") + byteArray.toHex(' ').toUpper());
			}
			else {
				// ASCII发送
				byteArray = data.toUtf8();
				m_tcp->write(byteArray);
				ui->textEditLog->append(QStringLiteral("[发送] 文本: ") + data);
			}
		}
	}
	else {
		ui->textEditLog->append(QStringLiteral("未连接服务器，无法发送数据"));
	}
}

// 断开连接
void communication::onDisconnectClicked()
{
	Tcp* tcp = qobject_cast<Tcp*>(m_tcp);
	if (!tcp) return;

	// 根据当前连接状态执行不同的断开操作
	if (m_tcpConnectionState == Connecting) {
		// 正在连接中，中止连接尝试
		tcp->abortConnection();
		ui->textEditLog->append(QStringLiteral("连接已取消"));
	}
	else if (m_tcpConnectionState == Connected) {
		// 已连接，正常断开
		tcp->disconnect();
		ui->textEditLog->append(QStringLiteral("连接已断开"));
	}

	// 更新UI状态
	ui->pushButtonConnect->setEnabled(true);
	ui->pushButtonDisConnect->setEnabled(false);
	ui->pushButtonDisConnect->setText(QStringLiteral("断开连接"));

	// 更新连接状态
	m_tcpConnectionState = Disconnected;
}

// 接收数据
void communication::onDataReceived(const QByteArray& data, bool hexDisplay)
{
	if (hexDisplay)
	{
		// 显示十六进制格式
		QString hexData = data.toHex(' ').toUpper();
		ui->textEditLog->append(QStringLiteral("[接收] 十六进制: ") + hexData);
	}
	else
	{
		// 尝试显示文本（如果是文本数据）
		QString textData = QString::fromUtf8(data);
		ui->textEditLog->append(QStringLiteral("[接收] 文本: ") + textData);
	}
}

// 错误处理
void communication::onConnectedResult(bool result, const QString& errorInfo)
{
	if (result) {
		m_tcpConnectionState = Connected;
		ui->pushButtonConnect->setText(QStringLiteral("已连接"));
		ui->pushButtonSend->setEnabled(true);
		ui->textEditLog->append(QStringLiteral("连接服务器成功!"));
		ui->pushButtonDisConnect->setEnabled(true);
		ui->pushButtonDisConnect->setText(QStringLiteral("断开连接"));
	}
	else {
		m_tcpConnectionState = Disconnected;


		if (!errorInfo.isEmpty()) {
			QString displayError;


			if (errorInfo == "disconnected" ||
				errorInfo == "Connection timed out" ||
				errorInfo == "连接已取消" ||
				errorInfo == "手动断开") {
				if (errorInfo == "disconnected") {
					displayError = QStringLiteral("连接已断开");
				}
				else if (errorInfo == "Connection timed out") {
					displayError = QStringLiteral("连接超时");
				}
				else {
					displayError = errorInfo;
				}
			}
			else {

				displayError = errorInfo;
				if (displayError.contains(QRegExp("[^\\x00-\\x7F]")) &&
					(displayError.contains("?") || displayError.contains("�"))) {
					QTextCodec* codec = QTextCodec::codecForLocale();
					QByteArray encodedString = errorInfo.toLocal8Bit();
					displayError = codec->toUnicode(encodedString);
					if (displayError.isEmpty() || displayError.contains("?") || displayError.contains("�")) {
						displayError = QStringLiteral("连接错误");
					}
				}
			}

			ui->textEditLog->append(QStringLiteral("连接错误: ") + displayError);
		}

		ui->pushButtonConnect->setText(QStringLiteral("连接服务器"));
		ui->pushButtonSend->setEnabled(false);
		ui->pushButtonDisConnect->setEnabled(false);
		ui->pushButtonDisConnect->setText(QStringLiteral("断开连接"));
	}

	ui->pushButtonConnect->setEnabled(true);
}


void communication::initNetwork()
{
	m_tcp = new Tcp(this);
}

void communication::handleTcpClearLog()
{
	ui->textEditLog->clear();
}
void communication::handleTcpClearTxData()
{
	ui->lineEditTextToSend->clear();
}

//0821注释将其移到配置对话框实现文件中
//void communication::handleLoadINI()
//{
//	QStringList defaultRates = { "1200" , "2400" , "4800" , "9600" , "19200" , "38400" , "57600" , "115200" , "230400" , "345600" , "460800" , "576000" , "921600" , "1382400" };
//	QStringList baudRates = m_settings->value("BaudRates/myBaud", defaultRates).toStringList();
//
//	ui->baudrate->addItems(baudRates);
//}

//void communication::saveINI()
//{
//	QSettings settings("config.ini", QSettings::IniFormat);
//	QCollator collator;
//	QStringList ratesList;
//
//	for (int i = 0; i < ui->baudrate->count(); i++) {
//		ratesList << ui->baudrate->itemText(i);
//	}
//	collator.setNumericMode(true);
//	std::sort(ratesList.begin(), ratesList.end(), collator);
//	m_settings->setValue("BaudRates/myBaud", ratesList);
//}

void communication::handleClearTxRx()
{
	ui->tx_display->clear();
	ui->rx_display->clear();
	m_rx_num = 0;
	m_tx_num = 0;
}


void communication::handleRecvData(const QByteArray& data, bool hexDisplay)
{
	if (!data.isEmpty()) {
		QString result;
		if (hexDisplay) {
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
		append_str += "  SEND";
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
	if (!m_serialPort->isConnected()) {
		QMessageBox::information(this, (QString::fromLocal8Bit("提示")), (QString::fromLocal8Bit("请先打开串口")));
	}
	else {
		if (m_serialHexDisplay) {
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
		else {
			write_buff.append(data_buff.toUtf8());
			displayStr = data_buff;
		}
		send_byte_size = m_serialPort->write(write_buff);
		m_tx_num += send_byte_size;
		ui->tx_display->setText("tx:" + QString::number(m_tx_num));
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


//void communication::refreshPorts()
//{
//	QStringList newPorts;
//	static QStringList lastPorts;
//	static QString usedPort;
//
//	for (const auto& port : QSerialPortInfo::availablePorts()) {
//		newPorts << QString(port.portName() + " " + port.description());
//	}
//
//	// 仅当端口变化时更新UI
//	if (newPorts != lastPorts) {
//		lastPorts = newPorts;
//		ui->serial_name->clear();
//		ui->serial_name->addItems(lastPorts);
//	}
//
//	if (ui->serial_switch->text() == (QString::fromLocal8Bit("关闭串口")) && (ui->serial_name->currentText() != usedPort)) {
//		QMessageBox::warning(this, (QString::fromLocal8Bit("警告")), (QString::fromLocal8Bit("串口设备已断开")));
//		handleOpenSerial();
//	}
//	else {
//		usedPort = ui->serial_name->currentText();
//	}
//}


void communication::handleOpenSerial()
{
	if (ui->serial_switch->text() == (QString::fromLocal8Bit("打开串口"))) {
		ui->serialConfigButton->setEnabled(false);
		m_serialPort->portConnect(getSerialParaList());
	}
	else {
		m_serialPort->disconnect();
		ui->serialConfigButton->setEnabled(true);
		ui->serial_switch->setText(QString::fromLocal8Bit("打开串口"));
	}
}

void communication::handleOpenSerialResult(bool result, const QString& errStr)
{
	if (!result) {
		QMessageBox::warning(this, (QString::fromLocal8Bit("警告")), (QString::fromLocal8Bit("打开串口失败")));
		ui->serialConfigButton->setEnabled(true);
	}
	else {
		ui->serial_switch->setText(QString::fromLocal8Bit("关闭串口"));
		ui->serialConfigButton->setEnabled(false);
	}
}

//void communication::serialPortClocked(bool flag)
//{
//	ui->serial_name->setEnabled(flag);
//	ui->baudrate->setEnabled(flag);
//	ui->parity->setEnabled(flag);
//	ui->databits->setEnabled(flag);
//	ui->stopbits->setEnabled(flag);
//}

//void communication::isBaudRateExist()
//{
//	bool isFound = false;
//	QString current_baud = ui->baudrate->currentText();
//	for (int i = 0; i < ui->baudrate->count(); i++) {
//		if (ui->baudrate->itemText(i) == current_baud) {
//			isFound = true;
//			break;
//		}
//	}
//	if (isFound == false) {
//		ui->baudrate->addItem(current_baud);
//	}
//}


void communication::initSerial()
{
	m_serialPort = new SerialPort(this);
	m_settings = new QSettings("config.ini", QSettings::IniFormat, this);
	m_tx_num = 0;
	m_rx_num = 0;

	// 初始化定时器，每秒扫描一次串口设备
	//m_timer = new QTimer(this);
	//m_timer->start(1000);

	// 清空显示区
	ui->send_data_display->clear();
	ui->log_display->clear();
}


//enum QSerialPort::Parity communication::getParity() {
//	QString currentParityBit = ui->parity->currentText();
//	if (currentParityBit == "None")
//		return QSerialPort::NoParity;
//	else if (currentParityBit == "Odd")//奇数
//		return QSerialPort::OddParity;
//	else if (currentParityBit == "Even")//偶校验
//		return QSerialPort::EvenParity;
//}

//qint32 communication::getBaud()
//{
//	bool ok;
//	return ui->baudrate->currentText().toLong(&ok, 10);
//}

//enum QSerialPort::StopBits communication::getStopBits()
//{
//	QString currentStopBits = ui->stopbits->currentText();
//	if (currentStopBits == "1")
//		return QSerialPort::OneStop;
//	else if (currentStopBits == "1.5")
//		return QSerialPort::OneAndHalfStop;
//	else if (currentStopBits == "2")
//		return QSerialPort::TwoStop;
//}

//enum QSerialPort::DataBits communication::getDataBits()
//{
//	QString currentDataBits = ui->databits->currentText();
//	if (currentDataBits == "5")
//		return QSerialPort::Data5;
//	else if (currentDataBits == "6")
//		return QSerialPort::Data6;
//	else if (currentDataBits == "7")
//		return QSerialPort::Data7;
//	else if (currentDataBits == "8")
//		return QSerialPort::Data8;
//}

//QString communication::getName()
//{
//	int index = ui->serial_name->currentText().indexOf(' ');
//	QString com_num = ui->serial_name->currentText().left(index);
//	return com_num;
//}

QVariantList communication::getSerialParaList()
{
	QVariantList paraList;
	paraList.append(m_serialPortName);
	paraList.append(m_serialBaudRate);
	paraList.append(m_serialDataBits);
	paraList.append(m_serialParity);
	paraList.append(m_serialStopBits);
	return paraList;
}

QVariantList communication::getTcpParaList()
{
	QVariantList paraList;
	paraList.append(m_tcpIp);
	paraList.append(m_tcpPort);
	return paraList;
}

void communication::handleSerialDataReceived(const QByteArray& data)
{
	handleRecvData(data, m_serialHexDisplay);
}

void communication::handleTcpDataReceived(const QByteArray& data)
{
	onDataReceived(data, m_tcpHexDisplay);
}