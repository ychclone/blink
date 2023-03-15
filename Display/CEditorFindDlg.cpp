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
	statusbar = new QStatusBar(this);
	statusBarLayout->addWidget(statusbar);

	createActions();
}

void CEditorFindDlg::setLineEditFocus()
{
	find_lineEdit->setFocus();
}

void CEditorFindDlg::createActions()
{
	connect(find_lineEdit, &QLineEdit::textChanged, this, &CEditorFindDlg::on_findButton_clicked);
}

void CEditorFindDlg::on_findButton_clicked()
{
	emit findText(find_lineEdit->text(), matchWholeWord_checkBox->isChecked(), caseSensitive_checkBox->isChecked(), regularExpression_checkBox->isChecked());
}

void CEditorFindDlg::on_cancelButton_clicked()
{
    this->done(QDialog::Rejected);
}

