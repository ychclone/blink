#ifndef CPROFILEDLG_H
#define CPROFILEDLG_H

#include <QDialog> 
#include "Model/CProfileItem.h"
#include "Model/CProfileManager.h"
#include "ui_profileDialog.h"

class CProfileDlg : public QDialog, private Ui::profileDialog
{
    Q_OBJECT

public:
    CProfileDlg(QWidget* parent = 0);
    CProfileDlg(const QString& profileName, const CProfileItem& profileItem, QWidget* parent = 0);

    virtual ~CProfileDlg() {}
private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_applyButton_clicked();
    void on_srcDir_toolBn_clicked();

    void profileContentChanged();
private:
	QString currentProfileName_;
};
#endif // CPROFILEDLG_H


