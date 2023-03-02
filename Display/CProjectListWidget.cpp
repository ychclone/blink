#include "CProjectListWidget.h"

CProjectListWidget::CProjectListWidget(QWidget *parent)
: QTreeView(parent)
{
	QFont currentFont = static_cast <QWidget*> (this)->font();

	projectFontSize_ = currentFont.pointSize();
}

void CProjectListWidget::updateProjectListWidget()
{
	resizeColumnToContents(0);
	resizeColumnToContents(1);
	resizeColumnToContents(2);
    resizeColumnToContents(3);
}

void CProjectListWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	emit projectItemTriggered();
    QTreeView::mouseDoubleClickEvent(event);
}

void CProjectListWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Return) {
		emit projectItemTriggered();
	} else if ((event->key() == Qt::Key_Equal) && (event->modifiers() == Qt::ControlModifier)) {
		projectZoomIn();
	} else if ((event->key() == Qt::Key_Minus) && (event->modifiers() == Qt::ControlModifier)) {
		projectZoomOut();
	}

	QTreeView::keyPressEvent(event);
}

void CProjectListWidget::updateProjectFont(const QFont& projectFont)
{
	QFont currentFont = static_cast <QWidget*> (this)->font();

	if (currentFont != projectFont) {
		static_cast <QWidget*> (this)->setFont(projectFont);
		projectFontSize_ = projectFont.pointSize();
		updateProjectListWidget();
	}
}

void CProjectListWidget::projectZoomIn()
{
	projectFontSize_++;
	QFont fnt = static_cast <QWidget*> (this)->font();;
	fnt.setPointSize(projectFontSize_);
	static_cast <QWidget*> (this)->setFont(fnt);

	updateProjectListWidget();
}

void CProjectListWidget::projectZoomOut()
{
	if (projectFontSize_ > 1) {
		projectFontSize_--;
		QFont fnt = static_cast <QWidget*> (this)->font();
		fnt.setPointSize(projectFontSize_);
		static_cast <QWidget*> (this)->setFont(fnt);

		updateProjectListWidget();
	}
}

void CProjectListWidget::wheelEvent(QWheelEvent *e)
{
	/* YCH modified (begin), commented temporary */
	/*
    if (e->modifiers() == Qt::ControlModifier) {
        e->accept();
        if (e->delta() > 0) {
		   projectZoomIn();
		} else {
		   projectZoomOut();
		}
	}
	*/
	/* YCH modified (end), commented temporary */
	QTreeView::wheelEvent(e);
}




