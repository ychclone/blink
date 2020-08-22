#include "CGroupItem.h"

CGroupItem::CGroupItem()
{
    name_ = "";
	projectList_ = ""; 
    tagUpdateDateTime_ = "";
    groupCreateDateTime_ = "";
	labels_ = "";
}

CGroupItem::CGroupItem(const QString& name, const QString& projectList, const QString& tagUpdateDateTime, 
						const QString& groupCreateDateTime, const QString& labels)
{
    name_ = name;

	projectList_ = projectList;

	tagUpdateDateTime_ = tagUpdateDateTime;
	groupCreateDateTime_ = groupCreateDateTime;

	labels_ = labels; 
}




