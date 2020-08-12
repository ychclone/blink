#include "TestCUtils.h"

void TestCUtils::initTestCase()
{
	qRegisterMetaType<BYTE_VECTOR> ("BYTE_VECTOR");

}

void TestCUtils::encode_data()
{
	QTest::addColumn<BYTE_VECTOR>("inBuf");
	QTest::addColumn<BYTE_VECTOR>("expectedBuf"); 

	BYTE_VECTOR bufInput, bufExpected;	
	
	bufInput.clear(); bufExpected.clear();
	bufInput << 0x00 << 0x00 << 0x00 << 0x01;
	bufExpected << 0x01;
	QTest::newRow("encodeToByte") << bufInput << bufExpected;

	bufInput.clear(); bufExpected.clear();
	bufInput << 0x00 << 0x00 << 0x00 << 0x7F;
	bufExpected << 0x7F;	
	QTest::newRow("encodeToByteBoundary") << bufInput << bufExpected;
	
	bufInput.clear(); bufExpected.clear();
	bufInput << 0x00 << 0x00 << 0x00 << 0x80;
	bufExpected << 0x80 << 0x80;	
	QTest::newRow("encodeToWord") << bufInput << bufExpected;

	bufInput.clear(); bufExpected.clear();
	bufInput << 0x00 << 0x00 << 0x3F << 0xFF;
	bufExpected << 0xBF << 0xFF;	
	QTest::newRow("encodeToWordBoundary") << bufInput << bufExpected;
                                            
	bufInput.clear(); bufExpected.clear();
	bufInput << 0x00 << 0x00 << 0x40 << 0x00;
	bufExpected << 0xC0 << 0x00 << 0x40 << 0x00;
	QTest::newRow("encodeToDWord") << bufInput << bufExpected;

	bufInput.clear(); bufExpected.clear();
	bufInput << 0x3F << 0xFF << 0xFF << 0xFF;
	bufExpected << 0xFF << 0xFF << 0xFF << 0xFF;
	QTest::newRow("encodeToDWordBoundary") << bufInput << bufExpected;

	bufInput.clear(); bufExpected.clear();
	bufInput << 0x00 << 0x00 << 0x00 << 0x7F << 0x00 << 0x00 << 0x00 << 0x80;
	bufExpected << 0x7F << 0x80 << 0x80;
	QTest::newRow("encodeToByteWord") << bufInput << bufExpected;

	// due to stored as little endian, commented by default
	//bufInput.clear(); bufExpected.clear(); 
	//bufInput << 0x40 << 0x00 << 0x00 << 0x00 << 0x6D << 0x00 << 0x00 << 0x00;
	//bufExpected << 0xFF;
    //QTest::newRow("encodeErrRunTimeReturnSizeZero") << bufInput << bufExpected;

}

void TestCUtils::encode()
{
	QFETCH(BYTE_VECTOR, inBuf);
	QFETCH(BYTE_VECTOR, expectedBuf); 

	BYTE inBufByte[BUFLEN];
	BYTE outBufByte[BUFLEN];
	BYTE expectedBufByte[BUFLEN];

	long outBufLen;
	int i;

    // input, output
	for (i = 0; i < inBuf.size(); ++i) {
        inBufByte[i] = inBuf[i];
	}
	CUtils::encodeDWORDSeq(inBufByte, inBuf.size(), outBufByte, outBufLen);

	// expected, expected length
	QCOMPARE(outBufLen, static_cast <long> (expectedBuf.size()));
	
	for (i = 0; i < expectedBuf.size(); ++i) {
        expectedBufByte[i] = expectedBuf[i];
	}
	
	// comparision
	QString outStr = CUtils::bufToQString(outBufByte, outBufLen);;
	QString expectedStr = CUtils::bufToQString(expectedBufByte, outBufLen);

	QCOMPARE(outStr, expectedStr);
}

void TestCUtils::decode_data()
{
	QTest::addColumn<BYTE_VECTOR>("inBuf");
	QTest::addColumn<BYTE_VECTOR>("expectedBuf"); 

	BYTE_VECTOR bufInput, bufExpected;	
	
	bufInput.clear(); bufExpected.clear();
	bufInput << 0x01;
	bufExpected << 0x00 << 0x00 << 0x00 << 0x01;
	QTest::newRow("decodeFromByte") << bufInput << bufExpected;

	bufInput.clear(); bufExpected.clear();
	bufInput << 0x7F;
	bufExpected << 0x00 << 0x00 << 0x00 << 0x7F;
	QTest::newRow("decodeFromByteBoundary") << bufInput << bufExpected;
	
	bufInput.clear(); bufExpected.clear();
	bufInput << 0x80 << 0x80;
	bufExpected << 0x00 << 0x00 << 0x00 << 0x80;
	QTest::newRow("decodeFromWord") << bufInput << bufExpected;

	bufInput.clear(); bufExpected.clear();
	bufInput << 0xBF << 0xFF;
	bufExpected << 0x00 << 0x00 << 0x3F << 0xFF;
	QTest::newRow("decodeFromWordBoundary") << bufInput << bufExpected;
                                            
	bufInput.clear(); bufExpected.clear();      
	bufInput << 0xC0 << 0x00 << 0x40 << 0x00;
	bufExpected << 0x00 << 0x00 << 0x40 << 0x00;
	QTest::newRow("decodeFromDWord") << bufInput << bufExpected;

	bufInput.clear(); bufExpected.clear();
	bufInput << 0xFF << 0xFF << 0xFF << 0xFF;
	bufExpected << 0x3F << 0xFF << 0xFF << 0xFF;
	QTest::newRow("decodeFromDWordBoundary") << bufInput << bufExpected;

	bufInput.clear(); bufExpected.clear();
	bufInput << 0x7F << 0x80 << 0x80;
	bufExpected << 0x00 << 0x00 << 0x00 << 0x7F << 0x00 << 0x00 << 0x00 << 0x80; 
	QTest::newRow("decodeFromByteWord") << bufInput << bufExpected;
}

void TestCUtils::decode()
{
	QFETCH(BYTE_VECTOR, inBuf);
	QFETCH(BYTE_VECTOR, expectedBuf); 

	BYTE inBufByte[BUFLEN];
	BYTE outBufByte[BUFLEN];
	BYTE expectedBufByte[BUFLEN];

	long outBufLen;
	int i;

    // input, output
	for (i = 0; i < inBuf.size(); ++i) {
        inBufByte[i] = inBuf[i];
	}
	CUtils::decodeDWORDSeq(inBufByte, inBuf.size(), outBufByte, outBufLen);

	// expected, expected length
	QCOMPARE(outBufLen, static_cast <long> (expectedBuf.size()));
	
	for (i = 0; i < expectedBuf.size(); ++i) {
        expectedBufByte[i] = expectedBuf[i];
	}
	
	// comparision
	QString outStr = CUtils::bufToQString(outBufByte, outBufLen);;
	QString expectedStr = CUtils::bufToQString(expectedBufByte, outBufLen);

	QCOMPARE(outStr, expectedStr);
}

void TestCUtils::cleanupTestCase()
{
}

QTEST_MAIN(TestCUtils)


