#include "Application.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <functional>
#include "../LoadInfoReporter/LoadInfoReporter.h"
#include "../GuidGenerator/GuidGenerator.h"
#include "../ClusterMessagesReceiver/ClusterMessagesReceiver.h"

Cluster Application::cluster = Cluster();
char Application::_privateGroup[MAX_GROUP_NAME];
char Application::_spreadName[80];
mailbox Application::_mailbox;

void SendLoadInfoTask(mailbox mailboxParam, std::string user, std::string clusterGroupName);
void ReceiveClusterMessagesTask(mailbox mailboxParam, std::string user, std::string clusterGroupName);

Application::Application() {
    
}

int Application::Run(int argc, char* argv[]) {
	SetUpStopSignalHandler();
	ProcessParameters(argc, argv);

	std::string user = GuidGenerator::GenerateGuid();

	/* connecting to the daemon, requesting group information */
	std::cout << "Connecting with username: " << user << std::endl;
	int ret = SP_connect(_spreadName, user.c_str(), 0, 1, &_mailbox, _privateGroup);
	if (ret < 0)
	{
		SP_error(ret);
		exit(0);
	}

	/* joining a group */
	std::cout << "Joining group: " << Application::CLUSTER_GROUP_NAME << std::endl;
	SP_join(_mailbox, CLUSTER_GROUP_NAME.c_str());


	auto sendLoadInfoBoundTask = std::bind(SendLoadInfoTask, _mailbox, user, CLUSTER_GROUP_NAME);
	std::thread sendLoadInfoThread(sendLoadInfoBoundTask);

	auto receiveClusterMessagesBoundTask = std::bind(ReceiveClusterMessagesTask, _mailbox, user, CLUSTER_GROUP_NAME);
	std::thread receiveClusterMessagesThread(receiveClusterMessagesBoundTask);

	sendLoadInfoThread.join();
    receiveClusterMessagesThread.join();


	return 0;
}

void SendLoadInfoTask(mailbox mailboxParam, std::string user, std::string clusterGroupName)
{
	LoadInfoReporter loadInfoReporter(mailboxParam, user, clusterGroupName);

	while (true)
	{
		loadInfoReporter.SendLoadInfo();
		
		std::chrono::seconds durationToWait(1);
		std::this_thread::sleep_for(durationToWait);
	}
}

void Application::ProcessParameters(int argc, char* argv[])
{
	/* Setting defaults */
	sprintf(this->_spreadName, "4803");
	while (--argc > 0)
	{
		argv++;

		if (!strncmp(*argv, "-s", 2)) {
			strcpy(this->_spreadName, argv[1]);
			argc--; argv++;
		}
		else {
			printf("Usage: user\n%s\n",
				"\t[-s <address>]    : either port or port@machine");
			exit(1);
		}
	}
}

void ReceiveClusterMessagesTask(mailbox mailboxParam, std::string user, std::string clusterGroupName)
{
    ClusterMessagesReceiver clusterMessageReceiver (mailboxParam, user, clusterGroupName);
    clusterMessageReceiver.ReceiveClusterMessages();
}

Application::~Application() {
    SP_disconnect(Application::_mailbox);
}

void StopSignalHandler(int s)
{
	printf("\nCaught signal %d\n", s);
	exit(1);
}

void Application::SetUpStopSignalHandler()
{
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = StopSignalHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
}