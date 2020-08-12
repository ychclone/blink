#ifndef TESTCRECORDPOOL_H

#define TESTCRECORDPOOL_H
#include <QtTest>
#include "CRecordPool.h"

class TestCRecordPool: public QObject
{
Q_OBJECT
public:
	TestCRecordPool() {};
    ~TestCRecordPool() {};
private slots:
	void initTestCase();
	void compress();
	void cleanupTestCase();

private:
	CRecord* m_record;
	CRecordItem* m_recordItem;

};

#endif // TESTCRECORDPOOL_H

