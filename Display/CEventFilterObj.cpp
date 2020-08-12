#include <QKeyEvent> 
#include <QApplication>
#include <QMenuBar>

#include <QMainWindow>

#include "Display/CMainWindow.h"
#include "Model/CConfigManager.h"
#include "CEventFilterObj.h"

bool CEventFilterObj::eventFilter(QObject *obj, QEvent *event)
{
	QPoint p;
	bool bAutoHideMenu;
	CConfigManager* confManager; 
	QKeyEvent *keyEvent;
	QMouseEvent *mouseEvent;

	// only process for the key press and mouse press event
	switch (event->type()) { 
        case QEvent::KeyPress:
        case QEvent::MouseButtonPress:
			break;
		default:
            return QObject::eventFilter(obj, event);
	}
	
	confManager	= CConfigManager::getInstance();
	
	bAutoHideMenu = confManager->getAppSettingValue("AutoHideMenu").toBool();

	// only hide/show menubar if active window is the main window
	if ((bAutoHideMenu) && (strcmp(QApplication::activeWindow()->metaObject()->className(), "CMainWindow") == 0)) {
		CMainWindow *mainWindow =  static_cast<CMainWindow *> (QApplication::activeWindow());
		
		switch (event->type()) {
			case QEvent::KeyPress:
				keyEvent = static_cast<QKeyEvent *>(event);

				if (keyEvent->key() == Qt::Key_Alt) {
					if (mainWindow->menuBar()->isHidden()) {
						mainWindow->menuBar()->show();
					}
				} else {
					if (!(mainWindow->menuBar()->isHidden())) {
						// only hide menu if menubar or menu not selected
						if ((strcmp(QApplication::focusWidget()->metaObject()->className(), "QMenu") == -1) &&
								(strcmp(QApplication::focusWidget()->metaObject()->className(), "QMenuBar") == -1)) {
							mainWindow->menuBar()->hide(); 
						}
					}
				}
				break;

			case QEvent::MouseButtonPress:
				mouseEvent = static_cast<QMouseEvent *>(event); 

				p = mainWindow->menuBar()->mapFromGlobal(mouseEvent->globalPos());  

				// hide menuBar only when not hidden
				if (!(mainWindow->menuBar()->isHidden())) { 
					// hide menuBar only when mouse not in menuBar area 
					if (!(mainWindow->menuBar()->rect().contains(p))) {
						mainWindow->menuBar()->hide(); 
					}
				}

				break;

			default:
				break;
		}

		return QObject::eventFilter(obj, event); 
	} else {
		return QObject::eventFilter(obj, event);
	}
} 

