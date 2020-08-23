#include "CProjectItem.h"

CProjectItem::CProjectItem()
{
    name_ = "";
    srcDir_ = ""; 
    srcMask_ = ""; 
    headerMask_ = ""; 
    tagUpdateDateTime_ = "";
    projectCreateDateTime_ = "";
    labels_ = ""; 
}

CProjectItem::CProjectItem(const QString& name, const QString& srcDir, const QString& srcMask, const QString& headerMask, 
						   const QString& tagUpdateDateTime, const QString& projectCreateDateTime, const QString& labels)
{
    name_ = name;
    srcDir_ = srcDir;
    srcMask_ = srcMask;
    headerMask_ = headerMask;

	tagUpdateDateTime_ = tagUpdateDateTime;
	projectCreateDateTime_ = projectCreateDateTime;
    labels_ = labels;
}




