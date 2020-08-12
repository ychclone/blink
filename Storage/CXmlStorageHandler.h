#ifndef CXML_STORAGE_HANDLER_H
#define CXML_STORAGE_HANDLER_H

#include <QList>
#include "Utils/commonType.h"
#include "Model/CProfileItem.h"
#include "Model/CGroupItem.h" 
#include "IStorageHandler.h"

class CXmlStorageHandler : IStorageHandler
{
public:
    enum FileError {
        NoError = 0,
        LoadFileError = 1,
        InvalidFileError = 2,
        SaveFileError = 3
    };

    CXmlStorageHandler() {};
    virtual ~CXmlStorageHandler() {};
    
    BYTE loadFromFile(const QString& filename, QMap<QString, CProfileItem>& profileMap, QMap<QString, CGroupItem>& groupMap);
    BYTE saveToFile(const QString& filename, const QMap<QString, CProfileItem>& profileMap, const QMap<QString, CGroupItem>& groupMap);

private:
    QDomElement createXMLNode(QDomDocument& document, const CProfileItem& profileItem);
	QDomElement createXMLNode(QDomDocument& document, const CGroupItem& groupItem); 

	void fillProfileItem(CProfileItem& profileItemToBeFill, const QDomElement& element);
	void fillGroupItem(CGroupItem& groupItemToBeFill, const QDomElement& element);

};

#endif // CXML_STORAGE_HANDLER_H

