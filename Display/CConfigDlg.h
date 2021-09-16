#ifndef CCONFIGDLG_H
#define CCONFIGDLG_H

#include <QDialog>
#include "ui_configDialog.h"

class CConfigDlg : public QDialog, private Ui::configDialog
{
	Q_OBJECT

public:
    CConfigDlg(QWidget* parent = 0);

	virtual ~CConfigDlg() {};

	QFont getProjectDefaultFont();
	QFont getSymbolDefaultFont();

private slots:
	void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void on_applyButton_clicked();

	void on_defaultEditor_toolBn_clicked();
	void on_projectFont_toolBn_clicked();

	void on_symbolFont_toolBn_clicked();
	void on_editorFont_toolBn_clicked();

	void on_tagDir_toolBn_clicked();
	void on_tmpDir_toolBn_clicked();

	void configContentChanged();

private:
	void createActions();

	void loadSetting();
	void saveSetting();

	QFont projectDefaultFont_;
	QFont symbolDefaultFont_;
	QFont editorDefaultFont_;


};

#endif // CCONFIGDLG_H
