#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QPlainTextEdit>
#include <QStandardPaths>
#include <QClipboard>
#include <QMenu>
#include <QAction>

#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexersql.h>

#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexeryaml.h>

#include <Qsci/qscilexerverilog.h>

#include <Qsci/qscilexermakefile.h>

#include <QFileInfo>

#include <QCloseEvent>
#include <QFileDialog>

#include "CEditor.h"

#include "Model/CConfigManager.h"

#include "CMainWindow.h"

CEditor::CEditor(CMainWindow *parent)
{
	setupUi(this);
	parent_ = parent;

	// actions for tab widget
	connect(tabWidget, &QTabWidget::currentChanged, this, &CEditor::tabChanged);
	connect(tabWidget, &QTabWidget::tabCloseRequested, this, &CEditor::closeTab);

	tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(tabWidget->tabBar(), &QWidget::customContextMenuRequested, this, &CEditor::tabContextMenuEvent);

	currentNewFileNumber_ = 1;

	endModificationTimer_ = new QTimer(this);
	endModificationTimer_->setSingleShot(true);
	connect(endModificationTimer_, &QTimer::timeout, this, &CEditor::onEndModificationTimeout);

	endNavigationTimer_ = new QTimer(this);
	endNavigationTimer_->setSingleShot(true);
	connect(endNavigationTimer_, &QTimer::timeout, this, &CEditor::onEndNavigationTimeout);

	connect(&fileWatcher_, &QFileSystemWatcher::fileChanged, this, &CEditor::fileChanged);

	isNavigatingHistory_  = false;
}

void CEditor::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty())
	{
		loadFileNewTab(fileName);
	}
}

void CEditor::save()
{
	saveFile(filePathInTab(tabWidget->currentIndex()));
}

void CEditor::saveAs()
{
	QString newFileName = QFileDialog::getSaveFileName(this, "Save File", filePathInTab(tabWidget->currentIndex()));
	if (newFileName.isEmpty())
	{
		return;
	}

	QString oldFileName = filePathInTab(tabWidget->currentIndex());

	QFile file(newFileName);
	if (!file.open(QFile::WriteOnly))
	{
		QMessageBox::warning(this, tr("Save file"),
							 tr("Failed to write to file %1:\n%2.")
								 .arg(newFileName)
								 .arg(file.errorString()));
		return;
	}

	QTextStream fileOutput(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);

	fileOutput << editorTabMap_[oldFileName].textEdit->text();

	QApplication::restoreOverrideCursor();

	emit statusLeft("File saved.");

	editorTabMap_[oldFileName].textEdit->setModified(false);

	delete editorTabMap_[oldFileName].textEdit->lexer();

	QsciLexer *newLexer = getLexer(newFileName);

	editorTabMap_[newFileName].textEdit = editorTabMap_[oldFileName].textEdit;
	editorTabMap_[newFileName].textEdit->setLexer(newLexer);
	editorTabMap_[newFileName].textEdit->setModified(false);

	editorTabMap_.remove(oldFileName);

	QFileInfo fileInfo(newFileName);
	tabWidget->setTabText(tabWidget->currentIndex(), fileInfo.fileName());

	emit statusRight(newFileName);
}

void CEditor::saveFile(const QString &fileName)
{
	beginFileModification(fileName);

	QFile file(fileName);
	if (!file.open(QFile::WriteOnly))
	{
		QMessageBox::warning(this, tr("Save file"),
							 tr("Failed to write to file %1:\n%2.")
								 .arg(fileName)
								 .arg(file.errorString()));
		endFileModification(fileName);
		return;
	}

	QTextStream fileOutput(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);

	fileOutput << editorTabMap_[fileName].textEdit->text();

	QApplication::restoreOverrideCursor();

	emit statusLeft("File saved.");

	editorTabMap_[fileName].textEdit->setModified(false);

	emit statusRight(fileName);

	endModificationTimer_->start(2000);
}

