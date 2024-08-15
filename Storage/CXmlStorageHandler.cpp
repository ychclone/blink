#include <QFile>
#include <QMessageBox>
#include <QtXml>
#include <QMap>

#include <iostream>
#include "CXmlStorageHandler.h"

BYTE CXmlStorageHandler::loadFromFile(const QString& filename, QMap<QString, CProjectItem>& projectMap)
{
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        return LoadFileError;
    }

    QDomDocument doc("ProjectML");

	QString errMsg;
	int errLine, errCol;

    if (!doc.setContent(&file, false, &errMsg, &errLine, &errCol)) {
        qDebug() << "QDomDocument setContent() failed in CXmlStorageHandler::loadFromFile()!" << Qt::endl;
		qDebug() << "err:" << errMsg << ", line:" << errLine << " column:" << errCol << Qt::endl;
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
    element.setAttribute("dirToExclude", projectItem.dirToExclude_);
    element.setAttribute("fileMaskToExclude", projectItem.fileMaskToExclude_);

    return element;
}

void CXmlStorageHandler::fillProjectItem(CProjectItem& projectItemToBeFill, const QDomElement& element)
{
	QString name, srcDir, srcMask, headerMask;
	QString tagUpdateDateTime, projectCreateDateTime;
    QString labels, dirToExclude, fileMaskToExclude;

    name = element.attribute("name", "");
    srcDir = element.attribute("srcDir", "");
    srcMask = element.attribute("srcMask", "");
    headerMask = element.attribute("headerMask", "");
    labels = element.attribute("labels", "");
    dirToExclude = element.attribute("dirToExclude", "");
    fileMaskToExclude = element.attribute("fileMaskToExclude", "");

	tagUpdateDateTime = element.attribute("tagUpdateDateTime", "");
	projectCreateDateTime = element.attribute("projectCreateDateTime", "");

	CProjectItem projectItem(name, srcDir, srcMask, headerMask,
		tagUpdateDateTime, projectCreateDateTime, labels, dirToExclude, fileMaskToExclude);

	projectItemToBeFill = projectItem;
}

BYTE CXmlStorageHandler::saveToFile(const QString& filename, const QMap<QString, CProjectItem>& projectMap)
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


