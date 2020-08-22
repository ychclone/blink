#ifndef CFIND_REPLACE_DLG_H
#define CFIND_REPLACE_DLG_H

#include <QDialog>
#include <QStatusBar>

#include "ui_findReplaceDialog.h"

#include "Model/CFindReplaceModel.h"
#include "Model/qFindReplacer/qFindReplacer.h"

class CFindReplaceDlg : public QDialog, private Ui::findReplaceDialog
{
	Q_OBJECT

public:
    CFindReplaceDlg(QWidget* parent, CFindReplaceModel* findReplaceModel);
	void setFindLineEdit(QString findStr);


	virtual ~CFindReplaceDlg() {};

private slots:
	void on_replaceButton_clicked();
	void on_cancelButton_clicked();

	void on_selectAllButton_clicked();
	void on_clearAllButton_clicked();

	void on_fileList_itemChanged(QStandardItem *item);


private:
	void createActions();
	void showFileSelectedInStatusBar();

	QStatusBar *statusbar;

	CFindReplaceModel* findReplaceModel_;
	QFindReplacer findReplacer_;
};

#endif // CFIND_REPLACE_DLG_H