void CEditor::findText(const QString &text, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());

	qDebug() << "findText = " << text;

	editorTabMap_[currentFileName].textEdit->findFirst(text, bRegularExpression, bCaseSensitive, bMatchWholeWord, true, true);

	if (bRegularExpression)
	{
		editorTabMap_[currentFileName].textEdit->findNext();
	}
}

void CEditor::replaceText(const QString &findText, const QString &replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());

	editorTabMap_[currentFileName].textEdit->findFirst(findText, bRegularExpression, bCaseSensitive, bMatchWholeWord, true, true);
	editorTabMap_[currentFileName].textEdit->replace(replaceText);
}

void CEditor::replaceAllText(const QString &findText, const QString &replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());

	long totalMatchCount = 0;
	long matchCount = 0;

	// bRegularExpression, bCaseSensitive, bMatchWholeWord
	QRegularExpression regExp(findText);

	Qt::CaseSensitivity caseSensitive;

	if (bCaseSensitive)
	{
		caseSensitive = Qt::CaseSensitive;
		// default expression default to case sensitive
	}
	else
	{
		caseSensitive = Qt::CaseInsensitive;
		regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}

	QString inputContent = editorTabMap_[currentFileName].textEdit->text();
	QString inputLine, replacedLine, outputFileStream = "";
	QTextStream inputStream(&inputContent);

	if (bRegularExpression || bMatchWholeWord)
	{
		// match whole word take effect only when regular expression is not enabled
		if (bMatchWholeWord && !bRegularExpression)
		{
			regExp.setPattern("\\b" + findText + "\\b");
		}
		while (inputStream.readLineInto(&inputLine))
		{
			matchCount = inputLine.count(regExp);

			if (matchCount > 0)
			{
				replacedLine = inputLine.replace(regExp, replaceText);
				outputFileStream += replacedLine + "\n";
				totalMatchCount += matchCount;
			}
			else
			{
				outputFileStream += inputLine + "\n";
			}
		}
	}
	else
	{
		while (inputStream.readLineInto(&inputLine))
		{
			matchCount = inputLine.count(findText, caseSensitive);
			if (matchCount > 0)
			{
				replacedLine = inputLine.replace(findText, replaceText, caseSensitive);
				outputFileStream += replacedLine + "\n";
				totalMatchCount += matchCount;
			}
			else
			{
				outputFileStream += inputLine + "\n";
			}
		}
	}

	if (totalMatchCount > 0)
	{
		editorTabMap_[currentFileName].textEdit->setText(outputFileStream);

		emit statusLeft("Replaced " + QString::number(totalMatchCount) + " strings.");
	}
}

// same tab
void CEditor::loadFileWithLineNum(const QString &filePath, int lineNumber)
{
	qDebug() << "loadFileWithLineNum IN, filePath =  " << filePath;
	int ret = loadFile(filePath);

	if (ret == -1)
	{
		return;
	}

	if (lineNumber > 4)
	{
		editorTabMap_[filePath].textEdit->setFirstVisibleLine(lineNumber - 4);
	}

	textEditGoToLine(editorTabMap_[filePath].textEdit, lineNumber);
	editorTabMap_[filePath].textEdit->setFocus();

	emit statusRight(filePath);
	qDebug() << "loadFileWithLineNum OUT, filePath =  " << filePath;
}

void CEditor::loadFileWithLineNumNewTab(const QString &filePath, int lineNumber)
{
	loadFileNewTab(filePath);

	if (lineNumber > 4)
	{
		editorTabMap_[filePath].textEdit->setFirstVisibleLine(lineNumber - 4);
	}
	textEditGoToLine(editorTabMap_[filePath].textEdit, lineNumber);
	editorTabMap_[filePath].textEdit->setFocus();

	emit statusRight(filePath);
}

