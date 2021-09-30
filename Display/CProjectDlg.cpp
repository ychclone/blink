#include <QDir>
#include <QFileDialog>

#include <QDateTime>
#include <QMessageBox>

#include "CProjectDlg.h"

CProjectDlg::CProjectDlg(QWidget* parent)
: QDialog(parent)
{
    setupUi(this);

	currentProjectName_ = "";
}

CProjectDlg::CProjectDlg(const QString& projectName, const CProjectItem& projectItem, QWidget* parent)
: QDialog(parent)
{
    setupUi(this);

	currentProjectName_ = projectName;

    projectName_lineEdit->setText(projectItem.name_);
    srcDir_lineEdit->setText(projectItem.srcDir_);
    srcFileMask_lineEdit->setText(projectItem.srcMask_);
    headerfileMask_lineEdit->setText(projectItem.headerMask_);
    labels_lineEdit->setText(projectItem.labels_);

    // connect slot only after when CProjectDlg has already been loaded and initial content filled in
    QObject::connect(projectName_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectContentChanged()));
    QObject::connect(srcDir_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectContentChanged()));
    QObject::connect(headerfileMask_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectContentChanged()));
    QObject::connect(srcFileMask_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectContentChanged()));
    QObject::connect(labels_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectContentChanged()));

}

void CProjectDlg::on_okButton_clicked()
{
    on_applyButton_clicked();
    this->done(QDialog::Accepted);
}

void CProjectDlg::on_cancelButton_clicked()
{
    this->done(QDialog::Rejected);
}

void CProjectDlg::on_applyButton_clicked()
{
    CProjectItem modifiedItem;
	QDateTime currDateTime;

	// input error checking
	if (projectName_lineEdit->text() == "") {
		QMessageBox::warning(this, "Project", "Project name cannot be empty!", QMessageBox::Ok);
		return;
	}
	if (srcDir_lineEdit->text() == "") {
		QMessageBox::warning(this, "Project", "Source directory cannot be empty!", QMessageBox::Ok);
		return;
	}
	if ((srcFileMask_lineEdit->text() == "") && (headerfileMask_lineEdit->text() == "")) {
		QMessageBox::warning(this, "Project", "Source and header file mask cannot be both empty!", QMessageBox::Ok);
		return;
	}
	// labels can be empty so no checking

    modifiedItem.name_ = projectName_lineEdit->text();
    modifiedItem.srcDir_ = srcDir_lineEdit->text();
    modifiedItem.srcMask_ = srcFileMask_lineEdit->text();
    modifiedItem.headerMask_ = headerfileMask_lineEdit->text();

	currDateTime = QDateTime::currentDateTime();

	modifiedItem.projectCreateDateTime_ = currDateTime.toString("dd/MM/yyyy hh:mm:ss");

    modifiedItem.labels_ = labels_lineEdit->text();

    qDebug() << "currentProjectName_ in on_applyButton_clicked() = " << currentProjectName_;

    CProjectItem projectItem = CProjectManager::getInstance()->getProjectItem(currentProjectName_);
    qDebug() << "projectItem.tagUpdateDateTime_ = " << projectItem.tagUpdateDateTime_;

    if (projectItem.tagUpdateDateTime_ == "") { // new project
        if (currentProjectName_ == "") { // no project loaded yet
            CProjectManager::getInstance()->updateProjectItem(true, modifiedItem.name_, modifiedItem);
        } else {
            // use currentProjectName_ for updateProjectItem as name may have be changed
            CProjectManager::getInstance()->updateProjectItem(true, currentProjectName_, modifiedItem);
        }
    } else { // old project which has updated tag time
        if (currentProjectName_ == "") { // no project loaded yet
            CProjectManager::getInstance()->updateProjectItem(false, modifiedItem.name_, modifiedItem);
        } else {
            // use currentProjectName_ for updateProjectItem as name may have be changed
            CProjectManager::getInstance()->updateProjectItem(false, currentProjectName_, modifiedItem);
        }
    }

	setWindowModified(false);
}

void CProjectDlg::on_srcDir_toolBn_clicked()
{
    QString sourceDir;

    if (srcDir_lineEdit->text() != "") {
		sourceDir = srcDir_lineEdit->text();
    } else {
        sourceDir = QDir::currentPath();
    }

    QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Source directory"), sourceDir);
    if (directory != "") {
    	srcDir_lineEdit->setText(directory);
    }
}

void CProjectDlg::projectContentChanged()
{
    applyButton->setEnabled(true);
	setWindowModified(true);
}


