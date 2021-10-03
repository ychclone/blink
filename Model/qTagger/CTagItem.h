#ifndef CTAG_ITEM_H
#define CTAG_ITEM_H

#include <QString>
#include "CTagFileRecord.h"

class CTagItem
{
public:

    CTagItem();
	CTagItem(const QString& tag, const QVector<CTagFileRecord>& tagFileRecord);

    virtual ~CTagItem() {};

	QString tag_;
    QVector<CTagFileRecord> tagFileRecord_;
};

#endif // CTAG_ITEM_H




