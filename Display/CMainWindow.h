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

#include "Model/CProjectManager.h"
#include "Model/CProjectLoadThread.h"
#include "Model/CProjectUpdateThread.h"

#include "Model/CGroupLoadThread.h"

#include "Model/CProjectListModel.h"
#include "Model/CGroupListModel.h"
#include "Model/CFileListModel.h"

#include "Model/CFindReplaceModel.h"

#include "Model/CConfigManager.h"

#include "ui_mainWindow.h"

#include "Utils/commonType.h"

class CMainWindow : public QMainWindow, private Ui::mainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget* parent = 0);
    virtual ~CMainWindow() {}

	void setSplitterSizes(const QList<int>& splitterSizeList);
    void restoreTabWidgetPos();

public slots:
    void loadProjectList();
    void loadGroupList();
	void loadFileList();

private slots:
    void on_newProjectButton_clicked();
    void on_loadProjectButton_clicked();
    void on_updateProjectButton_clicked();
    void on_rebuildTagProjectButton_clicked();
	void on_projectCopyPressed();

    void on_editProjectButton_clicked();
    void on_deleteProjectButton_clicked();

	void on_exploreProjectButton_clicked();
	void on_consoleProjectButton_clicked();


    void on_newGroupButton_clicked();
    void on_loadGroupButton_clicked();
    void on_updateGroupButton_clicked();
    void on_editGroupButton_clicked();
    void on_deleteGroupButton_clicked();

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

	void on_fileEditPressed();
	void on_fileCopyPressed();
	void on_fileExplorePressed();
	void on_fileConsolePressed();
	void on_filePropertiesPressed();

	void on_searchButton_clicked();

	void wheelEvent(QWheelEvent *e);

    void updateTagBuildProgress(int percentage);
	void updateCancelledTagBuild();

	void updateProjectLoadProgress(int percentage);
	void updateGroupLoadProgress(int percentage);

	void on_errorDuringRun(const QString& cmdStr);

	void on_projectPatternLineEditShortcutPressed();
	void on_groupPatternLineEditShortcutPressed();

	void on_filePatternLineEditShortcutPressed();
	void on_searchLineEditShortcutPressed();

	void on_infoTabWidgetToolBn_clicked();

	void projectFilterRegExpChanged();
	void groupFilterRegExpChanged();

	void fileFilterRegExpChanged();

	void searchLineEditChanged();

	void on_symbolSearchFrameShortcutPressed();
    void on_nextSymbolButton_clicked();
	void on_previousSymbolButton_clicked();

	void frameSymbolLineEditChanged();

	void on_cancelTagUpdate();

	void contextMenuEvent(QContextMenuEvent* event);
	void appendLogList(TRACE_LEVEL level, const QString& msg);
	void keyPressEvent(QKeyEvent *event);

	void queryTag(const QString& tag);

private:
	void updateProjectListWidget();
    void updateGroupListWidget();
	void updateFileListWidget();

	void setSymbolFont(QFont symbolFont);
    void createActions();

    QStringList getSelectedProjectItemNameList();
	QStringList getSelectedGroupItemNameList();
	QStringList getSelectedFileItemNameList();

    void setAlwaysOnTop(bool enable);
    void setTransparency(bool enable);

    void saveWidgetPosition();

    void closeEvent(QCloseEvent *event);

    QProgressBar progressBar_;

	QActionGroup *filterMethodGroup;
	QRegExp::PatternSyntax patternSyntax_;

	CProjectListModel* projectListModel_;
    CGroupListModel* groupListModel_;
    CFileListModel* fileListModel_;

	CFindReplaceModel findReplaceModel_;

    CProjectLoadThread projectLoadThread_;
    CProjectUpdateThread projectUpdateThread_;

	CGroupLoadThread groupLoadThread_;

	QSortFilterProxyModel* projectListProxyModel_;
	QItemSelectionModel* projectListSelectionModel_;

	QSortFilterProxyModel* groupListProxyModel_;
	QItemSelectionModel* groupListSelectionModel_;

    QTimeLine timeLine_;

    QShortcut* projectPatternLineEditShortcut;
	QShortcut* groupPatternLineEditShortcut;

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
	CGroupItem currentGroupItem_;

	CConfigManager* confManager_;

	QTagger tagger_;

	T_FileItemList fileItemList_;

	QCompleter completer_;
	QStringListModel stringListModel_;

	QTextDocument textDocument_;
	QPlainTextDocumentLayout* textLayout_;

	QMap<QString, unsigned char> findReplaceFileList_; // unsigned char value not used

};
#endif // CMAINWINDOW_H


