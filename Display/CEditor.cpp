#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QPlainTextEdit>
#include <QStandardPaths>

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

CEditor::CEditor(CMainWindow* parent)
{
	setupUi(this);
	parent_ = parent;

	// actions for tab widget
	connect(tabWidget, &QTabWidget::currentChanged, this, &CEditor::tabChanged);
	connect(tabWidget, &QTabWidget::tabCloseRequested, this, &CEditor::closeTab);
	
	currentNewFileNumber_ = 1;
}

void CEditor::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty()) {
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
	if (newFileName.isEmpty()) {
		return;
	}
	
	QString oldFileName = filePathInTab(tabWidget->currentIndex());

	QFile file(newFileName);
	if (!file.open(QFile::WriteOnly)) {
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
	
	QsciLexer* newLexer = getLexer(newFileName);
	
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
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly)) {
		QMessageBox::warning(this, tr("Save file"),
				tr("Failed to write to file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
		return;
	}

	QTextStream fileOutput(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);

	fileOutput << editorTabMap_[fileName].textEdit->text();

	QApplication::restoreOverrideCursor();

	emit statusLeft("File saved.");

	editorTabMap_[fileName].textEdit->setModified(false);
	
	emit statusRight(fileName);			
}

void CEditor::findText(const QString& text, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());

	qDebug() << "findText = " << text;

	editorTabMap_[currentFileName].textEdit->findFirst(text, bRegularExpression, bCaseSensitive, bMatchWholeWord, true, true);
	
	if (bRegularExpression) {
		editorTabMap_[currentFileName].textEdit->findNext();
	}
}

void CEditor::replaceText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());

	editorTabMap_[currentFileName].textEdit->findFirst(findText, bRegularExpression, bCaseSensitive, bMatchWholeWord, true, true);
	editorTabMap_[currentFileName].textEdit->replace(replaceText);
}

void CEditor::replaceAllText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());
	
	long totalMatchCount = 0;
	long matchCount = 0;
	
	// bRegularExpression, bCaseSensitive, bMatchWholeWord
	QRegularExpression regExp(findText);

	Qt::CaseSensitivity caseSensitive;
	
	if (bCaseSensitive) {
		caseSensitive = Qt::CaseSensitive;
		// default expression default to case sensitive
	} else {
		caseSensitive = Qt::CaseInsensitive;
		regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}
	
	QString inputContent = editorTabMap_[currentFileName].textEdit->text();
	QString inputLine, replacedLine, outputFileStream = "";
	QTextStream inputStream(&inputContent);
	
	if (bRegularExpression || bMatchWholeWord) {
		// match whole word take effect only when regular expression is not enabled
		if (bMatchWholeWord && !bRegularExpression) {
			regExp.setPattern("\\b" + findText + "\\b");
		}
		while (inputStream.readLineInto(&inputLine)) {
			matchCount = inputLine.count(regExp);

			if (matchCount > 0) {
				replacedLine = inputLine.replace(regExp, replaceText);
				outputFileStream += replacedLine + "\n";
				totalMatchCount += matchCount;
			} else {
				outputFileStream += inputLine + "\n";
			}
		}
	} else {
		while (inputStream.readLineInto(&inputLine)) {
			matchCount = inputLine.count(findText, caseSensitive);
			if (matchCount > 0) {
				replacedLine = inputLine.replace(findText, replaceText, caseSensitive);
				outputFileStream += replacedLine + "\n";
				totalMatchCount += matchCount;
			} else {
                outputFileStream += inputLine + "\n";
			}

		}
	}

	if (totalMatchCount > 0) {
		editorTabMap_[currentFileName].textEdit->setText(outputFileStream);
		
		emit statusLeft("Replaced " + QString::number(totalMatchCount) + " strings.");
	}
	
}

