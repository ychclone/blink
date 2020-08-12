#include "Display/CMainWindow.h"

#include <QApplication>
#include <QtXml>

#include <QDesktopWidget>

#include "Display/CEventFilterObj.h"
#include "Model/CConfigManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CMainWindow *window = new CMainWindow;

	CConfigManager* confManager;
	confManager	= CConfigManager::getInstance();

    CXmlStorageHandler xmlStorageHandler;

    CProfileManager::getInstance()->setProfileFile("record.xml");
    CProfileManager::getInstance()->setStorageHandler(xmlStorageHandler);
    CProfileManager::getInstance()->attachStorage();

    // load profile list after storage ready
	window->loadProfileList();

    // load group list after storage ready
	window->loadGroupList();

    // Center the main window on screen for the first time
    QDesktopWidget desktopWidget; // get desktop info

	QByteArray savedGeometry;

	savedGeometry = confManager->getValue("Window", "geometry").toByteArray();
	window->restoreGeometry(savedGeometry);

	QString splitterSizeStr;
	QStringList splitterSizeStrList;
	QList<int> splitterSizeList;

    // tab widget
    window->restoreTabWidgetPos();

	// splitter
    splitterSizeStr = confManager->getValue("Window", "splitter").toString();
    splitterSizeStr = splitterSizeStr.trimmed();

	splitterSizeStrList = splitterSizeStr.split(" ", QString::SkipEmptyParts);

    foreach (const QString& splitterSize, splitterSizeStrList) {
		splitterSizeList.append(splitterSize.toInt());
	}

	window->setSplitterSizes(splitterSizeList);

    qDebug() << desktopWidget.screenGeometry();

    qDebug() << window->geometry();
    qDebug() << window->frameGeometry();

    window->show();

	// event filter for show/hide menuBar
	CEventFilterObj *keyPressObj = new CEventFilterObj();
	app.installEventFilter(keyPressObj);

    return app.exec();
}

