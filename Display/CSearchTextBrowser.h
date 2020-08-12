#ifndef CSEARCH_TEXT_BROWSER_H
#define CSEARCH_TEXT_BROWSER_H

#include <QTextBrowser>
#include <QWheelEvent>
#include <QMenu>
#include <QShortcut>

#include "Model/CConfigManager.h"

class CSearchTextBrowser : public QTextBrowser
{
	Q_OBJECT

public:
	CSearchTextBrowser(QWidget *parent = 0);

	void textZoomIn();
	void textZoomOut();

	bool findText(const QString & subString, QTextDocument::FindFlags options);

public slots:
	void wheelEvent(QWheelEvent *e);
	void contextMenuEvent(QContextMenuEvent *event);

private:
	QShortcut* selectAllShortcut;

	CConfigManager* m_confManager; 

private slots:
	void on_selectAllPressed();
	void on_urlClicked(const QUrl &);


};



#endif // CSEARCH_TEXT_BROWSER_H

