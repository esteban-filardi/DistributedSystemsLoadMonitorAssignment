#pragma once

#include <string>
#include "../CpuUsageCalculator/CpuUsageCalculator.h"
#include "../../include/sp.h"

class LoadInfoReporter {
public:
    LoadInfoReporter(mailbox mailboxParam, std::string username, std::string groupName, CpuUsageCalculator cpuUsageCalculator);

    void SendLoadInfo();

private:
    mailbox _mailbox;
    std::string _username;
    std::string _groupName;
    CpuUsageCalculator _cpuUsageCalculator;
    
    std::string GetLoadInfoMessage();
};
