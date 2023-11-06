#ifndef CEDITOR_H
#define CEDITOR_H

#include <QMainWindow>
#include <QMap>
#include <QGridLayout>

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
	void cut();
	void copy();
	void paste();
	void undo();
	void redo();

signals:
    void statusLeft(const QString& status);
	void statusMiddle(const QString& status);
	void statusRight(const QString& status);
	
private slots:

private:
	void closeEvent(QCloseEvent *event);

	void saveWidgetPosition();

	void createActions(QsciScintilla* textEdit);

	void textEditModified();

	void setEditorFont(QsciLexer* lexer);

	void saveFile(const QString &fileName);	

	QString filePathInTab(int tabIndex);	

	void tabChanged(int tabIndex);
	void closeCurrentTab();
	void closeTab(int tabIndex);

	QMap<QString, EditorTab> editorTabMap_; // key: source filepath
	CMainWindow* parent_;

	int currentNewFileNumber_;
	
};

#endif // CEDITOR_H



