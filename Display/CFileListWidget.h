#ifndef CFILE_LIST_WIDGET_H
#define CFILE_LIST_WIDGET_H

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

	void mouseReleaseEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	void updateOutputFont(const QFont& outputFont);

	void mouseMoveEvent(QMouseEvent *event);

	void wheelEvent(QWheelEvent *e);

	QPoint dragStartPosition_;

	QStringList getSelectedItemNameList();

signals:
	void fileItemTriggered();

private:
	void updateFileListWidget();
	void fileZoomIn();
	void fileZoomOut();

	CFileListModel* fileListModel_;

	long fileFontSize_;

};

#endif // CFILE_LIST_WIDGET_H
