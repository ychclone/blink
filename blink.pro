TEMPLATE    = app
#CONFIG += release

CONFIG      += qscintilla2

#QMAKE_CXXFLAGS += -g -fno-omit-frame-pointer

QT += widgets xml network
RC_FILE = Resources/Icons/appIcons.rc
RESOURCES   = Resources/app.qrc
#RESOURCES   += Resources/webview.qrc 

INCLUDEPATH += "Utils"

#LIBS += -LC:/QScintilla_src-2.14.1/src/release -llibqscintilla2_qt6
#LIBS += -LC:/QScintilla_src-2.14.1/src/debug -llibqscintilla2_qt6d

unix {
LIBS += -L/opt/QScintilla_src-2.14.1/src -lqscintilla2_qt6
}
win32 {
LIBS += -LC:/QScintilla_src-2.14.1/src/release -llibqscintilla2_qt6
}

FORMS       = Resources/Forms/mainWindow.ui \
              Resources/Forms/editor.ui \
              Resources/Forms/projectDialog.ui \
              Resources/Forms/aboutDialog.ui \
              Resources/Forms/configDialog.ui \
              Resources/Forms/findReplaceDialog.ui \
              Resources/Forms/editorFindDialog.ui

SOURCES     += main.cpp \
               Utils/CUtils.cpp \
               Display/CMainWindow.cpp \
               Display/CEditor.cpp \
               Display/CEditorFindDlg.cpp \
               Display/CProjectDlg.cpp \
               Display/CAboutDlg.cpp \
               Display/CProjectListWidget.cpp \
               Display/CFileListWidget.cpp \
               Display/CConfigDlg.cpp \
               Display/CEventFilterObj.cpp \
               Display/CSearchTextBrowser.cpp \
               Display/CSearchTextEdit.cpp \
               Display/CFindReplaceDlg.cpp \
               Model/qTagger/CTagItem.cpp \
               Model/qTagger/CTagFileRecord.cpp \
               Model/qTagger/CTagResultItem.cpp \
               Model/qTagger/qTagger.cpp \
               Model/qTagger/CSourceFileList.cpp \
               Model/qFindReplacer/qFindReplacer.cpp \
               Model/CProjectListModel.cpp \
               Model/CFileListModel.cpp \
               Model/CProjectManager.cpp \
               Model/CProjectUpdateThread.cpp \
               Model/CProjectLoadThread.cpp \
               Model/CConfigManager.cpp \
               Model/CProjectItem.cpp \
               Model/CFileItem.cpp \
               Model/CRunCommand.cpp \ 
               Model/CFindReplaceModel.cpp \
               Storage/CXmlStorageHandler.cpp \


HEADERS     += Utils/commonType.h \
                Utils/CUtils.h \
                Display/CMainWindow.h \
                Display/CEditor.h \
                Display/CEditorFindDlg.h \
                Display/CProjectDlg.h \
                Display/CAboutDlg.h \
                Display/CProjectListWidget.h \
                Display/CFileListWidget.h \
                Display/CConfigDlg.h \
                Display/CEventFilterObj.h \
                Display/CSearchTextBrowser.h \
                Display/CSearchTextEdit.h \ 
                Display/CFindReplaceDlg.h \
                Model/qTagger/CTagItem.h \ 
                Model/qTagger/CTagFileRecord.h \
                Model/qTagger/CTagResultItem.h \
                Model/qTagger/qTagger.h \ 
                Model/qTagger/CSourceFileList.h \
                Model/qFindReplacer/qFindReplacer.h \
                Model/CProjectListModel.h \ 
                Model/CFileListModel.h \
                Model/CProjectManager.h \
                Model/CProjectUpdateThread.h \
                Model/CProjectLoadThread.h \
                Model/CConfigManager.h \
                Model/CProjectItem.h \ 
                Model/CFileItem.h \
                Model/CRunCommand.h \
                Model/CFindReplaceModel.h \
                Storage/CXmlStorageHandler.h \
                Storage/IStorageHandler.h \




