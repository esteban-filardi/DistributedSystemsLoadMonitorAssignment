#pragma once

#include <string>
#include "../../include/sp.h"

class LoadInfoReporter {
public:
    LoadInfoReporter(mailbox mailboxParam, std::string username, std::string groupName);

    void SendLoadInfo();

private:
    mailbox _mailbox;
    std::string _username;
    std::string _groupName;
    
    std::string GetLoadInfoMessage();
};
