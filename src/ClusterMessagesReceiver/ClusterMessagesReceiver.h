#pragma once

#include <string>
#include "../../include/sp.h"

class ClusterMessagesReceiver {
public:
    ClusterMessagesReceiver(mailbox mailboxParam, std::string username, std::string groupName);

    void ReceiveClusterMessages();

private:
    mailbox _mailbox;
    std::string _username;
    std::string _groupName;
    
    void ReceiveMessage();
};
