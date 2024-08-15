#include <QDir>
#include <QDirIterator>

#include <QFileDialog>
#include <QMessageBox>

#include <QWidget>
#include <QCloseEvent>

#include <QHeaderView>

#include <QStandardItemModel>

#include <QStandardItem>
#include <QDateTime>

#include <QClipboard>

#include <QFile>
#include <QFileInfo>

#include <QTextDocument>

#include <QInputDialog>

// qsciscintilla
#include <QTextStream>
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

#ifdef Q_OS_WIN
	#include <qt_windows.h>
#endif

#include "CMainWindow.h"
#include "CAboutDlg.h"
#include "CProjectDlg.h"
#include "CConfigDlg.h"
#include "CFindReplaceDlg.h"

#include "Utils/CUtils.h"

#include "Model/qTagger/CTagResultItem.h"

CMainWindow::CMainWindow(QWidget* parent)
: QMainWindow(parent),
bTagBuildInProgress_(false),
findDlg_(this),
editor_(this)
{
	bool bAutoHideMenu;

    setupUi(this);

	setWindowIcon(QIcon(":/Images/graphics3.png"));

    // setting for progress bar
    progressBar_.setTextVisible(false);
    progressBar_.setMaximumSize(100, 16);
    progressBar_.setRange(0, 100);
    progressBar_.setValue(0);
    progressBar_.hide();

	// hiding menuBar if needed
	confManager_ = CConfigManager::getInstance();
	bAutoHideMenu = confManager_->getAppSettingValue("AutoHideMenu", false).toBool();
	if (bAutoHideMenu) {
		menuBar()->hide();
	}

    // setting for timeline
    timeLine_.setDuration(200);
    timeLine_.setFrameRange(0, 100);

    // defining shortcut

	// filter under project tab
	projectPatternLineEditShortcut = new QShortcut(QKeySequence(Qt::ALT | Qt::Key_P), this);

	fileSearchShortcut = new QShortcut(QKeySequence(Qt::ALT | Qt::Key_F), this);
	tagSearchShortcut = new QShortcut(QKeySequence(Qt::ALT | Qt::Key_S), this);

	outputExploreShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_E), this);
	outputConsoleShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_M), this);

	projectLoadShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_L), this);
	projectUpdateShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_U), this);

	//symbolSearchFrameShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_B), this);
	//symbolSearchFrameShortcut->setContext(Qt::ApplicationShortcut);

	nextSymbolSearchShortcut = new QShortcut(QKeySequence::FindNext, this);
	nextSymbolSearchShortcut->setContext(Qt::ApplicationShortcut);

	previousSymbolSearchShortcut = new QShortcut(QKeySequence::FindPrevious, this);
	previousSymbolSearchShortcut->setContext(Qt::ApplicationShortcut);

#ifdef Q_OS_WIN
	previousSymbolSearchShortcut_win = new QShortcut(QKeySequence(Qt::Key_F4), this);
	previousSymbolSearchShortcut_win->setContext(Qt::ApplicationShortcut);
#endif

    // shortcut in application context
	projectPatternLineEditShortcut->setContext(Qt::ApplicationShortcut);

	fileSearchShortcut->setContext(Qt::ApplicationShortcut);
	tagSearchShortcut->setContext(Qt::ApplicationShortcut);

	outputExploreShortcut->setContext(Qt::ApplicationShortcut);
	outputConsoleShortcut->setContext(Qt::ApplicationShortcut);

	projectLoadShortcut->setContext(Qt::ApplicationShortcut);
	projectUpdateShortcut->setContext(Qt::ApplicationShortcut);

	// project_listView, projectListModel_
	projectListModel_ = new CProjectListModel(this);

	projectListProxyModel_ = new QSortFilterProxyModel;
	projectListProxyModel_->setSourceModel(static_cast <QStandardItemModel*> (projectListModel_));
    projectListProxyModel_->setDynamicSortFilter(true);

	projectListSelectionModel_ = new QItemSelectionModel(projectListProxyModel_);

	project_listView->setRootIsDecorated(false);
	project_listView->setModel(projectListProxyModel_);

	project_listView->setSelectionModel(projectListSelectionModel_);
	project_listView->setSortingEnabled(true);

	project_listView->setDragEnabled(false);
	project_listView->setAcceptDrops(true);
	project_listView->setDropIndicatorShown(true);
    updateProjectListWidget();

	// file_listView, fileListModel_
	fileListModel_ = new CFileListModel(this);

	file_listView->setFileListModel(fileListModel_);

	file_listView->setRootIsDecorated(false);
	file_listView->setModel(fileListModel_->getProxyModel());

	file_listView->setSelectionModel(fileListModel_->getSelectionModel());
	file_listView->setSortingEnabled(true);

	file_listView->setDragEnabled(true);
	file_listView->setAcceptDrops(false);

	//verticalSplitter->addWidget(&codeBrowser_);
	verticalSplitter->addWidget(&editor_);

	editor_.newFile();

    codeBrowser_.setCaretLineBackgroundColor(QColor("#FFFF66"));
	codeBrowser_.setCaretLineVisible(true);

	m_statusLeft = new QLabel("", this);
	m_statusMiddle = new QLabel("", this);
	m_statusRight = new QLabel("", this);
	statusBar()->addPermanentWidget(m_statusLeft, 1);
	statusBar()->addPermanentWidget(m_statusMiddle, 1);
	statusBar()->addPermanentWidget(m_statusRight, 1);

    // add progressbar for status bar
    statusBar()->addPermanentWidget(&progressBar_, 1);

	// view
	bool bAlwaysOnTop;
	bool bTransparent;

	bool bViewToolbar;
	bool bViewProjectPanel;
	bool bViewFilePanel;

	bAlwaysOnTop = confManager_->getAppSettingValue("AlwaysOnTop", false).toBool();
	if (bAlwaysOnTop) {
		setAlwaysOnTop(true);
		actionAlways_on_top->setChecked(true);
	}

    bTransparent = confManager_->getAppSettingValue("Transparency", false).toBool();
	if (bTransparent) {
		setTransparency(true);
		actionTransparent->setChecked(true);
	}

	bViewToolbar = confManager_->getAppSettingValue("ViewToolbar", true).toBool();
	if (!bViewToolbar) {
		toolBar->hide();
		actionToolbar->setChecked(false);
	} else {
        toolBar->show();
		actionToolbar->setChecked(true);
	}

	bViewProjectPanel = confManager_->getAppSettingValue("ViewProjectPanel", true).toBool();
	if (!bViewProjectPanel) {
	    mainTabWidget->hide();
		actionProject_Panel->setChecked(false);
	} else {
        mainTabWidget->show();
		actionProject_Panel->setChecked(true);
	}

	bViewFilePanel = confManager_->getAppSettingValue("ViewFilePanel", true).toBool();
	if (!bViewFilePanel) {
	    infoTabWidget->hide();
		actionFile_Panel->setChecked(false);
	} else {
        infoTabWidget->show();
		actionFile_Panel->setChecked(true);
	}

	searchFrame->hide();

	bool bProjectAndGroupFilterCaseSensitive;
	bool bFileFilterCaseSensitive;
	bool bSymbolSearchCaseSensitive;
	bool bSymbolSearchRegularExpression;
	bool bLiveSearch;
	bool bFuzzyAutoComplete;

	Qt::CaseSensitivity symbolCaseSensitivity;

	bProjectAndGroupFilterCaseSensitive = confManager_->getAppSettingValue("ProjectAndGroupFilterCaseSensitive", false).toBool();
	if (bProjectAndGroupFilterCaseSensitive) {
		actionProjectAndGroupCaseSensitive->setChecked(true);
	} else {
		actionProjectAndGroupCaseSensitive->setChecked(false);
	}

	bFileFilterCaseSensitive = confManager_->getAppSettingValue("FileFilterCaseSensitive", false).toBool();
	if (bFileFilterCaseSensitive) {
		actionFileCaseSensitive->setChecked(true);
	} else {
		actionFileCaseSensitive->setChecked(false);
	}

	// Project Font
	QString projectFontSettingStr;
	QFont projectFont;

	projectFontSettingStr = confManager_->getAppSettingValue("ProjectFont").toString();
	projectFont.fromString(projectFontSettingStr);

	if (projectFontSettingStr != "") {
		project_listView->updateProjectFont(projectFont);
		file_listView->updateOutputFont(projectFont); // update output font as well
	} else {
		project_listView->updateProjectFont(QApplication::font()); // using system font by default
		file_listView->updateOutputFont(QApplication::font());
	}

	// text document for symbol, need to be initialized before setting symbol font
	textLayout_ = new QPlainTextDocumentLayout(&textDocument_);
	textDocument_.setDocumentLayout(textLayout_);

	// Symbol font
	QString symbolFontSettingStr;
	QFont symbolFont;

	symbolFontSettingStr = confManager_->getAppSettingValue("SymbolFont").toString();
	symbolFont.fromString(symbolFontSettingStr);

	if (symbolFontSettingStr != "") {
		setSymbolFont(symbolFont);
	} else {
		setSymbolFont(QApplication::font()); // using system font by default
	}

	// Symbol Auto Complete font
	QString symbolAutoCompleteFontSettingStr;
	QFont symbolAutoCompleteFont;

	symbolAutoCompleteFontSettingStr = confManager_->getAppSettingValue("SymbolAutoCompleteFont").toString();
	symbolAutoCompleteFont.fromString(symbolAutoCompleteFontSettingStr);

	if (symbolAutoCompleteFontSettingStr != "") {
		setSymbolAutoCompleteFont(symbolAutoCompleteFont);
	} else {
		setSymbolAutoCompleteFont(QApplication::font()); // using system font by default
	}

    // editor font set directly in CEditor::CEditor

	// case sensitive
    bSymbolSearchCaseSensitive = confManager_->getAppSettingValue("SymbolSearchCaseSensitive", false).toBool();
	if (bSymbolSearchCaseSensitive) {
		actionSymbolCaseSensitive->setChecked(true);
		symbolCaseSensitivity = Qt::CaseSensitive;
	} else {
		actionSymbolCaseSensitive->setChecked(false);
		symbolCaseSensitivity = Qt::CaseInsensitive;
	}

	// symbol regular expression
    bSymbolSearchRegularExpression = confManager_->getAppSettingValue("SymbolSearchRegularExpression", false).toBool();
	if (bSymbolSearchRegularExpression) {
		actionSymbolRegularExpression->setChecked(true);
	} else {
		actionSymbolRegularExpression->setChecked(false);
	}

	// live search
    bLiveSearch = confManager_->getAppSettingValue("LiveSearch", true).toBool();
	if (bLiveSearch) {
		actionLiveSearch->setChecked(true);
	} else {
		actionLiveSearch->setChecked(false);
	}

	// fuzzy auto complete
    bFuzzyAutoComplete = confManager_->getAppSettingValue("FuzzyAutoComplete", true).toBool();
	if (bFuzzyAutoComplete) {
		actionFuzzyAutoComplete->setChecked(true);
	} else {
		actionFuzzyAutoComplete->setChecked(false);
	}

    createActions();

	completer_.setModelSorting(QCompleter::CaseSensitivelySortedModel);
	completer_.setCaseSensitivity(symbolCaseSensitivity);
	completer_.setFilterMode(Qt::MatchContains);

	if (bFuzzyAutoComplete) {
		completer_.setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	} else {
		completer_.setCompletionMode(QCompleter::PopupCompletion);
	}

	search_lineEdit->setCompleter(&completer_);
}

