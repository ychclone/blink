#include "CSearchWebView.h"
#include <QMessageBox> 

#ifdef Q_OS_WIN
	#include <qt_windows.h>
#endif

CSearchWebView::CSearchWebView(QWidget *parent)
	: QWebView(parent)
{
	selectAllShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
	connect(selectAllShortcut, SIGNAL(activated()), this, SLOT(on_selectAllPressed()));  

	page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(this, SIGNAL(linkClicked(const QUrl &)), this, SLOT(on_urlClicked(const QUrl &))); 

	m_confManager = CConfigManager::getInstance();
}

void CSearchWebView::webZoomIn()
{
	qreal currentZoomFactor = zoomFactor();

	setZoomFactor(currentZoomFactor * 1.1);
}

void CSearchWebView::webZoomOut()
{
	qreal currentZoomFactor = zoomFactor();

	setZoomFactor(currentZoomFactor * 0.9);
}

bool CSearchWebView::findText(const QString & subString, QWebPage::FindFlags options)
{
	return QWebView::findText(subString, options);
}

void CSearchWebView::wheelEvent(QWheelEvent *e)
{
	if (e->modifiers() == Qt::ControlModifier) {
		e->accept();
		if (e->delta() > 0) {
			webZoomIn();
		} else {
			webZoomOut();
		}
		QWebView::wheelEvent(e);
	} else {
		QWebView::wheelEvent(e);
	}
}

void CSearchWebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

	menu.addAction(pageAction(QWebPage::SelectAll));
	menu.addAction(pageAction(QWebPage::Copy)); 
	menu.exec(event->globalPos()); 

    // bypass original context menu, e.g. reload
	//QWebView::contextMenuEvent(event);
}

void CSearchWebView::on_selectAllPressed()
{
	this->triggerPageAction(QWebPage::SelectAll);
}

void CSearchWebView::on_urlClicked(const QUrl& urlClicked)
{
//	QMessageBox::information(this, "CSearchWebView",  urlClicked.path(), QMessageBox::Ok); 

	QString filePath = urlClicked.path();
	filePath.remove(0, 1); // remove beginning "/" character

	QString executeDir = "";

	QString lineNum = urlClicked.fragment();
	lineNum.remove(0, 4); // remove beginning "line" string

#ifdef Q_OS_WIN 
	QString cmdParam;

	QString executeMethod = "open";
	QString consoleCommnad = m_confManager->getAppSettingValue("DefaultEditor").toString();

	if (consoleCommnad.endsWith("gvim.exe")) {
        cmdParam = "+" + lineNum + " " + filePath;  // +lineNum fileName 
	} else {
	   	cmdParam = filePath;  // fileName 
	}

	ShellExecute(NULL, reinterpret_cast<const wchar_t*>(executeMethod.utf16()), reinterpret_cast<const wchar_t*>(consoleCommnad.utf16()), 
		reinterpret_cast<const wchar_t*> (cmdParam.utf16()), reinterpret_cast<const wchar_t*>(executeDir.utf16()), SW_NORMAL);
#else
	// not implemented
#endif
}