void CEditor::newFile()
{
	QsciScintilla *textEdit = new QsciScintilla;
	QsciLexer *lexer;

	lexer = new QsciLexerCPP;

	setEditorFont(lexer);

	textEdit->setText("");
	textEdit->setAutoIndent(true);
	textEdit->setLexer(lexer);

	textEdit->setCaretLineBackgroundColor(QColor("#8fdcf1"));
	textEdit->setCaretLineVisible(true);

	textEditGoToLine(textEdit, 0);

	createActions(textEdit);

	EditorTab editorTab = {.textEdit = textEdit, .lexer = lexer};
	editorTabMap_["new " + QString::number(currentNewFileNumber_)] = editorTab;

	tabWidget->addTab(textEdit, "new " + QString::number(currentNewFileNumber_));
	currentNewFileNumber_++;

	textEdit->setModified(false);
}

QsciLexer *CEditor::getLexer(const QString &fileName)
{
	QFileInfo fileInfo(fileName);
	QsciLexer *lexer;

	QString suffix = fileInfo.suffix();

	qDebug() << "suffix = " << suffix;

	if (suffix == "cpp")
	{
		lexer = new QsciLexerCPP;
	}
	else if (suffix == "java" || suffix == "kt")
	{
		lexer = new QsciLexerJava;
	}
	else if (suffix == "py")
	{
		lexer = new QsciLexerPython;
	}
	else if (suffix == "js" || suffix == "ts")
	{
		lexer = new QsciLexerJavaScript;
	}
	else if (suffix == "rb")
	{
		lexer = new QsciLexerRuby;
	}
	else if (suffix == "sql")
	{
		lexer = new QsciLexerSQL;
	}
	else if (suffix == "html")
	{
		lexer = new QsciLexerHTML;
	}
	else if (suffix == "xml")
	{
		lexer = new QsciLexerXML;
	}
	else if (suffix == "css")
	{
		lexer = new QsciLexerCSS;
	}
	else if (suffix == "md")
	{
		lexer = new QsciLexerMarkdown;
	}
	else if (suffix == "yaml")
	{
		lexer = new QsciLexerYAML;
	}
	else if (suffix == "cs")
	{
		lexer = new QsciLexerCSharp;
	}
	else if (suffix == "v" || suffix == "vh" || suffix == "sv" || suffix == "svh")
	{
		lexer = new QsciLexerVerilog;
	}
	else
	{
		lexer = new QsciLexerCPP;
	}

	if (fileName == "Makefile")
	{
		lexer = new QsciLexerMakefile;
	}

	setEditorFont(lexer);

	return lexer;
}

void CEditor::setTextEdit(QsciScintilla *textEdit)
{
	textEdit->setAutoIndent(true);
	textEdit->setAutoCompletionThreshold(1);
	textEdit->setAutoCompletionSource(QsciScintilla::AcsDocument);
	textEdit->setFolding(QsciScintilla::PlainFoldStyle);

	textEdit->setTabWidth(4);
}

