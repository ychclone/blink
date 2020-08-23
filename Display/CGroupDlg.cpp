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

	CProjectManager::getInstance()->getGroupMap(groupMap);   

	foreach (const CGroupItem& groupItem, groupMap) { 
		allGroup_comboBox->addItem(groupItem.name_, QVariant(Qt::DisplayRole));
	}

	// default as empty for group combox box
	allGroup_comboBox->setCurrentIndex(-1); 

	// load all project
	
	QMap<QString, CProjectItem> projectMap;

	CProjectManager::getInstance()->getProjectMap(projectMap);

	foreach (const CProjectItem& projectItem, projectMap) { 
		allProject_listWidget->addItem(projectItem.name_);
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
	
	// load project for current group
	loadGroupProjectListView(groupName);
	
	createActions();
}

void CGroupDlg::createActions()
{
    connect(addProject_toolButton, SIGNAL(clicked()), this, SLOT(on_addProjectToolBn_clicked())); 
	connect(removeProject_toolButton, SIGNAL(clicked()), this, SLOT(on_removeProjectToolBn_clicked()));

	connect(allGroup_comboBox, SIGNAL(activated(const QString&)), this, SLOT(on_groupComboBox_activated(const QString&))); 
}

void CGroupDlg::loadGroupProjectListView(const QString& groupName)
{	
	CGroupItem groupItem; 
	QStringList groupProjectList;

	// remove previous item if any
	includedProject_listWidget->clear(); 
	allProject_listWidget->clear();

	// selected project list
	groupItem = CProjectManager::getInstance()->getGroupItem(groupName);

	groupProjectList = groupItem.projectList_.split(CProjectManager::kGROUP_PROFILE_SEPERATOR, QString::SkipEmptyParts);
	includedProject_listWidget->addItems(groupProjectList); 

	// not selected project list
	QMap<QString, CProjectItem> projectMap;

	CProjectManager::getInstance()->getProjectMap(projectMap);

	foreach (const CProjectItem& projectItem, projectMap) { 
		if (!groupProjectList.contains(projectItem.name_)) {
			allProject_listWidget->addItem(projectItem.name_);
		}
	} 
}

void CGroupDlg::on_addProjectToolBn_clicked()
{
	QList<QListWidgetItem* > selecedItemList = allProject_listWidget->selectedItems();

	foreach (QListWidgetItem* listWidgetItem, selecedItemList) {
		includedProject_listWidget->addItem(listWidgetItem->text());  
	}

	qDeleteAll(selecedItemList); 
}

void CGroupDlg::on_removeProjectToolBn_clicked()
{
	QList<QListWidgetItem *> selecedItemList = includedProject_listWidget->selectedItems();

	QListWidgetItem* listWidgetItem;

	foreach (listWidgetItem, selecedItemList) {
		allProject_listWidget->addItem(listWidgetItem->text());  
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
		includedProject_listWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
	
	QStringList updateGroupProjectList;

	foreach (QListWidgetItem* listWidgetItem, includedItemList) {
		updateGroupProjectList << listWidgetItem->text();
	}

	// update project list in item
	modifiedItem.projectList_ = updateGroupProjectList.join(CProjectManager::kGROUP_PROFILE_SEPERATOR);

    if (currentGroupName_ == "") { // new Group
        CProjectManager::getInstance()->updateGroupItem(modifiedItem.name_, modifiedItem); 
	} else {
		// use currentGroupName_ for updateProjectItem as name may have be changed 
		CProjectManager::getInstance()->updateGroupItem(currentGroupName_, modifiedItem);
	}

	setWindowModified(false);
}

void CGroupDlg::on_groupComboBox_activated(const QString& comboText)
{
	groupName_lineEdit->setText(comboText);

	// load project for current group
	loadGroupProjectListView(comboText);
}


void CGroupDlg::groupContentChanged()
{
    applyButton->setEnabled(true);
	setWindowModified(true);
}