// same tab
void CEditor::loadFileWithLineNum(const QString& filePath, int lineNumber)
{
	qDebug() << "loadFileWithLineNum IN, filePath =  " << filePath;
	int ret = loadFile(filePath);
	
	if (ret == -1) {
		return;
	}		
	
	editorTabMap_[filePath].textEdit->setCaretLineBackgroundColor(QColor("#8fdcf1"));
	editorTabMap_[filePath].textEdit->setCaretLineVisible(true);
	
	if (lineNumber > 4) {
		editorTabMap_[filePath].textEdit->setFirstVisibleLine(lineNumber - 4);
	}
	editorTabMap_[filePath].textEdit->setCursorPosition(lineNumber, 0);
	editorTabMap_[filePath].textEdit->setFocus();
	
	emit statusRight(filePath);
	qDebug() << "loadFileWithLineNum OUT, filePath =  " << filePath;
}

void CEditor::loadFileWithLineNumNewTab(const QString& filePath, int lineNumber)
{
	loadFileNewTab(filePath);	
	
	editorTabMap_[filePath].textEdit->setCaretLineBackgroundColor(QColor("#8fdcf1"));
	editorTabMap_[filePath].textEdit->setCaretLineVisible(true);
	
	if (lineNumber > 4) {
		editorTabMap_[filePath].textEdit->setFirstVisibleLine(lineNumber - 4);
	}
	editorTabMap_[filePath].textEdit->setCursorPosition(lineNumber, 0);
	editorTabMap_[filePath].textEdit->setFocus();
	
	emit statusRight(filePath);
}

void CEditor::newFile()
{
	QsciScintilla* textEdit = new QsciScintilla;
	QsciLexer* lexer;

	lexer = new QsciLexerCPP;

	setEditorFont(lexer);

	textEdit->setText("");
	textEdit->setAutoIndent(true);
	textEdit->setLexer(lexer);
	
	textEdit->setCaretLineBackgroundColor(QColor("#8fdcf1"));	
	textEdit->setCaretLineVisible(true);

	createActions(textEdit);

	EditorTab editorTab = {.textEdit = textEdit, .lexer = lexer};
	editorTabMap_["new " + QString::number(currentNewFileNumber_)] = editorTab;

	tabWidget->addTab(textEdit, "new " + QString::number(currentNewFileNumber_));
	currentNewFileNumber_++;
	
	textEdit->setModified(false);
}

QsciLexer* CEditor::getLexer(const QString& fileName)
{
	QFileInfo fileInfo(fileName);
	QsciLexer* lexer;
	
	QString suffix = fileInfo.suffix();

	qDebug() << "suffix = " << suffix;

	if (suffix == "cpp") {
		lexer = new QsciLexerCPP;
	} else if (suffix == "java" || suffix == "kt") {
		lexer = new QsciLexerJava;
	} else if (suffix == "py") {
		lexer = new QsciLexerPython;
	} else if (suffix == "js" || suffix == "ts") {
		lexer = new QsciLexerJavaScript;
	} else if (suffix == "rb") {
		lexer = new QsciLexerRuby;
	} else if (suffix == "sql") {
		lexer = new QsciLexerSQL;
	} else if (suffix == "html") {
		lexer = new QsciLexerHTML;
	} else if (suffix == "xml") {
		lexer = new QsciLexerXML;
	} else if (suffix == "css") {
		lexer = new QsciLexerCSS;
	} else if (suffix == "md") {
		lexer = new QsciLexerMarkdown;
	} else if (suffix == "yaml") {
		lexer = new QsciLexerYAML;
	} else if (suffix == "cs") {
		lexer = new QsciLexerCSharp;
	} else if (suffix == "v" || suffix == "vh" || suffix == "sv" || suffix == "svh") {
		lexer = new QsciLexerVerilog;
	} else {
		lexer = new QsciLexerCPP;
	}

	if (fileName == "Makefile") {
		lexer = new QsciLexerMakefile;
	}
	
	setEditorFont(lexer);
	
	return lexer;
}

void CEditor::setTextEdit(QsciScintilla* textEdit)
{
	textEdit->setAutoIndent(true);				
	textEdit->setAutoCompletionThreshold(1);
	textEdit->setAutoCompletionSource(QsciScintilla::AcsDocument);
	textEdit->setFolding(QsciScintilla::PlainFoldStyle);

	textEdit->setTabWidth(4);
}

