#include <QTextCodec>
#include <QStringList>
#include <QCryptographicHash>

#include "SocketConnection.h"
#include "DataFrame.h"

const QString SocketConnection::NewLine = "\r\n";

SocketConnection::SocketConnection(QObject *parent)
	: QObject(parent)
{
	socket_ = NULL;
	is_data_masked_ = false;
	is_handshaked_ = false;

	handshake_msg_ = "HTTP/1.1 101 Switching Protocols" + NewLine;
	handshake_msg_ += "Upgrade: WebSocket" + NewLine;
	handshake_msg_ += "Connection: Upgrade" + NewLine;
	handshake_msg_ += "Sec-WebSocket-Accept: %1" + NewLine;
	handshake_msg_ += NewLine;
}

SocketConnection::~SocketConnection()
{

}

void SocketConnection::OnReadyRead() {
	QTcpSocket* socket = static_cast<QTcpSocket *>(QObject::sender());

	QByteArray msg_ba = socket->readAll();
	QString msg = QTextCodec::codecForName("UTF-8")->toUnicode(msg_ba);

	if (!is_handshaked_) {
		is_data_masked_ = true;
		QStringList handshakeLines = msg.split(NewLine);

		QString acceptKey;
		for (int i = 0; i < handshakeLines.size(); ++i) {
			qDebug() << handshakeLines[i];

			int commaIdx = -1;
			if (handshakeLines[i].indexOf("Sec-WebSocket-Key:") != -1) {
				QString secWebSocketKey = handshakeLines[i].right(handshakeLines[i].size() - (handshakeLines[i].indexOf(":") + 2));

				acceptKey = ComputeWebSocketHandshakeSecurityHash09(secWebSocketKey);
			}
		}

		handshake_msg_ = handshake_msg_.arg(acceptKey);

		qint64 w = socket_->write(handshake_msg_.toUtf8());
		qDebug() << "Handshake write bytes: " << w;

		is_handshaked_ = true;
	}
	else {
		QString msgRecved;

		DataFrame dataFrame(msg_ba);

		if (!is_data_masked_) {
			socket_->close();
		}
		else if (dataFrame.Header().OpCode() == 0x08) {
			qDebug() << "接受到的信息 [\"logout:" << name_ << "\"]";
			socket_->close();
		}
		else {
			msgRecved = dataFrame.Text();
		}

		if (!msgRecved.isEmpty()) {
			qDebug() << "接受到的信息 [\"" << msgRecved << "\"]";
			emit RecvMessage(msgRecved);
		}
	}
}

QString SocketConnection::ComputeWebSocketHandshakeSecurityHash09(const QString& secWebSocketKey) {
	const QString MagicKEY = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	QString secWebSocketAccept;
	// 1. Combine the request Sec-WebSocket-Key with magic key.
	QString ret = secWebSocketKey + MagicKEY;
	// 2. Compute the SHA1 hash
	QByteArray sha1 = QCryptographicHash::hash(ret.toLatin1(), QCryptographicHash::Sha1);
	// 3. Base64 encode the hash
	secWebSocketAccept = QString(sha1.toBase64());

	return secWebSocketAccept;
}