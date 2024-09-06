#ifndef CEDITOR_H
#define CEDITOR_H

#include <QMainWindow>
#include <QMap>
#include <QGridLayout>
#include <QSet>
#include <QFileSystemWatcher>
#include <QMessageBox>

#include "ui_editor.h"

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>

class QsciScintilla;
class CMainWindow;

struct EditorTab
{
	QsciScintilla* textEdit;
    QsciLexer* lexer;
};

class CEditor : public QWidget, private Ui::editor
{
	Q_OBJECT

public:
    CEditor(CMainWindow* parent);

	virtual ~CEditor() {};
	
	void newFile();
	void closeFile();
	void save();
	void saveAs();
	void openFile();
	
	void loadFileWithLineNum(const QString& filePath, int lineNumber);
	void loadFileWithLineNumNewTab(const QString& filePath, int lineNumber);
	
	QsciLexer* getLexer(const QString& fileName);
	void setTextEdit(QsciScintilla* textEdit);
	
	int loadFile(const QString& filePath);
	void loadFileNewTab(const QString& filePath);

	void updateAllEditorFont();
	
	void findText(const QString& text, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);
	void replaceText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);
	void replaceAllText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);
	
	void goToLine(int line);
	void textEditGoToLine(QsciScintilla *textEdit, int line);
	void cut();
	void copy();
	void paste();
	void undo();
	void redo();

	void resetNavHistory();

signals:
    void statusLeft(const QString& status);
	void statusMiddle(const QString& status);
	void statusRight(const QString& status);
	
private slots:
	void closeAllTabsButCurrent(const QPoint &pos);
	void closeAllTabsToLeft(const QPoint &pos);
    void closeAllTabsToRight(const QPoint &pos);
	void copyFilePath(const QPoint &pos);
	void copyFileName(const QPoint &pos);
	void copyFile(const QPoint &pos);

private:
	void closeEvent(QCloseEvent *event);

	void saveWidgetPosition();

	void createActions(QsciScintilla* textEdit);

	void textEditModified();
	void handleCursorPositionChanged(int line, int index);

	void setEditorFont(QsciLexer* lexer);

	void saveFile(const QString &fileName);	

	QString filePathInTab(int tabIndex);	

	void tabChanged(int tabIndex);
	void closeCurrentTab(const QPoint &pos);
	void closeTab(int tabIndex);

	void tabContextMenuEvent(const QPoint &pos);

	void setupFileWatcher(const QString& filePath);

	void beginFileModification(const QString& filePath);
    void endFileModification(const QString& filePath);

	void reloadFile(const QString& filePath);
	void onEndModificationTimeout();
	void onEndNavigationTimeout();

	void fileChanged(const QString& path);

	QMap<QString, EditorTab> editorTabMap_; // key: source filepath
	CMainWindow* parent_;

	int currentNewFileNumber_;

	QFileSystemWatcher fileWatcher_;
	QSet<QString> filesBeingModified_;

	QTimer* endModificationTimer_;
	QTimer* endNavigationTimer_;
	
	QVector<QPair<QString, int>> navHistory_;
	int currentHistoryIndex_;

	bool isNavigatingHistory_;
	QSet<QString> filesAlreadyPrompted_;
	
	void addToNavHistory(const QString& filePath, int line);

public slots:
	void goForward();
	void goBackward();

signals:
	void updateGoForwardBackwardActions(bool canGoForward, bool canGoBackward);
};

#endif // CEDITOR_H



