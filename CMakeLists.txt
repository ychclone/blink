cmake_minimum_required(VERSION 3.1.0)

project(blink VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt6 COMPONENTS Widgets Xml Network REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS Resources/Forms)

include_directories(. Utils /opt/QScintilla_src-2.14.1/src)

link_directories (/opt/QScintilla_src-2.14.1/src)

add_executable(blink
    Resources/Forms/mainWindow.ui
    Resources/Forms/editor.ui
    Resources/Forms/projectDialog.ui
    Resources/Forms/aboutDialog.ui
    Resources/Forms/configDialog.ui
    Resources/Forms/findReplaceDialog.ui
    Resources/Forms/editorFindDialog.ui

    Resources/app.qrc
    Resources/Icons/appIcons.rc 

    main.cpp
    Utils/CUtils.cpp
    Display/CMainWindow.cpp
    Display/CEditor.cpp
    Display/CEditorFindDlg.cpp
    Display/CProjectDlg.cpp
    Display/CAboutDlg.cpp
    Display/CProjectListWidget.cpp
    Display/CFileListWidget.cpp
    Display/CConfigDlg.cpp
    Display/CEventFilterObj.cpp
    Display/CSearchTextBrowser.cpp
    Display/CSearchTextEdit.cpp
    Display/CFindReplaceDlg.cpp
    Model/qTagger/CTagItem.cpp
    Model/qTagger/CTagFileRecord.cpp
    Model/qTagger/CTagResultItem.cpp
    Model/qTagger/qTagger.cpp
    Model/qTagger/CSourceFileList.cpp
    Model/qFindReplacer/qFindReplacer.cpp
    Model/CProjectListModel.cpp
    Model/CFileListModel.cpp
    Model/CProjectManager.cpp
    Model/CProjectUpdateThread.cpp
    Model/CProjectLoadThread.cpp
    Model/CConfigManager.cpp
    Model/CProjectItem.cpp
    Model/CFileItem.cpp
    Model/CRunCommand.cpp
    Model/CFindReplaceModel.cpp
    Storage/CXmlStorageHandler.cpp

)

target_link_libraries(blink Qt6::Widgets Qt6::Xml Qt6::Network qscintilla2_qt6)

set(CMAKE_INSTALL_PREFIX /app/blink)

install(TARGETS blink
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )


