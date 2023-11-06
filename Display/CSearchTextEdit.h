#ifndef CSEARCH_TEXT_EDIT_H_
#define CSEARCH_TEXT_EDIT_H_

#include <QPlainTextEdit>
#include <QMouseEvent>
#include <QPoint>

#include "Model/CConfigManager.h"

class CSearchTextEdit : public QPlainTextEdit
{
    Q_OBJECT

private:
    QString clickedAnchor;
    QPoint dragStartPosition;

public:
    CSearchTextEdit(QWidget *parent);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    bool event(QEvent *event);

    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void linkActivated(QString, int);
	void linkActivatedNewTab(QString, int);

private:
    void editFile(const QString& sourceFileName, int lineNumber);
	void editFileNewTab(const QString& sourceFileName, int lineNumber);
	void editFileExternal(const QString& sourceFileName);

    void exploreDir(const QString& executeDir);
    void consoleDir(const QString& executeDir);

    void contextMenuEvent(QContextMenuEvent *event);

    CConfigManager* confManager_;

};

#endif // CSEARCH_TEXT_EDIT_H_


