#ifndef CEDITOR_FIND_DLG_H
#define CEDITOR_FIND_DLG_H

#include <QDialog>
#include <QStatusBar>

#include "ui_editorFindDialog.h"

class CEditorFindDlg : public QDialog, private Ui::editorFindDialog
{
	Q_OBJECT

public:
    CEditorFindDlg(QWidget* parent);
	virtual ~CEditorFindDlg() {};
	
	void setLineEditFocus();

signals:
    void findText(const QString& text, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);
	void replaceText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);
	void replaceAllText(const QString& findText, const QString& replaceText, bool bMatchWholeWord, bool bCaseSensitive, bool bRegularExpression);

private slots:
	void on_findButton_clicked();
	void on_cancelButton_clicked();
	void on_replaceFindButton_clicked();
	void on_replaceButton_clicked();
	void on_replaceAllButton_clicked();

	void on_replaceCancelButton_clicked();

private:
	void createActions();

	QStatusBar *statusbar;

};

#endif // CEDITOR_FIND_DLG_H