int CEditor::loadFile(const QString &filePath)
{
	QMessageBox::StandardButton reply = QMessageBox::Cancel;

	if (editorTabMap_.contains(filePath))
	{ // load previous tab for existing file
		EditorTab editorTab = editorTabMap_[filePath];
		tabWidget->setCurrentWidget(editorTab.textEdit);
	}
	else
	{
		QFileInfo fileInfo(filePath);
		QString filename(fileInfo.fileName());

		if (tabWidget->currentIndex() != -1)
		{
			QString oldPath = filePathInTab(tabWidget->currentIndex());

			if (editorTabMap_[oldPath].textEdit->isModified())
			{
				reply = QMessageBox::question(this, "Save", "Save file \"" + oldPath + "\"?",
											  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

				if (reply == QMessageBox::Yes)
				{
					saveFile(oldPath);
				}
				else if (reply == QMessageBox::Cancel)
				{
					qDebug() << "cancel QMessagebox";
					return -1;
				}
			}

			qDebug() << "continue QMessagebox";

			QsciScintilla *currentTextEdit = static_cast<QsciScintilla *>(tabWidget->currentWidget());
			if (currentTextEdit != NULL)
			{
				for (auto editorTab = editorTabMap_.cbegin(), end = editorTabMap_.cend(); editorTab != end; ++editorTab)
				{
					if (editorTab.value().textEdit == currentTextEdit)
					{
						QsciLexer *previousLexer = currentTextEdit->lexer();

						QsciLexer *lexer = getLexer(filePath);

						QFile file(filePath);
						if (!file.open(QFile::ReadOnly))
						{
							QMessageBox::warning(this, tr("Editor"),
												 tr("Cannot read file %1:\n%2.")
													 .arg(filePath)
													 .arg(file.errorString()));
							return -1;
						}

						QTextStream in(&file);
						QApplication::setOverrideCursor(Qt::WaitCursor);

						currentTextEdit->setText(in.readAll());
						QApplication::restoreOverrideCursor();

						currentTextEdit->setLexer(lexer);

						setTextEdit(currentTextEdit);

						delete previousLexer;

						createActions(currentTextEdit);

						setupFileWatcher(filePath);

						EditorTab newEditorTab = {.textEdit = currentTextEdit, .lexer = lexer};
						editorTabMap_[filePath] = newEditorTab;
						
						editorTabMap_[filePath].textEdit->setCaretLineBackgroundColor(QColor("#8fdcf1"));
						editorTabMap_[filePath].textEdit->setCaretLineVisible(true);

						textEditGoToLine(editorTabMap_[filePath].textEdit, 0);

						editorTabMap_[filePath].textEdit->setModified(false);

						if (filePath != editorTab.key())
						{
							tabWidget->setTabText(tabWidget->currentIndex(), filename);
							editorTabMap_.remove(editorTab.key());
						}

						addToNavHistory(filePath, 0);

						break;
					}
				}
			}
			else
			{
				loadFileNewTab(filePath);
			}
		}
		else
		{
			loadFileNewTab(filePath);
		}
	}
	return 0;
}

void CEditor::loadFileNewTab(const QString &filePath)
{
	if (editorTabMap_.contains(filePath))
	{ // load previous tab for existing file
		EditorTab editorTab = editorTabMap_[filePath];
		tabWidget->setCurrentWidget(editorTab.textEdit);
	}
	else
	{
		QFileInfo fileInfo(filePath);
		QString filename(fileInfo.fileName());

		QsciScintilla *textEdit = new QsciScintilla;
		QsciLexer *lexer = getLexer(filePath);

		QFile file(filePath);
		if (!file.open(QFile::ReadOnly))
		{
			QMessageBox::warning(this, tr("Editor"),
								 tr("Cannot read file %1:\n%2.")
									 .arg(filePath)
									 .arg(file.errorString()));
			return;
		}

		QTextStream in(&file);
		// QApplication::setOverrideCursor(Qt::WaitCursor);

		textEdit->setText(in.readAll());
		// QApplication::restoreOverrideCursor();

		textEdit->setLexer(lexer);

		setTextEdit(textEdit);

		createActions(textEdit);

		setupFileWatcher(filePath);

		EditorTab editorTab = {.textEdit = textEdit, .lexer = lexer};
		editorTabMap_[filePath] = editorTab;

		tabWidget->addTab(textEdit, filename);

		tabWidget->setCurrentWidget(editorTab.textEdit);
		emit statusLeft("File loaded");

		editorTabMap_[filePath].textEdit->setModified(false);

		editorTabMap_[filePath].textEdit->setCaretLineBackgroundColor(QColor("#8fdcf1"));
		editorTabMap_[filePath].textEdit->setCaretLineVisible(true);

		textEditGoToLine(editorTabMap_[filePath].textEdit, 0);
	}
}

void CEditor::createActions(QsciScintilla *textEdit)
{
	// connect(actionCut, &QAction::triggered, textEdit, &QsciScintilla::cut);
	// connect(actionCopy, &QAction::triggered, textEdit, &QsciScintilla::copy);
	// connect(actionPaste, &QAction::triggered, textEdit, &QsciScintilla::paste);

	// connect(actionUndo, &QAction::triggered, textEdit, &QsciScintilla::undo);
	// connect(actionRedo, &QAction::triggered, textEdit, &QsciScintilla::redo);

	connect(textEdit, &QsciScintilla::textChanged, this, &CEditor::textEditModified);
	connect(textEdit, &QsciScintilla::cursorPositionChanged, parent_, &CMainWindow::showCurrentCursorPosition);

	connect(textEdit, &QsciScintilla::cursorPositionChanged, this, &CEditor::handleCursorPositionChanged);
}

void CEditor::textEditModified()
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());
	editorTabMap_[currentFileName].textEdit->setModified(true);
	emit statusRight("* " + currentFileName);

	// Add current edit location to history
	int line, index;
	editorTabMap_[currentFileName].textEdit->getCursorPosition(&line, &index);
}

