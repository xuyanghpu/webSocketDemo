#include "webSocketDemo.h"
#include <QDebug>
#include "SocketConnection.h"
#include "DataFrame.h"
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextCodec>


webSocketDemo::webSocketDemo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.textBrowser->setReadOnly(true);

	ctrlist << "fault_state";     			//故障状态
	ctrlist << "radio_ctr_state";   		//接管状态
	ctrlist << "power_state";      			//电台开关机状态
	ctrlist << "detect_state";				//电台处于探测状态
	ctrlist << "link_state";				//电台处于建联状态
	ctrlist << "free_state";				//电台处于扫描状态
	ctrlist << "local_position";			//上报类型为本地电台位置信息
	ctrlist << "target_and_local_position";	//上报类型为目标电台与本地电台位置信息
	ctrlist << "rmt_call_in";				//上报类型为目标台本址
	ctrlist << "send_msg_flag";				//发送短报文结果标志
	ctrlist << "recv_msg_content";			//上报接收到的短报文内容
	ctrlist << "stop_detect";				//网关结束探测指令
	ctrlist << "environment_state";			//网关上报环境状态
	ctrlist << "gw_config_ctl";				//上报网关配置参数
	ctrlist << "register_info";				//请求的类型为注册信息
	ctrlist << "radio_power_ctl";			//电台开关机请求
	ctrlist << "start_detect_ctl";			//开始探测电台请求
	ctrlist << "stop_detect_ctl";			//停止探测电台请求
	ctrlist << "start_track_ctl";			//开始追踪请求
	ctrlist << "stop_track_ctl";			//结束追踪请求
	ctrlist << "sent_msg_ctl";				//请求类型为控制发送短报文
	ctrlist << "gw_config_ctl";				//网关配置
	ctrlist << "call_out";					//呼叫控制
	ctrlist << "ack";						//回复类型为ACK
	ctrlist << "detect_channel_snr";		//探测时的频率信息与信道质量信息
	ctrlist << "connect_ok_channel_snr";	//建联成功时的频率信息与信道质量信息
	ctrlist << "ack_sent_msg_ctl";			//回复类型为发送短报文的ACK
	ctrlist << "ack_call_out";			    //回复类型为呼叫控制的ACK
	ctrlist << "gateway_id";				//回复类型为网关ID信息

	QSettings * ini = new QSettings("config.ini", QSettings::IniFormat);
	
	ui.lineEdit_2->setText(ini->value("address/src").toString());
	ui.lineEdit_3->setText(ini->value("address/dst").toString());
	delete ini;

	model1 = new QStandardItemModel(this);
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableView->verticalHeader()->setDefaultSectionSize(22);
	ui.tableView->setModel(model1);

	qint16 rcount = model1->rowCount();
	model1->setHeaderData(0, Qt::Horizontal, QStringLiteral("Src本址"));
	model1->setHeaderData(1, Qt::Horizontal, QStringLiteral("Dst本址"));
	model1->setHeaderData(2, Qt::Horizontal, QStringLiteral("频率"));
	model1->setHeaderData(3, Qt::Horizontal, QStringLiteral("Tx"));
	model1->setHeaderData(4, Qt::Horizontal, QStringLiteral("Rx"));
	model1->setHeaderData(5, Qt::Horizontal, QStringLiteral("评分"));

	model2 = new QStandardItemModel(this);
	ui.tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_2->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableView_2->verticalHeader()->setDefaultSectionSize(22);
	ui.tableView_2->setModel(model2);

	qint16 rcount2 = model2->rowCount();
	model2->setHeaderData(0, Qt::Horizontal, QStringLiteral("Src本址"));
	model2->setHeaderData(1, Qt::Horizontal, QStringLiteral("Dst本址"));
	model2->setHeaderData(2, Qt::Horizontal, QStringLiteral("Rx频率"));
	model2->setHeaderData(3, Qt::Horizontal, QStringLiteral("Tx频率"));
	model2->setHeaderData(4, Qt::Horizontal, QStringLiteral("Rx"));
	model2->setHeaderData(5, Qt::Horizontal, QStringLiteral("Tx"));
	model2->setHeaderData(6, Qt::Horizontal, QStringLiteral("评分"));
	model2->setHeaderData(7, Qt::Horizontal, QStringLiteral("方式"));
	model2->setHeaderData(8, Qt::Horizontal, QStringLiteral("模式"));

}
webSocketDemo::~webSocketDemo()
{
	Close();
}


