#ifndef CTAG_FILE_RECORD_H_
#define CTAG_FILE_RECORD_H_

#include <QList>

class CTagFileRecord
{
public:

    CTagFileRecord();
	CTagFileRecord(unsigned fileId, const QList<unsigned long>& lineNum);

    virtual ~CTagFileRecord() {};

	unsigned long fileId_;
    QList<unsigned long> lineNum_;
};

#endif // CTAG_FILE_RECORD_H_