void CEditor::handleCursorPositionChanged(int line, int index)
{
	if (!isNavigatingHistory_) {
		QString currentFileName = filePathInTab(tabWidget->currentIndex());
		qDebug() << "handleCursorPositionChanged() IN, " << "currentFileName = " << currentFileName << ", line = " << line << ", index = " << index << Qt::endl;	
		addToNavHistory(currentFileName, line);
		emit updateGoForwardBackwardActions(true, navHistory_.size() > 1);
		qDebug() << "handleCursorPositionChanged() OUT, " << "currentFileName = " << currentFileName << ", line = " << line << ", index = " << index << Qt::endl;	
	} else {
		qDebug() << "isNavigatingHistory_ = true in handleCursorPositionChanged()" << Qt::endl;
	}
}

void CEditor::updateAllEditorFont()
{
	QFont editorFont;

	QString editorFontSettingStr = CConfigManager::getInstance()->getAppSettingValue("EditorFont").toString();
	editorFont.fromString(editorFontSettingStr);

	if (editorFontSettingStr == "")
	{
		editorFont = QApplication::font();
	}

	for (auto editorTab : editorTabMap_)
	{
		editorTab.textEdit->lexer()->setFont(editorFont);
	}
}

void CEditor::setEditorFont(QsciLexer *lexer)
{
	QFont editorFont;

	QString editorFontSettingStr = CConfigManager::getInstance()->getAppSettingValue("EditorFont").toString();
	editorFont.fromString(editorFontSettingStr);

	qDebug() << "editorFontSettingStr = " << editorFontSettingStr << Qt::endl;

	QFont consolasFont("Consolas", QApplication::font().pointSize());

	if (editorFontSettingStr == "")
	{
		lexer->setFont(consolasFont);
	}
	else
	{
		lexer->setFont(editorFont);
	}
}

void CEditor::saveWidgetPosition()
{
	CConfigManager::getInstance()->setValue("Editor", "geometry", saveGeometry());
}

void CEditor::closeEvent(QCloseEvent *event)
{
	saveWidgetPosition();
	event->accept();
}

void CEditor::tabChanged(int tabIndex)
{
	if (tabIndex >= 0)
	{
		QString currentFileName = filePathInTab(tabIndex);

		if (currentFileName != "")
		{
			if (editorTabMap_[currentFileName].textEdit->isModified())
			{
				emit statusRight("* " + currentFileName);
			}
			else
			{
				emit statusRight(currentFileName);
			}
		}
	}
}

void CEditor::closeCurrentTab(const QPoint &pos)
{
	int tabIndex = tabWidget->tabBar()->tabAt(pos);

	if (tabIndex == -1) {
		return;
	}

	this->closeTab(tabIndex);
}

QString CEditor::filePathInTab(int tabIndex)
{
	QString filePathReturn = "";
	QsciScintilla *textEdit = static_cast<QsciScintilla *>(tabWidget->widget(tabIndex));

	for (const QString &filePath : editorTabMap_.keys())
	{
		if (editorTabMap_[filePath].textEdit == textEdit)
		{
			filePathReturn = filePath;
			break;
		}
	}

	return filePathReturn;
}

