#include <QDir>
#include <QFileDialog>

#include <QDateTime>
#include <QMessageBox>

#include "CProfileDlg.h"

CProfileDlg::CProfileDlg(QWidget* parent)
: QDialog(parent)
{
    setupUi(this);

	currentProfileName_ = "";
}

CProfileDlg::CProfileDlg(const QString& profileName, const CProfileItem& profileItem, QWidget* parent)
: QDialog(parent)
{
    setupUi(this);

	currentProfileName_ = profileName; 

    profileName_lineEdit->setText(profileItem.m_name);
    srcDir_lineEdit->setText(profileItem.m_srcDir);
    srcFileMask_lineEdit->setText(profileItem.m_srcMask);
    headerfileMask_lineEdit->setText(profileItem.m_headerMask);
    labels_lineEdit->setText(profileItem.m_labels); 

    // connect slot only after when CProfileDlg has already been loaded and initial content filled in
    QObject::connect(profileName_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(profileContentChanged()));
    QObject::connect(srcDir_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(profileContentChanged()));
    QObject::connect(headerfileMask_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(profileContentChanged()));
    QObject::connect(srcFileMask_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(profileContentChanged()));
    QObject::connect(labels_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(profileContentChanged())); 

}

void CProfileDlg::on_okButton_clicked()
{
    on_applyButton_clicked();
    this->done(QDialog::Accepted);
}

void CProfileDlg::on_cancelButton_clicked()
{
    this->done(QDialog::Rejected);
}

void CProfileDlg::on_applyButton_clicked()
{
    CProfileItem modifiedItem;
	QDateTime currDateTime;

	// input error checking
	if (profileName_lineEdit->text() == "") {
		QMessageBox::warning(this, "Profile", "Profile name cannot be empty!", QMessageBox::Ok);
		return;
	}
	if (srcDir_lineEdit->text() == "") {
		QMessageBox::warning(this, "Profile", "Source directory cannot be empty!", QMessageBox::Ok);
		return;
	}
	if ((srcFileMask_lineEdit->text() == "") && (headerfileMask_lineEdit->text() == "")) {
		QMessageBox::warning(this, "Profile", "Source and header file mask cannot be both empty!", QMessageBox::Ok);
		return;
	}
	// labels can be empty so no checking
    
    modifiedItem.m_name = profileName_lineEdit->text();
    modifiedItem.m_srcDir = srcDir_lineEdit->text();
    modifiedItem.m_srcMask = srcFileMask_lineEdit->text();
    modifiedItem.m_headerMask = headerfileMask_lineEdit->text();

	currDateTime = QDateTime::currentDateTime();

	modifiedItem.m_profileCreateDateTime = currDateTime.toString("dd/MM/yyyy hh:mm:ss");

    modifiedItem.m_labels = labels_lineEdit->text();

    if (currentProfileName_ == "") { // new Profile
		CProfileManager::getInstance()->updateProfileItem(modifiedItem.m_name, modifiedItem); 
	} else {
		// use currentProfileName_ for updateProfileItem as name may have be changed
		CProfileManager::getInstance()->updateProfileItem(currentProfileName_, modifiedItem);
	}

	setWindowModified(false);
}

void CProfileDlg::on_srcDir_toolBn_clicked()
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

void CProfileDlg::profileContentChanged()
{
    applyButton->setEnabled(true);
	setWindowModified(true);
}


