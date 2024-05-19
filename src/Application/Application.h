#pragma once

#include <string>
#include "../../include/sp.h"
#include "../Cluster/Cluster.h"

class Application {
public:
    static mailbox _mailbox;
    static Cluster cluster;

    Application();
    ~Application();

    int Run(int argc, char* argv[]);
    static void PrintMessage(std::string message);
    static void PrintMenu();
    static void PrintUserInputPrompt();
    static void PrintNodeList();
    static void UserCommand();
private:
    const std::string CLUSTER_GROUP_NAME = "load-reporters";
    /** Returned group name which can be used to send unicast messages to this connection (It is required for SP_Connect) */
    static char _privateGroup[MAX_GROUP_NAME];
    static char _spreadName[80];

    void ProcessParameters(int argc, char* argv[]);
};
