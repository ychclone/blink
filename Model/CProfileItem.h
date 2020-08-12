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

    QString m_name,
            m_srcDir,
            m_srcMask,
            m_headerMask,
			
			m_tagUpdateDateTime,
			m_profileCreateDateTime,
			
			m_labels;

    virtual ~CProfileItem() {};
};

#endif // CPROFILE_ITEM_H