void webSocketDemo::gb2312ToUtf8(QString strGb2312)
{
	printf("@@@ gb2312 str is:%s\n", strGb2312.toLocal8Bit().data());
	QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
	QTextCodec* gb2312Codec = QTextCodec::codecForName("gb2312");

	QString strUnicode = gb2312Codec->toUnicode(strGb2312.toLocal8Bit().data());
	QByteArray ByteUtf8 = utf8Codec->fromUnicode(strUnicode);

	char *utf8code = ByteUtf8.data();
	printf("@@@ Utf8  strGb2312toUtf8:%s\n", utf8code);
}

///////////////////////////////////////////////////////////////////////

void webSocketDemo::utf8ToGb2312(char *strUtf8)
{
	printf("~~~~ utf8  str is:%s\n", strUtf8);
	QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
	QTextCodec* gb2312Codec = QTextCodec::codecForName("gb2312");

	QString strUnicode = utf8Codec->toUnicode(strUtf8);
	QByteArray ByteGb2312 = gb2312Codec->fromUnicode(strUnicode);

	strUtf8 = ByteGb2312.data();
	printf("~~~~~ gb2312	strUtf8toGb2312:%s\n", strUtf8);
}

QString webSocketDemo::utf8ToGb2312_new(char *strUtf8)
{
	printf("~~~~utf8  strUtf8:%s\n", strUtf8);
	QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
	QTextCodec* gb2312Codec = QTextCodec::codecForName("gb2312");

	QString strUnicode = utf8Codec->toUnicode(strUtf8);
	QByteArray ByteGb2312 = gb2312Codec->fromUnicode(strUnicode);

	strUtf8 = ByteGb2312.data();
	printf("~~~~~gb2312	strUtf8toGb2312:%s\n", strUtf8);
	return QString::fromLocal8Bit(strUtf8);//注意这里要fromLocal8Bit()
}


void webSocketDemo::on_lineEdit_2_textChanged(QString s)
{
	QSettings * ini = new QSettings("config.ini", QSettings::IniFormat);
	ini->setValue("address/src", QVariant(ui.lineEdit_2->text()));
	delete ini;
}

void webSocketDemo::on_lineEdit_3_textChanged(QString s)
{
	QSettings * ini = new QSettings("config.ini", QSettings::IniFormat);
	ini->setValue("address/dst",QVariant(ui.lineEdit_3->text()));
	delete ini;
}

void webSocketDemo::on_pushButton_clean_clicked()
{
	ui.textBrowser->setText("");
}

bool webSocketDemo::StartServer() {
	listener_.listen(QHostAddress::Any, 4141);
	connect(&listener_, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));

	qDebug() << QString("聊天服务器启动。监听地址：%1, 端口：%2").arg("0.0.0.0").arg(4141);
	qDebug() << QString("WebSocket服务器地址: ws://%1:%2/chat").arg("0.0.0.0").arg(4141);

	return true;
}

void webSocketDemo::Close() {
	QList<SocketConnection *>::iterator iter = connect_socket_list_.begin();
	for (; iter != connect_socket_list_.end(); ++iter) {
		(*iter)->GetSocket()->close();
	}

	connect_socket_list_.clear();
}

