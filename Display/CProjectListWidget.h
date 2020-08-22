#ifndef CPROFILE_LIST_WIDGET_H
#define CPROFILE_LIST_WIDGET_H

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

	void updateProfileFont(const QFont& profileFont);

signals:
	void profileItemTriggered();

private:
	long profileFontSize_;

	void updateProjectListWidget();

	void profileZoomIn();
	void profileZoomOut();
};

#endif // CPROFILE_LIST_WIDGET_H
