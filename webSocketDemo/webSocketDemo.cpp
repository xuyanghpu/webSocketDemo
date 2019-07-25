#include "webSocketDemo.h"
#include <QDebug>
#include "SocketConnection.h"
#include "DataFrame.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


webSocketDemo::webSocketDemo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.textBrowser->setReadOnly(true);
}
webSocketDemo::~webSocketDemo()
{
	Close();
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
		Send(loginMsg);

		return;
	}
	ui.textBrowser->moveCursor(QTextCursor::End);
	ui.textBrowser->insertHtml("<br>");
	ui.textBrowser->insertPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + QStringLiteral("接收:") + msg);

}

void webSocketDemo::Send(const QString& msg) {
	QList<SocketConnection *>::iterator iter = connect_socket_list_.begin();
	for (; iter != connect_socket_list_.end(); ++iter) {
		SocketConnection* sc = *iter;

		if (sc->GetSocket()->state() != QAbstractSocket::ConnectedState) continue;

		if (sc->GetIsDataMasked()) {
			DataFrame dr(msg);
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
	QString s = QString("{\"msg\":\"request\",\"type\":\"sent_msg_ctl\",\"addr\":%1,\"args\":{\"content\":%2 ,\"rmtaddr\":%3,\"sn\":\"%4\"}}")
		.arg(ui.lineEdit_2->text().trimmed())
		.arg(ui.textEdit->toPlainText())
		.arg(ui.lineEdit_3->text().trimmed())
		.arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
	addMessage(s);
	ui.textEdit->setText("");
}
