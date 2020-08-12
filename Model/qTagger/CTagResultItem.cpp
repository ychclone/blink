#include "CTagResultItem.h"

const char* CTagResultItem::kFIELD_RESULT_SEPERATOR = ":";

CTagResultItem::CTagResultItem()
{

}

CTagResultItem::~CTagResultItem()
{

}

ostream& operator<<(ostream& ost, const CTagResultItem& item)
{
	QString resultStr = (item.filePath_ + CTagResultItem::kFIELD_RESULT_SEPERATOR + QString::number(item.fileLineNum_) + CTagResultItem::kFIELD_RESULT_SEPERATOR + item.fileLineSrc_);
    
	ost << resultStr.toStdString();

    return ost;
}




