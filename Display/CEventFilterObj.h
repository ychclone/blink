#ifndef CEVENTFILTEROBJ_H
#define CEVENTFILTEROBJ_H

#include <QApplication> 

class CEventFilterObj : public QObject
{
	Q_OBJECT

	protected:
		bool eventFilter(QObject *obj, QEvent *event);
};

#endif // CEVENTFILTEROBJ_H
