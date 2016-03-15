
#include <iostream>
#include <sstream>
#include <map>
#include "Applog.h"
#include "include_common.h"
#include "ProjectServer.h"
#include "boost/thread/thread.hpp"

#ifdef _Q_Linux
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>
#endif

using namespace itstation;
using namespace itstation::common;
using namespace std;

bool b_stop = false;
#define PACKAGE_STRING "1.0.0"

#ifdef __GNUG__
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
/* Bit-mask values for 'flags' argument of becomeDaemon() */

#define BD_NO_CHDIR           01    /* Don't chdir("/") */
#define BD_NO_CLOSE_FILES     02    /* Don't close all open files */
#define BD_NO_REOPEN_STD_FDS  04    /* Don't reopen stdin, stdout, and
                                       stderr to /dev/null */
#define BD_NO_UMASK0         010    /* Don't do a umask(0) */

#define BD_MAX_CLOSE  8192          /* Maximum file descriptors to close if
                                       sysconf(_SC_OPEN_MAX) is indeterminate */
int becomeDaemon(int flags)         /* Returns 0 on success, -1 on error */

{
    int maxfd, fd;

    switch (fork())						/* Become background process */
	{                  
    case -1: return -1;
    case 0:  break;                     /* Child falls through... */
    default: _exit(EXIT_SUCCESS);       /* while parent terminates */
    }

    if (setsid() == -1)                 /* Become leader of new session */
        return -1;

    switch (fork()) {                   /* Ensure we are not session leader */
    case -1: return -1;
    case 0:  break;
    default: _exit(EXIT_SUCCESS);
    }

    if (!(flags & BD_NO_UMASK0))
        umask(0);                       /* Clear file mode creation mask */

    if (!(flags & BD_NO_CHDIR))
        chdir("/");                     /* Change to root directory */

    if (!(flags & BD_NO_CLOSE_FILES)) { /* Close all open files */
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)                /* Limit is indeterminate... */
            maxfd = BD_MAX_CLOSE;       /* so take a guess */

        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);            /* Reopen standard fd's to /dev/null */

        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO)         /* 'fd' should be 0 */
            return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}

void hanler(int signo, siginfo_t* info, void* context)
{
	cout<<"catch "<<endl;
	if (signo == SIGINT) {

		cout<<"Crtl+c catched!"<<endl;
		b_stop = true;
	}
}

void printUsage(char* name)
{
	printf("usage: %s [options]\n\n", name);
	printf("    -d              debug - don't fork\n");
	printf("    -v              shows version information\n");
	printf("\n");
}
#endif

int main(int argc, char **argv)
{
#ifdef __GNUG__

	bool debug = false;
	int ch;
	while ((ch = getopt(argc, argv, "dvs")) != -1)
	{
		switch (ch)
		{
		case 'd':
			{
				debug = true;
			}
			break;
		case 'v':
			{
				std::cout << "BasePlatEngine (" << PACKAGE_STRING << ")" << std::endl;
				return 0;
			}
		case '?':
		default:
			{
				printUsage(argv[0]);
				exit(1); //表示异常退出.这个1是返回给操作系统的。
			}
			break;
		}
	}

	//建立守护进程
	if (!debug)
	{
		becomeDaemon(0);
	}

#endif

	int cout_log_level = Applog::LOG_ERROR;
#ifdef _DEBUG
#else
	char *env_data = getenv("COUT_LOG_LEVEL");
	if(env_data)
	{
		istringstream t(env_data);
		t >> cout_log_level;

		APP_LOG(Applog::LOG_INFO) << "SetLogLevel=" << cout_log_level;
	}
#endif
	cout_log_level = Applog::LOG_INFO;
	Applog::SetLogLevel(cout_log_level);


#ifdef __GNUG__

	struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_SIGINFO;
	sigact.sa_sigaction = hanler;

	// Establish a handler to catch CTRL+c and use it for exiting.
	sigaction(SIGINT, &sigact, NULL);
#endif

#ifdef __GNUG__
	struct rlimit lmt;

	getrlimit(RLIMIT_STACK,&lmt);
	char *tmplmt = new char[512];
	memset(tmplmt, 0, 512);
	sprintf(tmplmt, "rlim_cur=%u,rlim_max=%u\n", lmt.rlim_cur, lmt.rlim_max );

	APP_LOG(Applog::LOG_INFO)<<string(tmplmt);
	delete[] tmplmt;
#endif
	CProjectServer* server = CProjectServer::GetInstance();
	server->initialize();
	server->startServer();
	while(!b_stop)
	{
		//cout << "master thread" << endl;
		q_sleep(1000);
	}
	server->stopServer();   
	server->Join();

	return 0;
}