int CEditor::loadFile(const QString& filePath)
{
    QMessageBox::StandardButton reply = QMessageBox::Cancel;
	
	if (editorTabMap_.contains(filePath)) { // load previous tab for existing file
		EditorTab editorTab = editorTabMap_[filePath];
		tabWidget->setCurrentWidget(editorTab.textEdit);
	} else {
		QFileInfo fileInfo(filePath);
		QString filename(fileInfo.fileName());

		if (tabWidget->currentIndex() != -1) {
			QString oldPath = filePathInTab(tabWidget->currentIndex());
			
			if (editorTabMap_[oldPath].textEdit->isModified()) {
				reply = QMessageBox::question(this, "Save", "Save file \"" + oldPath + "\"?",
										QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
				
				if (reply == QMessageBox::Yes) {
					saveFile(oldPath);
				} else if (reply == QMessageBox::Cancel) {
					qDebug() << "cancel QMessagebox";
					return -1;
				}		
			}
			
			qDebug() << "continue QMessagebox";
			
			QsciScintilla* currentTextEdit = static_cast<QsciScintilla*> (tabWidget->currentWidget());
			if (currentTextEdit != NULL) {
				for (auto editorTab = editorTabMap_.cbegin(), end = editorTabMap_.cend(); editorTab != end; ++editorTab) {
					if (editorTab.value().textEdit == currentTextEdit) {
						QsciLexer* previousLexer = currentTextEdit->lexer();

						QsciLexer* lexer = getLexer(filePath);

						QFile file(filePath);
						if (!file.open(QFile::ReadOnly)) {
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

						EditorTab newEditorTab = {.textEdit = currentTextEdit, .lexer = lexer};
						editorTabMap_[filePath] = newEditorTab;
						
						editorTabMap_[filePath].textEdit->setModified(false);
						
						if (filePath != editorTab.key()) {
							tabWidget->setTabText(tabWidget->currentIndex(), filename);
							editorTabMap_.remove(editorTab.key());
						}
										
						break;
					}
				}
			} else {
				loadFileNewTab(filePath);
			}
		} else {
			loadFileNewTab(filePath);
		}

	}
	return 0;
}

void CEditor::loadFileNewTab(const QString& filePath)
{
	if (editorTabMap_.contains(filePath)) { // load previous tab for existing file
		EditorTab editorTab = editorTabMap_[filePath];
		tabWidget->setCurrentWidget(editorTab.textEdit);
	} else {
		QFileInfo fileInfo(filePath);
		QString filename(fileInfo.fileName());

		QsciScintilla* textEdit = new QsciScintilla;	
		QsciLexer* lexer = getLexer(filePath);

		QFile file(filePath);
		if (!file.open(QFile::ReadOnly)) {
			QMessageBox::warning(this, tr("Editor"),
					tr("Cannot read file %1:\n%2.")
					.arg(filePath)
					.arg(file.errorString()));
			return;
		}

		QTextStream in(&file);
		//QApplication::setOverrideCursor(Qt::WaitCursor);

		textEdit->setText(in.readAll());
		//QApplication::restoreOverrideCursor();

		textEdit->setLexer(lexer);
		
		setTextEdit(textEdit);

		createActions(textEdit);

		EditorTab editorTab = {.textEdit = textEdit, .lexer = lexer};
		editorTabMap_[filePath] = editorTab;

		tabWidget->addTab(textEdit, filename);

		tabWidget->setCurrentWidget(editorTab.textEdit);
		emit statusLeft("File loaded");

		editorTabMap_[filePath].textEdit->setModified(false);
	}
}

void CEditor::createActions(QsciScintilla* textEdit)
{
	//connect(actionCut, &QAction::triggered, textEdit, &QsciScintilla::cut);
	//connect(actionCopy, &QAction::triggered, textEdit, &QsciScintilla::copy);
	//connect(actionPaste, &QAction::triggered, textEdit, &QsciScintilla::paste);

	//connect(actionUndo, &QAction::triggered, textEdit, &QsciScintilla::undo);
	//connect(actionRedo, &QAction::triggered, textEdit, &QsciScintilla::redo);

	connect(textEdit, &QsciScintilla::textChanged, this, &CEditor::textEditModified);
	connect(textEdit, &QsciScintilla::cursorPositionChanged, parent_, &CMainWindow::showCurrentCursorPosition);
}

void CEditor::textEditModified()
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());

	editorTabMap_[currentFileName].textEdit->setModified(true);
	
	emit statusRight("* " + currentFileName);
}

