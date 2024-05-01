#include "LoadInfoReporter.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include "../get-percentage-cpu.h"

using namespace std;

const short MT_LOAD_INFO_MESS_TYPE  = 666;

std::string CurrentISO8601TimeUTC();

// Constructor definition
LoadInfoReporter::LoadInfoReporter(mailbox mailboxParam, std::string username, std::string groupName) {
    this->_mailbox = mailboxParam;
    this->_username = username;
    this->_groupName = groupName;
}

void LoadInfoReporter::SendLoadInfo() {
    cout << "SendLoadInfo" << endl;

    auto loadMesage = GetLoadInfoMessage();
    SP_multicast(_mailbox, AGREED_MESS, this->_groupName.c_str(), MT_LOAD_INFO_MESS_TYPE, loadMesage.length(), loadMesage.c_str());
}

std::string LoadInfoReporter::GetLoadInfoMessage()
{    
    auto loadAsInt = getPercentageCpu();
    auto loadAsString = std::to_string(loadAsInt);

    auto currentTimestampt = CurrentISO8601TimeUTC();

    // Build JSON message
    std::ostringstream jsonStringStream;
    jsonStringStream << "{\n";
    jsonStringStream << "\t\"node_name\": \"" << this->_username << "\",\n";
    jsonStringStream << "\t\"time\": \"" << currentTimestampt << "\",\n";
    jsonStringStream << "\t\"load\": " << loadAsInt << "\n";
    jsonStringStream << "}";

    return jsonStringStream.str();
}

std::string CurrentISO8601TimeUTC() {
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(gmtime(&itt), "%FT%TZ");
    return ss.str();
}
