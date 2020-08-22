#ifndef COUTPUT_LIST_WIDGET_H
#define COUTPUT_LIST_WIDGET_H

#include <QTreeView>
#include <QMouseEvent> 
#include <QStringList>

#include <QShortcut>

#include "Model/CFileListModel.h"

class CFileListWidget : public QTreeView
{
    Q_OBJECT

public:
    CFileListWidget(QWidget *parent = 0);

	void setFileListModel(CFileListModel *outputModel);

	void mousePressEvent(QMouseEvent *event);

	void mouseDoubleClickEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	void updateOutputFont(const QFont& outputFont); 

	void mouseMoveEvent(QMouseEvent *event);

	void wheelEvent(QWheelEvent *e);

	QPoint dragStartPosition_;

signals:
	void outputItemTriggered();

private:
	void updateFileListWidget();
	void fileZoomIn();
	void fileZoomOut();

	QStringList getSelectedItemNameList();

	CFileListModel* fileListModel_;

	long fileFontSize_;
    
};

#endif // COUTPUT_LIST_WIDGET_H
