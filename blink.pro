TEMPLATE    = app
#CONFIG += release

QT += widgets xml network
RC_FILE = Resources/Icons/appIcons.rc
RESOURCES   = Resources/app.qrc
#RESOURCES   += Resources/webview.qrc 

INCLUDEPATH += "Utils"

FORMS       = Resources/Forms/mainWindow.ui \
              Resources/Forms/profileDialog.ui \
              Resources/Forms/groupDialog.ui \ 
              Resources/Forms/aboutDialog.ui \
              Resources/Forms/configDialog.ui \
              Resources/Forms/findReplaceDialog.ui 

SOURCES     += main.cpp \
               Utils/CUtils.cpp \
               Display/CMainWindow.cpp \
               Display/CProfileDlg.cpp \
               Display/CGroupDlg.cpp \ 
               Display/CAboutDlg.cpp \
               Display/CProfileListWidget.cpp \
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
               Model/CProfileListModel.cpp \
               Model/CGroupListModel.cpp \ 
               Model/CFileListModel.cpp \
               Model/CProfileManager.cpp \
               Model/CProfileUpdateThread.cpp \
               Model/CProfileLoadThread.cpp \
               Model/CGroupLoadThread.cpp \
               Model/CConfigManager.cpp \
               Model/CGroupItem.cpp \ 
               Model/CProfileItem.cpp \
               Model/COutputItem.cpp \
               Model/CRunCommand.cpp \ 
               Model/CFindReplaceModel.cpp \
               Storage/CXmlStorageHandler.cpp \


HEADERS     += Utils/commonType.h \
               Utils/CUtils.h \
               Display/CMainWindow.h \
               Display/CProfileDlg.h \
               Display/CGroupDlg.h \ 
               Display/CAboutDlg.h \
               Display/CProfileListWidget.h \
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
               Model/CProfileListModel.h \ 
               Model/CGroupListModel.h \
               Model/CFileListModel.h \
               Model/CProfileManager.h \
               Model/CProfileUpdateThread.h \
               Model/CProfileLoadThread.h \
               Model/CGroupLoadThread.h \
               Model/CConfigManager.h \
               Model/CProfileItem.h \ 
               Model/CGroupItem.h \
               Model/COutputItem.h \
               Model/CRunCommand.h \
               Model/CFindReplaceModel.h \
               Storage/CXmlStorageHandler.h \
               Storage/IStorageHandler.h \