void webSocketDemo::OnNewConnection() {
	QTcpSocket* socket = listener_.nextPendingConnection();
	ui.textBrowser->moveCursor(QTextCursor::End);
 	ui.textBrowser->insertHtml("<br>");
 	ui.textBrowser->insertPlainText(QStringLiteral("客户端") + socket->peerAddress().toString() + QStringLiteral("连接成功"));
 	ui.textBrowser->insertHtml("<br>");
	

	SocketConnection* sc = new SocketConnection(NULL);
	sc->SetSocket(socket);

	connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), sc, SIGNAL(ConnectStateChanged(QAbstractSocket::SocketState)));
	connect(sc, SIGNAL(ConnectStateChanged(QAbstractSocket::SocketState)), this, SLOT(OnSocketStateChanged(QAbstractSocket::SocketState)));
	connect(socket, SIGNAL(readyRead()), sc, SLOT(OnReadyRead()));
	connect(sc, SIGNAL(RecvMessage(const QString&)), this, SLOT(OnRecvMessage(const QString&)));

	connect_socket_list_.push_back(sc);
}

void webSocketDemo::OnSocketStateChanged(QAbstractSocket::SocketState socketState) {
	if (socketState == QAbstractSocket::UnconnectedState) {
		SocketConnection* sc = static_cast<SocketConnection *>(QObject::sender());
		Send(QString("【%1】离开了聊天室！").arg(sc->GetName()));
		connect_socket_list_.removeOne(sc);
		delete sc;
	}
}

void webSocketDemo::OnRecvMessage(const QString& msg) {
	int loginIdx = msg.indexOf("login:");
	if (loginIdx != -1) {
		SocketConnection* sc = static_cast<SocketConnection *>(QObject::sender());
		QString name = msg.right(msg.size() - (loginIdx + QString("login:").size()));
		sc->SetName(name);

		QString loginMsg = QString("欢迎【%1】来到聊天室！").arg(name);
		//Send(loginMsg);

		return;
	}
	ui.textBrowser->moveCursor(QTextCursor::End);
	ui.textBrowser->insertHtml("<br>");
	ui.textBrowser->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + QStringLiteral("接收:") + msg);

	//处理接收到的消息

	DealRecvMessage(msg);
}

QString webSocketDemo::toStringJsonArray(QJsonArray & arr)
{
	QString  str = "";
	for (int i = 0; i < arr.count(); i++)
	{
		QJsonValue value = arr.at(i);
		str += QString().number(value.toInt());
		if (i != arr.count()-1)
		{
			str += ",";
		}
	}
	return str;
}

