#include "qTagger.h"

//#define kDEBUG_QTAGGER_QUERY
//#define kDEBUG_QTAGGER_UPDATE_TAG

const char* QTagger::kDB_FIELD_LINE_SEPERATOR = ",";
const char* QTagger::kDB_FIELD_FILE_SEPERATOR = ":";
const char* QTagger::kDB_FIELD_FILE_RECORD_SEPERATOR = ";";
const char* QTagger::kDB_TAG_RECORD_SEPERATOR = "#";
const char* QTagger::kQTAG_CONFIG_FILE = "qtag.conf";

const char* QTagger::kQTAG_TAGS_DIR = "tags";

const char* QTagger::kQTAG_DEFAULT_TAGDBNAME = "tag.db";

const char* QTagger::kQTAG_DEFAULT_INPUTLIST_FILE = "fileList.txt";

QTagger::QTagger()
{
}

QTagger::~QTagger()
{

}

int QTagger::createTag(const T_OutputItemList& inputFileList)
{
	long i;
	unsigned long fileId;

	QString currentFilePath;

	loadKeywordFile();
	m_tokenMap.clear();

	for (i = 0; i < inputFileList.size(); i++) {
		currentFilePath = inputFileList.at(i).m_fileName; // index start from 0
		fileId = inputFileList.at(i).m_fileId;

		qDebug() << fileId << ":" << currentFilePath;

		parseSourceFile(fileId, currentFilePath, m_tokenMap);
	}
	return 0;
}

int QTagger::updateTag(const QMap<long, COutputItem>& inputFileList, const QString& tagDbFileName, const QMap<long, long>& fileIdCreatedMap, const QMap<long, long>& fileIdModifiedMap, const QMap<long, long>& fileIdDeletedMap)
{
	loadKeywordFile();
	m_tokenMap.clear();

	QFile tagDbFile(tagDbFileName);
	if (!tagDbFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Cannot open tagDb file (" << tagDbFileName << ") for update!" << endl;
		return -1;
	}

	QTextStream tagDbFileStream(&tagDbFile);
	QString line;
	int tagFieldIndex;

	QString tagStr;

	QString resultFileRecordListStr;
	QStringList resultFileRecordList;

	unsigned long fileId;

	QString updatedResultFileRecordListStr;
	int fileIdFieldIndex;

	QString updatedFileRecordListStr;

	bool bDeletedTag, bModifiedTag;

	while (!tagDbFileStream.atEnd()) {
		line = tagDbFileStream.readLine();

		tagFieldIndex = line.indexOf(kDB_TAG_RECORD_SEPERATOR);

		if (tagFieldIndex != -1) {
			tagStr = line.mid(0, tagFieldIndex); // length should be tagFieldIndex - 0 i.e. tagFieldIndex

			resultFileRecordListStr = line.mid(tagFieldIndex + 1); // exclude kDB_TAG_RECORD_SEPERATOR
			resultFileRecordList = resultFileRecordListStr.split(kDB_FIELD_FILE_RECORD_SEPERATOR, QString::SkipEmptyParts);

			updatedFileRecordListStr = "";

			// check each file record in a tag entry
			foreach (const QString& resultFileRecord, resultFileRecordList) {
				fileIdFieldIndex = resultFileRecord.indexOf(kDB_FIELD_FILE_SEPERATOR);
				fileId = resultFileRecord.mid(0, fileIdFieldIndex).toULong();

				bModifiedTag = false;
				bDeletedTag = false;

				QMap<long, long>::const_iterator it = fileIdModifiedMap.find(fileId);
				if (it != fileIdModifiedMap.end()) { // modified tag
					// modified tag, remove it from tagDb, to be added later as new tag
					bModifiedTag = true;

				} else { // check whether it is a deleted tag

					QMap<long, long>::const_iterator it = fileIdDeletedMap.find(fileId);
					if (it != fileIdDeletedMap.end()) { // deleted tag
						// deleted tag, remove it from tagDb
						bDeletedTag = true;
					}
				}

				if (bModifiedTag || bDeletedTag) {
					// removal from tagDb
				} else {
					updatedFileRecordListStr = updatedFileRecordListStr + kDB_FIELD_FILE_RECORD_SEPERATOR + resultFileRecord;
				}
			}

			if (updatedFileRecordListStr != "") { // only add to token map if there is record
				m_tokenMap[tagStr] = updatedFileRecordListStr;
			}
		}
	}

	tagDbFile.close();

	// new file
	QString currentFilePath;

#ifdef kDEBUG_QTAGGER_UPDATE_TAG
	qDebug() << "parseSourceFile for newFile" << endl;
#endif

	foreach (long fileIdCreated, fileIdCreatedMap) {
#ifdef kDEBUG_QTAGGER_UPDATE_TAG
		qDebug() << "fileIdCreated =" << fileIdCreated << endl;
#endif
		currentFilePath = inputFileList[fileIdCreated].m_fileName;
#ifdef kDEBUG_QTAGGER_UPDATE_TAG
		qDebug() << "currentFilePath =" << currentFilePath << endl;
#endif
		parseSourceFile(fileIdCreated, currentFilePath, m_tokenMap);
	}

#ifdef kDEBUG_QTAGGER_UPDATE_TAG
	qDebug() << "parseSourceFile for modifiedFile" << endl;
#endif

	// modified file
	foreach (long fileIdCreated, fileIdModifiedMap) {
#ifdef kDEBUG_QTAGGER_UPDATE_TAG
		qDebug() << "fileIdCreated =" << fileIdCreated << endl;
#endif
		currentFilePath = inputFileList[fileIdCreated].m_fileName;
#ifdef kDEBUG_QTAGGER_UPDATE_TAG
		qDebug() << "currentFilePath =" << currentFilePath << endl;
#endif
		parseSourceFile(fileIdCreated, currentFilePath, m_tokenMap);
	}

	return 0;
}

