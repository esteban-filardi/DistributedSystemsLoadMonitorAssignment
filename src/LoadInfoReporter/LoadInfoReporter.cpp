#include "LoadInfoReporter.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include "../AppMessageTypes/AppMessageTypes.h"
#include "../../include/json.hpp"

using namespace std;

std::string CurrentISO8601TimeUTC();

// Constructor definition
LoadInfoReporter::LoadInfoReporter(mailbox mailboxParam, std::string username, std::string groupName, CpuUsageCalculator cpuUsageCalculator) {
    this->_mailbox = mailboxParam;
    this->_username = username;
    this->_groupName = groupName;
    this->_cpuUsageCalculator = cpuUsageCalculator;
}

void LoadInfoReporter::SendLoadInfo() {
    // cout << "Sending load info" << endl;

    auto loadMesage = GetLoadInfoMessage();
    SP_multicast(_mailbox, AGREED_MESS, this->_groupName.c_str(), MT_LOAD_INFO_MESS_TYPE, loadMesage.length(), loadMesage.c_str());
}

std::string LoadInfoReporter::GetLoadInfoMessage()
{
    auto loadAsInt = _cpuUsageCalculator.GetPercentageCpu();
    auto loadAsString = std::to_string(loadAsInt);

    auto currentTimestamp = CurrentISO8601TimeUTC();

    nlohmann::json jsonMessage;
    jsonMessage["node_name"] = this->_username;
    jsonMessage["time"] = currentTimestamp;
    jsonMessage["load"] = loadAsInt;

    return jsonMessage.dump();
}

std::string CurrentISO8601TimeUTC() {
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(gmtime(&itt), "%FT%TZ");
    return ss.str();
}
