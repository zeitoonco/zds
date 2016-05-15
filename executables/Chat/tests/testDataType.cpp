//
// Created by naeyrin on 5/4/16.
//
#include <iostream>
#include "executables/Chat/DSStructs.hpp"

using namespace std;
using namespace zeitoon::chat;

int main() {
    EnumMsgType MsgType;
    EnumReachState ReachState;
    EnumStatus Status;
    EnumCustomStatusIcon CustomStatusIcon;

    DSNewMessage testNewMsg(12, 1, "DSNewMessage", MsgType.user);
    std::cout << testNewMsg.toString("\t") << endl;

    DSAddUserSession testAddUserSession(12, 13);
    std::cout << testAddUserSession.toString("\t") << endl;

    DSChatUserData testChatUserData(ReachState.normal, Status.normal, CustomStatusIcon.online, "DSChatUserData");
    std::cout << testChatUserData.toString("\t") << endl;

    DSCheckMessageItem testCheckMessageItem(12, 13, 14);
    std::cout << testCheckMessageItem.toString("\t") << endl;

    /* vector<DSCheckMessageItem> Amghozi;
     Amghozi.push_back(testCheckMessageItem);
     DSCheckMessages testCheckMessages(Amghozi);
     std::cout << testCheckMessages.toString("\t") << endl;*/

    DSGetMessages testGetMessages(12,13, MsgType.user, "Ali_Naeyrin", "DSGetMessages");
    std::cout << testGetMessages.toString("\t") << endl;

    DSMessageItem testMessageItem(12, "Ali_Naeyrin", "DSMessageItem", MsgType.user);
    std::cout << testMessageItem.toString("\t") << endl;

    DSChangeUserState testChangeUserState(12,Status.normal, CustomStatusIcon.online, "DSChangeUserState");
    std::cout << testChangeUserState.toString("\t") << endl;

    vector<DSCheckMessageItem> Amghozi;
    Amghozi.push_back(testCheckMessageItem);
    DSCheckMessages testCheckMessages(Amghozi);
    std::cout << testCheckMessages.toString("\t") << endl;

    DSMessageItem testMessageList(12, "1395-02-18", "DSMessageItem", MsgType.user);
    std::cout << testMessageList.toString("\t") << endl;


    DTInteger<> il={"kk"};
    il=8;

    vector<DTInteger<int>> Amghozi_2;
    Amghozi_2.push_back(il);
    DSSessionList testSessionList(Amghozi_2);
    std::cout << testSessionList.toString("\t") << endl;


     DSSession testSession(12, "1395-02-18");
    std::cout << testSession.toString("\t") << endl;

}