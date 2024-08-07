#include <QRegularExpression>
#include <QDebug>

#include "qTagger.h"

//#define kDEBUG_QTAGGER_QUERY
//#define kDEBUG_QTAGGER_UPDATE_TAG

const char* QTagger::kDB_FIELD_LINE_SEPERATOR = ",";
const char* QTagger::kDB_FIELD_FILE_SEPERATOR = ":";
const char* QTagger::kDB_FIELD_FILE_RECORD_SEPERATOR = ";";
const char* QTagger::kDB_TAG_RECORD_SEPERATOR = "#";
const char* QTagger::kQTAG_CONFIG_FILE = "qtag.conf";

const char* QTagger::kQTAG_DEFAULT_TAGDBNAME = "tag.db";

const char* QTagger::kQTAG_DEFAULT_INPUTLIST_FILE = "fileList.txt";

QTagger::QTagger()
{
}

QTagger::~QTagger()
{

}

int QTagger::initKeywordFileTokenMap()
{
	loadKeywordFile();
	tokenMap_.clear();
	return 0;
}

int QTagger::createTag(const T_FileItemList& inputFileList)
{
	long i;
	unsigned long fileId;

	QString currentFilePath;

	initKeywordFileTokenMap();

	for (i = 0; i < inputFileList.size(); i++) {
		currentFilePath = inputFileList.at(i).fileName_; // index start from 0
		fileId = inputFileList.at(i).fileId_;

		qDebug() << fileId << ":" << currentFilePath;

		qDebug() << "parseSourceFile IN" << Qt::endl;
		parseSourceFile(fileId, currentFilePath, tokenMap_);
		qDebug() << "parseSourceFile OUT" << Qt::endl;
	}
	return 0;
}

int QTagger::writeTagDb(const QString& tagDbFileName)
{
	QFile tagDbFile(tagDbFileName);
	if (!tagDbFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Cannot open tagDb file (" << tagDbFileName << ") for writing!" << Qt::endl;
		return -1;
	}

	QTextStream tagDb(&tagDbFile);

	QList<QString> tagList = tokenMap_.keys();

	qDebug() << "Before writing. Tag total: " << tokenMap_.size() << Qt::endl;

	for (int i = 0; i < tagList.size(); ++i) {
		tagDb << tagList[i] << kDB_TAG_RECORD_SEPERATOR << tokenMap_[tagList[i]] << '\n';
	}

	tagDb.flush();
	tagDbFile.close();

	qDebug() << "Finish writing. Tag total: " << tokenMap_.size() << Qt::endl;

	return 0;
}

