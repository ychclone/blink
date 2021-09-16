#ifndef CEDITOR_H
#define CEDITOR_H

#include <QMainWindow>
#include <QMap>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>

#include "ui_editor.h"

#include "CEditorFindDlg.h"

class QsciScintilla;

struct EditorTab
{
	QsciScintilla* textEdit;
    QsciLexer* lexer;
};

class CEditor : public QMainWindow, private Ui::editor
{
	Q_OBJECT

public:
    CEditor(QWidget* parent = 0);

	virtual ~CEditor() {};
	void loadFile(const QString& filePath);

private slots:

private:
	void closeEvent(QCloseEvent *event);

	void saveWidgetPosition();

	void createActions(QsciScintilla* textEdit);

	void textEditModified();
	void setEditorFont(QsciLexer* lexer);

	void save();
	void saveAs();
	void saveFile(const QString &fileName);
	void showFindDialog();
	void findText(const QString& text, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);

	QString filePathInTab(int tabIndex);

	void openFile();

	void tabChanged(int tabIndex);
	void closeCurrentTab();
	void closeTab(int tabIndex);

	QMap<QString, EditorTab> editorTabMap_; // key: source filepath

	CEditorFindDlg findDlg_;
};

#endif // CEDITOR_H



