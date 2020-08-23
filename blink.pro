TEMPLATE    = app
#CONFIG += release

QT += widgets xml network
RC_FILE = Resources/Icons/appIcons.rc
RESOURCES   = Resources/app.qrc
#RESOURCES   += Resources/webview.qrc 

INCLUDEPATH += "Utils"

FORMS       = Resources/Forms/mainWindow.ui \
              Resources/Forms/projectDialog.ui \
              Resources/Forms/groupDialog.ui \ 
              Resources/Forms/aboutDialog.ui \
              Resources/Forms/configDialog.ui \
              Resources/Forms/findReplaceDialog.ui 

SOURCES     += main.cpp \
               Utils/CUtils.cpp \
               Display/CMainWindow.cpp \
               Display/CProjectDlg.cpp \
               Display/CGroupDlg.cpp \ 
               Display/CAboutDlg.cpp \
               Display/CProjectListWidget.cpp \
               Display/CGroupListWidget.cpp \ 
               Display/CFileListWidget.cpp \
               Display/CConfigDlg.cpp \
               Display/CEventFilterObj.cpp \
               Display/CSearchTextBrowser.cpp \
               Display/CFindReplaceDlg.cpp \
               Model/qTagger/CTagResultItem.cpp \
               Model/qTagger/qTagger.cpp \
               Model/qTagger/CSourceFileList.cpp \ 
               Model/qFindReplacer/qFindReplacer.cpp \
               Model/CProjectListModel.cpp \
               Model/CGroupListModel.cpp \ 
               Model/CFileListModel.cpp \
               Model/CProjectManager.cpp \
               Model/CProjectUpdateThread.cpp \
               Model/CProjectLoadThread.cpp \
               Model/CGroupLoadThread.cpp \
               Model/CConfigManager.cpp \
               Model/CGroupItem.cpp \ 
               Model/CProjectItem.cpp \
               Model/COutputItem.cpp \
               Model/CRunCommand.cpp \ 
               Model/CFindReplaceModel.cpp \
               Storage/CXmlStorageHandler.cpp \


HEADERS     += Utils/commonType.h \
               Utils/CUtils.h \
               Display/CMainWindow.h \
               Display/CProjectDlg.h \
               Display/CGroupDlg.h \ 
               Display/CAboutDlg.h \
               Display/CProjectListWidget.h \
               Display/CGroupListWidget.h \
               Display/CFileListWidget.h \
               Display/CConfigDlg.h \
               Display/CEventFilterObj.h \  
               Display/CSearchTextBrowser.h \
               Display/CFindReplaceDlg.h \
               Model/qTagger/CTagResultItem.h \
               Model/qTagger/qTagger.h \ 
               Model/qTagger/CSourceFileList.h \
			   Model/qFindReplacer/qFindReplacer.h \
               Model/CProjectListModel.h \ 
               Model/CGroupListModel.h \
               Model/CFileListModel.h \
               Model/CProjectManager.h \
               Model/CProjectUpdateThread.h \
               Model/CProjectLoadThread.h \
               Model/CGroupLoadThread.h \
               Model/CConfigManager.h \
               Model/CProjectItem.h \ 
               Model/CGroupItem.h \
               Model/COutputItem.h \
               Model/CRunCommand.h \
               Model/CFindReplaceModel.h \
               Storage/CXmlStorageHandler.h \
               Storage/IStorageHandler.h \




