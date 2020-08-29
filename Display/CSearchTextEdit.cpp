#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QDir>

#ifdef Q_OS_WIN
	#include <qt_windows.h>
#endif

#include "CSearchTextEdit.h"
#include "Model/CConfigManager.h"

CSearchTextEdit::CSearchTextEdit(QWidget *parent = 0)
	: QPlainTextEdit(parent)
{
}

void CSearchTextEdit::mousePressEvent(QMouseEvent *e)
{
	clickedAnchor = (e->button() & Qt::LeftButton) ? anchorAt(e->pos()) :
													 QString();
	QPlainTextEdit::mousePressEvent(e);
}

void CSearchTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() & Qt::LeftButton && !clickedAnchor.isEmpty() &&
		anchorAt(e->pos()) == clickedAnchor)
	{
		int lineFieldPos = clickedAnchor.lastIndexOf('#');
		QString sourceFileName = clickedAnchor.left(lineFieldPos);

		QFileInfo fileInfo(sourceFileName);
		QString executeDir = fileInfo.absoluteDir().absolutePath();
		QString editFilename = "";

		QString consoleCommnad = CConfigManager::getInstance()->getAppSettingValue("DefaultEditor").toString();

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

	QPlainTextEdit::mouseReleaseEvent(e);
}





