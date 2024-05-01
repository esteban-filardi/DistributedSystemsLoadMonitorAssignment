// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;

const string CLUSTER_GROUP_NAME = "assignment";

// Returned group name which can be used to send unicast messages to this connection (It is required)
char PrivateGroup[MAX_GROUP_NAME];
char SpreadName[80];
mailbox Mbox;

void ProcessParameters(int argc, char* argv[]);

void StopSignalHandler(int s);

void SetUpStopSignalHandler();

void SendLoadInfoTask(mailbox mailboxParam, string user, string clusterGroupName);

int main(int argc, char* argv[])
{
	SetUpStopSignalHandler();
	ProcessParameters(argc, argv);

	std::string user = GuidGenerator::GenerateGuid();

	/* connecting to the daemon, requesting group information */
	cout << "Connecting with username: " << user << endl;
	int ret = SP_connect(SpreadName, user.c_str(), 0, 1, &Mbox, PrivateGroup);
	if (ret < 0)
	{
		SP_error(ret);
		exit(0);
	}

	/* joining a group */
	cout << "Joining group: " << CLUSTER_GROUP_NAME << endl;
	SP_join(Mbox, CLUSTER_GROUP_NAME.c_str());


	auto sendLoadInfoBoundTask = std::bind(SendLoadInfoTask, Mbox, user, CLUSTER_GROUP_NAME);
	std::thread sendLoadInfoThread(sendLoadInfoBoundTask);
	sendLoadInfoThread.join();
	
	return 0;
}

void SendLoadInfoTask(mailbox mailboxParam, string user, string clusterGroupName)
{
	LoadInfoReporter loadInfoReporter(mailboxParam, user, clusterGroupName);

	while (true)
	{
		loadInfoReporter.SendLoadInfo();
		
		std::chrono::seconds durationToWait(1);
		std::this_thread::sleep_for(durationToWait);
	}
}

void ProcessParameters(int argc, char* argv[])
{
	/* Setting defaults */
	sprintf(SpreadName, "4803");
	while (--argc > 0)
	{
		argv++;

		if (!strncmp(*argv, "-s", 2)) {
			strcpy(SpreadName, argv[1]);
			argc--; argv++;
		}
		else {
			printf("Usage: user\n%s\n",
				"\t[-s <address>]    : either port or port@machine");
			exit(1);
		}
	}
}

void StopSignalHandler(int s)
{
	printf("\nCaught signal %d\n", s);
	SP_disconnect(Mbox);
	exit(1);
}

void SetUpStopSignalHandler()
{
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = StopSignalHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
}