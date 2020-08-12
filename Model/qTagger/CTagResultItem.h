#ifndef CTAG_RESULT_ITEM_H
#define CTAG_RESULT_ITEM_H

#include <QString>
#include <QStringList>

#include <iostream>
using namespace std;

class CTagResultItem
{
public:
	CTagResultItem();
	virtual ~CTagResultItem();

	QString filePath_;
	unsigned long fileLineNum_;
	QString fileLineSrc_; 

	QStringList fileLineSrcBeforeList_;
	int lineSrcIndentLevel_;
	QStringList fileLineSrcAfterList_;

	QList<int> beforeIndentLevelList_;
	QList<int> afterIndentLevelList_; 

	QString functionSignature_;

	static const char* kFIELD_RESULT_SEPERATOR; 

};

ostream& operator<<(ostream& ost, const CTagResultItem& item);
                                                    
#endif // CTAG_RESULT_ITEM_H