void webSocketDemo::DealRecvMessage(QString msg)
{
	//解析json 报文
	qint16 rcount = 0;
	QJsonObject tmpObj;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(msg.toLatin1().data());
	if (jsonDocument.isNull())
	{
		qDebug() << "String NULL" << msg.toLocal8Bit().data();
	}
	QJsonObject jsonObject = jsonDocument.object();
	qDebug() << jsonObject;
	if (jsonObject.contains("type"))
	{
		QJsonObject argsObject = jsonObject["args"].toObject();
		
		switch (ctrlist.indexOf(jsonObject["type"].toString()))
		{
		case 0:			//"fault_state",     			//故障状态
			if (argsObject["flag"].toBool())
			{
				ui.checkBox_3->setChecked(true);
				qDebug() << argsObject["code"];
				ui.label_15->setText(toStringJsonArray(argsObject["code"].toArray()));
			}
			else
			{
				ui.checkBox_3->setChecked(false);
				ui.label_15->setText("");
			}

			showMessage(QString("%1:%2:%3").arg(QStringLiteral("故障状态：")).arg(argsObject["flag"].toBool()?QStringLiteral("故障"):QStringLiteral("正常")).arg(ui.label_15->text()));
			break;
		case 1:			//"radio_ctr_state",   			//接管状态
			if (argsObject["flag"].toBool())
				ui.checkBox_2->setChecked(true);
			else
				ui.checkBox_2->setChecked(false);

			showMessage(QString("%1:%2").arg(QStringLiteral("接管状态：")).arg(argsObject["flag"].toBool()?QStringLiteral("接管状态"):QStringLiteral("未接管")));
			break;
		case 2:			//"power_state",      			//电台开关机状态
			if (argsObject["state"].toString() == "open")
				ui.checkBox->setChecked(true);
			else
				ui.checkBox->setChecked(false);

			showMessage(QString("%1:%2").arg(QStringLiteral("电台开关机状态：")).arg((argsObject["state"].toString() == "open") ? QStringLiteral("开机") : QStringLiteral("关机")));
			break;
		case 3:			//"detect_state",				//电台处于探测状态
			if (argsObject["state"].toString() == "active")
				ui.label_44->setText(QStringLiteral("主动探测"));
			else
				ui.label_44->setText(QStringLiteral("被动探测"));

			showMessage(QString("%1:%2 [%3]-[%4]").arg(QStringLiteral("电台处于探测状态：")).arg(ui.label_44->text()).arg(jsonObject["addr"].toInt()).arg(argsObject["rmtaddr"].toInt()));
			break;
		case 4:			//"link_state",					//电台处于建联状态
			if (argsObject["state"].toString() == "active")
				ui.label_44->setText(QStringLiteral("主动建联"));
			else
				ui.label_44->setText(QStringLiteral("被动建联"));

			showMessage(QString("%1:%2 [%3]-[%4]").arg(QStringLiteral("电台处于建联状态：")).arg(ui.label_44->text()).arg(jsonObject["addr"].toInt()).arg(argsObject["rmtaddr"].toInt()));

			break;
		case 5:			//"free_state",					//电台处于扫描状态
			ui.label_44->setText(QStringLiteral("电台正在扫描..."));
			showMessage(QString("%1:%2 power:[%3]").arg(QStringLiteral("电台处于建联状态：")).arg(ui.label_44->text()).arg(argsObject["power"].toDouble()));
			break;
		case 6:			//"local_position",				//上报类型为本地电台位置信息
			if (argsObject["flag"].toBool())
			{
				//有效
				ui.lineEdit_5->setText(QString().number(argsObject["longitude"].toDouble()));
				ui.lineEdit_6->setText(QString().number(argsObject["latitude"].toDouble()));
				ui.lineEdit_7->setText(QString().number(argsObject["height"].toDouble()));
			}
			showMessage(QString("%1:[flag:%2][longitude:%3] [latitude:%4] [height:%5]").arg(QStringLiteral("上报类型为本地电台位置信息：")).arg(argsObject["flag"].toBool()?QStringLiteral("有效"):QStringLiteral("无效")).arg(ui.lineEdit_5->text()).arg(ui.lineEdit_6->text()).arg(ui.lineEdit_7->text()));
			break;
		case 7:			//"target_and_local_position",	//上报类型为目标电台与本地电台位置信息
			if (argsObject["flag"].toBool())
			{
				//有效
				ui.checkBox_4->setChecked(true);
				ui.label_36->setText(QString().number(argsObject["longitude"].toDouble()));
				ui.label_37->setText(QString().number(argsObject["latitude"].toDouble()));
				ui.label_38->setText(QString().number(argsObject["height"].toDouble()));

				ui.label_39->setText(QString().number(argsObject["rmtaddr"].toInt()));
				ui.label_41->setText(QString().number(argsObject["rmt_longitude"].toDouble()));
				ui.label_42->setText(QString().number(argsObject["rmt_latitude"].toDouble()));
				ui.label_40->setText(QString().number(argsObject["rmt_height"].toDouble()));
				ui.label_43->setText(QString().number(argsObject["distance"].toDouble()));
			}
			else
			{
				ui.checkBox_4->setChecked(false);
			}

			showMessage(QString("%1:%2").arg(QStringLiteral("上报类型为目标电台与本地电台位置信息：")).arg(argsObject["flag"].toBool() ? QStringLiteral("有效") : QStringLiteral("无效")));
			break;
		case 8:			//"rmt_call_in",				//上报类型为目标台本址
			showMessage(QString("%1:%2").arg(QStringLiteral("上报类型为目标台本址：远端电台本址")).arg(argsObject["rmt_addr"].toInt()));
			break;
		case 9:			//"send_msg_flag",				//发送短报文结果标志
//			//showMessage(QString("%1 %2 %3 %4").arg(QStringLiteral("")).arg().arg().arg());
			showMessage(QString("%1: %2 %3 sn:%4").arg(QStringLiteral("发送短报文结果标志")).arg(argsObject["flag"].toBool()?QStringLiteral("有效"):QStringLiteral("无效")).arg(QStringLiteral("目标电台本址：") + QString().number(argsObject["rmtaddr"].toInt())).arg(argsObject["sn"].toInt()));
			break;
		case 10:		//"recv_msg_content",			//上报接收到的短报文内容
			//添加短信息
			//....


			showMessage(QString("%1: %2 %3").arg(QStringLiteral("上报接收到的短报文内容")).arg(QStringLiteral("目标电台本址")).arg(argsObject["rmtaddr"].toInt()));
			break;
		case 11:		//"stop_detect",				//网关结束探测指令
			ui.label_44->setText(QString("%1[%2]").arg(QStringLiteral("结束探测")).arg(argsObject["rmtaddr"].toInt()));

			showMessage(QString("%1:%2").arg(QStringLiteral("网关结束探测指令")).arg(ui.label_44->text()));
			break;
		case 12:		//"environment_state",			//网关上报环境状态
			ui.lineEdit_8->setText(QString().number(argsObject["temperature"].toInt()));
			ui.lineEdit_9->setText(QString().number(argsObject["humidity"].toInt()));

			showMessage(QString("%1:%2").arg(QStringLiteral("网关上报环境状态")).arg(ui.label_20->text() + ui.lineEdit_8->text() + ui.label_21->text() + ui.lineEdit_9->text()));
			break;
		case 13:		//"gw_config_ctl",				//上报网关配置参数
			ui.lineEdit_10->setText(QString().number(argsObject["vox_in"].toInt()));
			ui.lineEdit_11->setText(QString().number(argsObject["vox_out"].toInt()));
			ui.lineEdit_12->setText(QString().number(argsObject["denoise"].toInt()));
			ui.lineEdit_13->setText(QString().number(argsObject["gain_out"].toInt()));
	
			showMessage(QString("%1:vox_in %2 vox_out:%3 denoise:%4 gain_out:%5").arg(QStringLiteral("上报网关配置参数")).arg(ui.lineEdit_10->text()).arg(ui.lineEdit_11->text()).arg(ui.lineEdit_12->text()).arg(ui.lineEdit_13->text()));
			break;
		case 14:		//"register_info",				//请求的类型为注册信息
			showMessage(QStringLiteral("请求的类型为注册信息"));
			break;
		case 15:		//"radio_power_ctl",			//电台开关机请求
			break;
		case 16:		//"start_detect_ctl",			//开始探测电台请求
			break;
		case 17:		//"stop_detect_ctl",			//停止探测电台请求
			break;
		case 18:		//"start_track_ctl",			//开始追踪请求
			break;
		case 19:		//"stop_track_ctl",				//结束追踪请求
			break;
		case 20:		//"sent_msg_ctl",				//请求类型为控制发送短报文
			break;
		case 21:		//"gw_config_ctl",				//网关配置
			break;
		case 22:		//"call_out",					//呼叫控制
			break;
		case 23:		//"ack",						//回复类型为ACK
			if (argsObject["ack_type"].toString() == "register_info")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("回复类型为ACK")).arg(QStringLiteral("注册信息请求")));
			}
			else if (argsObject["ack_type"].toString() == "radio_power_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("回复类型为ACK")).arg(QStringLiteral("电台开关机控制请求")));
			}
			else if (argsObject["ack_type"].toString() == "start_detect_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("回复类型为ACK")).arg(QStringLiteral("开始探测请求")));
			}
			else if (argsObject["ack_type"].toString() == "stop_detect_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("回复类型为ACK")).arg(QStringLiteral("结束探测请求")));
			}
			else if (argsObject["ack_type"].toString() == "start_track_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("回复类型为ACK")).arg(QStringLiteral("开始追踪请求")));
			}
			else if (argsObject["ack_type"].toString() == "stop_track_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("回复类型为ACK")).arg(QStringLiteral("结束追踪请求")));
			}
			else if (argsObject["ack_type"].toString() == "gw_config_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("回复类型为ACK")).arg(QStringLiteral("下发网关配置请求")));
			}
			break;
		case 24:		//"detect_channel_snr",			//探测时的频率信息与信道质量信息
			rcount = model1->rowCount();
			model1->setItem(rcount, new QStandardItem(QString().number(jsonObject["addr"].toInt())));
			model1->setItem(rcount, 1, new QStandardItem(QString().number(argsObject["rmtaddr"].toInt())));
			model1->setItem(rcount, 2, new QStandardItem(QString().number(argsObject["fre"].toDouble())));

			tmpObj = argsObject["snr"].toObject();
			model1->setItem(rcount, 3, new QStandardItem(QString().number(tmpObj["tx"].toInt())));
			model1->setItem(rcount, 4, new QStandardItem(QString().number(tmpObj["rx"].toInt())));
			model1->setItem(rcount, 5, new QStandardItem(QString().number(tmpObj["result"].toInt())));

			model1->setHeaderData(0, Qt::Horizontal, QStringLiteral("Src本址"));
			model1->setHeaderData(1, Qt::Horizontal, QStringLiteral("Dst本址"));
			model1->setHeaderData(2, Qt::Horizontal, QStringLiteral("频率"));
			model1->setHeaderData(3, Qt::Horizontal, QStringLiteral("Tx"));
			model1->setHeaderData(4, Qt::Horizontal, QStringLiteral("Rx"));
			model1->setHeaderData(5, Qt::Horizontal, QStringLiteral("评分"));

			ui.tableView->setColumnWidth(0, 60);
			ui.tableView->setColumnWidth(1, 60);
			ui.tableView->setColumnWidth(2, 60);
			ui.tableView->setColumnWidth(3, 40);
			ui.tableView->setColumnWidth(4, 40);
			ui.tableView->setColumnWidth(5, 40);

			ui.tableView->scrollToBottom();
			showMessage(QString("%1").arg(QStringLiteral("探测时的频率信息与信道质量信息")));
			break;
		case 25:		//"connect_ok_channel_snr",		//建联成功时的频率信息与信道质量信息
			rcount = model2->rowCount();
			model2->setItem(rcount, new QStandardItem(QString().number(jsonObject["addr"].toInt())));
			model2->setItem(rcount, 1, new QStandardItem(QString().number(argsObject["rmtaddr"].toInt())));
			model2->setItem(rcount, 2, new QStandardItem(QString().number(argsObject["rx_fre"].toDouble())));
			model2->setItem(rcount, 3, new QStandardItem(QString().number(argsObject["tx_fre"].toDouble())));
			
			tmpObj = argsObject["snr"].toObject();
			model2->setItem(rcount, 4, new QStandardItem(QString().number(tmpObj["rx"].toInt())));
			model2->setItem(rcount, 5, new QStandardItem(QString().number(tmpObj["tx"].toInt())));
			model2->setItem(rcount, 6, new QStandardItem(QString().number(tmpObj["result"].toInt())));
			model2->setItem(rcount, 7, new QStandardItem((argsObject["state"].toString() == "active")?QStringLiteral("主动建联"):QStringLiteral("被动建联")));
			model2->setItem(rcount, 8, new QStandardItem((argsObject["mode"].toString() == "normal")?QStringLiteral("业务呼叫") : QStringLiteral("考核呼叫")));
			
			model2->setHeaderData(0, Qt::Horizontal, QStringLiteral("Src本址"));
			model2->setHeaderData(1, Qt::Horizontal, QStringLiteral("Dst本址"));
			model2->setHeaderData(2, Qt::Horizontal, QStringLiteral("Rx频率"));
			model2->setHeaderData(3, Qt::Horizontal, QStringLiteral("Tx频率"));

			model2->setHeaderData(4, Qt::Horizontal, QStringLiteral("Rx"));
			model2->setHeaderData(5, Qt::Horizontal, QStringLiteral("Tx"));
			model2->setHeaderData(6, Qt::Horizontal, QStringLiteral("评分"));

			model2->setHeaderData(7, Qt::Horizontal, QStringLiteral("方式"));
			model2->setHeaderData(8, Qt::Horizontal, QStringLiteral("模式"));

			ui.tableView_2->setColumnWidth(0, 60);
			ui.tableView_2->setColumnWidth(1, 60);
			ui.tableView_2->setColumnWidth(2, 60);
			ui.tableView_2->setColumnWidth(3, 60);
			ui.tableView_2->setColumnWidth(4, 40);
			ui.tableView_2->setColumnWidth(5, 40);
			ui.tableView_2->setColumnWidth(6, 40);
			ui.tableView_2->setColumnWidth(7, 60);
			ui.tableView_2->setColumnWidth(8, 60);

			ui.tableView_2->scrollToBottom();
			showMessage(QString("%1").arg(QStringLiteral("建联成功时的频率信息与信道质量信息")));
			break;
		case 26:		//"ack_sent_msg_ctl",			//回复类型为发送短报文的ACK
			break;
		case 27:		//"ack_call_out",			    //回复类型为呼叫控制的ACK
			break;
		case 28:		//"gateway_id"	,				//回复类型为网关ID信息
			break;
		default:
			break;
		}
	}
	else
	{
		qDebug() << "json file format is not right;";
	}
}

