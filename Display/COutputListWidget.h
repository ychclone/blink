#ifndef COUTPUT_LIST_WIDGET_H
#define COUTPUT_LIST_WIDGET_H

#include <QTreeView>
#include <QMouseEvent> 
#include <QStringList>

#include <QShortcut>

#include "Model/COutputListModel.h"

class COutputListWidget : public QTreeView
{
    Q_OBJECT

public:
    COutputListWidget(QWidget *parent = 0);

	void setOutputListModel(COutputListModel *outputModel);

	void mousePressEvent(QMouseEvent *event);

	void mouseDoubleClickEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	void updateOutputFont(const QFont& outputFont); 

	void mouseMoveEvent(QMouseEvent *event);

	void wheelEvent(QWheelEvent *e);

	QPoint m_dragStartPosition;

signals:
	void outputItemTriggered();

private:
	void updateOutputListWidget();
	void fileZoomIn();
	void fileZoomOut();

	QStringList getSelectedItemNameList();

	COutputListModel* m_outputListModel;

	long fileFontSize_;
    
};

#endif // COUTPUT_LIST_WIDGET_H
