#include "CGroupItem.h"

CGroupItem::CGroupItem()
{
    m_name = "";
	m_profileList = ""; 
    m_tagUpdateDateTime = "";
    m_groupCreateDateTime = "";
	m_labels = "";
}

CGroupItem::CGroupItem(const QString& name, const QString& profileList, const QString& tagUpdateDateTime, 
						const QString& groupCreateDateTime, const QString& labels)
{
    m_name = name;

	m_profileList = profileList;

	m_tagUpdateDateTime = tagUpdateDateTime;
	m_groupCreateDateTime = groupCreateDateTime;

	m_labels = labels; 
}