void CEditor::closeTab(int tabIndex)
{
	qDebug() << "tabIndex in closeTab = " << tabIndex;

	QString filePathRemoveFromMap = filePathInTab(tabIndex);

	qDebug() << "filePathRemoveFromMap = " << filePathRemoveFromMap;

	QMessageBox::StandardButton reply;

	if (editorTabMap_[filePathRemoveFromMap].textEdit->isModified())
	{
		reply = QMessageBox::question(this, "Save", "Save file \"" + filePathRemoveFromMap + "\"?",
									  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

		if (reply == QMessageBox::Yes)
		{
			saveFile(filePathRemoveFromMap);
		}
		else if (reply == QMessageBox::Cancel)
		{
			return;
		}
	}

	tabWidget->removeTab(tabIndex);

	delete editorTabMap_[filePathRemoveFromMap].textEdit;
	delete editorTabMap_[filePathRemoveFromMap].lexer;
	editorTabMap_.remove(filePathRemoveFromMap);

	fileWatcher_.removePath(filePathRemoveFromMap);

	if (tabWidget->count() == 0)
	{
		newFile();
	}
}

void CEditor::goToLine(int line)
{
	QString filePath = filePathInTab(tabWidget->currentIndex());
	textEditGoToLine(editorTabMap_[filePath].textEdit, line);
}

void CEditor::textEditGoToLine(QsciScintilla *textEdit, int line)
{
	qDebug() << "textEditGoToLine() IN, " << "line = " << line << Qt::endl;
	isNavigatingHistory_ = true;
	textEdit->setCursorPosition(line, 0);
	textEdit->setFocus();
	endNavigationTimer_->start(2000);
	qDebug() << "textEditGoToLine() OUT " << Qt::endl;
}

void CEditor::cut()
{
	QString filePath = filePathInTab(tabWidget->currentIndex());

	editorTabMap_[filePath].textEdit->cut();
}

void CEditor::copy()
{
	QString filePath = filePathInTab(tabWidget->currentIndex());

	editorTabMap_[filePath].textEdit->copy();
}

void CEditor::paste()
{
	QString filePath = filePathInTab(tabWidget->currentIndex());

	editorTabMap_[filePath].textEdit->paste();
}

void CEditor::undo()
{
	QString filePath = filePathInTab(tabWidget->currentIndex());

	editorTabMap_[filePath].textEdit->undo();
}

void CEditor::redo()
{
	QString filePath = filePathInTab(tabWidget->currentIndex());

	editorTabMap_[filePath].textEdit->redo();
}

void CEditor::closeAllTabsButCurrent(const QPoint &pos)
{
	int tabIndex = tabWidget->tabBar()->tabAt(pos);

	if (tabIndex == -1) {
		return;
	}

	for (int i = tabWidget->count() - 1; i >= 0; --i)
	{
		if (i != tabIndex)
		{
			closeTab(i);
		}
	}
}

void CEditor::closeAllTabsToLeft(const QPoint &pos)
{
	int tabIndex = tabWidget->tabBar()->tabAt(pos);

	if (tabIndex == -1) {
		return;
	}

	for (int i = tabIndex - 1; i >= 0; --i)
	{
		closeTab(i);
	}
}

void CEditor::closeAllTabsToRight(const QPoint &pos)
{
	int tabIndex = tabWidget->tabBar()->tabAt(pos);

	if (tabIndex == -1) {
		return;
	}

	for (int i = tabWidget->count() - 1; i > tabIndex; --i)
	{
		closeTab(i);
	}
}

void CEditor::copyFilePath(const QPoint &pos)
{
	int tabIndex = tabWidget->tabBar()->tabAt(pos);

	if (tabIndex == -1) {
		return;
	}

	QString filePath = filePathInTab(tabIndex);
	qDebug() << "filePath = " << filePath << Qt::endl;
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(filePath);
}

void CEditor::copyFileName(const QPoint &pos)
{
	int tabIndex = tabWidget->tabBar()->tabAt(pos);

	if (tabIndex == -1) {
		return;
	}

	QString filePath = filePathInTab(tabIndex);
	QFileInfo fileInfo(filePath);
	QString fileName = fileInfo.fileName();
	qDebug() << "fileName = " << fileName << Qt::endl;
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(fileName);
}

void CEditor::copyFile(const QPoint &pos)
{
	int tabIndex = tabWidget->tabBar()->tabAt(pos);

	if (tabIndex == -1) {
		return;
	}

	QString filePath = filePathInTab(tabIndex);
	QFile file(filePath);
	
	QMimeData *mimeData = new QMimeData();
	QList<QUrl> urls;
	urls << QUrl::fromLocalFile(filePath);
	mimeData->setUrls(urls);

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setMimeData(mimeData);

	qDebug() << "File MIME data copied to clipboard from: " << filePath << Qt::endl;
}



void CEditor::tabContextMenuEvent(const QPoint &pos)
{
	int tabIndex = tabWidget->tabBar()->tabAt(pos);
	if (tabIndex != -1)
	{
		QMenu menu(this);
		QAction *closeCurrentAction = menu.addAction("Close");
		QAction *closeOthersAction = menu.addAction("Close All But This");
		QAction *closeLeftAction = menu.addAction("Close All to the Left");
		QAction *closeRightAction = menu.addAction("Close All to the Right");
		QAction *copyPathAction = menu.addAction("Copy File Path");
		QAction *copyFileNameAction = menu.addAction("Copy File Name");
		QAction *copyFileAction = menu.addAction("Copy File");

		connect(closeCurrentAction, &QAction::triggered, this, [this, pos]() { this->closeCurrentTab(pos); });
		connect(closeOthersAction, &QAction::triggered, this, [this, pos]() { this->closeAllTabsButCurrent(pos); });
		connect(closeLeftAction, &QAction::triggered, this, [this, pos]() { this->closeAllTabsToLeft(pos); });
		connect(closeRightAction, &QAction::triggered, this, [this, pos]() { this->closeAllTabsToRight(pos); });
		connect(copyPathAction, &QAction::triggered, this, [this, pos]() { this->copyFilePath(pos); });
		connect(copyFileNameAction, &QAction::triggered, this, [this, pos]() { this->copyFileName(pos); });
		connect(copyFileAction, &QAction::triggered, this, [this, pos]() { this->copyFile(pos); });

		menu.exec(tabWidget->tabBar()->mapToGlobal(pos));
	}
}

void CEditor::addToNavHistory(const QString& filePath, int line)
{
	qDebug() << "addToNavHistory() IN, " << "filePath = " << filePath << ", line = " << line << Qt::endl;

    // Check if the new edit location is the same as the last one
    if (!navHistory_.isEmpty() && 
        navHistory_.last().first == filePath && 
        navHistory_.last().second == line)
    {
        // If it's the same, don't add a duplicate entry
        return;
    }

    // Remove any forward history
    while (navHistory_.size() > currentHistoryIndex_ + 1)
    {
        navHistory_.pop_back();
    }

    // Add the new edit location
    navHistory_.append(qMakePair(filePath, line));
    currentHistoryIndex_ = navHistory_.size() - 1;

    // Limit history size (e.g., to 100 entries)
    if (navHistory_.size() > 100)
    {
        navHistory_.removeFirst();
        currentHistoryIndex_--;
    }

	qDebug() << "navHistory_ in addToNavHistory() = " << navHistory_;
	qDebug() << "currentHistoryIndex_ in addToNavHistory() = " << currentHistoryIndex_;
	
    emit updateGoForwardBackwardActions(false, navHistory_.size() > 1);

	qDebug() << "addToNavHistory() OUT, " << "filePath = " << filePath << ", line = " << line << Qt::endl;
}

void CEditor::goForward()
{
	qDebug() << "goBackward() IN " << Qt::endl;

    if (currentHistoryIndex_ < navHistory_.size() - 1)
    {
        currentHistoryIndex_++;
        const auto& historyItem = navHistory_[currentHistoryIndex_];
        
		if (currentHistoryIndex_ > 0 && historyItem.first == navHistory_[currentHistoryIndex_ - 1].first)
        {
            // Same file as previous, just go to line
            goToLine(historyItem.second);
        }
        else
        {
            loadFileWithLineNum(historyItem.first, historyItem.second);
        }

        emit updateGoForwardBackwardActions(currentHistoryIndex_ < navHistory_.size() - 1, true);
    }

	qDebug() << "goBackward() OUT " << Qt::endl;
}

void CEditor::goBackward()
{
	qDebug() << "goBackward() IN " << Qt::endl;

    if (currentHistoryIndex_ > 0)
    {
        currentHistoryIndex_--;
        const auto& historyItem = navHistory_[currentHistoryIndex_];

        if (currentHistoryIndex_ < navHistory_.size() - 1 && historyItem.first == navHistory_[currentHistoryIndex_ + 1].first)
        {
            // Same file as next, just go to line
            goToLine(historyItem.second);
        }
        else
        {
            loadFileWithLineNum(historyItem.first, historyItem.second);
        }

        emit updateGoForwardBackwardActions(true, currentHistoryIndex_ > 0);
    }

	qDebug() << "goBackward() OUT " << Qt::endl;
}

void CEditor::reloadFile(const QString &filePath)
{
	filesAlreadyPrompted_.remove(filePath);

	QFile file(filePath);
	if (file.open(QFile::ReadOnly))
	{
		QTextStream in(&file);
		QString content = in.readAll();
		file.close();
		if (editorTabMap_.contains(filePath))
		{
			editorTabMap_[filePath].textEdit->setText(content);
			editorTabMap_[filePath].textEdit->setModified(false);

			editorTabMap_[filePath].textEdit->setCaretLineBackgroundColor(QColor("#8fdcf1"));
			editorTabMap_[filePath].textEdit->setCaretLineVisible(true);

			textEditGoToLine(editorTabMap_[filePath].textEdit, 0);

			tabWidget->setCurrentWidget(editorTabMap_[filePath].textEdit);
			emit statusLeft("File reloaded.");
		}
	}
	else
	{
		QMessageBox::warning(this, tr("Reload File"),
							 tr("Cannot read file %1:\n%2.")
								 .arg(filePath)
								 .arg(file.errorString()));
	}
}

void CEditor::setupFileWatcher(const QString &filePath)
{
	if (!fileWatcher_.files().contains(filePath))
	{
		fileWatcher_.addPath(filePath);
	}
}

void CEditor::beginFileModification(const QString &filePath)
{
	filesBeingModified_.insert(filePath);
}

void CEditor::endFileModification(const QString &filePath)
{
	filesBeingModified_.remove(filePath);
}

void CEditor::onEndModificationTimeout()
{
    QString currentFileName = filePathInTab(tabWidget->currentIndex());
    endFileModification(currentFileName);
}

void CEditor::onEndNavigationTimeout()
{
	isNavigatingHistory_ = false;
}

void CEditor::fileChanged(const QString & filePath)
{
	if (!filesBeingModified_.contains(filePath) && !filesAlreadyPrompted_.contains(filePath)) {
		filesAlreadyPrompted_.insert(filePath);
		QMessageBox::StandardButton reply = QMessageBox::question(this, "File Changed",
			filePath + " has been modified by another program. Do you want to reload it and lose the changes in blink code search?",
			QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			reloadFile(filePath);					
		}
	}
}

void CEditor::resetNavHistory()
{
	navHistory_.clear();
	currentHistoryIndex_ = -1;
	isNavigatingHistory_ = false;
	emit updateGoForwardBackwardActions(false, false);
}