void CMainWindow::setSplitterSizes(const QList<int>& splitterSizeList)
{
	splitter->setSizes(splitterSizeList);
}

void CMainWindow::setVerticalSplitterSizes(const QList<int>& splitterSizeList)
{
	verticalSplitter->setSizes(splitterSizeList);
}

void CMainWindow::restoreTabWidgetPos()
{
    // tab widget
    int projectTabIndex = confManager_->getValue("Window", "projectTabIndex", 0).toInt();
    int fileTabIndex = confManager_->getValue("Window", "fileTabIndex", 0).toInt();

    // group instead of project tab first
    if (projectTabIndex == 1) {
        mainTabWidget->clear();

        // from ui_mainWindow.h
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/Icons/22x22/view-process-all.png"), QSize(), QIcon::Normal, QIcon::Off);
        mainTabWidget->addTab(projectTab, icon14, QString());

        mainTabWidget->setTabText(mainTabWidget->indexOf(projectTab), QCoreApplication::translate("mainWindow", "Project", 0, 0));
    }

    // symbol instead of file tab first
    if (fileTabIndex == 1) {
        infoTabWidget->clear();

        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/Icons/22x22/code-class.png"), QSize(), QIcon::Normal, QIcon::Off);
        infoTabWidget->addTab(symbolTab, icon17, QString());

        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/Icons/22x22/document-new-4.png"), QSize(), QIcon::Normal, QIcon::Off);
        infoTabWidget->addTab(fileTab, icon16, QString());

        infoTabWidget->setTabText(infoTabWidget->indexOf(fileTab), QCoreApplication::translate("mainWindow", "File", 0, 0));
        infoTabWidget->setTabText(infoTabWidget->indexOf(symbolTab), QCoreApplication::translate("mainWindow", "Symbol", 0, 0));
    }
}

void CMainWindow::updateProjectListWidget()
{
	project_listView->resizeColumnToContents(0);
	project_listView->resizeColumnToContents(1);
	project_listView->resizeColumnToContents(2);
    project_listView->resizeColumnToContents(3);
}

void CMainWindow::updateFileListWidget()
{
	file_listView->resizeColumnToContents(0);
	file_listView->resizeColumnToContents(1);
	file_listView->resizeColumnToContents(2);
}

void CMainWindow::setSymbolFont(QFont symbolFont)
{
	QString symbolFontFamily = symbolFont.family();
	QString symbolFontSize = QString::number(symbolFont.pointSize());
	QString lineHeight = QString::number(symbolFont.pointSize() / 3);

	QString textDocumentSyleStr = QString("a {color: #0066FF; font-weight: bold; font-family: %1; text-decoration: none} functionsig {color: #33F000; font-weight:bold; font-family: %1;} code {display: table-row; font-family: Consolas; white-space: nowrap} linenum {color: #9999CC; font-family: %1} keyword {color: #00CCCC; font-weight:bold} spacesize {font-size: %3pt} body {background: #FAFAFA; font-size: %2pt}").arg(symbolFontFamily, symbolFontSize, lineHeight);

	textDocument_.setDefaultStyleSheet(textDocumentSyleStr);
}

void CMainWindow::setSymbolAutoCompleteFont(QFont symbolAutoCompleteFont)
{
	completer_.popup()->setFont(symbolAutoCompleteFont);
}

void CMainWindow::loadProjectList()
{
    QStringList projectList;

  	projectListModel_->clear();  // header data will also be clear
	projectListModel_->setColumnCount(4); // need to set back column count when QStandardItemModel clear otherwise setData will return false

    projectListModel_->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    projectListModel_->setHeaderData(1, Qt::Horizontal, QObject::tr("Tag Update Datatime"));
    projectListModel_->setHeaderData(2, Qt::Horizontal, QObject::tr("Project Create Datetime"));
    projectListModel_->setHeaderData(3, Qt::Horizontal, QObject::tr("Labels"));

	QMap<QString, CProjectItem> projectMap;

	CProjectManager::getInstance()->getProjectMap(projectMap);

    foreach (const CProjectItem& projectItem, projectMap) {
		projectListModel_->addProjectItem(projectItem);
    }

	updateProjectListWidget();
}

void CMainWindow::loadFileList()
{
	CFileItem fileItem;

	progressBar_.show();

	fileListModel_->clearAndResetModel();
	unsigned long eventLoopCounter = 0; // event loop counter to prevent ui freeze when loading large amount
	int progressBarVal = 0;
	unsigned long totalLoopCounter = 0;

	foreach (const CFileItem& fileItem, fileItemList_) {
		fileListModel_->addItem(fileItem);
		if (eventLoopCounter >= 2000) { // prevent ui freeze
			updateFileListWidget();
			QCoreApplication::processEvents();
			eventLoopCounter = 0;
			progressBarVal = static_cast<int> (static_cast<long double> (totalLoopCounter) / fileItemList_.length() * 100);
			progressBar_.setValue(progressBarVal);
		}
		eventLoopCounter++;
		totalLoopCounter++;
	}

	updateFileListWidget();
	progressBar_.setValue(100);
	progressBar_.hide();
}

void CMainWindow::createActions()
{
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(on_aboutButton_clicked()));

	// [Project action]
	connect(actionProjectAddDirectory, &QAction::triggered, this, &CMainWindow::on_projectAddDirectoryButton_clicked);

	connect(actionProjectLoad, SIGNAL(triggered()), this, SLOT(on_loadProjectButton_clicked()));

	// default double click, enter action for project list item
	connect(project_listView, SIGNAL(projectItemTriggered()), this, SLOT(on_loadProjectButton_clicked()));
	connect(actionProjectRebuildTag, SIGNAL(triggered()), this, SLOT(on_rebuildTagProjectButton_clicked()));
	connect(actionProjectModify, SIGNAL(triggered()), this, SLOT(on_editProjectButton_clicked()));
	connect(actionProjectDelete, SIGNAL(triggered()), this, SLOT(on_deleteProjectButton_clicked()));

	connect(actionProjectCopy, SIGNAL(triggered()), this, SLOT(on_projectCopyPressed()));

	connect(actionProjectExplore, SIGNAL(triggered()), this, SLOT(on_exploreProjectButton_clicked()));
	connect(actionProjectConsole, SIGNAL(triggered()), this, SLOT(on_consoleProjectButton_clicked()));

    // [File action]
    connect(actionFileEditExternal, &QAction::triggered, this, &CMainWindow::on_fileEditExternalPressed);
    connect(actionFileEdit, &QAction::triggered, this, &CMainWindow::on_fileEditPressed);
	connect(actionFileEditNewTab, &QAction::triggered, this, &CMainWindow::on_fileEditNewTabPressed);

    // default double click, enter action for file list item
	connect(file_listView, SIGNAL(fileItemTriggered()), this, SLOT(on_fileListItemDoubleClicked()));

	connect(actionFileCopy, SIGNAL(triggered()), this, SLOT(on_fileCopyPressed()));
	connect(actionFileExplore, SIGNAL(triggered()), this, SLOT(on_fileExplorePressed()));
	connect(actionFileConsole, SIGNAL(triggered()), this, SLOT(on_fileConsolePressed()));
    connect(actionFileProperties, SIGNAL(triggered()), this, SLOT(on_filePropertiesPressed()));

	connect(actionSearch, SIGNAL(triggered()), this, SLOT(on_searchButton_clicked()));

	connect(actionNextSymbolSearch, SIGNAL(triggered()), this, SLOT(on_nextSymbolButton_clicked()));
	connect(actionPreviousSymbolSearch, SIGNAL(triggered()), this, SLOT(on_previousSymbolButton_clicked()));

	connect(search_lineEdit, SIGNAL(returnPressed()), this, SLOT(on_searchButton_clicked()));

    connect(CProjectManager::getInstance(), &CProjectManager::projectMapUpdated, this, &CMainWindow::loadProjectList);
	connect(CProjectManager::getInstance(), &CProjectManager::newProjectAdded, this, &CMainWindow::projectRebuildTag);

    connect(&timeLine_, SIGNAL(frameChanged(int)), &progressBar_, SLOT(setValue(int)));
    connect(&projectUpdateThread_, &CProjectUpdateThread::percentageCompleted, this, &CMainWindow::updateTagBuildProgress);

	// update progress bar for cancelled tag build
	connect(&projectUpdateThread_, SIGNAL(cancelledTagBuild()), this, SLOT(updateCancelledTagBuild()));
	// error occurs during run command
	connect(&projectUpdateThread_, SIGNAL(errorDuringRun(const QString&)), this, SLOT(on_errorDuringRun(const QString&)));

	connect(&projectLoadThread_, &CProjectLoadThread::projectLoadPercentageCompleted, this, &CMainWindow::updateProjectLoadProgress);

    // connecting shortcut action
	connect(projectPatternLineEditShortcut, SIGNAL(activated()), this, SLOT(on_projectPatternLineEditShortcutPressed()));

    connect(fileSearchShortcut, SIGNAL(activated()), this, SLOT(on_filePatternLineEditShortcutPressed()));
	connect(tagSearchShortcut, SIGNAL(activated()), this, SLOT(on_searchLineEditShortcutPressed()));

    connect(outputExploreShortcut, SIGNAL(activated()), this, SLOT(on_fileExplorePressed()));
	connect(outputConsoleShortcut, SIGNAL(activated()), this, SLOT(on_fileConsolePressed()));

	connect(projectPattern_lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(projectFilterRegExpChanged()));

    connect(actionProjectAndGroupCaseSensitive, SIGNAL(toggled(bool)),
            this, SLOT(projectFilterRegExpChanged()));

    connect(actionProjectAndGroupCaseSensitive, SIGNAL(toggled(bool)),
            this, SLOT(groupFilterRegExpChanged()));

    // for cancel update tag
	connect(actionCancelTagUpdate, SIGNAL(triggered()), this, SLOT(on_cancelTagUpdate()));
	// for info tab widget tool button

	connect(filePattern_lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(fileFilterRegExpChanged()));
    connect(actionFileCaseSensitive, SIGNAL(toggled(bool)),
            this, SLOT(fileFilterRegExpChanged()));
    connect(actionSymbolCaseSensitive, SIGNAL(toggled(bool)),
            this, SLOT(searchLineEditChanged()));
    connect(actionSymbolRegularExpression, SIGNAL(toggled(bool)),
            this, SLOT(searchLineEditChanged()));

	connect(search_lineEdit, SIGNAL(textEdited(const QString &)),
            this, SLOT(searchLineEditChanged()));

	connect(&completer_, SIGNAL(activated(const QString &)),
            this, SLOT(queryTag(const QString&)));

	connect(&completer_, SIGNAL(highlighted(const QString &)),
            this, SLOT(queryTagTop1000(const QString&)));

    // symbol search frame
	//connect(symbolSearchFrameShortcut, SIGNAL(activated()), this, SLOT(on_symbolSearchFrameShortcutPressed()));
	connect(frameSymbol_lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(frameSymbolLineEditChanged()));

	connect(nextSymbolSearchShortcut, SIGNAL(activated()), this, SLOT(on_nextSymbolButton_clicked()));
	connect(previousSymbolSearchShortcut, SIGNAL(activated()), this, SLOT(on_previousSymbolButton_clicked()));

