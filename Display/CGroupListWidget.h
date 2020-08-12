#ifndef CGROUP_LIST_WIDGET_H
#define CGROUP_LIST_WIDGET_H

#include <QTreeView>
#include <QWheelEvent>

class CGroupListWidget : public QTreeView
{
    Q_OBJECT

public:
    CGroupListWidget(QWidget *parent = 0);

	void mouseDoubleClickEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	void wheelEvent(QWheelEvent *e);

	void updateGroupFont(const QFont& profileFont);
signals:
	void groupItemTriggered();

private:
	long groupFontSize_;

	void updateGroupListWidget();

	void groupZoomIn();
	void groupZoomOut();
};

#endif // CGROUP_LIST_WIDGET_H