int QTagger::getFileLineContent(const QString& fileName, const QList<unsigned long>& lineNumList, QList<CTagResultItem>& resultLineList,
							const QStringList& lineFilterStrList, const QStringList& excludePatternFilterStrList,
							int linePrintBeforeMatch, int linePrintAfterMatch, const Qt::CaseSensitivity& caseSensitivity, unsigned long limitSearchRow)
{
	QFile sourceFile(fileName);
	QString line;
	unsigned long currentLineNum = 0;
	unsigned long j;

	long k;
	int i;

	int closeBracketIndex;

	QStringList lastLines;
	QList<int> lastManualIndentLevelList;

	QString lastLinesJoined;

	bool bMatchedLineFilterStringList;
	bool bPassExcludePatternFilter;

	if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Cannot open source file (" << fileName << ") for reading!" << Qt::endl;
		return -1;
	}

	QTextStream sourceFileStream(&sourceFile);

	j = 0;

	unsigned long lineNumListTotal = static_cast <unsigned long> (lineNumList.size());

	int linePrintAfter = 0;

	CTagResultItem resultItem;

	CTagResultItem pendingResultItem;
	bool bGotPendingResultItem = false;

	int indentLevel = 0;

	int openBracketIndex = 0;

	resultItem.functionSignature_.clear();

	int manualIndentLevel = 0;

	QVector<QRegularExpression> lineFilterRegExpMatcher;
	QRegularExpressionMatch lineFilterRegExpMatch;

	for (i = 0; i < lineFilterStrList.size(); i++) {
		if (caseSensitivity == Qt::CaseSensitive) {
            lineFilterRegExpMatcher.push_back(QRegularExpression(lineFilterStrList[i]));
		} else {
			lineFilterRegExpMatcher.push_back(QRegularExpression(lineFilterStrList[i], QRegularExpression::CaseInsensitiveOption));
		}
	}

	while (!sourceFileStream.atEnd()) {
		currentLineNum++;
		line = sourceFileStream.readLine();

		manualIndentLevel = getManualIndentLevel(line);

		if (sourceFileStream.atEnd()) {
			break;
		}

		lastLines.append(line);
		lastManualIndentLevelList.append(manualIndentLevel);

		// increment indent level
		openBracketIndex = line.indexOf("{");
		if (openBracketIndex != -1) {
			indentLevel++;
		}

		// decrement indent level
		closeBracketIndex = line.indexOf("}");
		if (closeBracketIndex != -1) {
			indentLevel--;
		}

		if ((j < lineNumListTotal) || (linePrintAfter > 0)) {
			if ((j < lineNumListTotal) && (currentLineNum == lineNumList[j])) {

                if (bGotPendingResultItem) {
					resultLineList << pendingResultItem;
					if (resultLineList.length() >= limitSearchRow) {
						sourceFile.close();
						return 0;
					}
					bGotPendingResultItem = false;
				}

				// result in grep format
				resultItem.filePath_ = fileName;
				resultItem.fileLineNum_ = currentLineNum;
				resultItem.fileLineSrc_ = line;

				if (lineFilterRegExpMatcher.isEmpty()) { // assume match if no line filter
                    bMatchedLineFilterStringList = true;
				} else { // consider as not matching if a single item is not matched
					bMatchedLineFilterStringList = true;
					for (k = 0; k < lineFilterRegExpMatcher.size(); k++) {
						lineFilterRegExpMatch = lineFilterRegExpMatcher[k].match(line);
						if (!lineFilterRegExpMatch.hasMatch()) {
							bMatchedLineFilterStringList = false;
							break;
						}
					}
				}

				// exclude pattern filter, pass if not consist excluded pattern
				bPassExcludePatternFilter = true;

				for (k = 0; k < excludePatternFilterStrList.size(); k++) {
					if (line.contains(excludePatternFilterStrList.at(k))) {
						bPassExcludePatternFilter = false;
						break;
					}
				}

				// valid only if all matched
				if (bMatchedLineFilterStringList && bPassExcludePatternFilter) {
					if ((linePrintBeforeMatch > 0) && (linePrintAfterMatch > 0)) {
						resultItem.lineSrcIndentLevel_ = manualIndentLevel;

						int lastLineSize = lastLines.size();

						for (k = 1; k <= linePrintBeforeMatch; k++) {
							if (lastLineSize - k - 1 >= 0) { // excluding the current line
								resultItem.fileLineSrcBeforeList_.append(lastLines.at(lastLineSize - k - 1));
								resultItem.beforeIndentLevelList_.append(lastManualIndentLevelList.at(lastLineSize - k - 1));
							}
						}

						pendingResultItem = resultItem;
						resultItem.fileLineSrcBeforeList_.clear(); // reuse resultItem
						resultItem.beforeIndentLevelList_.clear();

						bGotPendingResultItem = true;

						linePrintAfter = linePrintAfterMatch;

					} else if (linePrintBeforeMatch > 0) {
						resultItem.lineSrcIndentLevel_ = manualIndentLevel;

						int lastLineSize = lastLines.size();

						for (k = 1; k <= linePrintBeforeMatch; k++) {
							if (lastLineSize - k - 1 >= 0) { // excluding the current line
								resultItem.fileLineSrcBeforeList_.append(lastLines.at(lastLineSize - k - 1));
								resultItem.beforeIndentLevelList_.append(lastManualIndentLevelList.at(lastLineSize - k - 1));
							}
						}

						resultLineList << resultItem;
						resultItem.fileLineSrcBeforeList_.clear(); // reuse resultItem
						resultItem.beforeIndentLevelList_.clear();

						if (resultLineList.length() >= limitSearchRow) {
							sourceFile.close();
							return 0;
						}
					} else if (linePrintAfterMatch > 0) {
						resultItem.lineSrcIndentLevel_ = manualIndentLevel;
						pendingResultItem = resultItem;
						bGotPendingResultItem = true;

						linePrintAfter = linePrintAfterMatch;
					} else {
						resultLineList << resultItem;

						if (resultLineList.length() >= limitSearchRow) {
							sourceFile.close();
							return 0;
						}
					}
				}

				j++;
			} else {
				if (linePrintAfter > 0) {
					pendingResultItem.fileLineSrcAfterList_.append(line);
					pendingResultItem.afterIndentLevelList_.append(manualIndentLevel);

					linePrintAfter--;

					if (linePrintAfter == 0) { // finish appending line after to print
                        resultLineList << pendingResultItem;

						if (resultLineList.length() >= limitSearchRow) {
							sourceFile.close();
							return 0;
						}
						bGotPendingResultItem = false;
					}
				}
			}
		} else { // already get all matched lines
			break;
		}

		if (lastLines.size() > 15) {
			lastLines.removeFirst();
			lastManualIndentLevelList.removeFirst();
		}
	}

	sourceFile.close();

	return 0;
}

