#include "communication_ui.h"
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

    initSerial();
	initNetwork();

	m_manager = new CommunicationManager(this);
	connect(m_serialPort, &ICommunication::dataReceived, m_manager, &CommunicationManager::onDataReceived);//接收数据
	connect(m_manager, &CommunicationManager::VFRecieve,this,&communication::test);
	//串口
	//connect(m_serialPort, &SerialPort::dataReceived, this, &communication::handleRecvData);//接收数据
    connect(m_timer, &QTimer::timeout, this, &communication::refreshPorts);//定时刷新设备
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
	connect(m_tcp, &Tcp::dataReceived, this, &communication::onDataReceived);
	connect(m_tcp, &Tcp::connectStatus, this, &communication::onConnectedResult);
}


communication::~communication()
{
	saveINI();
	delete ui;
}


void communication::test(VFProtocol::VFModuleFrame data) 
{
	ui->log_display->append("RECV:");
	ui->log_display->append("VFStruct.frameHead=" + QString::number(data.frameHead,16));
	ui->log_display->append("VFStruct.cmd=" + QString::number(data.cmd, 16));
	ui->log_display->append("VFStruct.frameLength=" + QString::number(data.frameLength, 16));
	ui->log_display->append("VFStruct.data=" + data.data.toHex());
}

// 连接服务器
void communication::onConnectClicked()
{
	if (!m_tcp->isConnected()) {
		// 获取IP和端口
		QVariantList paraList;
		paraList.append(ui->lineEditIP->text());
		paraList.append(ui->lineEditPort->text().toInt());

		// 验证输入
		if (ui->lineEditIP->text().isEmpty()) {
			QMessageBox::warning(this, QStringLiteral("输入错误"), QStringLiteral("请输入服务器IP地址"));
			return;
		}

		if (ui->lineEditPort->text().toInt() < 1 || ui->lineEditPort->text().toInt() > 65535) {
			QMessageBox::warning(this, QStringLiteral("端口错误"), QStringLiteral("端口号必须在1-65535之间"));
			return;
		}

		// 尝试连接服务器
		m_tcp->portConnect(paraList);
		ui->textEditLog->append(QStringLiteral("正在连接服务器: ") + ui->lineEditIP->text() + ":" + QString::number(ui->lineEditPort->text().toInt()));
		ui->pushButtonConnect->setEnabled(false); // 禁用连接按钮直到连接完成
	}
}

// 发送数据
void communication::onSendClicked()
{
	if (m_tcp->isConnected()) {
		QString data = ui->lineEditTextToSend->text();
		if (!data.isEmpty()) {
			// 发送数据到STM32
			QByteArray byteArray = data.toUtf8();
			m_tcp->write(byteArray);

			// 显示发送日志
			ui->textEditLog->append(QStringLiteral("[发送] 文本: ") + data);
			ui->textEditLog->append(QStringLiteral("[发送] 十六进制: ") + byteArray.toHex(' ').toUpper());
		}
	}
	else {
		ui->textEditLog->append(QStringLiteral("未连接服务器，无法发送数据"));
	}
}

// 断开连接
void communication::onDisconnectClicked()
{
	if (m_tcp->isConnected()) {
		m_tcp->disconnect();
		ui->textEditLog->append(QStringLiteral("已断开服务器连接"));
	}
}

// 接收数据
void communication::onDataReceived(const QByteArray& data)
{
	// 显示十六进制格式
	QString hexData = data.toHex(' ').toUpper();
	ui->textEditLog->append(QStringLiteral("[接收] 十六进制: ") + hexData);

	// 尝试显示文本（如果是文本数据）
	QString textData = QString::fromUtf8(data);
	if (!textData.isEmpty() && textData.toUtf8() == data) {
		ui->textEditLog->append(QStringLiteral("[接收] 文本: ") + textData);
	}
}

// 错误处理
void communication::onConnectedResult(bool result,const QString& errorInfo)
{
	if (!result) {
		if (errorInfo == "disconnected") {
			// 更新UI状态
			ui->pushButtonConnect->setText(QStringLiteral("连接服务器"));
			ui->pushButtonConnect->setEnabled(true);
			ui->pushButtonSend->setEnabled(false);
			ui->pushButtonDisConnect->setEnabled(false);
			ui->lineEditIP->setEnabled(true);
			ui->lineEditPort->setEnabled(true);

			// 显示断开信息
			ui->textEditLog->append(QStringLiteral("与服务器的连接已断开"));
		}
		else {
			ui->textEditLog->append(QStringLiteral("发生错误: ") + errorInfo);

			// 重置连接状态
			ui->pushButtonConnect->setText(QStringLiteral("连接服务器"));
			ui->pushButtonConnect->setEnabled(true);
			ui->pushButtonSend->setEnabled(false);
			ui->pushButtonDisConnect->setEnabled(false);
			ui->lineEditIP->setEnabled(true);
			ui->lineEditPort->setEnabled(true);
		}
	}
	else{
		// 更新UI状态
		ui->pushButtonConnect->setText(QStringLiteral("已连接"));
		ui->pushButtonConnect->setEnabled(true);
		ui->pushButtonSend->setEnabled(true);
		ui->pushButtonDisConnect->setEnabled(true);
		ui->lineEditIP->setEnabled(false);
		ui->lineEditPort->setEnabled(false);

		// 显示连接信息
		ui->textEditLog->append(QStringLiteral("连接服务器成功!"));
	}
}