int QTagger::writeTagDb(const QString& tagDbFileName)
{
	QFile tagDbFile(tagDbFileName);
	if (!tagDbFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Cannot open tagDb file (" << tagDbFileName << ") for writing!" << endl;
		return -1;
	}

	QTextStream tagDb(&tagDbFile);

	QList<QString> tagList = m_tokenMap.keys();

	qDebug() << "Before writing. Tag total: " << m_tokenMap.size() << endl;

	for (int i = 0; i < tagList.size(); ++i) {
		tagDb << tagList[i] << kDB_TAG_RECORD_SEPERATOR << m_tokenMap[tagList[i]] << '\n';
	}

	tagDb.flush();
	tagDbFile.close();

	qDebug() << "Finish writing. Tag total: " << m_tokenMap.size() << endl;

	return 0;
}

int QTagger::getFileLineContent(const QString& fileName, const QList<unsigned long>& lineNumList, QList<CTagResultItem>& resultLineList,
							const QStringList& lineFilterStrList, const QStringList& functionNameFilterStrList, const QStringList& excludePatternFilterStrList,
							int linePrintBeforeMatch, int linePrintAfterMatch, const Qt::CaseSensitivity& caseSensitivity)
{
	QFile sourceFile(fileName);
	QString line;
	unsigned long currentLineNum = 0;
	unsigned long j;

	long k;
	int i;

	int functionBracketIndex;
	int closeBracketIndex;

	bool bCanSkip;

	QStringList lastLines;
	QList<int> lastManualIndentLevelList;

	QString lastLinesJoined;


	bool bMatchedLineFilterStringList;
	bool bMatchedFunctionNameFilter;
	bool bPassExcludePatternFilter;

	if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Cannot open source file (" << fileName << ") for reading!" << endl;
		return -1;
	}

	QTextStream sourceFileStream(&sourceFile);

	j = 0;

	unsigned long lineNumListTotal = static_cast <unsigned long> (lineNumList.size());

	int linePrintAfter = 0;

	CTagResultItem resultItem;

	CTagResultItem pendingResultItem;
	bool bGotPendingResultItem = false;

	int prevousIndentLevel = 0;
	int indentLevel = 0;

	int openBracketIndex = 0;

	resetCommentSkip();

	resultItem.functionSignature_.clear();

	int manualIndentLevel = 0;

	QVector<QRegExp> lineFilterRegExpMatcher;

	for (i = 0; i < lineFilterStrList.size(); i++) {
		lineFilterRegExpMatcher.push_back(QRegExp(lineFilterStrList[i], caseSensitivity));
	}

	while (!sourceFileStream.atEnd()) {
		currentLineNum++;
		line = sourceFileStream.readLine();

		manualIndentLevel = getManualIndentLevel(line);

		if (sourceFileStream.atEnd()) {
			break;
		}

		bCanSkip = skipInsideCommentAndRemoveComment(line, currentLineNum);

		if (bCanSkip) {
			continue;
		}

		lastLines.append(line);
		lastManualIndentLevelList.append(manualIndentLevel);

		// increment indent level
		openBracketIndex = line.indexOf("{");
		if (openBracketIndex != -1) {
			indentLevel++;
		}

		if ((prevousIndentLevel == 0) && (indentLevel == 1)) {
			lastLinesJoined = "";

			for (i = lastLines.size() - 1; i >= 0; i--) { // search most recent bracket match
				functionBracketIndex = lastLines.at(i).indexOf("(");
                if (functionBracketIndex != -1) {
					lastLinesJoined = lastLines.at(i);
					lastLinesJoined.truncate(functionBracketIndex);
					lastLinesJoined = lastLinesJoined.trimmed();
					break;
				}
			}

			QString functionName;
			extractLastToken(lastLinesJoined, functionName);

			if (!m_keywordSet.isEmpty()) {
				if (!m_keywordSet.contains(functionName)) {
					resultItem.functionSignature_ = functionName;
				}
			}
		}

		// decrement indent level
		closeBracketIndex = line.indexOf("}");
		if (closeBracketIndex != -1) {
			indentLevel--;
		}

		if ((j < lineNumListTotal) || (linePrintAfter > 0)) {
			if (currentLineNum == lineNumList[j]) {

                if (bGotPendingResultItem) {
					resultLineList << pendingResultItem;
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
						if (lineFilterRegExpMatcher[k].indexIn(line) == -1) {
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

				// function filter, assume not matched if not empty, only matched if any query function name matched
				if (functionNameFilterStrList.empty()) {
					bMatchedFunctionNameFilter = true;
				} else {
					bMatchedFunctionNameFilter = false;

					// consider as already mismatch if function signature is empty
					if (resultItem.functionSignature_.isEmpty()) {
						bMatchedFunctionNameFilter = false;
					} else {
						bMatchedFunctionNameFilter = false;

                        //qDebug() << "resultItem.functionSignature_ = " << resultItem.functionSignature_ << endl;

						for (k = 0; k < functionNameFilterStrList.size(); k++) {
							//qDebug() << "functionNameFilterStrList.at(k) = " << functionNameFilterStrList.at(k) << endl;

							if (resultItem.functionSignature_.contains(functionNameFilterStrList.at(k))) {
								bMatchedFunctionNameFilter = true;
								break;
							}
						}
					}
				}

				// valid only if all matched
				if (bMatchedLineFilterStringList && bMatchedFunctionNameFilter && bPassExcludePatternFilter) {
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

					} else if (linePrintAfterMatch > 0) {
						resultItem.lineSrcIndentLevel_ = manualIndentLevel;
						pendingResultItem = resultItem;
						bGotPendingResultItem = true;

						linePrintAfter = linePrintAfterMatch;
					} else {
						resultLineList << resultItem;
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

		prevousIndentLevel = indentLevel;
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
		qDebug() << "Cannot open tagDb file (" << tagDbFileName << ") for reading!" << endl;
		return -1;
	}

	QTextStream tagDb(&tagDbFile);
	m_tagList.clear();

	while (!tagDb.atEnd()) {
		line = tagDb.readLine();
		tagFieldIndex = line.indexOf(kDB_TAG_RECORD_SEPERATOR);

		if (tagFieldIndex != -1) {
			tagName = line.mid(0, tagFieldIndex);
		}
		m_tagList.append(tagName);
	}

	tagDbFile.close();

	loadKeywordFile();
	return 0;
}

QStringList QTagger::getTagList()
{
	return m_tagList;
}

int QTagger::getMatchedTags(const QString& tagToQuery, QStringList& matchedTokenList, const Qt::CaseSensitivity& caseSensitivity)
{
	QString tagQueryStr = "^" + tagToQuery;
	QString tagQueryMatched = "";
	QRegExp tagQueryRegEx(tagQueryStr, caseSensitivity);

	QStringList result;
	foreach (const QString &tag, m_tagList) {
		if (tag.contains(tagQueryRegEx)) {
			matchedTokenList += tag;
		}
		if (matchedTokenList.size() > 20) {
			break;
		}
	}

	return 0;
}

int QTagger::queryTag(const QString& inputFileName, const QString& tagDbFileName, const QString& tagToQuery,
					QString& tagToQueryFiltered, QList<CTagResultItem>& resultList, const Qt::CaseSensitivity& caseSensitivity)
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

		tagToQueryList = tagToQuery.split(" ", QString::SkipEmptyParts);

        if (tagToQueryList.size() >= 2) {
			if (m_keywordSet.contains(tagToQueryList.at(0))) { // not using keyword for query, i.e. no keyword for first word
				tagToQueryList.swap(0, 1);
			}
		}

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
			} else if (queryField.startsWith("/t")) { // function name filter
                queryField.remove(0, 2); // remove header
				if (!queryField.isEmpty()) {
					functionNameFilterStrList << queryField;
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

		T_OutputItemList inputFileItemList;

		// load input file
        CSourceFileList::loadFileList(inputFileName, inputFileItemList);

		// query in tag db
		QFile tagDbFile(tagDbFileName);

		if (!tagDbFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug() << "Cannot open tagDb file (" << tagDbFileName << ") for reading!" << endl;
			return -1;
		}

		QTextStream tagDbFileStream(&tagDbFile);

		QString tagQueryStr = "^" + tagToQueryStr + kDB_TAG_RECORD_SEPERATOR + kDB_FIELD_FILE_RECORD_SEPERATOR; // tag start in column 1
		QString tagQueryMatched = "";

		QRegExp tagQueryRegEx(tagQueryStr, caseSensitivity);

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

		while (!tagDbFileStream.atEnd()) {
			line = tagDbFileStream.readLine();

			// Format: | Tag | ";" | file 1 ID | ":" | location 1 line num | "," | location 2 line num | ... | ";" | file 2 ID | ":" | location 1 line num | ...
			//tagFieldIndex = line.indexOf(tagQueryRegEx);

			tagFieldIndex = tagQueryRegEx.indexIn(line, 0);

			if (tagFieldIndex != -1) {

				// Result format: ";" | file 1 ID | ":" | location 1 line num | "," | location 2 line num | ... | ";" | file 2 ID | ":" | location 1 line num | ...

				//queryResultFileRecordListStr = line.mid(tagQueryStr.length() - 1); // exlcude tag and the caret regular expression character
				queryResultFileRecordListStr = line.mid(tagQueryRegEx.matchedLength() - 1); // exlcude tag and the caret regular expression character

				//qDebug() << "queryResultFileRecordListStr = " << queryResultFileRecordListStr;

				// Result format: file 1 ID | ":" | location 1 line num | "," | location 2 line num | ...
				queryResultFileRecordList = queryResultFileRecordListStr.split(kDB_FIELD_FILE_RECORD_SEPERATOR, QString::SkipEmptyParts);

				foreach (const QString& queryResultFileRecord, queryResultFileRecordList) {

					lineFieldIndex = queryResultFileRecord.indexOf(kDB_FIELD_FILE_SEPERATOR);

					fileId = queryResultFileRecord.mid(0, lineFieldIndex).toULong();

#ifdef kDEBUG_QTAGGER_QUERY
					qDebug() << "fileId = " << fileId;
#endif

					queryResultFileName = inputFileItemList[fileId].m_fileName;

#ifdef kDEBUG_QTAGGER_QUERY
					qDebug() << "queryResultFileName = " << queryResultFileName;
#endif

					// filename filter, assume not matched if not empty, only matched if any query filename matched
					if (fileNameFilterStrList.empty()) {
						bMatchedFileNameFilter = true;
					} else {
						bMatchedFileNameFilter = false;

						//qDebug() << "fileName = " << fileName << endl;

						for (k = 0; k < fileNameFilterStrList.size(); k++) {

							//qDebug() << "fileNameFilterStrList.at(k) = " << fileNameFilterStrList.at(k) << endl;
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
					queryResultLineNumStrList = queryResultLineNumListStr.split(kDB_FIELD_LINE_SEPERATOR, QString::SkipEmptyParts);

					queryResultLineNumList.clear();
					foreach (const QString& queryResultLineNumStr, queryResultLineNumStrList) {
						queryResultLineNumList << queryResultLineNumStr.toULong();
					}

					getFileLineContent(queryResultFileName, queryResultLineNumList, resultList,
									lineFilterStrList, functionNameFilterStrList, excludePatternFilterStrList,
									linePrintBeforeMatch, linePrintAfterMatch, caseSensitivity);
				}
			}
		}
		tagDbFile.close();
	}
	return 0;
}

int QTagger::resetCommentSkip()
{
	bBlockCommentFollowing_ = false;
	blockCommentStartIndex_ = -1;
	blockCommentEndIndex_ = -1;

	blockCommentBeginCol_ = -1;
	blockCommentBeginLine_ = 0;

	lineCommentStartIndex_ = -1;
	commentAction_ = NON_COMMENT;

	bSkipCommentIndexCheck_ = false;
	return 0;
}

bool QTagger::skipInsideCommentAndRemoveComment(QString& currentLineRead, const unsigned long lineNumReading)
{
	currentLineRead = currentLineRead.trimmed();

	// do not check comment start when already inside comment
	if (!bBlockCommentFollowing_) {
		commentAction_ = NON_COMMENT;

		lineCommentStartIndex_ = currentLineRead.indexOf("//");
		blockCommentStartIndex_ = currentLineRead.indexOf("/*");

		// check if contains comment
		if ((lineCommentStartIndex_ >= 0) && (blockCommentStartIndex_ == -1)) { // only line comment string
			commentAction_ = LINE_COMMENT;
		} else if ((blockCommentStartIndex_ >= 0) && (lineCommentStartIndex_ == -1)) { // only block comment string
			commentAction_ = BLOCK_COMMENT;
		} else if ((lineCommentStartIndex_ >= 0) && (blockCommentStartIndex_ >= 0)) { // contains both kind of comment, depends on which appears earlier
			if (lineCommentStartIndex_ < blockCommentStartIndex_) {
				commentAction_ = LINE_COMMENT;
			} else if (blockCommentStartIndex_ < lineCommentStartIndex_)  {
				commentAction_ = BLOCK_COMMENT;
			}
		}

		if (commentAction_ == LINE_COMMENT) {
			currentLineRead.remove(lineCommentStartIndex_, currentLineRead.length() - lineCommentStartIndex_); // remove from start of line comment until line end
		} else if (commentAction_ == BLOCK_COMMENT) {
			blockCommentBeginCol_ = blockCommentStartIndex_;

			bBlockCommentFollowing_ = true;
			blockCommentBeginLine_ = lineNumReading;
		}
	}

	// skipping comment, as comment following
	if (bBlockCommentFollowing_) {
		blockCommentEndIndex_ = currentLineRead.indexOf("*/");

		if (blockCommentEndIndex_ == -1) { // no comment end
			if (blockCommentBeginLine_ == lineNumReading) { // has comment started in this line, only skip the line start with comment
				currentLineRead.remove(blockCommentBeginCol_, currentLineRead.length() - blockCommentBeginCol_);
			} else {
				return true; // still comment, can skip
			}
		} else { // has comment end
			if (blockCommentBeginLine_ == lineNumReading) { // comment start, end at the same line
				currentLineRead.remove(blockCommentBeginCol_, blockCommentEndIndex_ - blockCommentBeginCol_ + 2); // include length of comment end string
			} else { // skip comment from column 0
				currentLineRead.remove(0, blockCommentEndIndex_ - blockCommentBeginCol_ + 2); // include length of comment end string
			}

			bBlockCommentFollowing_ = false;
		}
	}

    // after remove all comment, can skip the line if it becomes empty
	if (currentLineRead.isEmpty()) {
		return true;
	}

	return false;
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
		qDebug() << "Cannot open source file (" << fileName << ") for reading in parseSourceFile()!" << endl;
		return false;
	}

	QTextStream sourcefileStream(&sourcefile);

	qDebug() << "parseSourceFile IN, id =" << fileId << ", fileName =" << fileName <<  endl;

	QStringList tokenList;
	QString fileIdStr = QString::number(fileId);

	QString tokenMapValue;
	QString currentLineRead;
	QString lastLineStr;

	QString fileIdField;
	int fileIdFieldIndex;

	unsigned long lineNumReading = 0;
	bool bCanSkip;

	resetCommentSkip();

	while (!sourcefileStream.atEnd()) {
		lineNumReading++;

		tagCurrentLineSet.clear();

		currentLineRead = sourcefileStream.readLine();
		if (sourcefileStream.atEnd()) {
			break;
		}

		bCanSkip = skipInsideCommentAndRemoveComment(currentLineRead, lineNumReading);

		if (bCanSkip) {
			continue;
		}

		tokenList.clear();
		extractWordTokens(currentLineRead, tokenList);
		foreach (const QString& token, tokenList) {
			if (token.length() == 1) { // not storing single character
				continue;
			}

			if (token.length() == 2) {
				if (token.at(0).isDigit() && token.at(1).isDigit()) {  // not storing two digit tag
					continue;
				}
			}

			if (!tagCurrentLineSet.contains(token)) { // as not storing tag duplicate in same line
				tagCurrentLineSet.insert(token);

				if (!m_keywordSet.contains(token)) { // not storing language keyword

					fileIdField = kDB_FIELD_FILE_RECORD_SEPERATOR + fileIdStr + kDB_FIELD_FILE_SEPERATOR;

					if (tokenMap.contains(token)) { // token already exists

						tokenMapValue = tokenMap[token];

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
				}
			}
		}
	}
	sourcefile.close();

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
            currentWord += currentChar;
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

void QTagger::extractLastToken(const QString& str, QString& lastToken)
{
    int i;

	QString currentWord = "";
	unsigned char currentChar;

	for (i = str.length()-1; i >= 0 ; i--) { // search from the end of string
		currentChar = (unsigned char) (str.at(i).toLatin1());
		if (((currentChar >= 'a') && (currentChar <= 'z')) // small capital
			|| ((currentChar >= 'A') && (currentChar <= 'Z')) // large capital
			|| ((currentChar >= '0') && (currentChar <= '9')) // Number
			|| (currentChar == ':')
			|| (currentChar == '_')) { // undercore
            currentWord = currentChar + currentWord; // add char to the beginning
		} else {
			break;
		}
	}
	lastToken = currentWord;
}

void QTagger::loadKeywordFile()
{
	QString keywordFileName = QCoreApplication::applicationDirPath() + "/keyword.txt";
	QFile file(keywordFileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "cannot open keyword file (" << keywordFileName << ") for reading!" << endl;
		return;
	}

	QTextStream inputFileStream(&file);

	m_keywordSet.clear();
	while (!inputFileStream.atEnd()) {
		QString line = inputFileStream.readLine();
		QStringList keywordList = line.split(" ", QString::SkipEmptyParts);

		foreach (const QString& keyword, keywordList) {
			m_keywordSet.insert(keyword);
	   	}
	}
	file.close();
}




