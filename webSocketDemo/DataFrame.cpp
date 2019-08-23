#include <QByteArray>
#include <QTextCodec>

#include "DataFrame.h"

bool DataFrameHeader::DecodeHeader(const QByteArray& buffer) {
	if (buffer.size() < 2) return false;

	//第一个字节
	fin_ = (buffer[0] & 0x80) == 0x80;
	rsv1_ = (buffer[0] & 0x40) == 0x40;
	rsv2_ = (buffer[0] & 0x20) == 0x20;
	rsv3_ = (buffer[0] & 0x10) == 0x10;
	op_code_ = buffer[0] & 0x0f;

	//第二个字节
	mask_code_ = (buffer[1] & 0x80) == 0x80;
	payload_length_ = buffer[1] & 0x7f;

	return true;
}

void DataFrameHeader::EncodeHeader(bool fin, bool rsv1, bool rsv2, bool rsv3, quint8 opcode, bool hasmask, quint8 length) {
	fin_ = fin;
	rsv1_ = rsv1;
	rsv2_ = rsv2;
	rsv3_ = rsv3;
	op_code_ = opcode;
	mask_code_ = hasmask;
	payload_length_ = length;
}

QByteArray DataFrameHeader::GetByteArray() {
	QByteArray header_ba(2, '\0');

	if (fin_) header_ba[0] = header_ba[0] ^ 0x80;
	if (rsv1_) header_ba[0] = header_ba[0] ^ 0x40;
	if (rsv2_) header_ba[0] = header_ba[0] ^ 0x20;
	if (rsv3_) header_ba[0] = header_ba[0] ^ 0x10;
	header_ba[0] = header_ba[0] ^ op_code_;

	if (mask_code_) header_ba[1] = header_ba[1] ^ 0x80;
	header_ba[1] = header_ba[1] ^ payload_length_;

	return header_ba;
}

DataFrame::DataFrame(const QByteArray& buffer) {
	header_.DecodeHeader(buffer);

	if (header_.PayloadLength() == 126) {
		extend_.resize(2);
		extend_ = buffer.mid(2, 2);
	}
	else if (header_.PayloadLength() == 127) {
		extend_.resize(8);
		extend_ = buffer.mid(2, 8);
	}

	if (header_.HasMask()) {
		mask_.resize(4);
		mask_ = buffer.mid(2 + extend_.size(), 4);
	}

	if (extend_.size() == 0) {
		content_.resize(header_.PayloadLength());
		content_ = buffer.mid(2 + extend_.size() + mask_.size(), content_.size());
	}
	else if (extend_.size() == 2) {
		quint16 contentLength = (quint16)extend_[0] * 256 + extend_[1];
		content_.resize(contentLength);
		content_ = buffer.mid(2 + extend_.size() + mask_.size(), contentLength);
	}
	else {
		quint64 contentLength = 0;
		int n = 1;
		for (int i = 7; i >= 0; --i) {
			contentLength += (quint64)extend_[i] * n;
			n *= 256;
		}
		content_.resize(contentLength);
		content_ = buffer.mid(2 + extend_.size() + mask_.size(), contentLength);
	}

	if (header_.HasMask())
		Mask(content_, mask_);
}

DataFrame::DataFrame(const QString& content)
{
	content_ = content.toUtf8();
	int length = content_.size();

	if (length < 126) {
		header_.EncodeHeader(true, false, false, false, 1, false, length);
	}
	else if (length < 65536) {
		extend_.resize(2);
		header_.EncodeHeader(true, false, false, false, 1, false, 126);
		extend_[0] = length / 256;
		extend_[1] = length % 256;
	}
	else {
		extend_.resize(8);
		header_.EncodeHeader(true, false, false, false, 1, false, 127);

		int left = length;
		int unit = 256;

		for (int i = 7; i > 1; i--)
		{
			extend_[i] = left % unit;
			left = left / unit;

			if (left == 0)
				break;
		}
	}
}

DataFrame::DataFrame(QString& content, const QString& flag)
{
	content_ = content.toLocal8Bit();

	qDebug() << "content_:" << content_;
	char* data0 = content_.data();

	//content_ = content.toUtf8();
	int length = content_.size();

	if (length < 126) {
		header_.EncodeHeader(true, false, false, false, 1, false, length);
	}
	else if (length < 65536) {
		extend_.resize(2);
		header_.EncodeHeader(true, false, false, false, 1, false, 126);
		extend_[0] = length / 256;
		extend_[1] = length % 256;
	}
	else {
		extend_.resize(8);
		header_.EncodeHeader(true, false, false, false, 1, false, 127);

		int left = length;
		int unit = 256;

		for (int i = 7; i > 1; i--)
		{
			extend_[i] = left % unit;
			left = left / unit;

			if (left == 0)
				break;
		}
	}
}

DataFrame::~DataFrame()
{

}

QByteArray DataFrame::GetByteArray() {
	QByteArray ba;

	ba += header_.GetByteArray();
	ba += extend_;
	ba += mask_;
	ba += content_;

	qDebug() << ba;
	return ba;
	
}

QString DataFrame::Text() {
	if (header_.OpCode() != 1)
		return QString();

	//return QTextCodec::codecForName("UTF-8")->toUnicode(content_);
	return QTextCodec::codecForName("gb2312")->toUnicode(content_);
}

void DataFrame::Mask(QByteArray& data, const QByteArray& mask) {
	for (int i = 0; i < data.size(); ++i) {
		data[i] = data[i] ^ mask[i % 4];
	}
}