#ifdef Q_OS_WIN
	connect(previousSymbolSearchShortcut_win, SIGNAL(activated()), this, SLOT(on_previousSymbolButton_clicked()));
#endif

	// zoom in, zoom out
	actionWebZoomIn->setShortcut(QKeySequence::ZoomIn);
	actionWebZoomOut->setShortcut(QKeySequence::ZoomOut);

#ifdef ENABLE_SYMBOL_WEB_VIEW
	webView->addAction(actionWebZoomIn);
	webView->addAction(actionWebZoomOut);
#elif defined ENABLE_SYMBOL_RICH_TEXT_VIEW

	/*
	symbol_textBrowser->setContextMenuPolicy(Qt::NoContextMenu);
	symbol_textBrowser->setReadOnly(true);
	symbol_textBrowser->setUndoRedoEnabled(false);
	*/

	symbol_textBrowser->addAction(actionWebZoomIn);
	symbol_textBrowser->addAction(actionWebZoomOut);
#else
	symbol_textBrowser->setReadOnly(true);
	symbol_textBrowser->setLineWrapMode(QPlainTextEdit::NoWrap);
	//symbol_textBrowser->setUndoRedoEnabled(false);

	QTextCharFormat charFormat;

	QStringList fontFamilies;
	fontFamilies.push_back("Consolas");

	charFormat.setFontFamilies(fontFamilies);
	charFormat.setFontPointSize(7);

	symbol_textBrowser->setCurrentCharFormat(charFormat);
#endif


	connect(actionWebZoomIn, SIGNAL(triggered()), this, SLOT(webZoomIn()));
	connect(actionWebZoomOut, SIGNAL(triggered()), this, SLOT(webZoomOut()));

	// connect for lauching editor from symbol panel
	connect(symbol_textBrowser, &CSearchTextEdit::linkActivated, &editor_, &CEditor::loadFileWithLineNum);
	connect(symbol_textBrowser, &CSearchTextEdit::linkActivatedNewTab, &editor_, &CEditor::loadFileWithLineNumNewTab);

	// code browser
	//connect(actionNew, &QAction::triggered, this, &newFile);
	//connect(actionOpen, &QAction::triggered, this, &openFile);
	//connect(actionSave, &QAction::triggered, this, &saveFile);
	//connect(actionClose, &QAction::triggered, this, &closeFile);
	//connect(actionSaveAs, &QAction::triggered, this, &saveFileAs);

	connect(actionNew, &QAction::triggered, &editor_, &CEditor::newFile);
	connect(actionOpen, &QAction::triggered, &editor_, &CEditor::openFile);
	connect(actionSave, &QAction::triggered, &editor_, &CEditor::save);
	connect(actionSaveAs, &QAction::triggered, &editor_, &CEditor::saveAs);

	connect(&editor_, &CEditor::statusLeft, this, &CMainWindow::setStatusLeft);
	connect(&editor_, &CEditor::statusMiddle, this, &CMainWindow::setStatusMiddle);
	connect(&editor_, &CEditor::statusRight, this, &CMainWindow::setStatusRight);

	connect(actionFind, &QAction::triggered, this, &CMainWindow::showFindDialog);
	connect(actionGoTo, &QAction::triggered, this, &CMainWindow::showGoToDialog);

	connect(&findDlg_, &CEditorFindDlg::findText, this, &CMainWindow::findText);
	connect(&findDlg_, &CEditorFindDlg::replaceText, this, &CMainWindow::replaceText);
	connect(&findDlg_, &CEditorFindDlg::replaceAllText, this, &CMainWindow::replaceAllText);

	connect(actionCut, &QAction::triggered, &editor_, &CEditor::cut);
	connect(actionCopy, &QAction::triggered, &editor_, &CEditor::copy);
	connect(actionPaste, &QAction::triggered, &editor_, &CEditor::paste);

	connect(actionUndo, &QAction::triggered, &editor_, &CEditor::undo);
	connect(actionRedo, &QAction::triggered, &editor_, &CEditor::redo);

	connect(fileListModel_->getSelectionModel(), &QItemSelectionModel::selectionChanged, this, &CMainWindow::fileSelectionChanged);
}

void CMainWindow::on_projectAddDirectoryButton_clicked()
{
	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly);
	dialog.setViewMode(QFileDialog::Detail);

	int result = dialog.exec();

	QString directory;
	if (result) {
		directory = dialog.selectedFiles()[0];

		CProjectItem projectItem;
		QFileInfo fileInfo(directory);

		projectItem.name_ = fileInfo.fileName();
		projectItem.srcDir_ = directory;

		QString defaultMaskForNewProject = confManager_->getAppSettingValue("defaultMaskForNewProject").toString();

		if (defaultMaskForNewProject == "") {
			projectItem.srcMask_ = "*.*";
		} else {
			projectItem.srcMask_ =  defaultMaskForNewProject;
		}

		QString defaultDirToExclude = confManager_->getAppSettingValue("defaultDirToExclude").toString();
		QString defaultFileMaskToExclude = confManager_->getAppSettingValue("defaultFileMaskToExclude").toString();

		if (defaultDirToExclude != "") {
			projectItem.dirToExclude_ = defaultDirToExclude;
		} else {
			projectItem.dirToExclude_ = ".git";
		}

		if (defaultFileMaskToExclude != "") {
			projectItem.fileMaskToExclude_ = defaultFileMaskToExclude;
		} else {
			projectItem.fileMaskToExclude_ = "*.tmp";
		}

		projectItem.headerMask_ = "";
		projectItem.labels_ = "";

		QDialog* dialog = new CProjectDlg(projectItem.name_, projectItem, this);
		dialog->exec();
	}

}

// load the project into environment
void CMainWindow::on_loadProjectButton_clicked()
{
    QStringList projectItemNameList = getSelectedProjectItemNameList();
	int projectSelected = projectItemNameList.size();

	QString projectItemName;
    CProjectItem projectItem;

	if (projectSelected != 0) {
		if (projectSelected > 1) {
			QMessageBox::information(this, "Load", "Only one project can be loaded each time", QMessageBox::Ok);
		} else {

			projectItemName = projectItemNameList.at(0);
			projectItem = CProjectManager::getInstance()->getProjectItem(projectItemName);

			currentProjectItem_ = projectItem;

			QDir currentDir(QDir::currentPath());

			// only load project if source directory exists
			if (currentDir.exists(projectItem.srcDir_)) {
				m_statusLeft->setText("Loading project " + projectItem.name_ + "...");

                filePattern_lineEdit->clear();

				projectLoadThread_.setCurrentProjectItem(projectItem);
				projectLoadThread_.setTaggerPtr(&tagger_);
				projectLoadThread_.setFileItemListPtr(&fileItemList_);

				search_lineEdit->setEnabled(false);

				projectLoadThread_.start();

				setWindowTitle(projectItemName + " - Blink");
			} else {
				QMessageBox::warning(this, "Load", "Cannot load project. Source directory doesn't exists.", QMessageBox::Ok);
			}
		}
	}
}

