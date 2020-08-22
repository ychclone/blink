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

#include "commonType.h"
#include "CUtils.h"
#include "CTagResultItem.h"

#include "CSourceFileList.h"

typedef QMap<QString, QString> T_TokenMapType;

class QTagger
{
public:
	QTagger();
	virtual ~QTagger();

	int createTag(const T_OutputItemList& inputFileList);
	int updateTag(const QMap<long, COutputItem>& inputFileList, const QString& tagDbFileName,
		const QMap<long, long>& fileIdCreatedMap, const QMap<long, long>& fileIdModifiedMap, const QMap<long, long>& fileIdDeletedMap);

	int writeTagDb(const QString& tagDbFileName);

	int loadTagList(const QString& tagDbFileName);
	QStringList getTagList();
	int getMatchedTags(const QString& tagToQuery, QStringList& matchedTokenList, const Qt::CaseSensitivity& caseSensitivity);

	int queryTag(const QString& inputFileName, const QString& tagDbFileName, const QString& tagToQuery,
				QString& tagToQueryFiltered, QList<CTagResultItem>& resultList, const Qt::CaseSensitivity& caseSensitivity, bool symbolRegularExpression);

	int resetCommentSkip();
	bool skipInsideCommentAndRemoveComment(QString& currentLineRead, const unsigned long lineNumReading);

	int getManualIndentLevel(QString& line);

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
	bool parseSourceFile(unsigned long fileId, const QString& fileName, T_TokenMapType& tokenMap);

	void extractWordTokens(const QString& str, QStringList& tokenList);
	void extractLastToken(const QString& str, QString& lastToken);

	void loadKeywordFile();

	QSet<QString> keywordSet_;

	QStringList tagList_;

	T_TokenMapType tokenMap_;

	int getFileLineContent(const QString& fileName, const QList<unsigned long>& lineNumList, QList<CTagResultItem>& resultLineList,
			const QStringList& lineFilterStrList, const QStringList& functionNameFilterStrList, const QStringList& excludePatternFilterStrList,
			int linePrintBeforeMatch, int linePrintAfterMatch, const Qt::CaseSensitivity& caseSensitivity);

	typedef enum {NON_COMMENT, LINE_COMMENT, BLOCK_COMMENT} ENUM_CommentAction;

	// for comment handling
	bool bBlockCommentFollowing_;
	int blockCommentStartIndex_;
	int blockCommentEndIndex_;

	int blockCommentBeginCol_;
	unsigned long blockCommentBeginLine_;

	int lineCommentStartIndex_;
	ENUM_CommentAction commentAction_;

	bool bSkipCommentIndexCheck_;



};

#endif