void CEditor::updateAllEditorFont()
{
	QFont editorFont;

	QString editorFontSettingStr = CConfigManager::getInstance()->getAppSettingValue("EditorFont").toString();
	editorFont.fromString(editorFontSettingStr);

	if (editorFontSettingStr == "") {
        editorFont = QApplication::font();
	}

	for (auto editorTab: editorTabMap_) {
		editorTab.textEdit->lexer()->setFont(editorFont);
	}
}

void CEditor::setEditorFont(QsciLexer* lexer)
{
	QFont editorFont;

	QString editorFontSettingStr = CConfigManager::getInstance()->getAppSettingValue("EditorFont").toString();
	editorFont.fromString(editorFontSettingStr);

	qDebug() << "editorFontSettingStr = " << editorFontSettingStr << Qt::endl;

	QFont consolasFont("Consolas", QApplication::font().pointSize());

	if (editorFontSettingStr == "") {
		lexer->setFont(consolasFont);
	} else {
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

void CEditor::tabChanged(int tabIndex) {
	if (tabIndex >= 0) {
		QString currentFileName = filePathInTab(tabIndex);
		
		if (currentFileName != "") {
			if (editorTabMap_[currentFileName].textEdit->isModified()) {
				emit statusRight("* " + currentFileName);
			} else {
				emit statusRight(currentFileName);
			}
		}
	}
}

void CEditor::closeCurrentTab() {
	this->closeTab(tabWidget->currentIndex());
}

QString CEditor::filePathInTab(int tabIndex)
{
	QString filePathReturn = "";
	QsciScintilla* textEdit = static_cast<QsciScintilla*> (tabWidget->widget(tabIndex));

	for (const QString& filePath: editorTabMap_.keys()) {
		if (editorTabMap_[filePath].textEdit == textEdit) {
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

	if (editorTabMap_[filePathRemoveFromMap].textEdit->isModified()) {
		reply = QMessageBox::question(this, "Save", "Save file \"" + filePathRemoveFromMap + "\"?",
                                QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		
		if (reply == QMessageBox::Yes) {
			saveFile(filePathRemoveFromMap);
		} else if (reply == QMessageBox::Cancel) {
			return;
		}		
	}

	tabWidget->removeTab(tabIndex);

	delete editorTabMap_[filePathRemoveFromMap].textEdit;
	delete editorTabMap_[filePathRemoveFromMap].lexer;
	editorTabMap_.remove(filePathRemoveFromMap);
	
	if (tabWidget->count() == 0) {
		newFile();
	}
}

void CEditor::goToLine(int line) {
	QString filePath = filePathInTab(tabWidget->currentIndex());
	
	editorTabMap_[filePath].textEdit->setCursorPosition(line - 1, 0);
}

void CEditor::cut() {
	QString filePath = filePathInTab(tabWidget->currentIndex());
	
	editorTabMap_[filePath].textEdit->cut();
}

void CEditor::copy() {
	QString filePath = filePathInTab(tabWidget->currentIndex());
	
	editorTabMap_[filePath].textEdit->copy();
}

void CEditor::paste() {
	QString filePath = filePathInTab(tabWidget->currentIndex());
	
	editorTabMap_[filePath].textEdit->paste();
}

void CEditor::undo() {
	QString filePath = filePathInTab(tabWidget->currentIndex());
	
	editorTabMap_[filePath].textEdit->undo();
}

void CEditor::redo() {
	QString filePath = filePathInTab(tabWidget->currentIndex());
	
	editorTabMap_[filePath].textEdit->redo();
}