// update tags for the project
void CMainWindow::on_updateProjectButton_clicked()
{
	QStringList projectItemNameList = getSelectedProjectItemNameList();
	int projectSelected = projectItemNameList.size();

    QString projectItemName;
    CProjectItem projectItem;
	QDateTime currDateTime;

    if (projectSelected != 0) {
		if (projectSelected > 1) {
			QMessageBox::information(this, "Update", "Only one project can be updated each time", QMessageBox::Ok);
		} else {
			projectItemName = projectItemNameList.at(0);

			projectItem = CProjectManager::getInstance()->getProjectItem(projectItemName);

			QDir currentDir(QDir::currentPath());

			// only update project if source directory exists
			if (currentDir.exists(projectItem.srcDir_)) {
				currDateTime = QDateTime::currentDateTime();
				projectItem.tagUpdateDateTime_ = currDateTime.toString("dd/MM/yyyy hh:mm:ss");

				// tag last update date time updated so need update in project manager
				CProjectManager::getInstance()->updateProjectItem(false, projectItemName, projectItem);

				m_statusLeft->setText("Updating tag for " + projectItem.name_ + "...");

				projectUpdateThread_.setRebuildTag(false);
				projectUpdateThread_.setCurrentProjectItem(projectItem);
				projectUpdateThread_.start(QThread::HighestPriority); // priority for update thread
			} else {
                QMessageBox::warning(this, "Load", "Cannot update project. Source directory doesn't exists.", QMessageBox::Ok);
			}
		}
    }
}

void CMainWindow::projectRebuildTag(const QString projectItemName)
{
    CProjectItem projectItem;
	QDateTime currDateTime;

	projectItem = CProjectManager::getInstance()->getProjectItem(projectItemName);

	QDir currentDir(QDir::currentPath());

	// only update project if source directory exists
	if (currentDir.exists(projectItem.srcDir_)) {
		currDateTime = QDateTime::currentDateTime();
		projectItem.tagUpdateDateTime_ = currDateTime.toString("dd/MM/yyyy hh:mm:ss");

		// tag last update date time updated so need update in project manager
		CProjectManager::getInstance()->updateProjectItem(false, projectItemName, projectItem);

		m_statusLeft->setText("Rebuilding tag for " + projectItem.name_ + "...");

		projectUpdateThread_.setRebuildTag(true);
		projectUpdateThread_.setCurrentProjectItem(projectItem);
		projectUpdateThread_.start(QThread::HighestPriority); // priority for update thread
	} else {
		QMessageBox::warning(this, "Load", "Cannot rebuilt project. Source directory doesn't exists.", QMessageBox::Ok);
	}
}

void CMainWindow::on_rebuildTagProjectButton_clicked()
{
	QStringList projectItemNameList = getSelectedProjectItemNameList();
	int projectSelected = projectItemNameList.size();

    QString projectItemName;

    if (projectSelected != 0) {
		if (projectSelected > 1) {
			QMessageBox::information(this, "Rebuild", "Only one project can be rebuilt each time", QMessageBox::Ok);
		} else {
			projectItemName = projectItemNameList.at(0);
			projectRebuildTag(projectItemName);
		}
    }
}


// get the project name of the selected list item
QStringList CMainWindow::getSelectedProjectItemNameList()
{
    QModelIndexList indexSelectedList;
	QModelIndex mappedIndex;
	int rowSelected;
	QStandardItem* itemSelected;

    QString projectItemName;
	QStringList projectItemNameList;

    // get selected items index list
    indexSelectedList = projectListSelectionModel_->selectedIndexes();

	foreach (const QModelIndex& indexSelected, indexSelectedList) {
		// map back from proxy model
		mappedIndex = projectListProxyModel_->mapToSource(indexSelected);
		rowSelected = mappedIndex.row();

		if (indexSelected.isValid()) {
			itemSelected = projectListModel_->item(rowSelected, 0);
			if (itemSelected != 0) {
				projectItemName = itemSelected->text();
			}
		}

		// as all items in the same row with differnt columns will also be returned in selectedIndexes
		if (!projectItemNameList.contains(projectItemName)) {
			// not add project name to the list if already added
			projectItemNameList += projectItemName;
		}
	}

    return projectItemNameList;
}

// get the project name of the selected list item
QStringList CMainWindow::getSelectedFileItemNameList()
{
    QModelIndexList indexSelectedList;
	QModelIndex mappedIndex;
	int rowSelected;
	QStandardItem* itemSelected;

    QString fileItemName;
	QStringList fileItemNameList;

    // get selected items index list
    indexSelectedList = fileListModel_->getSelectionModel()->selectedIndexes();

	foreach (const QModelIndex& indexSelected, indexSelectedList) {
		// map back from proxy model
		mappedIndex = fileListModel_->getProxyModel()->mapToSource(indexSelected);
		rowSelected = mappedIndex.row();

		if (indexSelected.isValid()) {
			itemSelected = fileListModel_->item(rowSelected, 0);
			if (itemSelected != 0) {
				fileItemName = itemSelected->text();
			}
		}

		// as all items in the same row with differnt columns will also be returned in selectedIndexes
		if (!fileItemNameList.contains(fileItemName)) {
			// not add project name to the list if already added
			fileItemNameList += fileItemName;
		}
	}

    return fileItemNameList;
}

// edit selected project
void CMainWindow::on_editProjectButton_clicked()
{
    CProjectItem projectItem;

	QStringList projectItemNameList = getSelectedProjectItemNameList();
	int projectSelected = projectItemNameList.size();

    QString projectItemName;

	if (projectSelected != 0) { // only do processing when a item is selected
		if (projectSelected > 1) {
			QMessageBox::information(this, "Edit", "Only one project can be edited each time", QMessageBox::Ok);
		} else {

			projectItemName = projectItemNameList.at(0);

			projectItem = CProjectManager::getInstance()->getProjectItem(projectItemName);

			QDialog* dialog = new CProjectDlg(projectItemName, projectItem);

			dialog->exec();
		}
	}
}

// delete selected project
void CMainWindow::on_deleteProjectButton_clicked()
{
	QStringList projectItemNameList = getSelectedProjectItemNameList();
	int projectSelected = projectItemNameList.size();

    int ret;

    if (projectSelected != 0) { // only do processing when a item is selected
		if (projectSelected > 1) {
			ret = QMessageBox::question(this, "Confirm multiple delete", "Delete " + QString::number(projectSelected) + " selected projects?", QMessageBox::Yes, QMessageBox::No);
			if (ret == QMessageBox::Yes) {

				foreach (const QString& projectItemName, projectItemNameList) {
					CProjectManager::getInstance()->removeProjectItem(projectItemName);
				}
			}
		} else {
			QString projectItemName = projectItemNameList.at(0);
			ret = QMessageBox::question(this, "Confirm delete", "Delete the selected project?", QMessageBox::Yes, QMessageBox::No);
			if (ret == QMessageBox::Yes) {
				CProjectManager::getInstance()->removeProjectItem(projectItemName);
			}
		}
    }
}

void CMainWindow::on_projectCopyPressed()
{
	QStringList selectedItemList = getSelectedProjectItemNameList();
	QString clipBoardStr = "";
	CProjectItem projectItem;
	int i = 0;

	if (selectedItemList.size() > 0) {
		for (i = 0; i < selectedItemList.size(); i++) {
			projectItem = CProjectManager::getInstance()->getProjectItem(selectedItemList[i]);

			if (i > 0) {
				clipBoardStr += "\n";
			}

			clipBoardStr = clipBoardStr + projectItem.srcDir_;
		}

		QClipboard *clipboard = QApplication::clipboard();

		clipboard->setText(clipBoardStr);
	}
}

void CMainWindow::on_exploreProjectButton_clicked()
{
	CProjectItem projectItem;

	QStringList selectedItemList = getSelectedProjectItemNameList();
	int itemSelected = selectedItemList.size();

	QString projectItemName;

	QString executeDir;

	if (itemSelected > 0) {
		if (itemSelected > 1) {
			QMessageBox::information(this, "Explore", "Can only explore one path", QMessageBox::Ok);
		} else {
			projectItemName = selectedItemList.at(0);
			projectItem = CProjectManager::getInstance()->getProjectItem(projectItemName);

			executeDir = projectItem.srcDir_;

#ifdef Q_OS_WIN
			QString excuteMethod = "explore";
            ShellExecute(NULL, reinterpret_cast<const wchar_t*>(excuteMethod.utf16()), reinterpret_cast<const wchar_t*>(executeDir.utf16()), NULL, NULL, SW_NORMAL);
#else
			QDesktopServices::openUrl(QUrl(executeDir, QUrl::TolerantMode));
#endif
		}
	}
}

void CMainWindow::on_consoleProjectButton_clicked()
{
	CProjectItem projectItem;

	QStringList selectedItemList = getSelectedProjectItemNameList();
	int itemSelected = selectedItemList.size();

	QString projectItemName;

	QString executeDir;

	if (itemSelected > 0) {
		if (itemSelected > 1) {
			QMessageBox::information(this, "Console", "Can only open console from one path", QMessageBox::Ok);
		} else {
			projectItemName = selectedItemList.at(0);
			projectItem = CProjectManager::getInstance()->getProjectItem(projectItemName);

			executeDir = projectItem.srcDir_;

#ifdef Q_OS_WIN
			QString excuteMethod = "open";
			QString consoleCommnad = "cmd.exe";

            ShellExecute(NULL, reinterpret_cast<const wchar_t*>(excuteMethod.utf16()), reinterpret_cast<const wchar_t*>(consoleCommnad.utf16()), NULL, reinterpret_cast<const wchar_t*>(executeDir.utf16()), SW_NORMAL);
#else
			// not implemented
#endif
		}
	}
}

void CMainWindow::on_aboutButton_clicked()
{
    QDialog* dialog = new CAboutDlg();
    dialog->exec();
}

void CMainWindow::on_clearOutputButton_clicked()
{
//    file_listView->clear();
}

void CMainWindow::on_clearLogButton_clicked()
{

}

void CMainWindow::setAlwaysOnTop(bool enable)
{
    Qt::WindowFlags flags = windowFlags();

    if (enable) {
        flags |= Qt::WindowStaysOnTopHint;
    } else {
        flags ^= Qt::WindowStaysOnTopHint;
    }

    QPoint p(frameGeometry().x(),frameGeometry().y()); // current position before set window flags

    setWindowFlags(flags);

    this->move(p);
    this->show();

	// save setting
	confManager_->setAppSettingValue("AlwaysOnTop", enable);
}

