#ifndef CXML_STORAGE_HANDLER_H
#define CXML_STORAGE_HANDLER_H

#include <QList>
#include "Utils/commonType.h"
#include "Model/CProjectItem.h"
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

    BYTE loadFromFile(const QString& filename, QMap<QString, CProjectItem>& projectMap);
    BYTE saveToFile(const QString& filename, const QMap<QString, CProjectItem>& projectMap);

private:
    QDomElement createXMLNode(QDomDocument& document, const CProjectItem& projectItem);

	void fillProjectItem(CProjectItem& projectItemToBeFill, const QDomElement& element);

};

#endif // CXML_STORAGE_HANDLER_H

