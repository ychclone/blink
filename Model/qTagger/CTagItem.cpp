#include "CTagItem.h"

CTagItem::CTagItem()
{
    tag_ = "";
}

CTagItem::CTagItem(const QString& tag, const QVector<CTagFileRecord>& tagFileRecord)
{
    tag_ = tag;
    tagFileRecord_ = tagFileRecord;
}