void CMainWindow::on_actionAlways_on_top_toggled()
{
    if (actionAlways_on_top->isChecked()) {
        setAlwaysOnTop(true);
    } else {
        setAlwaysOnTop(false);
    }
    on_actionTransparent_toggled(); // still transparency
}

void CMainWindow::on_actionFuzzyAutoComplete_toggled()
{
	if (actionFuzzyAutoComplete->isChecked()) {
		confManager_->setAppSettingValue("FuzzyAutoComplete", true);
	} else {
		confManager_->setAppSettingValue("FuzzyAutoComplete", false);
	}
}

void CMainWindow::on_actionLiveSearch_toggled()
{
	if (actionLiveSearch->isChecked()) {
		confManager_->setAppSettingValue("LiveSearch", true);
	} else {
		confManager_->setAppSettingValue("LiveSearch", false);
	}
}

void CMainWindow::setTransparency(bool enable)
{
    if (enable) {
        setWindowOpacity(0.7);
    } else {
        setWindowOpacity(1);
    }

	// save setting
	confManager_->setAppSettingValue("Transparency", enable);
}

void CMainWindow::on_actionTransparent_toggled()
{
    if (actionTransparent->isChecked()) {
        setTransparency(true);
    } else {
        setTransparency(false);
    }
}

void CMainWindow::on_actionToolbar_toggled()
{
    if (actionToolbar->isChecked()) {
		toolBar->show();
		confManager_->setAppSettingValue("ViewToolbar", true);
    } else {
        toolBar->hide();
		confManager_->setAppSettingValue("ViewToolbar", false);
    }
}

void CMainWindow::on_actionProject_Panel_toggled()
{
    if (actionProject_Panel->isChecked()) {
		mainTabWidget->show();
		confManager_->setAppSettingValue("ViewProjectPanel", true);
    } else {
        mainTabWidget->hide();
		confManager_->setAppSettingValue("ViewProjectPanel", true);
    }
}

void CMainWindow::on_actionFile_Panel_toggled()
{
    if (actionFile_Panel->isChecked()) {
		infoTabWidget->show();
		confManager_->setAppSettingValue("ViewFilePanel", true);
    } else {
        infoTabWidget->hide();
		confManager_->setAppSettingValue("ViewFilePanel", true);
    }
}

void CMainWindow::on_actionExit_triggered()
{
    this->close();
}

// configuration dialog
void CMainWindow::on_actionSetting_triggered()
{
	QDialog* dialog = new CConfigDlg();

	int dialogCode = dialog->exec();

	if (dialogCode == QDialog::Accepted) {
		QFont updatedProjectFont = static_cast<CConfigDlg*> (dialog)->getProjectDefaultFont();

		project_listView->updateProjectFont(updatedProjectFont);
		file_listView->updateOutputFont(updatedProjectFont);

		editor_.updateAllEditorFont();

		QFont updatedSymbolFont = static_cast<CConfigDlg*> (dialog)->getSymbolDefaultFont();
		setSymbolFont(updatedSymbolFont);

		QFont updatedSymbolAutoCompleteFont = static_cast<CConfigDlg*> (dialog)->getSymbolAutoCompleteDefaultFont();
		setSymbolAutoCompleteFont(updatedSymbolAutoCompleteFont);

		// update symbol panel
		on_searchButton_clicked();
	}
}

// find & replace dialog
void CMainWindow::on_actionFindReplaceDialog_triggered()
{
	int i = 0;

	const int fileTabIndex = infoTabWidget->indexOf(fileTab);
	const int symbolTabIndex = infoTabWidget->indexOf(symbolTab);

	QStringList fileList;

	// if current tab is file tab
	if (infoTabWidget->currentIndex() == fileTabIndex) {
		// copy from selected file list in file filter
		QModelIndexList selectedIndexList = fileListModel_->getSelectionModel()->selectedIndexes();

		if (selectedIndexList.size() > 0) { // got selected files
			foreach(const QModelIndex &index, selectedIndexList) {
				if (index.column() == 0) { // only 0 column corresponding to filename
					fileList.append(index.data(Qt::DisplayRole ).toString());
				}
			}
		} else { // default to all files if no selection
			for (i = 0; i < fileItemList_.size(); i++) {
				fileList.append(fileItemList_[i].fileName_);
			}
		}

		findReplaceModel_.setFileList(fileList);
	} else {
		fileList = findReplaceFileList_.keys();

		// set default find replace file list model updated when symbol queries
		findReplaceModel_.setFileList(fileList);
	}

	if (infoTabWidget->currentIndex() == symbolTabIndex) {
		if (findReplaceFileList_.size() == 0) {
			QMessageBox::information(this, "Find Replace", "No file found for the symbol. Please search the symbol first or select files from the file tab.", QMessageBox::Ok);
			return;
		}
	}

	CFindReplaceDlg* dialog = new CFindReplaceDlg(this, &findReplaceModel_);

	if (infoTabWidget->currentIndex() == symbolTabIndex) {
	   dialog->setFindLineEdit(search_lineEdit->text());
	}

	// non model dialog
	dialog->show();
}

void CMainWindow::saveWidgetPosition()
{
	QList<int> splitterSizeList;
	QString splitterSizeListStr = "";

	QList<int> vsplitterSizeList;
	QString vsplitterSizeListStr = "";

	confManager_->setValue("Window", "geometry", saveGeometry());

    splitterSizeList = splitter->sizes();
	foreach (const int& splitterSize, splitterSizeList) {
		splitterSizeListStr += QString::number(splitterSize) + " ";
	}

	confManager_->setValue("Window", "splitter", splitterSizeListStr);

	vsplitterSizeList = verticalSplitter->sizes();
	foreach (const int& splitterSize, vsplitterSizeList) {
		vsplitterSizeListStr += QString::number(splitterSize) + " ";
	}

	confManager_->setValue("Window", "vsplitter", vsplitterSizeListStr);

	int projectTabIndex = mainTabWidget->indexOf(projectTab);
	int fileTabIndex = infoTabWidget->indexOf(fileTab);

	confManager_->setValue("Window", "projectTabIndex", QString::number(projectTabIndex));
	confManager_->setValue("Window", "fileTabIndex", QString::number(fileTabIndex));

    confManager_->updateConfig();
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
    saveWidgetPosition();
    event->accept();
}

void CMainWindow::updateTagBuildProgress(int percentage, QString indexingFileName)
{
    // show the progress bar when pecentage completed >= 0
    if (percentage >= 0) {
        progressBar_.show();
		bTagBuildInProgress_ = true;
    }

    progressBar_.setValue(percentage);

	if (indexingFileName != "") {
        m_statusLeft->setText("Indexing " + indexingFileName + "...");
	}

    // hide the progress bar when completed
    if (percentage == 100) {
        m_statusLeft->setText("Tag update completed.");
		bTagBuildInProgress_ = false;
        progressBar_.hide();
    }
}

void CMainWindow::updateProjectLoadProgress(int percentage)
{
    if (percentage == 100) {
		loadFileList();

		search_lineEdit->clear(); // clear symbol search line edit
		symbol_textBrowser->clear(); // clear symbol text widget as well

		codeBrowser_.clear();
		m_statusRight->clear();

		search_lineEdit->setEnabled(true);

        m_statusLeft->setText("Project " + currentProjectItem_.name_ + " loaded.");
    } else if (percentage == 0) {
		m_statusLeft->setText("Failed to load Project " + currentProjectItem_.name_ + ".");

		search_lineEdit->setEnabled(true);
	}
}

void CMainWindow::updateCancelledTagBuild()
{
	progressBar_.reset();
	progressBar_.setValue(0);
	progressBar_.hide();
	bTagBuildInProgress_ = false;

	m_statusLeft->setText("Tag update cancelled.");
}

void CMainWindow::on_errorDuringRun(const QString& cmdStr)
{
	progressBar_.reset();
	progressBar_.setValue(0);
	progressBar_.hide();
	bTagBuildInProgress_ = false;

	appendLogList(TRACE_ERROR, cmdStr);

	QMessageBox::warning(this, "Tag update", "Error running command.\n"
											 "Please check log message for details.", QMessageBox::Ok);
}

void CMainWindow::on_projectPatternLineEditShortcutPressed()
{
	const int projectTabIndex = mainTabWidget->indexOf(projectTab);

    mainTabWidget->setCurrentIndex(projectTabIndex);
	projectPattern_lineEdit->setFocus();
}

void CMainWindow::on_filePatternLineEditShortcutPressed()
{
	const int fileTabIndex = infoTabWidget->indexOf(fileTab);

    infoTabWidget->setCurrentIndex(fileTabIndex);
	filePattern_lineEdit->setFocus();
}

void CMainWindow::on_searchLineEditShortcutPressed()
{
	const int symbolTabIndex = infoTabWidget->indexOf(symbolTab);

    infoTabWidget->setCurrentIndex(symbolTabIndex);
	search_lineEdit->setFocus();
}

void CMainWindow::on_infoTabWidgetToolBn_clicked()
{
	if (infoTabWidget->isHidden()) {
		infoTabWidget->show();
	} else {
		priorMainTabWidgetSize_ = infoTabWidget->size();

		infoTabWidget->hide();

		/*
		containerwidget->layout()->invalidate();
		QWidget *parent = containerwidget;
		while (parent) {
			parent->adjustSize();
			parent = parent->parentWidget();
		}
		*/
	}
}