int QTagger::loadTagList(const QString& tagDbFileName)
{
	QFile tagDbFile(tagDbFileName);
	int tagFieldIndex;
	QString line;
	QString tagName;

	if (!tagDbFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Cannot open tagDb file (" << tagDbFileName << ") for reading!" << Qt::endl;
		return -1;
	}

	QTextStream tagDb(&tagDbFile);
	tagList_.clear();

	CTagItem tagItem;
	QString tagFileRecordStr = "";
	QStringList tagFileRecordStrList;
	CTagFileRecord tagFileRecord;

	int lineFieldIndex = 0;
	QString lineNumListStr = "";
	QStringList lineNumStrList;

	while (!tagDb.atEnd()) {
		line = tagDb.readLine();
	
		tagFieldIndex = line.indexOf(kDB_TAG_RECORD_SEPERATOR);

		if (tagFieldIndex != -1) {
			tagName = line.left(tagFieldIndex);
		}
		tagItem.tag_ = tagName;

		tagFileRecordStr = line.mid(tagFieldIndex + 1);

		tagFileRecordStrList = tagFileRecordStr.split(kDB_FIELD_FILE_RECORD_SEPERATOR, Qt::SkipEmptyParts);

		tagItem.tagFileRecord_.clear();

		foreach (const QString& fileRecordStr, tagFileRecordStrList) {
			lineFieldIndex = fileRecordStr.indexOf(kDB_FIELD_FILE_SEPERATOR);
			tagFileRecord.fileId_ = fileRecordStr.left(lineFieldIndex).toULong();

			lineNumListStr = fileRecordStr.mid(lineFieldIndex + 1);
			lineNumStrList = lineNumListStr.split(kDB_FIELD_LINE_SEPERATOR, Qt::SkipEmptyParts);

			tagFileRecord.lineNum_.clear();
			foreach (const QString& lineNumStr, lineNumStrList) {
				tagFileRecord.lineNum_ << lineNumStr.toULong();
			}

			tagItem.tagFileRecord_ << tagFileRecord;
		}

		tagList_.append(tagItem);
	}

	tagDbFile.close();

	loadKeywordFile();
	return 0;
}

int QTagger::levenshteinDistance(const QString &source, const QString &target)
{
    // Mostly token from https://qgis.org/api/2.14/qgsstringutils_8cpp_source.html
    if (source == target) {
        return 0;
    }

    const int sourceCount = source.length();
    const int targetCount = target.length();

    if (source.isEmpty()) {
        return targetCount;
    }

    if (target.isEmpty()) {
        return sourceCount;
    }

    if (sourceCount > targetCount) {
        return levenshteinDistance(target, source);
    }

    QVector<int> column;
    column.fill(0, targetCount + 1);
    QVector<int> previousColumn;
    previousColumn.reserve(targetCount + 1);
    for (int i = 0; i < targetCount + 1; i++) {
        previousColumn.append(i);
    }

    for (int i = 0; i < sourceCount; i++) {
        column[0] = i + 1;
        for (int j = 0; j < targetCount; j++) {
            column[j + 1] = std::min({
                1 + column.at(j),
                1 + previousColumn.at(1 + j),
                previousColumn.at(j) + ((source.at(i) == target.at(j)) ? 0 : 1)
            });
        }
        column.swap(previousColumn);
    }

    return previousColumn.at(targetCount);
}