void communication::initNetwork() 
{
	m_tcp = new Tcp(this);

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
	if (!m_serialPort->isConnected()){
		QMessageBox::information(this, (QString::fromLocal8Bit("提示")), (QString::fromLocal8Bit("请先打开串口")));
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
		send_byte_size = m_serialPort->write(write_buff);
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


void communication::refreshPorts()
{
    QStringList newPorts;
	static QStringList lastPorts;
	static QString usedPort;

    for (const auto& port : QSerialPortInfo::availablePorts()) {
        newPorts << QString(port.portName() + " " + port.description());
    }

    // 仅当端口变化时更新UI
    if (newPorts != lastPorts) {
		lastPorts = newPorts;
        ui->serial_name->clear();
        ui->serial_name->addItems(lastPorts);
    }

	if (ui->serial_switch->text() == (QString::fromLocal8Bit("关闭串口"))&& (ui->serial_name->currentText()!= usedPort)) {
		QMessageBox::warning(this, (QString::fromLocal8Bit("警告")), (QString::fromLocal8Bit("串口设备已断开")));
		handleOpenSerial();
	}
	else {
		usedPort = ui->serial_name->currentText();
	}
}


void communication::handleOpenSerial()
{
	if (ui->serial_switch->text() == (QString::fromLocal8Bit("打开串口"))){
		m_serialPort->portConnect(getParaList());
	}
	else{
		m_serialPort->disconnect();
		serialPortClocked(true);

		ui->serial_switch->setText(QString::fromLocal8Bit("打开串口"));
	}
}

void communication::handleOpenSerialResult(bool result,const QString& errStr)
{
	if (!result) {
		QMessageBox::warning(this, (QString::fromLocal8Bit("警告")), (QString::fromLocal8Bit("打开串口失败")));
	}
	else {
		ui->serial_switch->setText(QString::fromLocal8Bit("关闭串口"));
		serialPortClocked(false);
		isBaudRateExist();
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
	m_serialPort = new SerialPort(this);
	m_settings = new QSettings("config.ini", QSettings::IniFormat, this);
	m_tx_num = 0;
	m_rx_num = 0;

    //初始化定时器，每秒扫描一次串口设备
    m_timer = new QTimer(this);
    m_timer->start(1000);

	//初始化波特率组件
	handleLoadINI();
	ui->baudrate->setCurrentText("9600");
	//初始化停止位 默认1位
	QStringList mystopBits;
	mystopBits.clear();
	mystopBits << "1" << "1.5" << "2";
	ui->stopbits->addItems(mystopBits);
	ui->stopbits->setCurrentText("1");
	//初始化校验位 默认无校验
	QStringList mycheckBits;
	mycheckBits.clear();
	mycheckBits << "None" << "Even" << "Odd";
	ui->parity->addItems(mycheckBits);
	ui->parity->setCurrentText("None");
	//初始化数据位 默认8位
	QStringList mydataBits;
	mydataBits.clear();
	mydataBits << "5" << "6" << "7" << "8";
	ui->databits->addItems(mydataBits);
	ui->databits->setCurrentText("8");
	//清空显示区
	ui->send_data_display->clear();
	ui->log_display->clear();

	ui->hex_Button->setChecked(true);//设置默认是16进制显示
}


enum QSerialPort::Parity communication::getParity() {
	QString currentParityBit = ui->parity->currentText();
	if (currentParityBit == "None")
		return QSerialPort::NoParity;
	else if (currentParityBit == "Odd")//奇数
		return QSerialPort::OddParity;
	else if (currentParityBit == "Even")//偶校验
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

QVariantList communication::getParaList()
{
	QVariantList paraList;
	paraList.append(getName());
	paraList.append(getBaud());
	paraList.append(getDataBits());
	paraList.append(getParity());
	paraList.append(getStopBits());
	return paraList;
}
