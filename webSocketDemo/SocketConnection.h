#pragma once

#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>


class SocketConnection : public QObject
{
	Q_OBJECT

public:
	SocketConnection(QObject *parent);
	~SocketConnection();

	void SetSocket(QTcpSocket* socket) { socket_ = socket; }
	QTcpSocket* GetSocket() { return socket_; }

	void SetName(const QString& name) { name_ = name; }
	QString GetName() { return name_; }
	bool GetIsDataMasked() { return is_data_masked_; }
	public slots:
	void OnReadyRead();

signals:
	void ConnectStateChanged(QAbstractSocket::SocketState);
	void RecvMessage(const QString&);

private:
	const static QString NewLine;

	QTcpSocket* socket_;
	QString name_;
	bool is_data_masked_;
	bool is_handshaked_;

	QString handshake_msg_;

	QString ComputeWebSocketHandshakeSecurityHash09(const QString& secWebSocketKey);
};

#endif // SOCKETCONNECTION_H