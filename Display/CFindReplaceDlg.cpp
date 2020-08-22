#include <QDir>
#include <QFileDialog>

#include <QFontDialog>
#include <QMessageBox>

#include <QDebug>
#include "CFindReplaceDlg.h"

CFindReplaceDlg::CFindReplaceDlg(QWidget* parent, CFindReplaceModel* findReplaceModel)
: QDialog(parent)
{
	setupUi(this);

	progressBar->setMinimum(0);
	progressBar->setMaximum(100);

	// connect slot only after when dialog has already been loaded and initial content filled in
	createActions();

	progressBar->hide();

	findReplaceModel_ = findReplaceModel;

	fileListView->setModel(findReplaceModel_->getFileListModel());

	// add status bar
	statusbar = new QStatusBar(this);
	statusBarLayout->addWidget(statusbar);

	QStringList fileList = findReplaceModel_->getFileList();
	statusbar->showMessage("Total " + QString::number(fileList.size()) + " files.");
}

void CFindReplaceDlg::createActions()
{
}

void CFindReplaceDlg::setFindLineEdit(QString findStr)
{
	find_lineEdit->setText(findStr);
}

void CFindReplaceDlg::on_replaceButton_clicked()
{
	QStringList fileList = findReplaceModel_->getFileList();

    int i = 0;
	int fileTotal = fileList.size();

	int replacedFileCount = 0;
    long totalMatchCount = 0;
	long matchCount = 0;

	// only letter or number or underscore for match whole word
	if (matchWholeWord_checkBox->isChecked()) {
		bool bLetterNumOrUnderscore = true;

		for (i = 0; i < find_lineEdit->text().size(); i++) {
			if (!(find_lineEdit->text().at(i).isLetterOrNumber() || find_lineEdit->text().at(i) == QChar('_'))) {
				bLetterNumOrUnderscore = false;
				break;
			}
		}

		if (!bLetterNumOrUnderscore) {
			QMessageBox::information(this, "File Replaces", "Only letter, number or underscore is supported for match whole word", QMessageBox::Ok);
		}
	}

	progressBar->show();
	for (i = 0; i < fileTotal; i++) {
		matchCount = findReplacer_.replaceInFile(find_lineEdit->text(), replace_lineEdit->text(), fileList[i], caseSensitive_checkBox->isChecked(), matchWholeWord_checkBox->isChecked(), regularExpression_checkBox->isChecked());
		if (matchCount > 0) {
			totalMatchCount += matchCount;
			replacedFileCount++;
		}

		progressBar->setValue(static_cast<int> (static_cast<float> (i + 1) / fileTotal * 100));
	}

	statusbar->showMessage("Replaced " + QString::number(totalMatchCount) + " times, " + QString::number(replacedFileCount) + " files.");
}

void CFindReplaceDlg::on_cancelButton_clicked()
{
    this->done(QDialog::Rejected);
}

void CFindReplaceDlg::on_selectAllButton_clicked()
{
	findReplaceModel_->selectAllFiles();

	QStringList fileList = findReplaceModel_->getFileList();
	statusbar->showMessage("Total " + QString::number(fileList.size()) + " files selected.");
}

void CFindReplaceDlg::on_clearAllButton_clicked()
{
	findReplaceModel_->clearSelectAllFiles();

	QStringList fileList = findReplaceModel_->getFileList();
	statusbar->showMessage("Total " + QString::number(fileList.size()) + " files selected.");
}