void webSocketDemo::Send(QString& msg) {
	QList<SocketConnection *>::iterator iter = connect_socket_list_.begin();
	for (; iter != connect_socket_list_.end(); ++iter) {
		SocketConnection* sc = *iter;

		if (sc->GetSocket()->state() != QAbstractSocket::ConnectedState) continue;

		if (sc->GetIsDataMasked()) {
			DataFrame dr(msg,"");
			qDebug() << dr.GetByteArray();
			sc->GetSocket()->write(dr.GetByteArray());
		}
		else {

		}
	}
}

QString webSocketDemo::JsonToString(const QJsonObject& json) const
{
	return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
}
void webSocketDemo::showMessage(QString value)
{
	ui.textBrowser->moveCursor(QTextCursor::End);
	ui.textBrowser->insertHtml("<br>");
	ui.textBrowser->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + QStringLiteral("解析结果："));
	ui.textBrowser->insertPlainText(value);
}

void webSocketDemo::addMessage(QString value)
{
	ui.textBrowser->moveCursor(QTextCursor::End);
	ui.textBrowser->insertHtml("<br>");
	ui.textBrowser->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + QStringLiteral("发送："));
	ui.textBrowser->insertPlainText(value);
	Send(value);
}

void webSocketDemo::on_pushButton_radioOpen_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"radio_power_ctl\",\"addr\":%1,\"args\":{\"state\":\"open\"}}").arg(ui.lineEdit_2->text().trimmed());
	addMessage(s);
	
}
void webSocketDemo::on_pushButton_radioClose_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"radio_power_ctl\",\"addr\":%1,\"args\":{\"state\":\"close\"}}").arg(ui.lineEdit_2->text().trimmed());
	addMessage(s);
}