void CMainWindow::projectFilterRegExpChanged()
{
	QRegularExpression regExp(projectPattern_lineEdit->text());

    if (actionProjectAndGroupCaseSensitive->isChecked()) {
		confManager_->setAppSettingValue("actionProjectAndGroupCaseSensitive", true);
    } else {
		confManager_->setAppSettingValue("actionProjectAndGroupCaseSensitive", false);
		regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    projectListProxyModel_->setFilterRegularExpression(regExp);
}

void CMainWindow::fileFilterRegExpChanged()
{
	QString trimmedFilePattern = filePattern_lineEdit->text().trimmed() ;
	QRegularExpression regExp(trimmedFilePattern);

    if (actionFileCaseSensitive->isChecked()) {
		confManager_->setAppSettingValue("FileFilterCaseSensitive", true);
    } else {
		confManager_->setAppSettingValue("FileFilterCaseSensitive", false);
		regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    fileListModel_->getProxyModel()->setFilterRegularExpression(regExp);
}

void CMainWindow::searchLineEditChanged()
{
	Qt::CaseSensitivity caseSensitivity;

    if (actionSymbolCaseSensitive->isChecked()) {
		caseSensitivity = Qt::CaseSensitive;
		confManager_->setAppSettingValue("SymbolSearchCaseSensitive", true);
    } else {
        caseSensitivity = Qt::CaseInsensitive;
		confManager_->setAppSettingValue("SymbolSearchCaseSensitive", false);
    }

    if (actionSymbolRegularExpression->isChecked()) {
		confManager_->setAppSettingValue("SymbolSearchRegularExpression", true);
    } else {
		confManager_->setAppSettingValue("SymbolSearchRegularExpression", false);
    }

	QStringList tagList;
	QMultiMap<int, QString> tagMap;

    bool bFuzzyAutoComplete = confManager_->getAppSettingValue("FuzzyAutoComplete", true).toBool();

	if (bFuzzyAutoComplete) {
		completer_.setCompletionMode(QCompleter::UnfilteredPopupCompletion);
		tagger_.getFuzzyMatchedTags(search_lineEdit->text(), tagMap, caseSensitivity);
	} else {
		completer_.setCompletionMode(QCompleter::PopupCompletion);
		tagger_.getMatchedTags(search_lineEdit->text(), tagMap, caseSensitivity);
	}

	for (auto tag: tagMap) {
		tagList.push_back(tag);
	}

	stringListModel_.setStringList(tagList);
	completer_.setModel(&stringListModel_);

	bool bLiveSearch = confManager_->getAppSettingValue("LiveSearch", true).toBool();
	if (bLiveSearch) {
		queryTagRowLimit(search_lineEdit->text(), 1000);
	}
}

void CMainWindow::on_symbolSearchFrameShortcutPressed()
{
	const int symbolTabIndex = infoTabWidget->indexOf(symbolTab);

	if (infoTabWidget->currentIndex() == symbolTabIndex) {
		if (searchFrame->isHidden()) {
			searchFrame->show();
			frameSymbol_lineEdit->setFocus();
		} else {
			searchFrame->hide();
			search_lineEdit->setFocus();
		}
	}
}

void CMainWindow::frameSymbolLineEditChanged()
{
#ifdef ENABLE_SYMBOL_WEB_VIEW
	webView->findText("", (QWebPage::FindFlags) QWebPage::HighlightAllOccurrences);
	webView->findText(frameSymbol_lineEdit->text(), (QWebPage::FindFlags) QWebPage::HighlightAllOccurrences);
#elif defined ENABLE_SYMBOL_RICH_TEXT_VIEW
	symbol_textBrowser->findText("", (QTextDocument::FindFlags) 0);
	symbol_textBrowser->findText(frameSymbol_lineEdit->text(), (QTextDocument::FindFlags) 0);
#else
	symbol_textBrowser->find("", (QTextDocument::FindFlags) 0);
	symbol_textBrowser->find(frameSymbol_lineEdit->text(), (QTextDocument::FindFlags) 0);
#endif
}

void CMainWindow::on_nextSymbolButton_clicked()
{
#ifdef ENABLE_SYMBOL_WEB_VIEW
    webView->findText(frameSymbol_lineEdit->text(), QWebPage::FindWrapsAroundDocument);
#elif defined ENABLE_SYMBOL_RICH_TEXT_VIEW
    symbol_textBrowser->findText(frameSymbol_lineEdit->text(), (QTextDocument::FindFlags) 0);
#else
    symbol_textBrowser->find(frameSymbol_lineEdit->text(), (QTextDocument::FindFlags) 0);
#endif
}

void CMainWindow::on_previousSymbolButton_clicked()
{
#ifdef ENABLE_SYMBOL_WEB_VIEW
    webView->findText(frameSymbol_lineEdit->text(), QWebPage::FindBackward | QWebPage::FindWrapsAroundDocument);
#elif defined ENABLE_SYMBOL_RICH_TEXT_VIEW
	symbol_textBrowser->findText(frameSymbol_lineEdit->text(), (QTextDocument::FindFlags) QTextDocument::FindBackward);
#else
	symbol_textBrowser->find(frameSymbol_lineEdit->text(), (QTextDocument::FindFlags) QTextDocument::FindBackward);
#endif
}


void CMainWindow::on_cancelTagUpdate()
{
	int ret;

	ret = QMessageBox::question(this, "Tag update", "Cancel tag update?", QMessageBox::Yes, QMessageBox::No);
	if (ret == QMessageBox::Yes) {
		m_statusLeft->setText("Cancelling tag update...");
		projectUpdateThread_.cancelUpdate();
	}
}

// right click context menu
void CMainWindow::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint p;

	// project_listView area and have selected project
	p = project_listView->mapFromGlobal(event->globalPos());

	// get current active tab
	const int mainTabIndex = mainTabWidget->currentIndex();
	const int projectTabIndex = mainTabWidget->indexOf(projectTab);

	if (mainTabIndex == projectTabIndex) {
		// in area of project_listView
		if (project_listView->rect().contains(p)) {
			QStringList projectItemNameList = getSelectedProjectItemNameList();
			int projectSelected = projectItemNameList.size();

			if (projectSelected != 0) { // only show menu if have more than 1 selected project
				// only show delete menu if more than one project selected
				if (projectSelected > 1) {
					QMenu menu(this);
					menu.addAction(actionProjectDelete);

					menu.exec(event->globalPos());
				} else {
					QMenu menu(this);

					menu.addAction(actionProjectAddDirectory);
					menu.addAction(actionProjectLoad);
					menu.addAction(actionProjectRebuildTag);
					menu.addAction(actionProjectModify);
					menu.addAction(actionProjectDelete);

					menu.addAction(actionProjectExplore);
					menu.addAction(actionProjectCopy);

#ifdef Q_OS_WIN
					menu.addAction(actionProjectConsole);
#endif
					menu.exec(event->globalPos());
				}
			}
		}
	}


	// get current active tab
	const int infoTabIndex = infoTabWidget->currentIndex();

	const int fileTabIndex = infoTabWidget->indexOf(fileTab);

	// in area of file_listView and correct tab page
	p = file_listView->mapFromGlobal(event->globalPos());
	if ((file_listView->rect().contains(p)) && (infoTabIndex == fileTabIndex)) {

		QStringList fileItemNameList = getSelectedFileItemNameList();
		int fileItemSelected = fileItemNameList.size();

		if (fileItemSelected != 0) { // only show menu if have more than 1 selected project
			if (fileItemSelected > 1) {
				QMenu menu(this);

				menu.addAction(actionFileCopy);
				menu.exec(event->globalPos());
			} else {
				QMenu menu(this);

				menu.addAction(actionFileEdit);
				menu.addAction(actionFileEditNewTab);
				menu.addAction(actionFileEditExternal);

				menu.addAction(actionFileExplore);

				menu.addAction(actionFileCopy);
				menu.addAction(actionFileConsole);

#ifdef Q_OS_WIN
				menu.addAction(actionFileProperties);
#endif
				menu.exec(event->globalPos());
			}
		}

	}

	// in area of status bar
	p = statusBar()->mapFromGlobal(event->globalPos());
	// only show cancel tag update menu when tag build in progress
	if (statusBar()->rect().contains(p) && bTagBuildInProgress_) {
		QMenu menu(this);

		menu.addAction(actionCancelTagUpdate);

		menu.exec(event->globalPos());
	}

}

void CMainWindow::on_fileEditExternalPressed()
{
	QStringList selectedItemList = getSelectedFileItemNameList();
	int itemSelected = selectedItemList.size();

	QString executeDir;
	QString editFilename;

	if (itemSelected > 0) {
		if (itemSelected > 1) {
			QMessageBox::information(this, "Edit", "Can only edit one file", QMessageBox::Ok);
		} else {
			QFileInfo fileInfo(selectedItemList.at(0));
			executeDir = fileInfo.absoluteDir().absolutePath();

			QString consoleCommnad = confManager_->getAppSettingValue("DefaultEditor").toString();

			if (consoleCommnad == "") {
				QMessageBox msgBox;
				msgBox.setIcon(QMessageBox::Information);
				msgBox.setText("External editor has not be defined. Please set it in Options -> Settings -> Main -> External Editor");
				msgBox.exec();
				return;
			}

#ifdef Q_OS_WIN
			editFilename = "\"" + selectedItemList.at(0) + "\"";
#else
			editFilename = selectedItemList.at(0);
#endif

#ifdef Q_OS_WIN
			QString excuteMethod = "open";

            ShellExecute(NULL, reinterpret_cast<const wchar_t*>(excuteMethod.utf16()), reinterpret_cast<const wchar_t*>(consoleCommnad.utf16()), reinterpret_cast<const wchar_t*> (editFilename.utf16()), reinterpret_cast<const wchar_t*>(executeDir.utf16()), SW_NORMAL);
#else
			QProcess::startDetached(consoleCommnad, QStringList(editFilename));
#endif
		}
	}
}

void CMainWindow::setCodeBrowserFont(QsciLexer* lexer)
{
	QFont editorFont;

	QString editorFontSettingStr = CConfigManager::getInstance()->getAppSettingValue("EditorFont").toString();
	editorFont.fromString(editorFontSettingStr);

	QFont consolasFont("Consolas", QApplication::font().pointSize());

	if (editorFontSettingStr == "") {
		lexer->setFont(consolasFont);
	} else {
		lexer->setFont(editorFont);
	}
}

void CMainWindow::showInCodeBrowser(const QString &filePath, int lineNum)
{
	QFileInfo fileInfo(filePath);
	QString filename(fileInfo.fileName());

	codeBrowserFileName_ = filePath;

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

	setCodeBrowserFont(lexer);

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

	codeBrowser_.setText(in.readAll());
	QApplication::restoreOverrideCursor();

	codeBrowser_.setAutoIndent(true);
	codeBrowser_.setLexer(lexer);

	codeBrowser_.setAutoCompletionThreshold(1);
    codeBrowser_.setAutoCompletionSource(QsciScintilla::AcsDocument);

	codeBrowser_.setFolding(QsciScintilla::PlainFoldStyle);
	codeBrowser_.setTabWidth(4);

	if (lineNum > 4) {
		codeBrowser_.setFirstVisibleLine(lineNum - 4);
	}
	codeBrowser_.setCursorPosition(lineNum, 0);
	codeBrowser_.setFocus();

	m_statusRight->setText(filePath);
}

void CMainWindow::codeBrowserModified() {
	QString fileNameModified = QString("* ") + codeBrowserFileName_;
    m_statusRight->setText(fileNameModified);

	codeBrowser_.setModified(true);
	setWindowModified(true);
}

void CMainWindow::showCurrentCursorPosition(int line, int index)
{
	m_statusMiddle->setText("Ln: " + QString::number(line + 1) + ", Col: " +  QString::number(index));
}

void CMainWindow::findText(const QString& text, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	editor_.findText(text, bMatchWholeWord, bCaseSensitive, bRegularExpression);
}

void CMainWindow::replaceText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	editor_.replaceText(findText, replaceText, bMatchWholeWord, bCaseSensitive, bRegularExpression);
}