bool QTagger::fuzzyMatch(const QString& targetInput, const QString& patternInput, const Qt::CaseSensitivity& caseSensitivity)
{
	long i = 0;
	long patternPos = 0;
	long matchedChar = 0;
	QString target, pattern;

	if (caseSensitivity == Qt::CaseInsensitive) {
		target = targetInput.toLower();
		pattern = patternInput.toLower();
	} else {
		target = targetInput;
		pattern = patternInput;
	}

	if (pattern == "") {
		return true;
	}

	for (i = 0; i < target.length(); i++) {
		if (pattern.at(patternPos) == target.at(i)) {
			patternPos++;
			matchedChar++;
		} else {
			continue;
		}
		if (matchedChar == pattern.length()) { // all char in pattern match
			break;
		}
		if (patternPos >= pattern.length()) { // no match for all chars in pattern
			break;
		}
		if (i + pattern.length() - patternPos > target.length()) { // not match yet pattern reach end of string
			break;
		}
	}

	if (matchedChar == pattern.length()) {
		return true;
	} else {
		return false;
	}
}

int QTagger::getFuzzyMatchedTags(const QString& tagToQuery, QMultiMap<int, QString>& matchedTokenList, const Qt::CaseSensitivity& caseSensitivity)
{
	QStringList result;
	foreach (const CTagItem &tagItem, tagList_) {
		if (abs(tagItem.tag_.length() - tagToQuery.length()) > 15) {
			continue;
		}

		if (fuzzyMatch(tagItem.tag_, tagToQuery, caseSensitivity)) {
			int distance = levenshteinDistance(tagToQuery, tagItem.tag_);
			matchedTokenList.insert(distance, tagItem.tag_);
		}

		if (matchedTokenList.size() > 1000) {
			break;
		}
	}

	return 0;
}

int QTagger::getMatchedTags(const QString& tagToQuery, QMultiMap<int, QString>& matchedTokenList, const Qt::CaseSensitivity& caseSensitivity)
{
	QStringList result;
	foreach (const CTagItem &tagItem, tagList_) {
		if (abs(tagItem.tag_.length() - tagToQuery.length()) > 15) {
			continue;
		}

		if (tagItem.tag_.contains(tagToQuery, caseSensitivity)) {
			int distance = levenshteinDistance(tagToQuery, tagItem.tag_);
			matchedTokenList.insert(distance, tagItem.tag_);
		}

		if (matchedTokenList.size() > 1000) {
			break;
		}
	}

	return 0;
}

