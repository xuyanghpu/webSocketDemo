#pragma once

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "ui_webSocketDemo.h"
class SocketConnection;

class webSocketDemo : public QMainWindow
{
	Q_OBJECT

public:
	webSocketDemo(QWidget *parent = Q_NULLPTR);
	~webSocketDemo();
	bool StartServer();
	void Close();

	public slots:
	void OnNewConnection();
	void OnSocketStateChanged(QAbstractSocket::SocketState socketState);
	void OnRecvMessage(const QString&);
	QString JsonToString(const QJsonObject& json) const;

private slots:
	void on_pushButton_radioOpen_clicked();
	void on_pushButton_radioClose_clicked();

	void on_pushButton_beginTC_clicked();
	void on_pushButton_stopTC_clicked();

	void on_pushButton_beginZZ_clicked();
	void on_pushButton_stopZZ_clicked();

	void on_pushButton_beginCall_clicked();

	void on_pushButton_setCfg_clicked();

	void on_pushButton_sendMessage_clicked();

	void on_pushButton_clean_clicked();

	void addMessage(QString value);

	void on_pushButton_sendselfmessage_clicked();

private:
	QTcpServer listener_;
	QList<SocketConnection *> connect_socket_list_;

	void Send(const QString& msg);
private:
	Ui::webSocketDemoClass ui;

};
