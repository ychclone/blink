#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QMenu>
#include <QDesktopServices>
#include <QEvent>
#include <QToolTip>
#include <QClipboard>
#include <QApplication>
#include <QMessageBox>
#include <QMimeData>
#include <QDrag>

#ifdef Q_OS_WIN
	#include <qt_windows.h>
#endif

#include "CSearchTextEdit.h"

CSearchTextEdit::CSearchTextEdit(QWidget *parent = 0)
	: QPlainTextEdit(parent)
{
	confManager_ = CConfigManager::getInstance();
	setMouseTracking(true);
}

void CSearchTextEdit::mousePressEvent(QMouseEvent *e)
{
	clickedAnchor = (e->button() & Qt::LeftButton) ? anchorAt(e->pos()) :
													 QString();

    if (e->button() == Qt::LeftButton) {
        dragStartPosition = e->pos();
	}

	QPlainTextEdit::mousePressEvent(e);
}

void CSearchTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton)) {
        return;
	}
    if ((e->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
	}

	if (clickedAnchor != "") {
		int lineFieldPos = clickedAnchor.lastIndexOf('#');
		QString sourceFileName = clickedAnchor.left(lineFieldPos);

		QDrag *drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;

		QList<QUrl> urls;
		QUrl url = QUrl::fromLocalFile(sourceFileName);
		urls.push_back(url);

		mimeData->setUrls(urls);
		drag->setMimeData(mimeData);

		Qt::DropAction dropAction = drag->exec(Qt::CopyAction);
	}

	QPlainTextEdit::mouseMoveEvent(e);
}

bool CSearchTextEdit::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
		QString clickedAnchor = anchorAt(helpEvent->pos());
		int lineFieldPos = clickedAnchor.lastIndexOf('#');
		QString sourceFileName = clickedAnchor.left(lineFieldPos);

        if (clickedAnchor != "") {
            QToolTip::showText(helpEvent->globalPos(), sourceFileName);
		} else {
            QToolTip::hideText();
		}
        return true;
    }
    return QPlainTextEdit::event(event);
}

void CSearchTextEdit::editFileExternal(const QString& sourceFileName)
{
	qDebug() << "sourceFileName in editFileExternal() = " << sourceFileName;

	QFileInfo fileInfo(sourceFileName);
	QString executeDir = fileInfo.absoluteDir().absolutePath();
	QString editFilename = "";

	QString consoleCommnad = CConfigManager::getInstance()->getAppSettingValue("DefaultEditor").toString();

	if (consoleCommnad == "") {
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Information);
		msgBox.setText("External editor has not be defined. Please set it in Options -> Settings -> Main -> External Editor");
		msgBox.exec();
		return;
	}

#ifdef Q_OS_WIN
	editFilename = "\"" + sourceFileName + "\"";
#else
	editFilename = sourceFileName;
#endif

#ifdef Q_OS_WIN
	QString excuteMethod = "open";

	ShellExecute(NULL, reinterpret_cast<const wchar_t*>(excuteMethod.utf16()), reinterpret_cast<const wchar_t*>(consoleCommnad.utf16()), reinterpret_cast<const wchar_t*> (editFilename.utf16()), reinterpret_cast<const wchar_t*>(executeDir.utf16()), SW_NORMAL);
#else
	QProcess::startDetached(consoleCommnad, QStringList(editFilename));
#endif
}

void CSearchTextEdit::editFile(const QString& sourceFileName, int lineNumber)
{
	emit linkActivated(sourceFileName, lineNumber - 1);
}

void CSearchTextEdit::editFileNewTab(const QString& sourceFileName, int lineNumber)
{
	emit linkActivatedNewTab(sourceFileName, lineNumber - 1);
}

void CSearchTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
	if (!clickedAnchor.isEmpty() && anchorAt(e->pos()) == clickedAnchor) {
		if (e->button() & Qt::LeftButton) {
			int lineFieldPos = clickedAnchor.lastIndexOf('#');
			QString sourceFileName = clickedAnchor.left(lineFieldPos);
			int lineNumber = clickedAnchor.mid(lineFieldPos + 1, -1).toInt();

			if (confManager_->getAppSettingValue("UseExternalEditor").toBool()) {
				editFileExternal(sourceFileName);
			}

			editFile(sourceFileName, lineNumber);
		}
	}

	QPlainTextEdit::mouseReleaseEvent(e);
}

