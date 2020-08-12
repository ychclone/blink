#include "CProfileItem.h"

CProfileItem::CProfileItem()
{
    m_name = "";
    m_srcDir = ""; 
    m_srcMask = ""; 
    m_headerMask = ""; 
    m_tagUpdateDateTime = "";
    m_profileCreateDateTime = "";
    m_labels = ""; 
}

CProfileItem::CProfileItem(const QString& name, const QString& srcDir, const QString& srcMask, const QString& headerMask, 
						   const QString& tagUpdateDateTime, const QString& profileCreateDateTime, const QString& labels)
{
    m_name = name;
    m_srcDir = srcDir;
    m_srcMask = srcMask;
    m_headerMask = headerMask;

	m_tagUpdateDateTime = tagUpdateDateTime;
	m_profileCreateDateTime = profileCreateDateTime;
    m_labels = labels;
}




