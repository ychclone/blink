#ifndef CRUN_COMMAND_H
#define CRUN_COMMAND_H

#include <QString>
#include <QProcess>
#include <QDebug>

class CRunCommand
{
public:
	CRunCommand();
	~CRunCommand();

	typedef enum {E_RUNCMD_NO_ERROR, E_RUNCMD_CANCELLED, E_RUNCMD_FAILSTART, E_RUNCMD_ERRRUN, E_RUNCMD_CRASHED} ENUM_RunCommandErr;

	int startRun(const QString& program, const QString& workDir, const QString& redirectFile, QString& errStr);

	int cancelCommand(bool bCancel);

private:
	bool m_bCommandCancelled;

	const int MS_EACH_TIME_SLOT_WAIT;

};

#endif // CRUN_COMMAND_H


