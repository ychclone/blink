#include <QDir>
#include <QFileDialog>

#include <QDateTime>
#include <QMessageBox> 

#include "CGroupDlg.h"

CGroupDlg::CGroupDlg(QWidget* parent)
: QDialog(parent)
{
    setupUi(this);

	currentGroupName_ = "";

	// load group list
	QMap<QString, CGroupItem> groupMap; 

	CProfileManager::getInstance()->getGroupMap(groupMap);   

	foreach (const CGroupItem& groupItem, groupMap) { 
		allGroup_comboBox->addItem(groupItem.name_, QVariant(Qt::DisplayRole));
	}

	// default as empty for group combox box
	allGroup_comboBox->setCurrentIndex(-1); 

	// load all profile
	
	QMap<QString, CProfileItem> profileMap;

	CProfileManager::getInstance()->getProfileMap(profileMap);

	foreach (const CProfileItem& profileItem, profileMap) { 
		allProfile_listWidget->addItem(profileItem.name_);
	}

	createActions(); 
}

CGroupDlg::CGroupDlg(const QString& groupName, const CGroupItem& groupItem, QWidget* parent)
: QDialog(parent)
{
    setupUi(this);

	currentGroupName_ = groupName;

    groupName_lineEdit->setText(groupItem.name_);
	labels_lineEdit->setText(groupItem.labels_);  

    // connect slot only after when CGroupDlg has already been loaded and initial content filled in
    QObject::connect(groupName_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(groupContentChanged()));
	QObject::connect(labels_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(groupContentChanged()));  

	// fix group name comboBox
	allGroup_comboBox->addItem(groupName, QVariant(Qt::DisplayRole)); 
    allGroup_comboBox->setEnabled(false);

    groupName_lineEdit->setText(groupName);
	
	// load profile for current group
	loadGroupProjectListView(groupName);
	
	createActions();
}

void CGroupDlg::createActions()
{
    connect(addProfile_toolButton, SIGNAL(clicked()), this, SLOT(on_addProfileToolBn_clicked())); 
	connect(removeProfile_toolButton, SIGNAL(clicked()), this, SLOT(on_removeProfileToolBn_clicked()));

	connect(allGroup_comboBox, SIGNAL(activated(const QString&)), this, SLOT(on_groupComboBox_activated(const QString&))); 
}

void CGroupDlg::loadGroupProjectListView(const QString& groupName)
{	
	CGroupItem groupItem; 
	QStringList groupProjectList;

	// remove previous item if any
	includedProfile_listWidget->clear(); 
	allProfile_listWidget->clear();

	// selected profile list
	groupItem = CProfileManager::getInstance()->getGroupItem(groupName);

	groupProjectList = groupItem.projectList_.split(CProfileManager::kGROUP_PROFILE_SEPERATOR, QString::SkipEmptyParts);
	includedProfile_listWidget->addItems(groupProjectList); 

	// not selected profile list
	QMap<QString, CProfileItem> profileMap;

	CProfileManager::getInstance()->getProfileMap(profileMap);

	foreach (const CProfileItem& profileItem, profileMap) { 
		if (!groupProjectList.contains(profileItem.name_)) {
			allProfile_listWidget->addItem(profileItem.name_);
		}
	} 
}

void CGroupDlg::on_addProfileToolBn_clicked()
{
	QList<QListWidgetItem* > selecedItemList = allProfile_listWidget->selectedItems();

	foreach (QListWidgetItem* listWidgetItem, selecedItemList) {
		includedProfile_listWidget->addItem(listWidgetItem->text());  
	}

	qDeleteAll(selecedItemList); 
}

void CGroupDlg::on_removeProfileToolBn_clicked()
{
	QList<QListWidgetItem *> selecedItemList = includedProfile_listWidget->selectedItems();

	QListWidgetItem* listWidgetItem;

	foreach (listWidgetItem, selecedItemList) {
		allProfile_listWidget->addItem(listWidgetItem->text());  
	}

	qDeleteAll(selecedItemList);
}

void CGroupDlg::on_okButton_clicked()
{
    on_applyButton_clicked();
    this->done(QDialog::Accepted);
}

void CGroupDlg::on_cancelButton_clicked()
{
    this->done(QDialog::Rejected);
}

void CGroupDlg::on_applyButton_clicked()
{
    CGroupItem modifiedItem;
	QDateTime currDateTime;

	// input error checking
	if (groupName_lineEdit->text() == "") {
		QMessageBox::warning(this, "Group", "Group name cannot be empty!", QMessageBox::Ok);
		return;
	}
	// labels can be empty so no checking 
    
    modifiedItem.name_ = groupName_lineEdit->text();

	currDateTime = QDateTime::currentDateTime();

	modifiedItem.groupCreateDateTime_ = currDateTime.toString("dd/MM/yyyy hh:mm:ss");

	modifiedItem.labels_ = labels_lineEdit->text(); 

	// get included item list
	QList<QListWidgetItem* > includedItemList = 
		includedProfile_listWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
	
	QStringList updateGroupProjectList;

	foreach (QListWidgetItem* listWidgetItem, includedItemList) {
		updateGroupProjectList << listWidgetItem->text();
	}

	// update profile list in item
	modifiedItem.projectList_ = updateGroupProjectList.join(CProfileManager::kGROUP_PROFILE_SEPERATOR);

    if (currentGroupName_ == "") { // new Group
        CProfileManager::getInstance()->updateGroupItem(modifiedItem.name_, modifiedItem); 
	} else {
		// use currentGroupName_ for updateProfileItem as name may have be changed 
		CProfileManager::getInstance()->updateGroupItem(currentGroupName_, modifiedItem);
	}

	setWindowModified(false);
}

void CGroupDlg::on_groupComboBox_activated(const QString& comboText)
{
	groupName_lineEdit->setText(comboText);

	// load profile for current group
	loadGroupProjectListView(comboText);
}


void CGroupDlg::groupContentChanged()
{
    applyButton->setEnabled(true);
	setWindowModified(true);
}


