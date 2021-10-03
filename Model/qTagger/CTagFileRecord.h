#ifndef CTAG_FILE_RECORD_H_
#define CTAG_FILE_RECORD_H_

#include <QVector>

class CTagFileRecord
{
public:

    CTagFileRecord();
	CTagFileRecord(unsigned fileId, const QVector<unsigned long>& lineNum);

    virtual ~CTagFileRecord() {};

	unsigned long fileId_;
    QVector<unsigned long> lineNum_;
};

#endif // CTAG_FILE_RECORD_H_




