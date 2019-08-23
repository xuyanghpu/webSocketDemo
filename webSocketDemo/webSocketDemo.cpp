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

	ctrlist << "fault_state";     			//����״̬
	ctrlist << "radio_ctr_state";   		//�ӹ�״̬
	ctrlist << "power_state";      			//��̨���ػ�״̬
	ctrlist << "detect_state";				//��̨����̽��״̬
	ctrlist << "link_state";				//��̨���ڽ���״̬
	ctrlist << "free_state";				//��̨����ɨ��״̬
	ctrlist << "local_position";			//�ϱ�����Ϊ���ص�̨λ����Ϣ
	ctrlist << "target_and_local_position";	//�ϱ�����ΪĿ���̨�뱾�ص�̨λ����Ϣ
	ctrlist << "rmt_call_in";				//�ϱ�����ΪĿ��̨��ַ
	ctrlist << "send_msg_flag";				//���Ͷ̱��Ľ����־
	ctrlist << "recv_msg_content";			//�ϱ����յ��Ķ̱�������
	ctrlist << "stop_detect";				//���ؽ���̽��ָ��
	ctrlist << "environment_state";			//�����ϱ�����״̬
	ctrlist << "gw_config_ctl";				//�ϱ��������ò���
	ctrlist << "register_info";				//���������Ϊע����Ϣ
	ctrlist << "radio_power_ctl";			//��̨���ػ�����
	ctrlist << "start_detect_ctl";			//��ʼ̽���̨����
	ctrlist << "stop_detect_ctl";			//ֹͣ̽���̨����
	ctrlist << "start_track_ctl";			//��ʼ׷������
	ctrlist << "stop_track_ctl";			//����׷������
	ctrlist << "sent_msg_ctl";				//��������Ϊ���Ʒ��Ͷ̱���
	ctrlist << "gw_config_ctl";				//��������
	ctrlist << "call_out";					//���п���
	ctrlist << "ack";						//�ظ�����ΪACK
	ctrlist << "detect_channel_snr";		//̽��ʱ��Ƶ����Ϣ���ŵ�������Ϣ
	ctrlist << "connect_ok_channel_snr";	//�����ɹ�ʱ��Ƶ����Ϣ���ŵ�������Ϣ
	ctrlist << "ack_sent_msg_ctl";			//�ظ�����Ϊ���Ͷ̱��ĵ�ACK
	ctrlist << "ack_call_out";			    //�ظ�����Ϊ���п��Ƶ�ACK
	ctrlist << "gateway_id";				//�ظ�����Ϊ����ID��Ϣ

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
	model1->setHeaderData(0, Qt::Horizontal, QStringLiteral("Src��ַ"));
	model1->setHeaderData(1, Qt::Horizontal, QStringLiteral("Dst��ַ"));
	model1->setHeaderData(2, Qt::Horizontal, QStringLiteral("Ƶ��"));
	model1->setHeaderData(3, Qt::Horizontal, QStringLiteral("Tx"));
	model1->setHeaderData(4, Qt::Horizontal, QStringLiteral("Rx"));
	model1->setHeaderData(5, Qt::Horizontal, QStringLiteral("����"));

	model2 = new QStandardItemModel(this);
	ui.tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_2->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableView_2->verticalHeader()->setDefaultSectionSize(22);
	ui.tableView_2->setModel(model2);

	qint16 rcount2 = model2->rowCount();
	model2->setHeaderData(0, Qt::Horizontal, QStringLiteral("Src��ַ"));
	model2->setHeaderData(1, Qt::Horizontal, QStringLiteral("Dst��ַ"));
	model2->setHeaderData(2, Qt::Horizontal, QStringLiteral("RxƵ��"));
	model2->setHeaderData(3, Qt::Horizontal, QStringLiteral("TxƵ��"));
	model2->setHeaderData(4, Qt::Horizontal, QStringLiteral("Rx"));
	model2->setHeaderData(5, Qt::Horizontal, QStringLiteral("Tx"));
	model2->setHeaderData(6, Qt::Horizontal, QStringLiteral("����"));
	model2->setHeaderData(7, Qt::Horizontal, QStringLiteral("��ʽ"));
	model2->setHeaderData(8, Qt::Horizontal, QStringLiteral("ģʽ"));

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
	return QString::fromLocal8Bit(strUtf8);//ע������ҪfromLocal8Bit()
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

	qDebug() << QString("���������������������ַ��%1, �˿ڣ�%2").arg("0.0.0.0").arg(4141);
	qDebug() << QString("WebSocket��������ַ: ws://%1:%2/chat").arg("0.0.0.0").arg(4141);

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
 	ui.textBrowser->insertPlainText(QStringLiteral("�ͻ���") + socket->peerAddress().toString() + QStringLiteral("���ӳɹ�"));
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
		Send(QString("��%1���뿪�������ң�").arg(sc->GetName()));
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

		QString loginMsg = QString("��ӭ��%1�����������ң�").arg(name);
		//Send(loginMsg);

		return;
	}
	ui.textBrowser->moveCursor(QTextCursor::End);
	ui.textBrowser->insertHtml("<br>");
	ui.textBrowser->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + QStringLiteral("����:") + msg);

	//������յ�����Ϣ

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
	//����json ����
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
		case 0:			//"fault_state",     			//����״̬
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

			showMessage(QString("%1:%2:%3").arg(QStringLiteral("����״̬��")).arg(argsObject["flag"].toBool()?QStringLiteral("����"):QStringLiteral("����")).arg(ui.label_15->text()));
			break;
		case 1:			//"radio_ctr_state",   			//�ӹ�״̬
			if (argsObject["flag"].toBool())
				ui.checkBox_2->setChecked(true);
			else
				ui.checkBox_2->setChecked(false);

			showMessage(QString("%1:%2").arg(QStringLiteral("�ӹ�״̬��")).arg(argsObject["flag"].toBool()?QStringLiteral("�ӹ�״̬"):QStringLiteral("δ�ӹ�")));
			break;
		case 2:			//"power_state",      			//��̨���ػ�״̬
			if (argsObject["state"].toString() == "open")
				ui.checkBox->setChecked(true);
			else
				ui.checkBox->setChecked(false);

			showMessage(QString("%1:%2").arg(QStringLiteral("��̨���ػ�״̬��")).arg((argsObject["state"].toString() == "open") ? QStringLiteral("����") : QStringLiteral("�ػ�")));
			break;
		case 3:			//"detect_state",				//��̨����̽��״̬
			if (argsObject["state"].toString() == "active")
				ui.label_44->setText(QStringLiteral("����̽��"));
			else
				ui.label_44->setText(QStringLiteral("����̽��"));

			showMessage(QString("%1:%2 [%3]-[%4]").arg(QStringLiteral("��̨����̽��״̬��")).arg(ui.label_44->text()).arg(jsonObject["addr"].toInt()).arg(argsObject["rmtaddr"].toInt()));
			break;
		case 4:			//"link_state",					//��̨���ڽ���״̬
			if (argsObject["state"].toString() == "active")
				ui.label_44->setText(QStringLiteral("��������"));
			else
				ui.label_44->setText(QStringLiteral("��������"));

			showMessage(QString("%1:%2 [%3]-[%4]").arg(QStringLiteral("��̨���ڽ���״̬��")).arg(ui.label_44->text()).arg(jsonObject["addr"].toInt()).arg(argsObject["rmtaddr"].toInt()));

			break;
		case 5:			//"free_state",					//��̨����ɨ��״̬
			ui.label_44->setText(QStringLiteral("��̨����ɨ��..."));
			showMessage(QString("%1:%2 power:[%3]").arg(QStringLiteral("��̨���ڽ���״̬��")).arg(ui.label_44->text()).arg(argsObject["power"].toDouble()));
			break;
		case 6:			//"local_position",				//�ϱ�����Ϊ���ص�̨λ����Ϣ
			if (argsObject["flag"].toBool())
			{
				//��Ч
				ui.lineEdit_5->setText(QString().number(argsObject["longitude"].toDouble()));
				ui.lineEdit_6->setText(QString().number(argsObject["latitude"].toDouble()));
				ui.lineEdit_7->setText(QString().number(argsObject["height"].toDouble()));
			}
			showMessage(QString("%1:[flag:%2][longitude:%3] [latitude:%4] [height:%5]").arg(QStringLiteral("�ϱ�����Ϊ���ص�̨λ����Ϣ��")).arg(argsObject["flag"].toBool()?QStringLiteral("��Ч"):QStringLiteral("��Ч")).arg(ui.lineEdit_5->text()).arg(ui.lineEdit_6->text()).arg(ui.lineEdit_7->text()));
			break;
		case 7:			//"target_and_local_position",	//�ϱ�����ΪĿ���̨�뱾�ص�̨λ����Ϣ
			if (argsObject["flag"].toBool())
			{
				//��Ч
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

			showMessage(QString("%1:%2").arg(QStringLiteral("�ϱ�����ΪĿ���̨�뱾�ص�̨λ����Ϣ��")).arg(argsObject["flag"].toBool() ? QStringLiteral("��Ч") : QStringLiteral("��Ч")));
			break;
		case 8:			//"rmt_call_in",				//�ϱ�����ΪĿ��̨��ַ
			showMessage(QString("%1:%2").arg(QStringLiteral("�ϱ�����ΪĿ��̨��ַ��Զ�˵�̨��ַ")).arg(argsObject["rmt_addr"].toInt()));
			break;
		case 9:			//"send_msg_flag",				//���Ͷ̱��Ľ����־
//			//showMessage(QString("%1 %2 %3 %4").arg(QStringLiteral("")).arg().arg().arg());
			showMessage(QString("%1: %2 %3 sn:%4").arg(QStringLiteral("���Ͷ̱��Ľ����־")).arg(argsObject["flag"].toBool()?QStringLiteral("��Ч"):QStringLiteral("��Ч")).arg(QStringLiteral("Ŀ���̨��ַ��") + QString().number(argsObject["rmtaddr"].toInt())).arg(argsObject["sn"].toInt()));
			break;
		case 10:		//"recv_msg_content",			//�ϱ����յ��Ķ̱�������
			//��Ӷ���Ϣ
			//....


			showMessage(QString("%1: %2 %3").arg(QStringLiteral("�ϱ����յ��Ķ̱�������")).arg(QStringLiteral("Ŀ���̨��ַ")).arg(argsObject["rmtaddr"].toInt()));
			break;
		case 11:		//"stop_detect",				//���ؽ���̽��ָ��
			ui.label_44->setText(QString("%1[%2]").arg(QStringLiteral("����̽��")).arg(argsObject["rmtaddr"].toInt()));

			showMessage(QString("%1:%2").arg(QStringLiteral("���ؽ���̽��ָ��")).arg(ui.label_44->text()));
			break;
		case 12:		//"environment_state",			//�����ϱ�����״̬
			ui.lineEdit_8->setText(QString().number(argsObject["temperature"].toInt()));
			ui.lineEdit_9->setText(QString().number(argsObject["humidity"].toInt()));

			showMessage(QString("%1:%2").arg(QStringLiteral("�����ϱ�����״̬")).arg(ui.label_20->text() + ui.lineEdit_8->text() + ui.label_21->text() + ui.lineEdit_9->text()));
			break;
		case 13:		//"gw_config_ctl",				//�ϱ��������ò���
			ui.lineEdit_10->setText(QString().number(argsObject["vox_in"].toInt()));
			ui.lineEdit_11->setText(QString().number(argsObject["vox_out"].toInt()));
			ui.lineEdit_12->setText(QString().number(argsObject["denoise"].toInt()));
			ui.lineEdit_13->setText(QString().number(argsObject["gain_out"].toInt()));
	
			showMessage(QString("%1:vox_in %2 vox_out:%3 denoise:%4 gain_out:%5").arg(QStringLiteral("�ϱ��������ò���")).arg(ui.lineEdit_10->text()).arg(ui.lineEdit_11->text()).arg(ui.lineEdit_12->text()).arg(ui.lineEdit_13->text()));
			break;
		case 14:		//"register_info",				//���������Ϊע����Ϣ
			showMessage(QStringLiteral("���������Ϊע����Ϣ"));
			break;
		case 15:		//"radio_power_ctl",			//��̨���ػ�����
			break;
		case 16:		//"start_detect_ctl",			//��ʼ̽���̨����
			break;
		case 17:		//"stop_detect_ctl",			//ֹͣ̽���̨����
			break;
		case 18:		//"start_track_ctl",			//��ʼ׷������
			break;
		case 19:		//"stop_track_ctl",				//����׷������
			break;
		case 20:		//"sent_msg_ctl",				//��������Ϊ���Ʒ��Ͷ̱���
			break;
		case 21:		//"gw_config_ctl",				//��������
			break;
		case 22:		//"call_out",					//���п���
			break;
		case 23:		//"ack",						//�ظ�����ΪACK
			if (argsObject["ack_type"].toString() == "register_info")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("�ظ�����ΪACK")).arg(QStringLiteral("ע����Ϣ����")));
			}
			else if (argsObject["ack_type"].toString() == "radio_power_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("�ظ�����ΪACK")).arg(QStringLiteral("��̨���ػ���������")));
			}
			else if (argsObject["ack_type"].toString() == "start_detect_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("�ظ�����ΪACK")).arg(QStringLiteral("��ʼ̽������")));
			}
			else if (argsObject["ack_type"].toString() == "stop_detect_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("�ظ�����ΪACK")).arg(QStringLiteral("����̽������")));
			}
			else if (argsObject["ack_type"].toString() == "start_track_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("�ظ�����ΪACK")).arg(QStringLiteral("��ʼ׷������")));
			}
			else if (argsObject["ack_type"].toString() == "stop_track_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("�ظ�����ΪACK")).arg(QStringLiteral("����׷������")));
			}
			else if (argsObject["ack_type"].toString() == "gw_config_ctl")
			{
				showMessage(QString("%1:%2").arg(QStringLiteral("�ظ�����ΪACK")).arg(QStringLiteral("�·�������������")));
			}
			break;
		case 24:		//"detect_channel_snr",			//̽��ʱ��Ƶ����Ϣ���ŵ�������Ϣ
			rcount = model1->rowCount();
			model1->setItem(rcount, new QStandardItem(QString().number(jsonObject["addr"].toInt())));
			model1->setItem(rcount, 1, new QStandardItem(QString().number(argsObject["rmtaddr"].toInt())));
			model1->setItem(rcount, 2, new QStandardItem(QString().number(argsObject["fre"].toDouble())));

			tmpObj = argsObject["snr"].toObject();
			model1->setItem(rcount, 3, new QStandardItem(QString().number(tmpObj["tx"].toInt())));
			model1->setItem(rcount, 4, new QStandardItem(QString().number(tmpObj["rx"].toInt())));
			model1->setItem(rcount, 5, new QStandardItem(QString().number(tmpObj["result"].toInt())));

			model1->setHeaderData(0, Qt::Horizontal, QStringLiteral("Src��ַ"));
			model1->setHeaderData(1, Qt::Horizontal, QStringLiteral("Dst��ַ"));
			model1->setHeaderData(2, Qt::Horizontal, QStringLiteral("Ƶ��"));
			model1->setHeaderData(3, Qt::Horizontal, QStringLiteral("Tx"));
			model1->setHeaderData(4, Qt::Horizontal, QStringLiteral("Rx"));
			model1->setHeaderData(5, Qt::Horizontal, QStringLiteral("����"));

			ui.tableView->setColumnWidth(0, 60);
			ui.tableView->setColumnWidth(1, 60);
			ui.tableView->setColumnWidth(2, 60);
			ui.tableView->setColumnWidth(3, 40);
			ui.tableView->setColumnWidth(4, 40);
			ui.tableView->setColumnWidth(5, 40);

			ui.tableView->scrollToBottom();
			showMessage(QString("%1").arg(QStringLiteral("̽��ʱ��Ƶ����Ϣ���ŵ�������Ϣ")));
			break;
		case 25:		//"connect_ok_channel_snr",		//�����ɹ�ʱ��Ƶ����Ϣ���ŵ�������Ϣ
			rcount = model2->rowCount();
			model2->setItem(rcount, new QStandardItem(QString().number(jsonObject["addr"].toInt())));
			model2->setItem(rcount, 1, new QStandardItem(QString().number(argsObject["rmtaddr"].toInt())));
			model2->setItem(rcount, 2, new QStandardItem(QString().number(argsObject["rx_fre"].toDouble())));
			model2->setItem(rcount, 3, new QStandardItem(QString().number(argsObject["tx_fre"].toDouble())));
			
			tmpObj = argsObject["snr"].toObject();
			model2->setItem(rcount, 4, new QStandardItem(QString().number(tmpObj["rx"].toInt())));
			model2->setItem(rcount, 5, new QStandardItem(QString().number(tmpObj["tx"].toInt())));
			model2->setItem(rcount, 6, new QStandardItem(QString().number(tmpObj["result"].toInt())));
			model2->setItem(rcount, 7, new QStandardItem((argsObject["state"].toString() == "active")?QStringLiteral("��������"):QStringLiteral("��������")));
			model2->setItem(rcount, 8, new QStandardItem((argsObject["mode"].toString() == "normal")?QStringLiteral("ҵ�����") : QStringLiteral("���˺���")));
			
			model2->setHeaderData(0, Qt::Horizontal, QStringLiteral("Src��ַ"));
			model2->setHeaderData(1, Qt::Horizontal, QStringLiteral("Dst��ַ"));
			model2->setHeaderData(2, Qt::Horizontal, QStringLiteral("RxƵ��"));
			model2->setHeaderData(3, Qt::Horizontal, QStringLiteral("TxƵ��"));

			model2->setHeaderData(4, Qt::Horizontal, QStringLiteral("Rx"));
			model2->setHeaderData(5, Qt::Horizontal, QStringLiteral("Tx"));
			model2->setHeaderData(6, Qt::Horizontal, QStringLiteral("����"));

			model2->setHeaderData(7, Qt::Horizontal, QStringLiteral("��ʽ"));
			model2->setHeaderData(8, Qt::Horizontal, QStringLiteral("ģʽ"));

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
			showMessage(QString("%1").arg(QStringLiteral("�����ɹ�ʱ��Ƶ����Ϣ���ŵ�������Ϣ")));
			break;
		case 26:		//"ack_sent_msg_ctl",			//�ظ�����Ϊ���Ͷ̱��ĵ�ACK
			break;
		case 27:		//"ack_call_out",			    //�ظ�����Ϊ���п��Ƶ�ACK
			break;
		case 28:		//"gateway_id"	,				//�ظ�����Ϊ����ID��Ϣ
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
	ui.textBrowser->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + QStringLiteral("���������"));
	ui.textBrowser->insertPlainText(value);
}

void webSocketDemo::addMessage(QString value)
{
	ui.textBrowser->moveCursor(QTextCursor::End);
	ui.textBrowser->insertHtml("<br>");
	ui.textBrowser->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + QStringLiteral("���ͣ�"));
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