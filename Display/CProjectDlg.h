#ifndef CPROJECTDLG_H
#define CPROJECTDLG_H

#include <QDialog> 
#include "Model/CProjectItem.h"
#include "Model/CProjectManager.h"
#include "ui_projectDialog.h"

class CProjectDlg : public QDialog, private Ui::projectDialog
{
    Q_OBJECT

public:
    CProjectDlg(QWidget* parent = 0);
    CProjectDlg(const QString& projectName, const CProjectItem& projectItem, QWidget* parent = 0);

    virtual ~CProjectDlg() {}
private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_applyButton_clicked();
    void on_srcDir_toolBn_clicked();

    void projectContentChanged();
private:
	QString currentProjectName_;
};
#endif // CPROJECTDLG_H