void CMainWindow::replaceAllText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression)
{
	editor_.replaceAllText(findText, replaceText, bRegularExpression, bCaseSensitive, bMatchWholeWord);
}

void CMainWindow::newFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("New File"),
			QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/untitled.txt");

	if (!fileName.isEmpty()) {
		codeBrowserFileName_ = fileName;
		m_statusRight->setText(fileName);
	}
}

void CMainWindow::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty()) {
		loadFile(fileName);

		codeBrowserFileName_ = fileName;
		m_statusRight->setText(fileName);
	}
}

void CMainWindow::saveFile()
{
	if (codeBrowserFileName_ != "") {
		saveFileImpl(codeBrowserFileName_);
	}
}

void CMainWindow::saveFileAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save File", codeBrowserFileName_);
	if (fileName.isEmpty()) {
		return;
	}

	saveFileImpl(fileName);
}

void CMainWindow::saveFileImpl(const QString &fileName)
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

	fileOutput << codeBrowser_.text();

	QApplication::restoreOverrideCursor();

	m_statusLeft->setText("File saved");

	codeBrowser_.setModified(false);
	setWindowModified(false);

    m_statusRight->setText(codeBrowserFileName_);
}

void CMainWindow::loadFile(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	QString filename(fileInfo.fileName());

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

	codeBrowser_.setText(in.readAll());
	QApplication::restoreOverrideCursor();

	codeBrowser_.setAutoIndent(true);
	codeBrowser_.setLexer(lexer);

	m_statusLeft->setText("File loaded");

	codeBrowserModified();
}

void CMainWindow::setEditorFont(QsciLexer* lexer)
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

void CMainWindow::showFindDialog()
{
	findDlg_.show();
	findDlg_.setFocus();
	findDlg_.setLineEditFocus();
}

void CMainWindow::showGoToDialog()
{
	bool ok;
    int line = QInputDialog::getInt(this, tr("Go to Line"),
                                         tr("Line:"),
                                         0,  -2147483647, 2147483647, 1, &ok);

	if (ok) {
		editor_.goToLine(line);
	}

}

void CMainWindow::on_fileListItemDoubleClicked()
{
	if (confManager_->getAppSettingValue("UseExternalEditor").toBool()) {
        on_fileEditExternalPressed();
	}

	on_fileEditPressed();
}

void CMainWindow::on_fileEditPressed()
{
	QStringList selectedItemList = getSelectedFileItemNameList();
	int itemSelected = selectedItemList.size();

	QString executeDir;

	if (itemSelected > 0) {
		if (itemSelected == 1) {
			editor_.loadFile(selectedItemList.at(0));
			m_statusRight->setText(selectedItemList.at(0));
		}
	}
}

void CMainWindow::on_fileEditNewTabPressed()
{
	QStringList selectedItemList = getSelectedFileItemNameList();
	int itemSelected = selectedItemList.size();

	QString executeDir;

	if (itemSelected > 0) {
		if (itemSelected > 1) {
			QMessageBox::information(this, "Edit", "Can only edit one file", QMessageBox::Ok);
		} else {
			editor_.loadFileNewTab(selectedItemList.at(0));
			m_statusRight->setText(selectedItemList.at(0));
		}
	}
}

void CMainWindow::on_fileCopyPressed()
{
	QStringList selectedItemList = getSelectedFileItemNameList();
	QString clipBoardStr = "";
	int i = 0;

	if (selectedItemList.size() > 0) {
		for (i = 0; i < selectedItemList.size(); i++) {
			if (i > 0) {
				clipBoardStr += "\n";
			}

			clipBoardStr = clipBoardStr + selectedItemList[i];
		}

		QClipboard *clipboard = QApplication::clipboard();

		clipboard->setText(clipBoardStr);
	}
}

void CMainWindow::on_fileExplorePressed()
{
	QStringList selectedItemList = getSelectedFileItemNameList();
	int itemSelected = selectedItemList.size();

	QString executeDir;

	if (itemSelected > 0) {
		if (itemSelected > 1) {
			QMessageBox::information(this, "Explore", "Can only explore one path", QMessageBox::Ok);
		} else {
			QFileInfo fileInfo(selectedItemList.at(0));
			executeDir = fileInfo.absoluteDir().absolutePath();

#ifdef Q_OS_WIN
			QString excuteMethod = "explore";
            ShellExecute(NULL, reinterpret_cast<const wchar_t*>(excuteMethod.utf16()), reinterpret_cast<const wchar_t*>(executeDir.utf16()), NULL, NULL, SW_NORMAL);
#else
			QDesktopServices::openUrl(QUrl(executeDir, QUrl::TolerantMode));
#endif
		}
	}
}

void CMainWindow::on_fileConsolePressed()
{
	QStringList selectedItemList = getSelectedFileItemNameList();
	int itemSelected = selectedItemList.size();

	QString executeDir;

	if (itemSelected > 0) {
		if (itemSelected > 1) {
			QMessageBox::information(this, "Console", "Can only open console from one path", QMessageBox::Ok);
		} else {
			QFileInfo fileInfo(selectedItemList.at(0));
			executeDir = fileInfo.absoluteDir().absolutePath();

#ifdef Q_OS_WIN
			QString excuteMethod = "open";
			QString consoleCommnad = "cmd.exe";

            ShellExecute(NULL, reinterpret_cast<const wchar_t*>(excuteMethod.utf16()), reinterpret_cast<const wchar_t*>(consoleCommnad.utf16()), NULL, reinterpret_cast<const wchar_t*>(executeDir.utf16()), SW_NORMAL);
#else
			executeDir = "--working-directory=" + executeDir;
			QProcess::startDetached("gnome-terminal", QStringList(executeDir));
#endif
		}
	}
}

void CMainWindow::on_filePropertiesPressed()
{
	QStringList selectedItemList = getSelectedFileItemNameList();
	int itemSelected = selectedItemList.size();

	QString executeDir;

	if (itemSelected > 0) {
		if (itemSelected > 1) {
			QMessageBox::information(this, "Properties", "Can only view properties for one file", QMessageBox::Ok);
		} else {
#ifdef Q_OS_WIN
			SHELLEXECUTEINFO execInfo;
			QString excuteMethod = "properties";

			ZeroMemory(&execInfo, sizeof(execInfo));
			execInfo.cbSize= sizeof(execInfo);
			execInfo.hwnd= NULL;
			execInfo.lpVerb= reinterpret_cast<const wchar_t*>(excuteMethod.utf16());
			execInfo.fMask= SEE_MASK_INVOKEIDLIST;
			execInfo.lpFile = reinterpret_cast<const wchar_t*>(selectedItemList.at(0).utf16());

            ShellExecuteEx(&execInfo);
#else
			// not implemented
#endif
		}
	}
}

QString CMainWindow::findStringCaseInsensitive(const QString& str, const QString& searchStr)
{
	const int index = str.indexOf(searchStr, 0, Qt::CaseInsensitive);
	if (index != -1) {
		return str.mid(index, searchStr.length());
	}
	return QString();
}

void CMainWindow::queryTag(const QString& tag)
{
	unsigned int limitSearchRow = confManager_->getAppSettingValue("limitSearchRow").toUInt();
	if (limitSearchRow == 0) {
		limitSearchRow = 30000;
	}

	queryTagRowLimit(tag, limitSearchRow);
}