void webSocketDemo::on_pushButton_beginTC_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"start_detect_ctl\",\"addr\":%1,\"args\":{\"rmtaddr\":%2}}").arg(ui.lineEdit_2->text().trimmed()).arg(ui.lineEdit_3->text().trimmed());
	addMessage(s);
}
void webSocketDemo::on_pushButton_stopTC_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"stop_detect_ctl\",\"addr\":%1,\"args\":{\"rmtaddr\":%2}}").arg(ui.lineEdit_2->text().trimmed()).arg(ui.lineEdit_3->text().trimmed());
	addMessage(s);
}

void webSocketDemo::on_pushButton_beginZZ_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"start_track_ctl\",\"addr\":%1,\"args\":{\"rmtaddr\":%2}}").arg(ui.lineEdit_2->text().trimmed()).arg(ui.lineEdit_3->text().trimmed());
	addMessage(s);
}
void webSocketDemo::on_pushButton_stopZZ_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"stop_track_ctl\",\"addr\":%1,\"args\":{\"rmtaddr\":%2}}").arg(ui.lineEdit_2->text().trimmed()).arg(ui.lineEdit_3->text().trimmed());
	addMessage(s);
}

void webSocketDemo::on_pushButton_sendselfmessage_clicked()
{
	addMessage(ui.textEdit_3->toPlainText());
	ui.textEdit_3->setText("");
}

