#ifndef CABOUTDLG_H
#define CABOUTDLG_H

#include <QDialog> 
#include "ui_aboutDialog.h"

class CAboutDlg : public QDialog, private Ui::aboutDialog
{
    Q_OBJECT

public:
    CAboutDlg(QWidget* parent = 0);
    virtual ~CAboutDlg() {}
};
#endif // CABOUTDLG_H

//