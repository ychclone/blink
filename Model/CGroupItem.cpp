#include "CGroupItem.h"

CGroupItem::CGroupItem()
{
    name_ = "";
	profileList_ = ""; 
    tagUpdateDateTime_ = "";
    groupCreateDateTime_ = "";
	labels_ = "";
}

CGroupItem::CGroupItem(const QString& name, const QString& profileList, const QString& tagUpdateDateTime, 
						const QString& groupCreateDateTime, const QString& labels)
{
    name_ = name;

	profileList_ = profileList;

	tagUpdateDateTime_ = tagUpdateDateTime;
	groupCreateDateTime_ = groupCreateDateTime;

	labels_ = labels; 
}




