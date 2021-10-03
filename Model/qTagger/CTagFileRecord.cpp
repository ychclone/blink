#include "CTagFileRecord.h"

CTagFileRecord::CTagFileRecord()
{
	fileId_ = 0;
}

CTagFileRecord::CTagFileRecord(unsigned fileId, const QList<unsigned long>& lineNum)
{
	fileId_ = fileId;
	lineNum_ = lineNum;
}
