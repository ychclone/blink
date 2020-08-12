#include "TestCRecordPool.h"

void TestCRecordPool::initTestCase()
{
	m_record = new CRecord("test", 1);
	m_recordItem = new CRecordItem(1);
	m_record->addItem(m_recordItem);
}

void TestCRecordPool::compress()
{
	unsigned char buffer[5000];
	unsigned char testData1[] = {0, 1};
	long bufLen;
	long maxBufferLen = 5000;

	QVERIFY(m_record != 0); 
	m_record->packBuffer(static_cast <void *> (buffer), bufLen, 5000);
	QCOMPARE(buffer, testData1);
	
}

void TestCRecordPool::cleanupTestCase()
{
	delete m_record;
	m_record = 0;
}

QTEST_MAIN(TestCRecordPool)


