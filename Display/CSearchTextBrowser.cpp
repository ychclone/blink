#include "CSearchTextBrowser.h"
#include <QMessageBox> 

#ifdef Q_OS_WIN
	#include <qt_windows.h>
#endif

CSearchTextBrowser::CSearchTextBrowser(QWidget *parent)
	: QTextBrowser(parent)
{
	setLineWrapMode(QTextEdit::NoWrap);

	selectAllShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
	connect(selectAllShortcut, SIGNAL(activated()), this, SLOT(on_selectAllPressed()));  

//	page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(this, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(on_urlClicked(const QUrl &))); 

	m_confManager = CConfigManager::getInstance();

	// default zoom level
	zoomIn(3);

	// no open links by QTextbrowser itself
	setOpenLinks(false);
}

void CSearchTextBrowser::textZoomIn()
{
	zoomIn(1);
}

void CSearchTextBrowser::textZoomOut()
{
	zoomOut(1);
}

bool CSearchTextBrowser::findText(const QString & subString, QTextDocument::FindFlags options)
{
	return QTextBrowser::find(subString, options);
}

void CSearchTextBrowser::wheelEvent(QWheelEvent *e)
{
	if (e->modifiers() == Qt::ControlModifier) {
		e->accept();
		if (e->delta() > 0) {
			textZoomIn();
		} else {
			textZoomOut();
		}
		QTextBrowser::wheelEvent(e);
	} else {
		QTextBrowser::wheelEvent(e);
	}
}

void CSearchTextBrowser::contextMenuEvent(QContextMenuEvent *event)
{
/*
    QMenu menu(this);

	menu.addAction(pageAction(QWebPage::SelectAll));
	menu.addAction(pageAction(QWebPage::Copy)); 

	menu.exec(event->globalPos()); 
*/	

    // bypass original context menu, e.g. reload
	//QWebView::contextMenuEvent(event);
}

void CSearchTextBrowser::on_selectAllPressed()
{
	selectAll();
}

void CSearchTextBrowser::on_urlClicked(const QUrl& urlClicked)
{
//	QMessageBox::information(this, "CSearchWebView",  urlClicked.path(), QMessageBox::Ok); 

	QString filePath = urlClicked.path();
	filePath.remove(0, 1); // remove beginning "/" character

	filePath = "\"" + filePath + "\""; // add quote, needed for gvim handling for filename with space

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



