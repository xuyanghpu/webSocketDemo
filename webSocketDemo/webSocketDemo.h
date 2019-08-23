#pragma once

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QStringList>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QStandardItemModel>

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
	void gb2312ToUtf8(QString strGb2312);
	void utf8ToGb2312(char *strUtf8);
	QString utf8ToGb2312_new(char *strUtf8);
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
	void on_pushButton_3_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_clicked();

	void on_pushButton_clean_clicked();

	void addMessage(QString value);

	void on_pushButton_sendselfmessage_clicked();

	void on_lineEdit_2_textChanged(QString s);
	void on_lineEdit_3_textChanged(QString s);

private:
	QTcpServer listener_;
	QList<SocketConnection *> connect_socket_list_;

	void Send(QString& msg);
	void DealRecvMessage(QString msg);
	void showMessage(QString value);
	QString toStringJsonArray(QJsonArray & arr);
private:
	Ui::webSocketDemoClass ui;
	QStringList ctrlist;

	QStandardItemModel * model1;
	QStandardItemModel * model2;

};
