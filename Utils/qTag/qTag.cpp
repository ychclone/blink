/* vim: set fdm=marker : */

#include <QtCore>
#include <QCoreApplication>
#include <QFileSystemWatcher>

#include <stdio.h>
#include <stdlib.h>
#include <QStringList>

#include <QElapsedTimer>
#include <QHash>

#include <unistd.h> // for getopt

#include <iostream>

#include "qTagApp.h"
#include "qTagger.h"

#include "CUtils.h"

#include "CTagResultItem.h"

using namespace std;

int main(int argc, char *argv[])
{
    QTagApplication app(argc, argv);
	QTagger tagger;
	QString tagToQuery;

	QString inputFileName(QTagger::kQTAG_DEFAULT_INPUTLIST_FILE); // default input file name
	QString tagDbFileName(QTagger::kQTAG_DEFAULT_TAGDBNAME); // default tag database file name
	QString configFileName(QTagger::kQTAG_CONFIG_FILE);

	Qt::CaseSensitivity caseSensitive;

	bool cFlag = false; // create tag
	bool qFlag = false; // query tag
	bool tFlag = false;
	bool iFlag = false; // default to false, i.e. case sensitive in query
	bool dFlag = false; // debug flag
	bool lFlag = false; // input file

	char *qValue = NULL;
	char *tValue = NULL;
	char *lValue = NULL;

	int cOpt;

	while ((cOpt = getopt(argc, argv, "cq:t:i:d:l")) != -1) {
		switch (cOpt)
		{
			case 'c':
				cFlag = true;
				break;
			case 'q':
				qFlag = true;
				qValue = optarg;
				break;
			case 't':
				tFlag = true;
				tValue = optarg;
				break;
			case 'i':
				iFlag = true;
				break;
			case 'd':
				dFlag = true;
				break;
			case 'l':
				lFlag = true;
				lValue = optarg;
				break;

			case '?':
				if ((optopt == 'q') || (optopt == 't') || (optopt == 'l')) {
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				} else if (isprint(optopt)) {
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				} else {
					fprintf (stderr,
							"Unknown option character `\\x%x'.\n",
							optopt);
				}
		}
	}

	// Case Sensitivity
	if (iFlag) {
		caseSensitive = Qt::CaseInsensitive;
	} else {
		caseSensitive = Qt::CaseSensitive;
	}

	// handling of input file
	if (lFlag) {
		inputFileName = lValue;
	}

	// handling of tag database file
	if (tFlag) {
		tagDbFileName = tValue;
	}

	if (cFlag) { // remove existing env and db files first
		QElapsedTimer timer;
		timer.start();

		T_FileItemList fileList;

		CSourceFileList::loadFileList(inputFileName, fileList);

		qDebug() << "Processing File..." << Qt::endl;

		tagger.createTag(fileList);
		tagger.writeTagDb(tagDbFileName);

		qDebug() << "Operation took" << QString::number(((double) timer.elapsed() / 1000), 'f', 3) << "s" << Qt::endl;
	}

    // handling of tag to be query
	if (qFlag) {
		// read currently load project

        qDebug() << "applicationDirPath() A IN" << Qt::endl;
		qDebug() << "QCoreApplication::applicationDirPath() = " << QCoreApplication::applicationDirPath() << Qt::endl;
		QFile qTagConfigFile(QCoreApplication::applicationDirPath() + "/" + configFileName);
		qDebug() << "applicationDirPath() A OUT" << Qt::endl;

		if (!qTagConfigFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug() << "Cannot open config file (" << configFileName << ") for reading!" << Qt::endl;
			return -1;
		}

		QTextStream qTagConfigFileIn(&qTagConfigFile);

		QString configStr;
		QString projectLoadStr = "projectLoad=";

		while (!qTagConfigFileIn.atEnd()) {
			configStr = qTagConfigFileIn.readLine();
			if (configStr.startsWith(projectLoadStr)) {
				configStr.remove(0, projectLoadStr.length());

				qDebug() << "applicationDirPath() B IN" << Qt::endl;
				qDebug() << "QCoreApplication::applicationDirPath() = " << QCoreApplication::applicationDirPath() << Qt::endl;
				tagDbFileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tags/" + configStr + "/" + tagDbFileName;
				qDebug() << "applicationDirPath() B OUT" << Qt::endl;

				qDebug() << "applicationDirPath() C IN" << Qt::endl;
				qDebug() << "QCoreApplication::applicationDirPath() = " << QCoreApplication::applicationDirPath() << Qt::endl;
				inputFileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tags/" + configStr + "/" + inputFileName;
				qDebug() << "applicationDirPath() C OUT" << Qt::endl;

				break;
			}
		}

		qTagConfigFile.close();

		// start query
		tagToQuery = QString(qValue);

		QList<CTagResultItem> resultList;
		QString tagToQueryFiltered;

		tagger.queryTag(inputFileName, tagDbFileName, tagToQuery, tagToQueryFiltered, resultList, caseSensitive);

		foreach (const CTagResultItem& result, resultList) {
            //cout << "[" << result.functionSignature_.toStdString() << "]" << Qt::endl;
			cout << result << Qt::endl;
		}
	}

	return 0;
}


