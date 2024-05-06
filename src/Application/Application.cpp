#include "Application.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <sstream>
#include <iomanip>
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

	PrintMenu();
	PrintUserInputPrompt();

	auto sendLoadInfoBoundTask = std::bind(SendLoadInfoTask, _mailbox, user, CLUSTER_GROUP_NAME);
	std::thread sendLoadInfoThread(sendLoadInfoBoundTask);

	auto receiveClusterMessagesBoundTask = std::bind(ReceiveClusterMessagesTask, _mailbox, user, CLUSTER_GROUP_NAME);
	std::thread receiveClusterMessagesThread(receiveClusterMessagesBoundTask);

	while (true) {
		Application::UserCommand();;
	}


	return 0;
}

void SendLoadInfoTask(mailbox mailboxParam, std::string user, std::string clusterGroupName)
{
	CpuUsageCalculator cpuUsageCalculator;
	LoadInfoReporter loadInfoReporter(mailboxParam, user, clusterGroupName, cpuUsageCalculator);

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

void Application::PrintMenu()
{
	std::ostringstream builder;
	builder << std::endl;
	builder << "==========" << std::endl;
	builder << "User Menu:" << std::endl;
	builder << "----------" << std::endl;
	builder << std::endl;
	builder << "\tl -- print node list" << std::endl;
	builder << "\tq -- quit" << std::endl;
	builder << std::endl;

	std::cout << builder.str();
}

void Application::PrintUserInputPrompt()
{
	std::cout << "User > " << std::flush;
}

void Application::PrintNodeList()
{
	std::ostringstream builder;

	builder << std::left << std::setw(27) << "Name" << std::setw(13) << "Load" << std::setw(25) << "Load Timestamp" << std::endl;
	builder << std::setfill('-') << std::setw(65) << "" << std::setfill(' ') << std::endl;

	auto nodesMap = Application::cluster.GetClusterNodesSnapshot();

	for (auto it = nodesMap.begin(); it != nodesMap.end(); ++it)
	{
		std::string loadInfoString = it->second.load.has_value() ? std::to_string(it->second.load.value()) + " %" : "";

		builder << std::setw(27) << it->first 
			<< std::left
			<< std::setw(13) << loadInfoString 
			<< std::left
			<< std::setw(25) << it->second.loadTimestamp.value_or("") << std::endl;
	}

	std::cout << builder.str() << std::endl;
}

void Application::UserCommand()
{
	std::string command;

	std::cin >> command;

	switch (command.c_str()[0])
	{
	case 'l':
		Application::PrintNodeList();
		break;
	case 'q':
		exit(0);
		break;
	default:
		Application::PrintMenu();
	}

	Application::PrintUserInputPrompt();
}

Application::~Application() {
    SP_disconnect(Application::_mailbox);
}