void CMainWindow::queryTagRowLimit(const QString& tag, unsigned int limitSearchRow)
{
	QElapsedTimer timerQuery;
	timerQuery.start();

	QString tagDbFileName, inputFileName;

	QString resultHtml;

	QList<CTagResultItem> resultList;

	QString resultItemSrcLine;
    QFileInfo resultItemFileInfo;

	bool bSymbolRegularExpression = false;

    Qt::CaseSensitivity caseSensitivity =
            actionSymbolCaseSensitive->isChecked() ? Qt::CaseSensitive
                                                       : Qt::CaseInsensitive;

    if (actionSymbolRegularExpression->isChecked()) {
       bSymbolRegularExpression = true;
	}

	/*
	webView->load(QUrl(search_lineEdit->text()));
	webView->show();
	*/

	QString lineSrcBeforeToPrint;
	QString lineSrcAfterToPrint;

	int i, j;
	int lineSrcSize;

	QString lineNumStr;
	QString modifiedLineSrc;
	QString tagToQueryFiltered;

	if (!currentProjectItem_.name_.isEmpty()) {

		tagDbFileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tags/" + currentProjectItem_.name_ + "/" + QString(QTagger::kQTAG_DEFAULT_TAGDBNAME);
		inputFileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tags/" + currentProjectItem_.name_ + "/" + QString(CSourceFileList::kFILE_LIST);

		QElapsedTimer timer;
		timer.start();

		tagger_.queryTagLoadedSymbol(fileItemList_, tag, tagToQueryFiltered, resultList, caseSensitivity, bSymbolRegularExpression, limitSearchRow);

		qDebug() << "queryTag took" << timer.elapsed() << "ms";

		//QString tagToQuery = tag.toHtmlEscaped();
		QString tagToQuery = tagToQueryFiltered.toHtmlEscaped();

		resultHtml = "<html>";
		resultHtml += "<body>";
		resultHtml += "<pre>";

		QList<int>::const_iterator indentIt;
		int minIndent = 0;

		findReplaceFileList_.clear();

		QString matchedStr = "";

		foreach (const CTagResultItem& resultItem, resultList) {
			// drive letter colon for first field

            resultItemFileInfo.setFile(resultItem.filePath_);

			resultItemSrcLine = resultItem.fileLineSrc_;
			resultItemSrcLine = resultItemSrcLine.toHtmlEscaped();

			if (caseSensitivity) {
				resultItemSrcLine.replace(tagToQuery, "<keyword>" + tagToQuery + "</keyword>", Qt::CaseSensitive);
			} else {
				matchedStr = findStringCaseInsensitive(resultItemSrcLine, tagToQuery);
				resultItemSrcLine.replace(matchedStr, "<keyword>" + matchedStr + "</keyword>", Qt::CaseSensitive);
			}

			lineSrcBeforeToPrint = "";
			lineSrcAfterToPrint = "";

			minIndent = resultItem.lineSrcIndentLevel_;

			if (resultItem.beforeIndentLevelList_.size() > 0) {
				indentIt = std::min_element(resultItem.beforeIndentLevelList_.begin(), resultItem.beforeIndentLevelList_.end());
				if (*indentIt < minIndent) {
					minIndent = *indentIt;
				}
			}

			if (resultItem.afterIndentLevelList_.size() > 0) {
				indentIt = std::min_element(resultItem.afterIndentLevelList_.begin(), resultItem.afterIndentLevelList_.end());
				if (*indentIt < minIndent) {
					minIndent = *indentIt;
				}
			}

			for (j = 0; j < resultItem.lineSrcIndentLevel_ - minIndent; j++) {
				resultItemSrcLine = "&nbsp;&nbsp;&nbsp;" + resultItemSrcLine;
			}

			lineSrcSize = resultItem.fileLineSrcBeforeList_.size();
			if (lineSrcSize != 0) {
				i = 0;

				foreach (const QString& lineSrc, resultItem.fileLineSrcBeforeList_) {

					lineNumStr = "<linenum>" + QString::number(resultItem.fileLineNum_ - lineSrcSize + i) + "</linenum> ";
					modifiedLineSrc = lineSrc;
					modifiedLineSrc = modifiedLineSrc.toHtmlEscaped();

					if (caseSensitivity) {
						modifiedLineSrc.replace(tagToQuery, "<keyword>" + tagToQuery + "</keyword>", Qt::CaseSensitive);
					} else {
						matchedStr = findStringCaseInsensitive(modifiedLineSrc, tagToQuery);
						modifiedLineSrc.replace(matchedStr, "<keyword>" + matchedStr + "</keyword>", Qt::CaseSensitive);
					}

					for (j = 0; j < resultItem.beforeIndentLevelList_.at(i) - minIndent; j++) {
						modifiedLineSrc = "&nbsp;&nbsp;&nbsp;" + modifiedLineSrc;
					}

					lineSrcBeforeToPrint += lineNumStr + modifiedLineSrc + "<br />";
					i++;
				}
			}

			if (resultItem.fileLineSrcAfterList_.size() != 0) {
				i = 0;

				foreach (const QString& lineSrc, resultItem.fileLineSrcAfterList_) {

					lineNumStr = "<linenum>" + QString::number(resultItem.fileLineNum_  + i + 1) + "</linenum> ";
					modifiedLineSrc = lineSrc;
					modifiedLineSrc = modifiedLineSrc.toHtmlEscaped();

					if (caseSensitivity) {
						modifiedLineSrc.replace(tagToQuery, "<keyword>" + tagToQuery + "</keyword>", Qt::CaseSensitive);
					} else {
						matchedStr = findStringCaseInsensitive(modifiedLineSrc, tagToQuery);
						modifiedLineSrc.replace(matchedStr, "<keyword>" + matchedStr + "</keyword>", Qt::CaseSensitive);
					}

					for (j = 0; j < resultItem.afterIndentLevelList_.at(i) - minIndent; j++) {
						modifiedLineSrc = "&nbsp;&nbsp;&nbsp;" + modifiedLineSrc;
					}

					lineSrcAfterToPrint += "<br />" + lineNumStr + modifiedLineSrc;
					i++;
				}
			}

			resultHtml += QString("<div><a href=\"") + resultItem.filePath_ + "#" + QString::number(resultItem.fileLineNum_) + "\">" +
						resultItemFileInfo.fileName() + "</a></div>" +
						lineSrcBeforeToPrint +

						"<linenum>" + QString::number(resultItem.fileLineNum_) + "</linenum> " +
						resultItemSrcLine +
						lineSrcAfterToPrint + "<div><spacesize>&nbsp;</spacesize></div>";

			//qDebug() << "resultHtml = " << resultHtml << Qt::endl;

            findReplaceFileList_.insert(resultItem.filePath_, 0);
		}

		m_statusLeft->setText("Found " + QString::number(resultList.size()) + " symbols in " + QString::number(findReplaceFileList_.size()) + " files.");

		resultHtml += "</pre>";
		resultHtml += "</body></html>";

		/*
		QString resultHtmlFileName = "result.html";
		QFile resultHtmlFile(resultHtmlFileName);
		if (!resultHtmlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << "Cannot open result html file (" << resultHtmlFileName << ") for writing!" << Qt::endl;
			return;
		}

		QTextStream resultHtmlStream(&resultHtmlFile);
		*/

		//webView->load(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/Html/index.html"));

#ifdef ENABLE_SYMBOL_WEB_VIEW
		webView->setHtml(resultHtml);
		webView->show();
#elif defined ENABLE_SYMBOL_RICH_TEXT_VIEW
		symbol_textBrowser->setHtml(resultHtml);
		symbol_textBrowser->show();
#else
		QElapsedTimer timerSetHtml;
		timerSetHtml.start();

		textDocument_.setHtml(resultHtml);

		qDebug() << "setHtml took" << timerSetHtml.elapsed() << "ms";

		symbol_textBrowser->setDocument(&textDocument_);

		symbol_textBrowser->show();
		/*
		symbol_textBrowser->clear();
		symbol_textBrowser->appendHtml(resultHtml);
		symbol_textBrowser->show();
        */
#endif

		/*
		resultHtmlStream << resultHtml;
		resultHtmlStream.flush();
		resultHtmlFile.close();
		*/
	} else {
		QMessageBox::information(this, "Search", "Please load an existing project or create a new project and load it", QMessageBox::Ok);

	}

	qDebug() << "queryTag full took" << timerQuery.elapsed() << "ms";
}

void CMainWindow::queryTagTop1000(const QString& tag)
{
	bool bLiveSearch = confManager_->getAppSettingValue("LiveSearch", true).toBool();
	if (bLiveSearch) {
		queryTagRowLimit(tag, 1000);
	}

}
void CMainWindow::on_searchButton_clicked()
{
	QString tagToQuery;

	tagToQuery = search_lineEdit->text();
	tagToQuery = tagToQuery.toHtmlEscaped();

	queryTag(tagToQuery);
}


void CMainWindow::wheelEvent(QWheelEvent *e)
{
	/* YCH modified (begin), commented temporary */
	/*
	QPoint p;

	p = project_listView->mapFromGlobal(e->globalPosition());

    if (e->modifiers() == Qt::ControlModifier) {
        e->accept();
	}
	*/
	/* YCH modified (end), commented temporary */

	QMainWindow::wheelEvent(e);
}


void CMainWindow::appendLogList(TRACE_LEVEL level, const QString& msg)
{
	QString qStrBuf;

	switch (level) {
		case TRACE_DEBUG:   qStrBuf = "DEBUG:   " + msg; break;
		case TRACE_INFO:    qStrBuf = "INFO:    " + msg; break;
		case TRACE_WARNING: qStrBuf = "WARNING: " + msg; break;
		case TRACE_ERROR:   qStrBuf = "ERROR:   " + msg; break;
	}
}

void CMainWindow::keyPressEvent(QKeyEvent *event)
{

	if (filePattern_lineEdit->hasFocus()) {
		switch (event->key()) {
			case Qt::Key_Up:
				file_listView->setFocus();
				break;
			case Qt::Key_Down:
				file_listView->setFocus();
				break;
		}
	} else if (projectPattern_lineEdit->hasFocus()) {
		switch (event->key()) {
			case Qt::Key_Up:
				project_listView->setFocus();
				break;
			case Qt::Key_Down:
				project_listView->setFocus();
				break;
		}
	}

	QMainWindow::keyPressEvent(event);
}

void CMainWindow::setStatusLeft(const QString& status)
{
	m_statusLeft->setText(status);
}

void CMainWindow::setStatusMiddle(const QString& status)
{
	m_statusMiddle->setText(status);
}

void CMainWindow::setStatusRight(const QString& status)
{
	m_statusRight->setText(status);
}

void CMainWindow::fileSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QStringList fileSelected = getSelectedFileItemNameList();

	if (fileSelected.size() == 1) {
		setStatusLeft(QString::number(fileSelected.size()) + " file selected.");
	} else if (fileSelected.size() > 1) {
		setStatusLeft(QString::number(fileSelected.size()) + " files selected.");
	}

}

