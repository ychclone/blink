#ifndef CSEARCH_TEXT_EDIT_H_
#define CSEARCH_TEXT_EDIT_H_

#include <QPlainTextEdit>
#include <QMouseEvent>

class CSearchTextEdit : public QPlainTextEdit
{
    Q_OBJECT

private:
    QString clickedAnchor;

public:
    CSearchTextEdit(QWidget *parent);
    void mousePressEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void linkActivated(QString);
};

#endif // CSEARCH_TEXT_EDIT_H_


