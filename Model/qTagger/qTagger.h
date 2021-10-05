#ifndef QTAGGER_H
#define QTAGGER_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QSet>
#include <QHash>
#include <QtCore>
#include <QElapsedTimer>
#include <QVector>

#include "commonType.h"
#include "CUtils.h"
#include "CTagResultItem.h"

#include "CSourceFileList.h"

#include "CTagItem.h"

typedef QMap<QString, QString> T_TokenMapType;

class QTagger
{
public:
	QTagger();
	virtual ~QTagger();

	int initKeywordFileTokenMap();

	int createTag(const T_FileItemList& inputFileList);
	int updateTag(const QMap<long, CFileItem>& inputFileList, const QString& tagDbFileName,
		const QMap<long, long>& fileIdCreatedMap, const QMap<long, long>& fileIdModifiedMap, const QMap<long, long>& fileIdDeletedMap);

	int writeTagDb(const QString& tagDbFileName);

	int loadTagList(const QString& tagDbFileName);

	int levenshteinDistance(const QString &source, const QString &target);
	int getMatchedTags(const QString& tagToQuery, QMap<int, QString>& matchedTokenList, const Qt::CaseSensitivity& caseSensitivity);

	int queryTagLoadedSymbol(const T_FileItemList& inputFileItemList, const QString& tagToQuery,
					QString& tagToQueryFiltered, QList<CTagResultItem>& resultList, const Qt::CaseSensitivity& caseSensitivity, bool symbolRegularExpression, unsigned long limitSearchRow);

	int queryTag(const QString& inputFileName, const QString& tagDbFileName, const QString& tagToQuery,
				QString& tagToQueryFiltered, QList<CTagResultItem>& resultList, const Qt::CaseSensitivity& caseSensitivity, bool symbolRegularExpression);

	int getManualIndentLevel(QString& line);

	bool parseSourceFile(unsigned long fileId, const QString& fileName, T_TokenMapType& tokenMap);
	bool parseSourceFile(unsigned long fileId, const QString& fileName);

	static const char* kQTAG_CONFIG_FILE;
	static const char* kDB_FIELD_LINE_SEPERATOR;
	static const char* kDB_FIELD_FILE_SEPERATOR;
	static const char* kDB_FIELD_FILE_RECORD_SEPERATOR;
	static const char* kDB_TAG_RECORD_SEPERATOR;

	static const char* kDB_FIELD_RESULT_SEPERATOR;

	static const char* kQTAG_TAGS_DIR;

	static const char* kQTAG_DEFAULT_TAGDBNAME;

	static const char* kQTAG_DEFAULT_INPUTLIST_FILE;

private:

	void extractWordTokens(const QString& str, QStringList& tokenList);

	void loadKeywordFile();

	QSet<QString> keywordSet_;

	QVector<CTagItem> tagList_;

	T_TokenMapType tokenMap_;

	int getFileLineContent(const QString& fileName, const QList<unsigned long>& lineNumList, QList<CTagResultItem>& resultLineList,
			const QStringList& lineFilterStrList, const QStringList& excludePatternFilterStrList,
			int linePrintBeforeMatch, int linePrintAfterMatch, const Qt::CaseSensitivity& caseSensitivity, unsigned long limitSearchRow);



};

#endif

