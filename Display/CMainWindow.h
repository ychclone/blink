#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QTimeLine>
#include <QShortcut>

#include <QSortFilterProxyModel>

#include <QContextMenuEvent>

#include <QToolButton>

#include <QWheelEvent>

#include <QDesktopServices>

#include <QCompleter>
#include <QStringListModel>

// qsciscintilla
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>



#include "CEditor.h"

#include "Model/CProjectManager.h"
#include "Model/CProjectLoadThread.h"
#include "Model/CProjectUpdateThread.h"

#include "Model/CProjectListModel.h"
#include "Model/CFileListModel.h"

#include "Model/CFindReplaceModel.h"

#include "Model/CConfigManager.h"

#include "Display/CEditor.h"

#include "ui_mainWindow.h"

#include "CEditorFindDlg.h"

#include "Utils/commonType.h"

class CMainWindow : public QMainWindow, private Ui::mainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget* parent = 0);
    virtual ~CMainWindow() {}

	void setSplitterSizes(const QList<int>& splitterSizeList);
	void setVerticalSplitterSizes(const QList<int>& splitterSizeList);
    void restoreTabWidgetPos();
	
	QLabel* m_statusLeft;
	QLabel* m_statusMiddle;
    QLabel* m_statusRight;

public slots:
    void loadProjectList();
	void loadFileList();
	void showCurrentCursorPosition(int line, int index);

private slots:
	void on_projectAddDirectoryButton_clicked();

    void on_loadProjectButton_clicked();
    void on_updateProjectButton_clicked();

	void projectRebuildTag(const QString projectItemName);
    void on_rebuildTagProjectButton_clicked();
	void on_projectCopyPressed();

    void on_editProjectButton_clicked();
    void on_deleteProjectButton_clicked();

	void on_exploreProjectButton_clicked();
	void on_consoleProjectButton_clicked();

    void on_aboutButton_clicked();
    void on_clearOutputButton_clicked();
    void on_clearLogButton_clicked();
    void on_actionAlways_on_top_toggled();
    void on_actionTransparent_toggled();
	void on_actionToolbar_toggled();

	void on_actionProject_Panel_toggled();
	void on_actionFile_Panel_toggled();

    void on_actionExit_triggered();
	void on_actionSetting_triggered();
	void on_actionFindReplaceDialog_triggered();

	void setCodeBrowserFont(QsciLexer* lexer);
	void showInCodeBrowser(const QString &filePath, int lineNum);
	void codeBrowserModified();

	void findText(const QString& text, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);
	void replaceText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);
	void replaceAllText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);
	
	void newFile();
	void openFile();
	void saveFile();	
	void saveFileAs();
	void saveFileImpl(const QString &fileName);
	void showFindDialog();
	void showGoToDialog();
	void loadFile(const QString& filePath);
	void setEditorFont(QsciLexer* lexer);

	void on_fileListItemDoubleClicked();
	void on_fileEditExternalPressed();
    void on_fileEditPressed();
	void on_fileEditNewTabPressed();
	void on_fileCopyPressed();
	void on_fileExplorePressed();
	void on_fileConsolePressed();
	void on_filePropertiesPressed();

	void on_searchButton_clicked();

	void wheelEvent(QWheelEvent *e);

    void updateTagBuildProgress(int percentage, QString indexingFileName);
	void updateCancelledTagBuild();

	void updateProjectLoadProgress(int percentage);

	void on_errorDuringRun(const QString& cmdStr);

	void on_projectPatternLineEditShortcutPressed();

	void on_filePatternLineEditShortcutPressed();
	void on_searchLineEditShortcutPressed();

	void on_infoTabWidgetToolBn_clicked();

	void projectFilterRegExpChanged();

	void fileFilterRegExpChanged();

	void searchLineEditChanged();

	void on_symbolSearchFrameShortcutPressed();
    void on_nextSymbolButton_clicked();
	void on_previousSymbolButton_clicked();

	void on_actionFuzzyAutoComplete_toggled();
	void on_actionLiveSearch_toggled();

	void frameSymbolLineEditChanged();

	void on_cancelTagUpdate();

	void contextMenuEvent(QContextMenuEvent* event);
	void appendLogList(TRACE_LEVEL level, const QString& msg);
	void keyPressEvent(QKeyEvent *event);

	QString findStringCaseInsensitive(const QString& str, const QString& searchStr);
	void queryTag(const QString& tag);
	void queryTagRowLimit(const QString& tag, unsigned int limitSearchRow);
	void queryTagTop1000(const QString& tag);
	
	
	void setStatusLeft(const QString& status);
	void setStatusMiddle(const QString& status);
	void setStatusRight(const QString& status);

private:
	void updateProjectListWidget();
	void updateFileListWidget();

	void setSymbolFont(QFont symbolFont);
	void setSymbolAutoCompleteFont(QFont symbolFont);
    void createActions();

    QStringList getSelectedProjectItemNameList();
	QStringList getSelectedFileItemNameList();

    void setAlwaysOnTop(bool enable);
    void setTransparency(bool enable);

    void saveWidgetPosition();

    void closeEvent(QCloseEvent *event);

    QProgressBar progressBar_;

	CProjectListModel* projectListModel_;
    CFileListModel* fileListModel_;

	CFindReplaceModel findReplaceModel_;

    CProjectLoadThread projectLoadThread_;
    CProjectUpdateThread projectUpdateThread_;

	QSortFilterProxyModel* projectListProxyModel_;
	QItemSelectionModel* projectListSelectionModel_;

    QTimeLine timeLine_;

    QShortcut* projectPatternLineEditShortcut;

	QShortcut* fileSearchShortcut;
	QShortcut* tagSearchShortcut;

	QShortcut* outputExploreShortcut;
	QShortcut* outputConsoleShortcut;
	QShortcut* outputPropertiesShortcut;

	QShortcut* projectLoadShortcut;
	QShortcut* projectUpdateShortcut;

	QShortcut* symbolSearchFrameShortcut;
	QShortcut* nextSymbolSearchShortcut;
	QShortcut* previousSymbolSearchShortcut;

#ifdef Q_OS_WIN
    QShortcut* previousSymbolSearchShortcut_win;
#endif

	QToolButton *pInfoTabWidgetToolBn_;

	bool bTagBuildInProgress_;

	QSize priorMainTabWidgetSize_;
	int infoTabWidgetWidth;

	CProjectItem currentProjectItem_;

	CConfigManager* confManager_;

	QTagger tagger_;

	T_FileItemList fileItemList_;

	QCompleter completer_;
	QStringListModel stringListModel_;

	QTextDocument textDocument_;
	QPlainTextDocumentLayout* textLayout_;

	QMap<QString, unsigned char> findReplaceFileList_; // unsigned char value not used

    QsciScintilla codeBrowser_;
	CEditorFindDlg findDlg_;
	CEditor editor_;
	
	QString codeBrowserFileName_;
};
#endif // CMAINWINDOW_H