int QTagger::queryTagLoadedSymbol(const T_FileItemList& inputFileItemList, const QString& tagToQuery,
					QString& tagToQueryFiltered, QList<CTagResultItem>& resultList, const Qt::CaseSensitivity& caseSensitivity, bool symbolRegularExpression, unsigned long limitSearchRow)
{
	if (!tagToQuery.isEmpty()) {
		QStringList tagToQueryList;
		QStringList lineFilterStrList;

		QStringList fileNameFilterStrList;
		QStringList excludePatternFilterStrList;
		QString tagToQueryStr;

		int linePrintBeforeMatch = 0;
		int linePrintAfterMatch = 0;

		tagToQueryList = tagToQuery.split(" ", Qt::SkipEmptyParts);

		/* YCH modified (begin), commented temporary */
		/*
        if (tagToQueryList.size() >= 2) {
			if (keywordSet_.contains(tagToQueryList.at(0))) { // not using keyword for query, i.e. no keyword for first word
				tagToQueryList.swap(0, 1);
			}
		}
		*/
		/* YCH modified (end), commented temporary */

		tagToQueryStr = tagToQueryList.takeFirst(); // take first and remove first tag for query

		int i;

		QString queryField;

		bool bStrConvertOk;

		for (i = 0; i < tagToQueryList.size(); i++) {
			queryField = tagToQueryList.at(i);

			if (queryField.startsWith("/f")) { // file name filter
                queryField.remove(0, 2); // remove header
				if (!queryField.isEmpty()) {
					fileNameFilterStrList << queryField;
				}
			} else if (queryField.startsWith("/x")) { // exclude pattern filter
                queryField.remove(0, 2); // remove header
				if (!queryField.isEmpty()) {
					excludePatternFilterStrList << queryField;
				}
			} else if (queryField.startsWith("/b")) { // number of line to print before matches
                queryField.remove(0, 2); // remove header
				linePrintBeforeMatch = queryField.toInt(&bStrConvertOk);
			} else if (queryField.startsWith("/a")) { // number of line to print after matches
                queryField.remove(0, 2); // remove header
				linePrintAfterMatch = queryField.toInt(&bStrConvertOk);
			} else if (queryField.startsWith("/n")) { // number of line to print before and after matches
                queryField.remove(0, 2); // remove header
				linePrintBeforeMatch = queryField.toInt(&bStrConvertOk);
				linePrintAfterMatch = linePrintBeforeMatch;
			} else {
				lineFilterStrList << queryField;
			}
		}

		tagToQueryFiltered = tagToQueryStr;

		// query in tag list
		QRegularExpression tagQueryRegEx(tagToQueryStr);
		QRegularExpressionMatch tagQueryMatch;
		QString tagField;
		bool bSymbolMatch;

		unsigned long fileId;

		QString line;

		QString queryResultFileRecordListStr;
		QStringList queryResultFileRecordList;

		QString queryResultFileName;

		QString queryResultLineNumListStr;
		QStringList queryResultLineNumStrList;

		QList<unsigned long> queryResultLineNumList;

		bool bMatchedFileNameFilter;
		long k;

		if (caseSensitivity == Qt::CaseInsensitive) {
			tagQueryRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
		}

		foreach (const CTagItem& tagItem, tagList_) {
			tagField = tagItem.tag_;

			bSymbolMatch = false;

			if (symbolRegularExpression) {
				tagQueryMatch = tagQueryRegEx.match(tagField);
				if (tagQueryMatch.hasMatch()) {
					bSymbolMatch = true;
				}
			} else {
                if (QString::compare(tagField, tagToQueryStr, caseSensitivity) == 0) { // exact match
                    bSymbolMatch = true;
				}
			}

			if (bSymbolMatch) {
				foreach (const CTagFileRecord& tagFileRecord, tagItem.tagFileRecord_) {
					fileId = tagFileRecord.fileId_;

#ifdef kDEBUG_QTAGGER_QUERY
					qDebug() << "fileId = " << fileId;
#endif

					queryResultFileName = inputFileItemList[fileId].fileName_;

#ifdef kDEBUG_QTAGGER_QUERY
					qDebug() << "queryResultFileName = " << queryResultFileName;
#endif

					// filename filter, assume not matched if not empty, only matched if any query filename matched
					if (fileNameFilterStrList.empty()) {
						bMatchedFileNameFilter = true;
					} else {
						bMatchedFileNameFilter = false;

						//qDebug() << "fileName = " << fileName << Qt::endl;

						for (k = 0; k < fileNameFilterStrList.size(); k++) {

							//qDebug() << "fileNameFilterStrList.at(k) = " << fileNameFilterStrList.at(k) << Qt::endl;
							if (queryResultFileName.contains(fileNameFilterStrList.at(k))) {
								bMatchedFileNameFilter = true;
								break;
							}
						}
					}

					if (!bMatchedFileNameFilter) { // skip if not match file name
						continue;
					}

					if (resultList.length() >= limitSearchRow) {
						break;
					}

					getFileLineContent(queryResultFileName, tagFileRecord.lineNum_, resultList,
									lineFilterStrList, excludePatternFilterStrList,
									linePrintBeforeMatch, linePrintAfterMatch, caseSensitivity, limitSearchRow);
				}
			}

			if (resultList.length() >= limitSearchRow) {
				break;
			}
		}
	}
	return 0;
}

