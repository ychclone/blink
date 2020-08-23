#ifndef CFIND_REPLACE_DLG_H
#define CFIND_REPLACE_DLG_H

#include <QDialog>
#include <QStatusBar>

#include <QContextMenuEvent>

#include "ui_findReplaceDialog.h"

#include "Model/CFindReplaceModel.h"
#include "Model/qFindReplacer/qFindReplacer.h"

class CFindReplaceDlg : public QDialog, private Ui::findReplaceDialog
{
	Q_OBJECT

public:
    CFindReplaceDlg(QWidget* parent, CFindReplaceModel* findReplaceModel);
	virtual ~CFindReplaceDlg() {};

	void setFindLineEdit(QString findStr);

	QString getSelectedFile();
	void contextMenuEvent(QContextMenuEvent* event);

private slots:
	void on_replaceButton_clicked();
	void on_cancelButton_clicked();

	void on_selectAllButton_clicked();
	void on_clearAllButton_clicked();

	void on_fileList_itemChanged(QStandardItem *item);
	void on_fileEditPressed();


private:
	void createActions();
	void showFileSelectedInStatusBar();

	QStatusBar *statusbar;

	CFindReplaceModel* findReplaceModel_;
	QFindReplacer findReplacer_;
};

#endif // CFIND_REPLACE_DLG_H
