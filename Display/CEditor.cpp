#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

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

CEditor::CEditor(QWidget* parent)
	: QMainWindow(parent),
	findDlg_(this)
{
	setupUi(this);

	QByteArray savedGeometry;

	savedGeometry = CConfigManager::getInstance()->getValue("Editor", "geometry").toByteArray();
	this->restoreGeometry(savedGeometry);

	connect(actionOpen, &QAction::triggered, this, &CEditor::openFile);
	connect(actionSave, &QAction::triggered, this, &CEditor::save);
	connect(actionSaveAs, &QAction::triggered, this, &CEditor::saveAs);
	connect(actionFind, &QAction::triggered, this, &CEditor::showFindDialog);

	connect(&findDlg_, &CEditorFindDlg::findText, this, &CEditor::findText);

	// actions for tab widget
	connect(tabWidget, &QTabWidget::currentChanged, this, &CEditor::tabChanged);

	connect(tabWidget, &QTabWidget::tabCloseRequested, this, &CEditor::closeTab);
	connect(actionExit, &QAction::triggered, this, &CEditor::hide);

	connect(actionClose, &QAction::triggered, this, &CEditor::closeCurrentTab);
}

void CEditor::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty()) {
		loadFile(fileName);
	}
}

void CEditor::save()
{
	saveFile(filePathInTab(tabWidget->currentIndex()));
}

void CEditor::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save File", filePathInTab(tabWidget->currentIndex()));
	if (fileName.isEmpty()) {
		return;
	}

	saveFile(fileName);
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

	statusBar()->showMessage(tr("File saved"), 2000);

	editorTabMap_[fileName].textEdit->setModified(false);
	setWindowModified(false);
}

void CEditor::showFindDialog()
{
	findDlg_.show();
}

void CEditor::findText(const QString& text, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());

	qDebug() << "findText = " << text;

	editorTabMap_[currentFileName].textEdit->findFirst(text, bRegularExpression, bCaseSensitive, bMatchWholeWord, true, true);
}

void CEditor::loadFileWithLineNum(const QString& filePath, int lineNumber)
{
	loadFile(filePath);
	editorTabMap_[filePath].textEdit->setFirstVisibleLine(lineNumber);
}

void CEditor::loadFile(const QString& filePath)
{
	if (editorTabMap_.contains(filePath)) { // load previous tab for existing file
		EditorTab editorTab = editorTabMap_[filePath];
		tabWidget->setCurrentWidget(editorTab.textEdit);
	} else {
		QFileInfo fileInfo(filePath);
		QString filename(fileInfo.fileName());

		QsciScintilla* textEdit = new QsciScintilla;
		QsciLexer* lexer;

		QString suffix = fileInfo.suffix();

		qDebug() << "suffix = " << suffix;

		if (suffix == "cpp") {
			lexer = new QsciLexerCPP;
		} else if (suffix == "java") {
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

		if (filename == "Makefile") {
			lexer = new QsciLexerMakefile;
		}

		setEditorFont(lexer);

		QFile file(filePath);
		if (!file.open(QFile::ReadOnly)) {
			QMessageBox::warning(this, tr("Editor"),
					tr("Cannot read file %1:\n%2.")
					.arg(filePath)
					.arg(file.errorString()));
			return;
		}

		QTextStream in(&file);
		QApplication::setOverrideCursor(Qt::WaitCursor);

		textEdit->setText(in.readAll());
		QApplication::restoreOverrideCursor();

		textEdit->setAutoIndent(true);
		textEdit->setLexer(lexer);

		createActions(textEdit);

		EditorTab editorTab = {.textEdit = textEdit, .lexer = lexer};
		editorTabMap_[filePath] = editorTab;

		tabWidget->addTab(textEdit, filename);

		tabWidget->setCurrentWidget(editorTab.textEdit);
		statusBar()->showMessage(tr("File loaded"), 2000);

		// set windows title
		setWindowTitle("[*]" + tabWidget->tabText(tabWidget->currentIndex()) + " - Editor");

		editorTabMap_[filePath].textEdit->setModified(false);
		setWindowModified(false);
	}
}

void CEditor::createActions(QsciScintilla* textEdit)
{
	connect(actionCut, &QAction::triggered, textEdit, &QsciScintilla::cut);
	connect(actionCopy, &QAction::triggered, textEdit, &QsciScintilla::copy);
	connect(actionPaste, &QAction::triggered, textEdit, &QsciScintilla::paste);

	connect(actionUndo, &QAction::triggered, textEdit, &QsciScintilla::undo);
	connect(actionRedo, &QAction::triggered, textEdit, &QsciScintilla::redo);

	connect(textEdit, &QsciScintilla::textChanged, this, &CEditor::textEditModified);
}

void CEditor::textEditModified()
{
	QString currentFileName = filePathInTab(tabWidget->currentIndex());

	qDebug() << "currentFileName in textEditModified = " << currentFileName;

	setWindowModified(editorTabMap_[currentFileName].textEdit->isModified());
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

		setWindowTitle("[*]" + tabWidget->tabText(tabIndex) + " - Editor");

		setWindowModified(editorTabMap_[currentFileName].textEdit->isModified());
	}
}

void CEditor::closeCurrentTab() {
	this->closeTab(tabWidget->currentIndex());
}

QString CEditor::filePathInTab(int tabIndex)
{
	QString filePathReturn;
	QsciScintilla* textEdit = static_cast<QsciScintilla*> (tabWidget->widget(tabIndex));

	for (const QString& filePath: editorTabMap_.keys()) {
		if (editorTabMap_[filePath].textEdit == textEdit) {
			filePathReturn = filePath;
		}
	}

	return filePathReturn;
}

void CEditor::closeTab(int tabIndex)
{
	qDebug() << "tabIndex in closeTab = " << tabIndex;

	QString filePathRemoveFromMap = filePathInTab(tabIndex);

	qDebug() << "filePathRemoveFromMap = " << filePathRemoveFromMap;

	tabWidget->removeTab(tabIndex);

	// close widget if tab count equals zero
	if (tabWidget->count() == 0) {
		this->hide();
	}

	delete editorTabMap_[filePathRemoveFromMap].textEdit;
	delete editorTabMap_[filePathRemoveFromMap].lexer;
	editorTabMap_.remove(filePathRemoveFromMap);
}
