#include <QFile>
#include <QMessageBox>
#include <QtXml>
#include <QMap>

#include <iostream>
#include "CXmlStorageHandler.h"

BYTE CXmlStorageHandler::loadFromFile(const QString& filename, QMap<QString, CProjectItem>& projectMap, QMap<QString, CGroupItem>& groupMap)
{
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        return LoadFileError;
    }

    QDomDocument doc("ProjectML");

	QString errMsg;
	int errLine, errCol;

    if (!doc.setContent(&file, false, &errMsg, &errLine, &errCol)) {
        qDebug() << "QDomDocument setContent() failed in CXmlStorageHandler::loadFromFile()!" << endl;  
		qDebug() << "err:" << errMsg << ", line:" << errLine << " column:" << errCol << endl;
		file.close();
        return LoadFileError;
    }

    file.close();

	QDomElement docElement = doc.documentElement(); 
	
	// start from doc first child
	QDomNode docNode = docElement.firstChild();  

	while (!docNode.isNull()) { 
		QDomElement rootItem = docNode.toElement();

		if (rootItem.tagName() == "project") {
			projectMap.clear();
			QDomNode node = rootItem.firstChild();

			while (!node.isNull()) {
			   QDomElement element = node.toElement();
			   if (!element.isNull()) {
				  if (element.tagName() == "projectItem") {
					  CProjectItem item;

					  fillProjectItem(item, element);
					  projectMap[element.attribute("name", "")] = item;
				  }
				  node = node.nextSibling();
			   }
			}
		} else if (rootItem.tagName() == "group") {
			groupMap.clear();
			QDomNode node = rootItem.firstChild();

			while (!node.isNull()) {
			   QDomElement element = node.toElement();
			   if (!element.isNull()) {
				  if (element.tagName() == "groupItem") {
					  CGroupItem item;

					  fillGroupItem(item, element);
					  groupMap[element.attribute("name", "")] = item;
				  }
				  node = node.nextSibling();
			   }
			}
		}
		docNode = docNode.nextSibling(); 
	}

    return NoError;
}

QDomElement CXmlStorageHandler::createXMLNode(QDomDocument &document, const CProjectItem &projectItem)
{
    QDomElement element = document.createElement("projectItem");

    element.setAttribute("name", projectItem.name_);
    element.setAttribute("srcDir", projectItem.srcDir_);
    element.setAttribute("srcMask", projectItem.srcMask_);
    element.setAttribute("headerMask", projectItem.headerMask_);
	element.setAttribute("tagUpdateDateTime", projectItem.tagUpdateDateTime_); 
	element.setAttribute("projectCreateDateTime", projectItem.projectCreateDateTime_); 
    element.setAttribute("labels", projectItem.labels_);  

    return element;
}

QDomElement CXmlStorageHandler::createXMLNode(QDomDocument &document, const CGroupItem &groupItem)
{
    QDomElement element = document.createElement("groupItem");

    element.setAttribute("name", groupItem.name_);
    element.setAttribute("projectList", groupItem.projectList_);
	element.setAttribute("tagUpdateDateTime", groupItem.tagUpdateDateTime_); 
	element.setAttribute("groupCreateDateTime", groupItem.groupCreateDateTime_); 
    element.setAttribute("labels", groupItem.labels_);  

    return element;
} 

void CXmlStorageHandler::fillProjectItem(CProjectItem& projectItemToBeFill, const QDomElement& element)
{
	QString name, srcDir, srcMask, headerMask;
	QString tagUpdateDateTime, projectCreateDateTime;

    QString labels;
    
    name = element.attribute("name", "");
    srcDir = element.attribute("srcDir", ""); 
    srcMask = element.attribute("srcMask", ""); 
    headerMask = element.attribute("headerMask", "");
    labels = element.attribute("labels", "");

	tagUpdateDateTime = element.attribute("tagUpdateDateTime", ""); 
	projectCreateDateTime = element.attribute("projectCreateDateTime", ""); 
    
	CProjectItem projectItem(name, srcDir, srcMask, headerMask, 
		tagUpdateDateTime, projectCreateDateTime, labels);

	projectItemToBeFill = projectItem;
}

void CXmlStorageHandler::fillGroupItem(CGroupItem& groupItemToBeFill, const QDomElement& element)
{
	QString name;
	QString projectList;
	QString tagUpdateDateTime, groupCreateDateTime; 
    QString labels;
    
    name = element.attribute("name", "");
	projectList = element.attribute("projectList", "");

	tagUpdateDateTime = element.attribute("tagUpdateDateTime", ""); 
	groupCreateDateTime = element.attribute("groupCreateDateTime", ""); 
	
    labels = element.attribute("labels", "");

	CGroupItem groupItem(name, projectList, tagUpdateDateTime, groupCreateDateTime, labels);

	groupItemToBeFill = groupItem;
}


BYTE CXmlStorageHandler::saveToFile(const QString& filename, const QMap<QString, CProjectItem>& projectMap, const QMap<QString, CGroupItem>& groupMap)
{
    QDomDocument doc("ProjectML");

	// record
	QDomElement recordRoot = doc.createElement("record"); 
	doc.appendChild(recordRoot); 

	// project
    QDomElement projectRoot = doc.createElement("project");
    recordRoot.appendChild(projectRoot);

    CProjectItem projectItem;
    foreach (projectItem, projectMap) {
        QDomElement node = createXMLNode(doc, projectItem);
        projectRoot.appendChild(node);
    }

	// group
    QDomElement groupRoot = doc.createElement("group");
    recordRoot.appendChild(groupRoot);

    CGroupItem groupItem;
    foreach (groupItem, groupMap) {
        QDomElement node = createXMLNode(doc, groupItem);
        groupRoot.appendChild(node);
    }

#ifdef DEBUG_XML
    QDomElement element = doc.createElement("projectItem");
    element.setAttribute("name", "a1");
    element.setAttribute("srcDir", "a2"); 
    element.setAttribute("srcMask", "a3"); 
    element.setAttribute("headerMask", "a4");
    element.setAttribute("labels", "a5");
    root.appendChild(element); 
#endif

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        //QMessageBox::warning(this, "Saving", "Failed to save file.", QMessageBox::Ok, QMessageBox::NoButton);  /* <-- YCH modified 31/08/06, just for reference, to be removed */
        return SaveFileError;
    }

    QTextStream ts(&file);
    ts << doc.toString();
    file.close();

    return NoError;
}


