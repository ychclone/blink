#ifndef CPROJECT_LIST_WIDGET_H
#define CPROJECT_LIST_WIDGET_H

#include <QTreeView>
#include <QWheelEvent>

class CProjectListWidget : public QTreeView
{
    Q_OBJECT

public:
    CProjectListWidget(QWidget *parent = 0);

	void mouseDoubleClickEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	void wheelEvent(QWheelEvent *e);

	void updateProjectFont(const QFont& projectFont);

signals:
	void projectItemTriggered();

private:
	long projectFontSize_;

	void updateProjectListWidget();

	void projectZoomIn();
	void projectZoomOut();
};

#endif // CPROJECT_LIST_WIDGET_H
