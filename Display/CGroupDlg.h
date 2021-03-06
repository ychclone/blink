#ifndef CGROUPDLG_H
#define CGROUPDLG_H

#include <QDialog> 
#include "Model/CGroupItem.h"
#include "Model/CProjectManager.h"
#include "ui_groupDialog.h"

class CGroupDlg : public QDialog, private Ui::groupDialog
{
    Q_OBJECT

public:
    CGroupDlg(QWidget* parent = 0);
    CGroupDlg(const QString& groupName, const CGroupItem& groupItem, QWidget* parent = 0);

    virtual ~CGroupDlg() {}

private slots:
	void on_addProjectToolBn_clicked();
	void on_removeProjectToolBn_clicked();

    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_applyButton_clicked();

	void on_groupComboBox_activated(const QString& comboText);

    void groupContentChanged();

private:

	void createActions();

	void loadAllProjectListView();
	void loadGroupProjectListView(const QString& groupName);

	QString currentGroupName_;
};
#endif // CGROUPDLG_H