int QTagger::queryTag(const QString& inputFileName, const QString& tagDbFileName, const QString& tagToQuery,
					QString& tagToQueryFiltered, QList<CTagResultItem>& resultList, const Qt::CaseSensitivity& caseSensitivity, bool symbolRegularExpression)
{
	if (!tagToQuery.isEmpty()) {
		QStringList tagToQueryList;
		QStringList lineFilterStrList;

		QStringList fileNameFilterStrList;
		QStringList functionNameFilterStrList;
		QStringList excludePatternFilterStrList;
		QString tagToQueryStr;

		int linePrintBeforeMatch = 0;
		int linePrintAfterMatch = 0;

		tagToQueryList = tagToQuery.split(" ", Qt::SkipEmptyParts);

		/* YCH modified (begin), commented temporary */
		/*
        if (tagToQueryList.size() >= 2) {
			if (keywordSet_.contains(tagToQueryList.at(0))) { // not using keyword for query, i.e. no keyword for first word
				tagToQueryList.swap(0, 1);
			}
		}
		*/
		/* YCH modified (end), commented temporary */

		tagToQueryStr = tagToQueryList.takeFirst(); // take first and remove first tag for query

		int i;

		QString queryField;

		bool bStrConvertOk;

		for (i = 0; i < tagToQueryList.size(); i++) {
			queryField = tagToQueryList.at(i);

			if (queryField.startsWith("/f")) { // file name filter
				if (queryField.length() <= 2) { // number not input yet
					continue;
				}
                queryField.remove(0, 2); // remove header
				if (!queryField.isEmpty()) {
					fileNameFilterStrList << queryField;
				}
			} else if (queryField.startsWith("/x")) { // exclude pattern filter
				if (queryField.length() <= 2) { // number not input yet
					continue;
				}
                queryField.remove(0, 2); // remove header
				if (!queryField.isEmpty()) {
					excludePatternFilterStrList << queryField;
				}
			} else if (queryField.startsWith("/b")) { // number of line to print before matches
				if (queryField.length() <= 2) { // number not input yet
					continue;
				}
                queryField.remove(0, 2); // remove header
				linePrintBeforeMatch = queryField.toInt(&bStrConvertOk);
			} else if (queryField.startsWith("/a")) { // number of line to print after matches
				if (queryField.length() <= 2) { // number not input yet
					continue;
				}
                queryField.remove(0, 2); // remove header
				linePrintAfterMatch = queryField.toInt(&bStrConvertOk);
			} else if (queryField.startsWith("/n")) { // number of line to print before and after matches
				if (queryField.length() <= 2) { // number not input yet
					continue;
				}
                queryField.remove(0, 2); // remove header
				linePrintBeforeMatch = queryField.toInt(&bStrConvertOk);
				linePrintAfterMatch = linePrintBeforeMatch;
			} else {
				lineFilterStrList << queryField;
			}
		}

		tagToQueryFiltered = tagToQueryStr;

		T_FileItemList inputFileItemList;

		// load input file
        CSourceFileList::loadFileList(inputFileName, inputFileItemList);

		// query in tag db
		QFile tagDbFile(tagDbFileName);

		if (!tagDbFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug() << "Cannot open tagDb file (" << tagDbFileName << ") for reading!" << Qt::endl;
			return -1;
		}

		QTextStream tagDbFileStream(&tagDbFile);

		QRegularExpression tagQueryRegEx(tagToQueryStr);
		QRegularExpressionMatch tagQueryMatch;
		QString tagField;
		bool bSymbolMatch;

		int tagFieldIndex;
		int lineFieldIndex;

		unsigned long fileId;

		QString line;

		QString queryResultFileRecordListStr;
		QStringList queryResultFileRecordList;

		QString queryResultFileName;

		QString queryResultLineNumListStr;
		QStringList queryResultLineNumStrList;

		QList<unsigned long> queryResultLineNumList;

		bool bMatchedFileNameFilter;
		long k;

		if (caseSensitivity == Qt::CaseInsensitive) {
			tagQueryRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
		}

		while (!tagDbFileStream.atEnd()) {
			line = tagDbFileStream.readLine();

			// Format: | Tag | ";" | file 1 ID | ":" | location 1 line num | "," | location 2 line num | ... | ";" | file 2 ID | ":" | location 1 line num | ...
			//tagFieldIndex = line.indexOf(tagQueryRegEx);

			tagFieldIndex = line.indexOf(kDB_TAG_RECORD_SEPERATOR, 0);
			tagField = line.left(tagFieldIndex);

			bSymbolMatch = false;

			if (symbolRegularExpression) {
				tagQueryMatch = tagQueryRegEx.match(tagField);
				if (tagQueryMatch.hasMatch()) {
					bSymbolMatch = true;
				}
			} else {
                if (tagField == tagToQueryStr) { // exact match
                    bSymbolMatch = true;
				}
			}

			if (bSymbolMatch) {

				// Result format: ";" | file 1 ID | ":" | location 1 line num | "," | location 2 line num | ... | ";" | file 2 ID | ":" | location 1 line num | ...

				queryResultFileRecordListStr = line.mid(tagFieldIndex); // exlcude tag and the caret regular expression character

				//qDebug() << "queryResultFileRecordListStr = " << queryResultFileRecordListStr;

				// Result format: file 1 ID | ":" | location 1 line num | "," | location 2 line num | ...
				queryResultFileRecordList = queryResultFileRecordListStr.split(kDB_FIELD_FILE_RECORD_SEPERATOR, Qt::SkipEmptyParts);

				foreach (const QString& queryResultFileRecord, queryResultFileRecordList) {

					lineFieldIndex = queryResultFileRecord.indexOf(kDB_FIELD_FILE_SEPERATOR);

					fileId = queryResultFileRecord.mid(0, lineFieldIndex).toULong();

#ifdef kDEBUG_QTAGGER_QUERY
					qDebug() << "fileId = " << fileId;
#endif

					queryResultFileName = inputFileItemList[fileId].fileName_;

#ifdef kDEBUG_QTAGGER_QUERY
					qDebug() << "queryResultFileName = " << queryResultFileName;
#endif

					// filename filter, assume not matched if not empty, only matched if any query filename matched
					if (fileNameFilterStrList.empty()) {
						bMatchedFileNameFilter = true;
					} else {
						bMatchedFileNameFilter = false;

						//qDebug() << "fileName = " << fileName << Qt::endl;

						for (k = 0; k < fileNameFilterStrList.size(); k++) {

							//qDebug() << "fileNameFilterStrList.at(k) = " << fileNameFilterStrList.at(k) << Qt::endl;
							if (queryResultFileName.contains(fileNameFilterStrList.at(k))) {
								bMatchedFileNameFilter = true;
								break;
							}
						}
					}

					if (!bMatchedFileNameFilter) { // skip if not match file name
						continue;
					}

					// Result format: location 1 line num | "," | location 2 line num | ...
					queryResultLineNumListStr = queryResultFileRecord.mid(lineFieldIndex + 1);

#ifdef kDEBUG_QTAGGER_QUERY
					qDebug() << "queryResultLineNumListStr = " << queryResultLineNumListStr;
#endif

					// Result format: location 1 line num
					queryResultLineNumStrList = queryResultLineNumListStr.split(kDB_FIELD_LINE_SEPERATOR, Qt::SkipEmptyParts);

					queryResultLineNumList.clear();
					foreach (const QString& queryResultLineNumStr, queryResultLineNumStrList) {
						queryResultLineNumList << queryResultLineNumStr.toULong();
					}

					getFileLineContent(queryResultFileName, queryResultLineNumList, resultList,
									lineFilterStrList, excludePatternFilterStrList,
									linePrintBeforeMatch, linePrintAfterMatch, caseSensitivity, 30000);
				}
			}
		}
		tagDbFile.close();
	}
	return 0;
}