void webSocketDemo::on_pushButton_beginCall_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"call_out\",\"addr\":%1,\"args\":{\"mode\":\"%3\",\"rmtaddr\":%2}}").arg(ui.lineEdit_2->text().trimmed()).arg(ui.lineEdit_3->text().trimmed()).arg((ui.comboBox->currentIndex()==0)?"normal":"advance");
	addMessage(s);
}

void webSocketDemo::on_pushButton_setCfg_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"gw_config_ctl\",\"addr\":%1,\"args\":{\"vox_in\":%2,\"vox_out\":%3,\"denoise\":%4,\"gain_out\":%5}}")
		.arg(ui.lineEdit_2->text().trimmed())
		.arg(ui.comboBox_2->currentIndex())
		.arg(ui.comboBox_3->currentIndex())
		.arg(ui.comboBox_4->currentIndex())
		.arg(ui.comboBox_5->currentIndex());
	addMessage(s);
}

void webSocketDemo::on_pushButton_sendMessage_clicked()
{
	QString s = QString("{\"msg\":\"request\",\"type\":\"sent_msg_ctl\",\"addr\":%1,\"args\":{\"content\":\"%2\" ,\"rmtaddr\":%3,\"sn\":\"%4\"}}")
		.arg(ui.lineEdit_2->text().trimmed())
		.arg((ui.textEdit->toPlainText()))
		.arg(ui.lineEdit_3->text().trimmed())
		.arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
	addMessage(s);

	ui.textEdit->setText("");
}


