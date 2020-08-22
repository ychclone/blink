#include "TestCRecordPool.h"

void TestCRecordPool::initTestCase()
{
	record_ = new CRecord("test", 1);
	recordItem_ = new CRecordItem(1);
	record_->addItem(recordItem_);
}

void TestCRecordPool::compress()
{
	unsigned char buffer[5000];
	unsigned char testData1[] = {0, 1};
	long bufLen;
	long maxBufferLen = 5000;

	QVERIFY(record_ != 0); 
	record_->packBuffer(static_cast <void *> (buffer), bufLen, 5000);
	QCOMPARE(buffer, testData1);
	
}

void TestCRecordPool::cleanupTestCase()
{
	delete record_;
	record_ = 0;
}

QTEST_MAIN(TestCRecordPool)


