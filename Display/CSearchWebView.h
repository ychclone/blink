#ifndef CSEARCH_WEB_VIEW_H
#define CSEARCH_WEB_VIEW_H

#include <QWebView>
#include <QWheelEvent>
#include <QMenu>
#include <QShortcut>

#include <QWebPage>

#include "Model/CConfigManager.h"

class CSearchWebView : public QWebView
{
	Q_OBJECT

public:
	CSearchWebView(QWidget *parent = 0);

	void webZoomIn();
	void webZoomOut();

	bool findText(const QString & subString, QWebPage::FindFlags options);

public slots:
	void wheelEvent(QWheelEvent *e);
	void contextMenuEvent(QContextMenuEvent *event);

private:
	QShortcut* selectAllShortcut;

	CConfigManager* confManager_; 

private slots:
	void on_selectAllPressed();
	void on_urlClicked(const QUrl &);


};



#endif // CSEARCH_WEB_VIEW_H