void webSocketDemo::on_pushButton_2_clicked()
{
	model2->removeRows(0, model2->rowCount());
}

void webSocketDemo::on_pushButton_clicked()
{
	model1->removeRows(0, model1->rowCount());
}

void webSocketDemo::on_pushButton_3_clicked()
{
	model1->removeRows(0, model2->rowCount());
	model2->removeRows(0, model2->rowCount());
	ui.textBrowser->setText("");
	ui.textBrowser_2->setText("");

	ui.checkBox_4->setCheckState(Qt::Unchecked);
	ui.label_36->setText("");
	ui.label_37->setText("");
	ui.label_38->setText("");
	
	ui.label_39->setText("");
	ui.label_40->setText("");
	ui.label_41->setText("");
	ui.label_42->setText("");

	ui.label_43->setText("");

	ui.checkBox->setCheckState(Qt::Unchecked);
	ui.checkBox_2->setCheckState(Qt::Unchecked);
	ui.checkBox_3->setCheckState(Qt::Unchecked);
	ui.label_15->setText("");

	ui.lineEdit_5->setText("");
	ui.lineEdit_6->setText("");
	ui.lineEdit_7->setText("");
	ui.lineEdit_8->setText("");
	ui.lineEdit_9->setText("");

	ui.lineEdit_10->setText("");
	ui.lineEdit_11->setText("");
	ui.lineEdit_12->setText("");
	ui.lineEdit_13->setText("");
}