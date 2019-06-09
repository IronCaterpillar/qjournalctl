#ifndef REMOTE_H
#define REMOTE_H

#include <thread>
#include <mutex>
#include <cassert>
#include <libssh/libssh.h>
#include <libssh/callbacks.h>
#include <QObject>

#include "sshconnectionsettings.h"

class Remote: public QObject
{
	Q_OBJECT

public:
	Remote(QObject *qObject, SSHConnectionSettings *sshSettings);
	~Remote();

	void run(QString);
	void close();
	bool isRunning();

private:
	std::thread *readerThread;
	volatile bool destroyAllThreads;
	QString sshCmd;

	std::mutex sshMutex;
	ssh_session ssh;
	ssh_channel sshChannel;
	void initSSHChannel();

signals:
	void remoteDataAvailable(QString);

public slots:
	void processHasData();
};

#endif // REMOTE_H