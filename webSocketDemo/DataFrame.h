#pragma once

#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <QObject>
#include <QDebug>
#include <QTextCodec>

class DataFrameHeader {
private:
	bool fin_;
	bool rsv1_;
	bool rsv2_;
	bool rsv3_;
	quint8 op_code_;
	bool mask_code_;
	quint8 payload_length_;

public:
	DataFrameHeader() {}


	bool Fin() { return fin_; }
	bool Rsv1() { return rsv1_; }
	bool Rsv2() { return rsv2_; }
	bool Rsv3() { return rsv3_; }
	quint8 OpCode() { return op_code_; }
	bool HasMask() { return mask_code_; }
	quint8 PayloadLength() { return payload_length_; }

	bool DecodeHeader(const QByteArray& buffer);
	void EncodeHeader(bool fin, bool rsv1, bool rsv2, bool rsv3, quint8 opcode, bool hasmask, quint8 length);

	QByteArray GetByteArray();
};

class DataFrame : public QObject
{
public:
	DataFrame(const QByteArray& buffer);
	DataFrame(const QString& content);
	DataFrame(QString& content,const QString& flag);
	~DataFrame();

	QByteArray GetByteArray();
	QString Text();
	DataFrameHeader Header() { return header_; }
private:
	DataFrameHeader header_;
	QByteArray extend_;
	QByteArray mask_;
	QByteArray content_;

	void Mask(QByteArray& data, const QByteArray& mask);
};

#endif // DATAFRAME_H