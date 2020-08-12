#ifndef QTAGAPP_H
#define QTAGAPP_H

#include <QtCore>

class QTagApplication : public QCoreApplication
{
	Q_OBJECT
public:
	QTagApplication(int& argc, char**& argv);

	virtual ~QTagApplication();


};


#endif
