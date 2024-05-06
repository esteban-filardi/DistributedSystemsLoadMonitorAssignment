#include "ClusterMessagesReceiver.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include "../Application/Application.h"
#include "../AppMessageTypes/AppMessageTypes.h"
#include "../../include/json.hpp"

#define DSLM_MAX_NUMBER_OF_GROUPS 32

using namespace std;

void DisplayMessageInformation(char * message, int messageServiceType, int ret, char * sender, int16 applicationSpecificMessageType, int isThereEndianMisMatch, int numberOfGroupMessageWasSentTo, char(*groupsMessageWasSentTo)[DSLM_MAX_NUMBER_OF_GROUPS]);

// Constructor definition
ClusterMessagesReceiver::ClusterMessagesReceiver(mailbox mailboxParam, std::string username, std::string groupName) {
    this->_mailbox = mailboxParam;
    this->_username = username;
    this->_groupName = groupName;
}

void ClusterMessagesReceiver::ReceiveClusterMessages() {
    // cout << std::endl <<  "ReceiveClusterMessages" << endl;

    while (true)
    {
        this->ReceiveMessage();
    }
}

void ClusterMessagesReceiver::ReceiveMessage()
{
    // Not expecting longer messages
    static char message[1024];
    char sender[MAX_GROUP_NAME];
    char groupsMessageWasSentTo[DSLM_MAX_NUMBER_OF_GROUPS][MAX_GROUP_NAME];
    int numberOfGroupMessageWasSentTo;
    int messageServiceType = 0;
    int16 applicationSpecificMessageType;
    int isThereEndianMisMatch;
    
    int ret = SP_receive(this->_mailbox, &messageServiceType, sender, 100, &numberOfGroupMessageWasSentTo, groupsMessageWasSentTo,
        &applicationSpecificMessageType, &isThereEndianMisMatch, sizeof(message), message);
    if (ret < 0)
    {
        SP_error(ret);
        exit(0);
    }

    DisplayMessageInformation(message, messageServiceType, ret, sender, applicationSpecificMessageType, isThereEndianMisMatch, numberOfGroupMessageWasSentTo, groupsMessageWasSentTo);

    Application::PrintUserInputPrompt();

    if (Is_regular_mess(messageServiceType))
    {
        /* A regular message, sent by one of the processes */

        // In this context ret is message size
        int messageSize = ret;
        // Prevents message from being corrupted with characters of previous messages
        message[messageSize] = 0;

        // Application specific message type will always be the same for this assignment
        if (applicationSpecificMessageType == MT_LOAD_INFO_MESS_TYPE) {
            auto jsonMessage = nlohmann::json::parse(message);
            Application::cluster.SetNodeLoad(sender, jsonMessage["load"], jsonMessage["time"]);
        }
    }
    else if (Is_membership_mess(messageServiceType))
    {
        /* A membership notification */
        membership_info membershipInfo;
        int retGetMembInfo = SP_get_memb_info(message, messageServiceType, &membershipInfo);
        if (retGetMembInfo < 0)
        {
            printf("BUG: membership message does not have valid body\n");
            SP_error(retGetMembInfo);
            exit(1);
        }

        if (Is_reg_memb_mess(messageServiceType))
        {
            if (Is_caused_join_mess(messageServiceType))
            {
                auto node = new ClusterNode(membershipInfo.changed_member);
                Application::cluster.AddNode(node);
            }

            if (Is_caused_disconnect_mess(messageServiceType) || Is_caused_leave_mess(messageServiceType))
            {
                Application::cluster.RemoveNode(membershipInfo.changed_member);
            }
        }
    }
}

/// <summary>
/// Original receive logic from simple_user.c sample with just some slight modifications for clarity.
/// </summary>
/// <param name="message"></param>
/// <param name="messageServiceType"></param>
/// <param name="ret"></param>
/// <param name="sender"></param>
/// <param name="applicationSpecificMessageType"></param>
/// <param name="isThereEndianMisMatch"></param>
/// <param name="numberOfGroupMessageWasSentTo"></param>
/// <param name="groupsMessageWasSentTo"></param>
void DisplayMessageInformation(char* message, int messageServiceType, int ret, char* sender, int16 applicationSpecificMessageType, int isThereEndianMisMatch, int numberOfGroupMessageWasSentTo, char (*groupsMessageWasSentTo) [DSLM_MAX_NUMBER_OF_GROUPS])
{
    printf("\n");
    if (Is_regular_mess(messageServiceType))
    {
        /* A regular message, sent by one of the processes */

        // Prevents message from being corrupted with characters of previous messages
        message[ret] = 0;

        if (Is_unreliable_mess(messageServiceType))
            printf("received UNRELIABLE ");
        else if (Is_reliable_mess(messageServiceType))
            printf("received RELIABLE ");
        else if (Is_fifo_mess(messageServiceType))
            printf("received FIFO ");
        else if (Is_causal_mess(messageServiceType))
            printf("received CAUSAL ");
        else if (Is_agreed_mess(messageServiceType))
            printf("received AGREED ");
        else if (Is_safe_mess(messageServiceType))
            printf("received SAFE ");
        printf("message from %s of type %d (endian %d), to %d groups \n(%d bytes): %s\n",
            sender, applicationSpecificMessageType, isThereEndianMisMatch, numberOfGroupMessageWasSentTo, ret, message);
    }
    else if (Is_membership_mess(messageServiceType))
    {
        /* A membership notification */
        membership_info memb_info;
        ret = SP_get_memb_info(message, messageServiceType, &memb_info);
        if (ret < 0)
        {
            printf("BUG: membership message does not have valid body\n");
            SP_error(ret);
            exit(1);
        }
        if (Is_reg_memb_mess(messageServiceType))
        {
            printf("received REGULAR membership ");
            if (Is_caused_join_mess(messageServiceType))
            {
                printf("caused by JOIN ");
            }
            if (Is_caused_leave_mess(messageServiceType))
            {
                printf("caused by LEAVE ");
            }
            if (Is_caused_disconnect_mess(messageServiceType))
            {
                printf("caused by DISCONNECT ");
            }

            printf("for group %s with %d members:\n", sender, numberOfGroupMessageWasSentTo);

            for (int i = 0; i < numberOfGroupMessageWasSentTo; i++)
            {
                printf("\t%s\n", &groupsMessageWasSentTo[i][0]);
            }
            printf("grp id is %d %d %d\n", memb_info.gid.id[0], memb_info.gid.id[1], memb_info.gid.id[2]);
        }
        else if (Is_transition_mess(messageServiceType))
        {
            printf("received TRANSITIONAL membership for group %s\n", sender);
        }
        else if (Is_caused_leave_mess(messageServiceType))
        {
            printf("received membership message that left group %s\n", sender);
        }
        else
        {
            printf("received incorrect membership message of type %d\n", messageServiceType);
        }
    }
    else
    {
        printf("received message of unknown message type %d with %d bytes\n", messageServiceType, ret);
    };
}
