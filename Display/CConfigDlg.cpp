#include <QDir>
#include <QFileDialog> 

#include <QFontDialog>

#include "CConfigDlg.h"
#include "Model/CConfigManager.h"

CConfigDlg::CConfigDlg(QWidget* parent)
: QDialog(parent)
{
	setupUi(this);
	
	loadSetting();

	// connect slot only after when dialog has already been loaded and initial content filled in 
	createActions();
}

QFont CConfigDlg::getProfileDefaultFont()
{
	return profileDefaultFont_;
}

void CConfigDlg::loadSetting()
{
	CConfigManager* confManager;
	
	confManager	= CConfigManager::getInstance();
	
	defaultEditor_lineEdit->setText(confManager->getAppSettingValue("DefaultEditor").toString()); 
	tagDir_lineEdit->setText(confManager->getAppSettingValue("TagDir").toString());
	srcSubstDrv_lineEdit->setText(confManager->getAppSettingValue("SrcSubstDrv").toString()); 
	tagSubstDrv_lineEdit->setText(confManager->getAppSettingValue("TagSubstDrv").toString()); 
    tmpDir_lineEdit->setText(confManager->getAppSettingValue("TmpDir").toString());

	timeoutRunExtProgram_lineEdit->setText(confManager->getAppSettingValue("TimeoutRunExtProgram").toString());  

	QString profileFontStr = confManager->getAppSettingValue("ProfileFont").toString();
	if (profileFontStr != "") { // load from setting
		profileDefaultFont_.fromString(profileFontStr);
	} else {
		profileDefaultFont_ = QApplication::font(); // using application font as default font
	}

	// update profile font setting display
	QString fontTextToDisplay = profileDefaultFont_.family() + ", " + QString::number(profileDefaultFont_.pointSize());
	profileFont_lineEdit->setText(fontTextToDisplay);
}

void CConfigDlg::saveSetting()
{
	CConfigManager* confManager;
	
	confManager	= CConfigManager::getInstance();
	
	confManager->setAppSettingValue("DefaultEditor", defaultEditor_lineEdit->text()); 
	confManager->setAppSettingValue("TagDir", tagDir_lineEdit->text());
	confManager->setAppSettingValue("SrcSubstDrv", srcSubstDrv_lineEdit->text());
	confManager->setAppSettingValue("TagSubstDrv", tagSubstDrv_lineEdit->text());
    confManager->setAppSettingValue("TmpDir", tmpDir_lineEdit->text());

	confManager->setAppSettingValue("TimeoutRunExtProgram", timeoutRunExtProgram_lineEdit->text());

	confManager->setAppSettingValue("ProfileFont", profileDefaultFont_.toString()); 
}

void CConfigDlg::createActions()
{
    // for different pages
	connect(configListWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));

    // when setting change
	QObject::connect(defaultEditor_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged())); 
	QObject::connect(tagDir_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged()));
	QObject::connect(srcSubstDrv_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged())); 
	QObject::connect(tagSubstDrv_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged())); 
	QObject::connect(tmpDir_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged())); 
	QObject::connect(timeoutRunExtProgram_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged()));
	QObject::connect(profileFont_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged()));  
}

void CConfigDlg::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current) {
        current = previous;
	}

    configStackedWidget->setCurrentIndex(configListWidget->row(current));
}

void CConfigDlg::on_okButton_clicked()
{
    on_applyButton_clicked();
    this->done(QDialog::Accepted);
}

void CConfigDlg::on_cancelButton_clicked()
{
    this->done(QDialog::Rejected);
}

void CConfigDlg::on_applyButton_clicked()
{                   
    saveSetting();
	setWindowModified(false);
}

void CConfigDlg::configContentChanged()
{
    applyButton->setEnabled(true);
	setWindowModified(true);
}

void CConfigDlg::on_defaultEditor_toolBn_clicked()
{
	QString editorPathname;

	if (defaultEditor_lineEdit->text() != "") {
		editorPathname = defaultEditor_lineEdit->text();
	} else {
		editorPathname = QDir::currentPath();
	}

	QString programPath = QFileDialog::getOpenFileName(this,
			tr("Default Editor"), editorPathname, tr("Program (*.exe)"));
	if (programPath != "") {
		defaultEditor_lineEdit->setText(programPath);
	}
} 

void CConfigDlg::on_profileFont_toolBn_clicked()
{
	bool bOkClicked;
	QFont selectedFont = QFontDialog::getFont(
						&bOkClicked, profileDefaultFont_, this);

	if (bOkClicked) {
		profileDefaultFont_ = selectedFont;

		// update profile font setting display
		QString fontTextToDisplay = profileDefaultFont_.family() + ", " + QString::number(profileDefaultFont_.pointSize());
		profileFont_lineEdit->setText(fontTextToDisplay);
	}

}

void CConfigDlg::on_tagDir_toolBn_clicked()
{
	QString tagDir;

	if (tagDir_lineEdit->text() != "") {
		tagDir = tagDir_lineEdit->text();
	} else {
		tagDir = QDir::currentPath();
	}

	QString directory = QFileDialog::getExistingDirectory(this,
			tr("Tag directory"), tagDir);
	if (directory != "") {
		tagDir_lineEdit->setText(directory);
	}
}

void CConfigDlg::on_tmpDir_toolBn_clicked() 
{
	QString tmpDir;

	if (tmpDir_lineEdit->text() != "") {
		tmpDir = tmpDir_lineEdit->text();
	} else {
		tmpDir = QDir::currentPath();
	}

	QString directory = QFileDialog::getExistingDirectory(this,
			tr("Temp directory"), tmpDir);
	if (directory != "") {
		tmpDir_lineEdit->setText(directory);
	} 
}