void CSearchTextEdit::exploreDir(const QString& executeDir)
{
#ifdef Q_OS_WIN
	QString excuteMethod = "explore";
	ShellExecute(NULL, reinterpret_cast<const wchar_t*>(excuteMethod.utf16()), reinterpret_cast<const wchar_t*>(executeDir.utf16()), NULL, NULL, SW_NORMAL);
#else
	QDesktopServices::openUrl(QUrl(executeDir, QUrl::TolerantMode));
#endif
}

void CSearchTextEdit::consoleDir(const QString& executeDir)
{
#ifdef Q_OS_WIN
	QString excuteMethod = "open";
	QString consoleCommnad = "cmd.exe";

	ShellExecute(NULL, reinterpret_cast<const wchar_t*>(excuteMethod.utf16()), reinterpret_cast<const wchar_t*>(consoleCommnad.utf16()), NULL, reinterpret_cast<const wchar_t*>(executeDir.utf16()), SW_NORMAL);
#else
	QString workingDir = "--working-directory=" + executeDir;
	QProcess::startDetached("gnome-terminal", QStringList(workingDir));
#endif
}


void CSearchTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

	QString clickedAnchor = anchorAt(event->pos());

	if (clickedAnchor != "") {
		int lineFieldPos = clickedAnchor.lastIndexOf('#');
		QString sourceFileName = clickedAnchor.left(lineFieldPos);
		int lineNumber = clickedAnchor.mid(lineFieldPos + 1, -1).toInt();

		QAction actionEdit("Edit", this);
		QAction actionEditNewTab("Edit in New Tab", this);
		QAction actionEditExternal("Edit (External)", this);
		
		QAction actionExplore("Explore", this);
		QAction actionCopy("Copy Pathnames", this);
		QAction actionConsole("Console", this);

        QIcon iconEdit;
        iconEdit.addFile(QString::fromUtf8(":/Icons/22x22/edit-4.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconExplore;
        iconExplore.addFile(QString::fromUtf8(":/Icons/22x22/document-open-folder.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconCopy;
        iconCopy.addFile(QString::fromUtf8(":/Icons/22x22/edit-copy-4.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconConsole;
        iconConsole.addFile(QString::fromUtf8(":/Icons/22x22/xconsole-2.png"), QSize(), QIcon::Normal, QIcon::Off);

		actionEdit.setIcon(iconEdit);
		actionEditNewTab.setIcon(iconEdit);
		
		actionEditExternal.setIcon(iconEdit);
		
		actionExplore.setIcon(iconExplore);
		actionCopy.setIcon(iconCopy);
		actionConsole.setIcon(iconConsole);

		QFileInfo fileInfo(sourceFileName);
		QString executeDir = fileInfo.absoluteDir().absolutePath();
		
		connect(&actionEdit, &QAction::triggered, this, [&]{
				this->editFile(sourceFileName, lineNumber);
		});
		
		connect(&actionEditNewTab, &QAction::triggered, this, [&]{
				this->editFileNewTab(sourceFileName, lineNumber);
		});

		connect(&actionEditExternal, &QAction::triggered, this, [&]{
				this->editFileExternal(sourceFileName);
		});

		connect(&actionExplore, &QAction::triggered, this, [&]{
				this->exploreDir(executeDir);
		});

		connect(&actionCopy, &QAction::triggered, this, [&]{
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(sourceFileName);
		});

		connect(&actionConsole, &QAction::triggered, this, [&]{
				this->consoleDir(executeDir);
		});

		menu->addAction(&actionEdit);
		menu->addAction(&actionEditNewTab);
		menu->addAction(&actionEditExternal);	
		menu->addAction(&actionExplore);
		menu->addAction(&actionCopy);
		menu->addAction(&actionConsole);

		menu->exec(event->globalPos());
	} else {
		menu->exec(event->globalPos());
	}

    delete menu;
}


