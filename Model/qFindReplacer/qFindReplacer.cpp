#include <QTextStream>
#include <QFile>

#include <QDebug>
#include <QRegularExpression>

#include "qFindReplacer.h"

QFindReplacer::QFindReplacer()
{
}

QFindReplacer::~QFindReplacer()
{

}

long QFindReplacer::replaceInFile(const QString& findStr, const QString& replaceStr, const QString& filePath, bool bCaseSensitive, bool bMatchWholeWord, bool bRegularExpression)
{
	int i = 0;
	QString inputLine;
	QString replacedLine;
	long totalMatchCount = 0;
	long matchCount = 0;

	qDebug() << "Find and replace file: " << filePath << Qt::endl;

	QFile inputFile(filePath);
	QString outputFileName = filePath + ".tmp";
	QFile outputFile(outputFileName);

	if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Cannot open inputFile file (" << filePath << ") for reading!" << Qt::endl;
		return -1;
	}

	if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Cannot open outputFile file (" << outputFileName << ") for writing!" << Qt::endl;
		return -1;
	}

	QTextStream inputFileStream(&inputFile);
	QTextStream outputFileStream(&outputFile);

	QRegularExpression regExp(findStr);

	Qt::CaseSensitivity caseSensitive;

	if (bCaseSensitive) {
		caseSensitive = Qt::CaseSensitive;
		// default expression default to case sensitive
	} else {
		caseSensitive = Qt::CaseInsensitive;
		regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}

	if (bRegularExpression || bMatchWholeWord) {
		// match whole word take effect only when regular expression is not enabled
		if (bMatchWholeWord && !bRegularExpression) {
			regExp.setPattern("\\b" + findStr + "\\b");
		}
		while (!inputFileStream.atEnd()) {
			inputLine = inputFileStream.readLine();

			matchCount = inputLine.count(regExp);

			if (matchCount > 0) {
				replacedLine = inputLine.replace(regExp, replaceStr);
				outputFileStream << replacedLine << Qt::endl;
				totalMatchCount += matchCount;
			} else {
                outputFileStream << inputLine << Qt::endl;
			}
		}
	} else {
		while (!inputFileStream.atEnd()) {
			inputLine = inputFileStream.readLine();

			matchCount = inputLine.count(findStr, caseSensitive);
			if (matchCount > 0) {
				replacedLine = inputLine.replace(findStr, replaceStr, caseSensitive);
				outputFileStream << replacedLine << Qt::endl;
				totalMatchCount += matchCount;
			} else {
                outputFileStream << inputLine << Qt::endl;
			}

		}
	}

	if (totalMatchCount > 0) {
		inputFile.remove(); // remove the original file
		outputFile.close();
		QFile::rename(outputFileName, filePath); // move the temporary file as input file
	} else {
		outputFile.remove(); // remove temporary file as no replace taken place
		inputFile.close(); // no action for input file
	}


	return totalMatchCount;
}




