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

QFont CConfigDlg::getProjectDefaultFont()
{
	return projectDefaultFont_;
}

QFont CConfigDlg::getSymbolDefaultFont()
{
	return symbolDefaultFont_;
}

void CConfigDlg::loadSetting()
{
	CConfigManager* confManager;

	confManager	= CConfigManager::getInstance();

	defaultEditor_lineEdit->setText(confManager->getAppSettingValue("DefaultEditor").toString());
	tagDir_lineEdit->setText(confManager->getAppSettingValue("TagDir").toString());
    tmpDir_lineEdit->setText(confManager->getAppSettingValue("TmpDir").toString());

	timeoutRunExtProgram_lineEdit->setText(confManager->getAppSettingValue("TimeoutRunExtProgram").toString());

	// Project font
	QString projectFontStr = confManager->getAppSettingValue("ProjectFont").toString();
	if (projectFontStr != "") { // load from setting
		projectDefaultFont_.fromString(projectFontStr);
	} else {
		projectDefaultFont_ = QApplication::font(); // using application font as default font
	}

	// update project font in setting display
	QString projectFontTextToDisplay = projectDefaultFont_.family() + ", " + QString::number(projectDefaultFont_.pointSize());
	projectFont_lineEdit->setText(projectFontTextToDisplay);

	// Symbol font
	QString symbolFontStr = confManager->getAppSettingValue("SymbolFont").toString();
	if (symbolFontStr != "") { // load from setting
		symbolDefaultFont_.fromString(symbolFontStr);
	} else {
		symbolDefaultFont_ = QApplication::font(); // using application font as default font
	}

	// update symbol font in setting display
	QString symbolFontTextToDisplay = symbolDefaultFont_.family() + ", " + QString::number(symbolDefaultFont_.pointSize());
	symbolFont_lineEdit->setText(symbolFontTextToDisplay);
}

void CConfigDlg::saveSetting()
{
	CConfigManager* confManager;

	confManager	= CConfigManager::getInstance();

	confManager->setAppSettingValue("DefaultEditor", defaultEditor_lineEdit->text());
	confManager->setAppSettingValue("TagDir", tagDir_lineEdit->text());
    confManager->setAppSettingValue("TmpDir", tmpDir_lineEdit->text());

	confManager->setAppSettingValue("TimeoutRunExtProgram", timeoutRunExtProgram_lineEdit->text());

	confManager->setAppSettingValue("ProjectFont", projectDefaultFont_.toString());
	confManager->setAppSettingValue("SymbolFont", symbolDefaultFont_.toString());
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
	QObject::connect(tmpDir_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged()));
	QObject::connect(timeoutRunExtProgram_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged()));
	QObject::connect(projectFont_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged()));
	QObject::connect(symbolFont_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(configContentChanged()));
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

#ifdef Q_OS_WIN
	QString programPath = QFileDialog::getOpenFileName(this,
			tr("Default Editor"), editorPathname, tr("Program (*.exe)"));
	if (programPath != "") {
		defaultEditor_lineEdit->setText(programPath);
	}
#else
	QFileDialog fileDialog(this, tr("Default editor"));
	fileDialog.setFilter(QDir::Executable);

	QString programPath =  fileDialog.getOpenFileName(this, tr("Default editor"), editorPathname);

	if (programPath != "") {
		defaultEditor_lineEdit->setText(programPath);
	}
#endif
}

void CConfigDlg::on_projectFont_toolBn_clicked()
{
	bool bOkClicked;
	QFont selectedFont = QFontDialog::getFont(
						&bOkClicked, projectDefaultFont_, this);

	if (bOkClicked) {
		projectDefaultFont_ = selectedFont;

		// update project font setting display
		QString fontTextToDisplay = projectDefaultFont_.family() + ", " + QString::number(projectDefaultFont_.pointSize());
		projectFont_lineEdit->setText(fontTextToDisplay);
	}
}

void CConfigDlg::on_symbolFont_toolBn_clicked()
{
	bool bOkClicked;
	QFont selectedFont = QFontDialog::getFont(
						&bOkClicked, symbolDefaultFont_, this);

	if (bOkClicked) {
		symbolDefaultFont_ = selectedFont;

		// update symbol font setting display
		QString fontTextToDisplay = symbolDefaultFont_.family() + ", " + QString::number(symbolDefaultFont_.pointSize());
		symbolFont_lineEdit->setText(fontTextToDisplay);
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




