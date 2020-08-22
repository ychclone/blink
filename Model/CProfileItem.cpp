#include "CProfileItem.h"

CProfileItem::CProfileItem()
{
    name_ = "";
    srcDir_ = ""; 
    srcMask_ = ""; 
    headerMask_ = ""; 
    tagUpdateDateTime_ = "";
    profileCreateDateTime_ = "";
    labels_ = ""; 
}

CProfileItem::CProfileItem(const QString& name, const QString& srcDir, const QString& srcMask, const QString& headerMask, 
						   const QString& tagUpdateDateTime, const QString& profileCreateDateTime, const QString& labels)
{
    name_ = name;
    srcDir_ = srcDir;
    srcMask_ = srcMask;
    headerMask_ = headerMask;

	tagUpdateDateTime_ = tagUpdateDateTime;
	profileCreateDateTime_ = profileCreateDateTime;
    labels_ = labels;
}