int QTagger::getManualIndentLevel(QString& line)
{
    int indentLevel = 0;
	int i;
	int tabCount = 0;
	int spaceCount = 0;

	for (i = 0; i < line.length(); i++) {
		if (line.at(i) == '\t') {
			tabCount++;
		} else if (line.at(i) == ' ') {
			spaceCount++;
		} else {
			break;
		}
	}

	indentLevel = tabCount + spaceCount / 3;

	return indentLevel;
}

/***
 * Parse source file and update token map
 */
bool QTagger::parseSourceFile(unsigned long fileId, const QString& fileName, T_TokenMapType& tokenMap)
{
	QFile sourcefile(fileName);

	QSet<QString> tagCurrentLineSet; // remember tag for current line, not putting tag duplicated in the same line

	if (!sourcefile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Cannot open source file (" << fileName << ") for reading in parseSourceFile()!" << Qt::endl;
		return false;
	}

	QTextStream sourcefileStream(&sourcefile);

	qDebug() << "parseSourceFile IN, id =" << fileId << ", fileName =" << fileName <<  Qt::endl;

	QStringList tokenList;
	QString fileIdStr = QString::number(fileId);

	QString tokenMapValue;
	QString currentLineRead;
	QString lastLineStr;

	QString fileIdField;
	int fileIdFieldIndex;

	unsigned long lineNumReading = 0;

	while (!sourcefileStream.atEnd()) {
		lineNumReading++;

		tagCurrentLineSet.clear();

		currentLineRead = sourcefileStream.readLine();
		if (sourcefileStream.atEnd()) {
			break;
		}

		tokenList.clear();
		extractWordTokens(currentLineRead, tokenList);
		foreach (const QString& token, tokenList) {
			// commented by default, enable token length of one and two
			/*
			if (token.length() == 1) { // not storing single character
				continue;
			}

			if (token.length() == 2) {
				if (token.at(0).isDigit() && token.at(1).isDigit()) {  // not storing two digit tag
					continue;
				}
			}
			*/

			if (!tagCurrentLineSet.contains(token)) { // as not storing tag duplicate in same line
				tagCurrentLineSet.insert(token);

				//if (!keywordSet_.contains(token)) { // not storing language keyword
                    // store keyword as well
					fileIdField = kDB_FIELD_FILE_RECORD_SEPERATOR + fileIdStr + kDB_FIELD_FILE_SEPERATOR;

					if (tokenMap.contains(token)) { // token already exists

						tokenMapValue = tokenMap[token];

						// avoid memory bad_alloc exception
						if (tokenMapValue.length() >= kTAG_LENGTH_LIMIT) {
							continue;
						}

						// File format: ";" | file 1 ID | ":" | location 1 line num | "," | location 2 line num | ... | ";" | file 2 ID | ":" | location 1 line num | ...

						fileIdFieldIndex = tokenMapValue.lastIndexOf(fileIdField); // search from backward

						if (fileIdFieldIndex >= 0) { // already exists in the same file

							tokenMap[token] = tokenMapValue + kDB_FIELD_LINE_SEPERATOR + QString::number(lineNumReading); // append new location
						} else {
							tokenMap[token] = tokenMapValue + fileIdField + QString::number(lineNumReading);
						}
					} else { // new entry
						tokenMap[token] = fileIdField + QString::number(lineNumReading);
					}
				//}
			}
		}
	}
	sourcefile.close();

	qDebug() << "parseSourceFile OUT, id =" << fileId << ", fileName =" << fileName <<  Qt::endl;

	return true;
}

