#ifndef CPROFILE_ITEM_H
#define CPROFILE_ITEM_H

#include <QtXml>
#include <QTextStream>
#include <QString>

class CProfileItem
{
public:

    CProfileItem();

    CProfileItem(const QString& name, const QString& srcDir, const QString& srcMask, const QString& headerMask, 
						   const QString& tagUpdateDateTime, const QString& profileCreateDateTime, const QString& labels);

    QString name_,
            srcDir_,
            srcMask_,
            headerMask_,
			
			tagUpdateDateTime_,
			profileCreateDateTime_,
			
			labels_;

    virtual ~CProfileItem() {};
};

#endif // CPROFILE_ITEM_H




