#include <QMessageBox>

#include <QDebug>
#include <QProcess>

#include <QMenu>

#ifdef Q_OS_WIN
	#include <qt_windows.h>
#endif

#include "CEditorFindDlg.h"

CEditorFindDlg::CEditorFindDlg(QWidget* parent)
: QDialog(parent)
{
	setupUi(this);

	// add status bar
	//statusbar = new QStatusBar(this);
	//statusBarLayout->addWidget(statusbar);

	createActions();
}

void CEditorFindDlg::setLineEditFocus()
{
	find_lineEdit->setFocus();
}

void CEditorFindDlg::createActions()
{
	connect(find_lineEdit, &QLineEdit::textChanged, this, &CEditorFindDlg::on_findButton_clicked);
	connect(replaceFind_lineEdit, &QLineEdit::textChanged, this, &CEditorFindDlg::on_replaceFindButton_clicked);
}

void CEditorFindDlg::on_findButton_clicked()
{
	emit findText(find_lineEdit->text(), matchWholeWord_checkBox->isChecked(), caseSensitive_checkBox->isChecked(), regularExpression_checkBox->isChecked());
}

void CEditorFindDlg::on_replaceFindButton_clicked()
{
	emit findText(replaceFind_lineEdit->text(), replaceMatchWholeWord_checkBox->isChecked(), replaceCaseSensitive_checkBox->isChecked(), replaceRegularExpression_checkBox->isChecked());
}

void CEditorFindDlg::on_replaceButton_clicked()
{
	emit replaceText(replaceFind_lineEdit->text(), replace_lineEdit->text(), replaceMatchWholeWord_checkBox->isChecked(), replaceCaseSensitive_checkBox->isChecked(), replaceRegularExpression_checkBox->isChecked());
}

void CEditorFindDlg::on_replaceAllButton_clicked()
{
	emit replaceAllText(replaceFind_lineEdit->text(), replace_lineEdit->text(), replaceMatchWholeWord_checkBox->isChecked(), replaceCaseSensitive_checkBox->isChecked(), replaceRegularExpression_checkBox->isChecked());
}

void CEditorFindDlg::on_replaceCancelButton_clicked()
{
    this->done(QDialog::Rejected);
}

void CEditorFindDlg::on_cancelButton_clicked()
{
    this->done(QDialog::Rejected);
}

