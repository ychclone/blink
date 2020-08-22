#ifndef QFIND_REPLACER_H
#define QFIND_REPLACER_H

#include <QString>
#include <QStringList>

class QFindReplacer
{
public:
	QFindReplacer();
	virtual ~QFindReplacer();

	// return number of matched and replaced
	long replaceInFile(const QString& findStr, const QString& replaceStr, const QString& filePath, bool bCaseSensitive, bool bMatchWholeWord, bool bRegularExpression);

};

#endif


