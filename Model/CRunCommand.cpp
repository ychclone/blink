#include "CRunCommand.h"

CRunCommand::CRunCommand()
	: MS_EACH_TIME_SLOT_WAIT(5000) // max wait for 5 second for each time slot  
{

}

CRunCommand::~CRunCommand()
{


}

int CRunCommand::startRun(const QString& program, const QString& workDir, const QString& redirectFile, QString& errStr)
{
	QProcess::ProcessError processErr = QProcess::UnknownError;
	bool bProcessNotRunning;
	QString outputStr;

	m_bCommandCancelled = false;

	qDebug() << "startCommand IN, cmd = (" << program << ")";
	qDebug() << "workDir = " << workDir;

	QProcess extProcess;

	extProcess.setWorkingDirectory(workDir);
	if (redirectFile != "") {
		extProcess.setStandardOutputFile(redirectFile, QIODevice::Append);
		extProcess.setStandardErrorFile(redirectFile, QIODevice::Append); 
	}

	extProcess.start(program);
	qDebug("Waiting process to start..."); 
	
	bProcessNotRunning = false;
	
	// wait the process to start
	do {
		if (!(extProcess.waitForStarted(MS_EACH_TIME_SLOT_WAIT))) {
			processErr = extProcess.error();
			qDebug("waitForStarted(), QProcess::error(): %d", processErr);
		}

		qDebug() << "process state = " << QString::number(int(extProcess.state()));

		// also break if process not running/finish running otherwise would result in busy loop
        if (extProcess.state() != QProcess::Running) {
			processErr = extProcess.error();
			qDebug("process state != QProcess::Running, QProcess::error(): %d", processErr);
			
			bProcessNotRunning = true;
			break;
		}
	} while (((!m_bCommandCancelled) && (processErr == QProcess::Timedout)) && (extProcess.state() != QProcess::Running));

	qDebug("Process started, now running...");
	
	if (processErr == QProcess::FailedToStart) {
		errStr = program + outputStr;
		return E_RUNCMD_FAILSTART;
	}
	
	if (m_bCommandCancelled) {
		// kill the runnning process
		if (extProcess.state() == QProcess::Running) {
			extProcess.kill();
		}
		return E_RUNCMD_CANCELLED;
	}
	if (bProcessNotRunning) {
		if (extProcess.exitStatus() == QProcess::CrashExit) {
			errStr = program;
			return E_RUNCMD_CRASHED;
		}
	}

    // wait the process to finish
	bProcessNotRunning = false;
	do {
		if (!(extProcess.waitForFinished(MS_EACH_TIME_SLOT_WAIT))) {
			processErr = extProcess.error();
			qDebug("waitForFinished(), QProcess::error(): %d", processErr);
		}
		// also break if process not running/finish running otherwise would result in busy loop
        if (extProcess.state() != QProcess::Running) {
			bProcessNotRunning = true;
			break;
		}
	} while ((!m_bCommandCancelled) && (processErr == QProcess::Timedout));

	// User cancel, kill the runnning process 
	if (m_bCommandCancelled) {
		if (extProcess.state() == QProcess::Running) {
			extProcess.kill();
		}
		
		return E_RUNCMD_CANCELLED;
	}
	
	if (bProcessNotRunning) {
		if (extProcess.exitStatus() == QProcess::CrashExit) {
			errStr = program;
			return E_RUNCMD_CRASHED;
		}		
	}

	qDebug() << "startCommand OUT";

	return E_RUNCMD_NO_ERROR;
}

int CRunCommand::cancelCommand(bool bCancel)
{
	m_bCommandCancelled = true;
	return 0;
}