// overload function to remove need of tokenMap_ when called by CProjectUpdateThread
bool QTagger::parseSourceFile(unsigned long fileId, const QString& fileName)
{
	parseSourceFile(fileId, fileName, tokenMap_);
	return true;
}

void QTagger::extractWordTokens(const QString& str, QStringList& tokenList)
{
    int i;

	QString currentWord = "";
	unsigned char currentChar;
	bool bContinueScanning = false;

	for (i = 0; i < str.length(); i++) {
		bContinueScanning = false;
		currentChar = (unsigned char) (str.at(i). toLatin1());
		if (((currentChar >= 'a') && (currentChar <= 'z')) // small capital
			|| ((currentChar >= 'A') && (currentChar <= 'Z')) // large capital
			|| ((currentChar >= '0') && (currentChar <= '9')) // Number
			|| (currentChar == '_')) { // undercore
            currentWord += QChar(currentChar);
			bContinueScanning = true;
		}

		if (!bContinueScanning || (i == (str.length() - 1))) { // encounter seperator or line end
			if (!currentWord.isEmpty()) {
				tokenList << currentWord;
				currentWord = "";
			}
		}
	}
}

void QTagger::loadKeywordFile()
{
	/* YCH modified, commented temporary */
	/*
	qDebug() << "QCoreApplication::applicationDirPath() IN" << Qt::endl;
	qDebug() << "QCoreApplication::applicationDirPath() = " << QCoreApplication::applicationDirPath() << Qt::endl;
	QString keywordFileName = QCoreApplication::applicationDirPath() + "/keyword.txt";
	qDebug() << "QCoreApplication::applicationDirPath() OUT" << Qt::endl;

	QFile file(keywordFileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "cannot open keyword file (" << keywordFileName << ") for reading!" << Qt::endl;
		return;
	}

	QTextStream inputFileStream(&file);

	keywordSet_.clear();
	while (!inputFileStream.atEnd()) {
		QString line = inputFileStream.readLine();
		QStringList keywordList = line.split(" ", Qt::SkipEmptyParts);

		foreach (const QString& keyword, keywordList) {
			keywordSet_.insert(keyword);
	   	}
	}
	file.close();
	*/
}




