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

#include "Model/CProfileManager.h"
#include "Model/CProfileLoadThread.h"
#include "Model/CProfileUpdateThread.h"

#include "Model/CGroupLoadThread.h"

#include "Model/CProfileListModel.h"
#include "Model/CGroupListModel.h"
#include "Model/COutputListModel.h"

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
    void loadProfileList();
    void loadGroupList();
	void loadOutputList();

private slots:
    void on_newProfileButton_clicked();
    void on_loadProfileButton_clicked();
    void on_updateProfileButton_clicked();
    void on_rebuildTagProfileButton_clicked();

    void on_editProfileButton_clicked();
    void on_deleteProfileButton_clicked();

	void on_exploreProfileButton_clicked();
	void on_consoleProfileButton_clicked();


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

	void on_actionProfile_Panel_toggled();
	void on_actionFile_Panel_toggled();

    void on_actionExit_triggered();
	void on_actionSetting_triggered();
	void on_actionFindReplaceDialog_triggered();

	void on_outputEditPressed();
	void on_outputCopyPressed();
	void on_outputExplorePressed();
	void on_outputConsolePressed();
	void on_outputPropertiesPressed();

	void on_searchButton_clicked();

	void wheelEvent(QWheelEvent *e);

    void updateTagBuildProgress(int percentage);
	void updateCancelledTagBuild();

	void updateProfileLoadProgress(int percentage);
	void updateGroupLoadProgress(int percentage);

	void on_errorDuringRun(const QString& cmdStr);

	void on_profilePatternLineEditShortcutPressed();
	void on_groupPatternLineEditShortcutPressed();

	void on_filePatternLineEditShortcutPressed();
	void on_searchLineEditShortcutPressed();

	void on_infoTabWidgetToolBn_clicked();

	void profileFilterRegExpChanged();
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
	void updateProfileListWidget();
    void updateGroupListWidget();
	void updateOutputListWidget();

	void setSymbolFont(QFont symbolFont);
    void createActions();

    QStringList getSelectedProfileItemNameList();
	QStringList getSelectedGroupItemNameList();
	QStringList getSelectedOutputItemNameList();

    void setAlwaysOnTop(bool enable);
    void setTransparency(bool enable);

    void saveWidgetPosition();

    void closeEvent(QCloseEvent *event);

    QProgressBar progressBar_;

	QActionGroup *filterMethodGroup;
	QRegExp::PatternSyntax patternSyntax_;

	CProfileListModel* profileListModel_;
    CGroupListModel* groupListModel_;
    COutputListModel* outputListModel_;

	CFindReplaceModel findReplaceModel_;

    CProfileLoadThread profileLoadThread_;
    CProfileUpdateThread profileUpdateThread_;

	CGroupLoadThread groupLoadThread_;

	QSortFilterProxyModel* profileListProxyModel_;
	QItemSelectionModel* profileListSelectionModel_;

	QSortFilterProxyModel* groupListProxyModel_;
	QItemSelectionModel* groupListSelectionModel_;

    QTimeLine timeLine_;

    QShortcut* profilePatternLineEditShortcut;
	QShortcut* groupPatternLineEditShortcut;

	QShortcut* fileSearchShortcut;
	QShortcut* tagSearchShortcut;

	QShortcut* outputExploreShortcut;
	QShortcut* outputConsoleShortcut;
	QShortcut* outputPropertiesShortcut;

	QShortcut* profileLoadShortcut;
	QShortcut* profileUpdateShortcut;

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

	CProfileItem currentProfileItem_;
	CGroupItem currentGroupItem_;

	CConfigManager* confManager_;

	QTagger tagger_;

	T_OutputItemList outputItemList_;

	QCompleter completer_;
	QStringListModel stringListModel_;

	QTextDocument textDocument_;
	QPlainTextDocumentLayout* textLayout_;

	QMap<QString, unsigned char> findReplaceFileList_; // unsigned char value not used

};
#endif // CMAINWINDOW_H


