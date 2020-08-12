#ifndef TESTCUTILS_H

#define TESTCUTILS_H
#include <QtTest>
#include "CUtils.h"

typedef QVector<BYTE> BYTE_VECTOR;

class TestCUtils: public QObject
{
Q_OBJECT
public:
	TestCUtils() {};
    ~TestCUtils() {};
private slots:
	void initTestCase();
	void encode_data();
	void encode();
	
	void decode_data();
	void decode();
	void cleanupTestCase();

private:
	static const long BUFLEN = 5000;

};

Q_DECLARE_METATYPE(BYTE_VECTOR)

#endif // TESTCUTILS_H

