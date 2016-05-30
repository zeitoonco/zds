//
// Created by naeyrin on 5/8/16.
//

#ifndef ZDS_CHAT_HPP
#define ZDS_CHAT_HPP

#include "DSStructs.hpp"
#include "executables/UserManagement/UManagementUtility.hpp"
#include <string>
#include <executables/UserManagement/DTStructs.hpp>

namespace zeitoon {
namespace chat {

class ChatCHI;

class chaT {
	ChatCHI *chatCHI;
public:
	chaT(ChatCHI *ptr);

	int newMessage(int userID, int sessionID, std::string msg, EnumMsgType::msgType type);

	void removeMessage(int id);

	DSCheckMessages checkNewMessages(int userID);

	DSMessageList getMessages(int userID, int sessionID, EnumGetMsgType::getMsgType type, string from,
	                          string to);

	void messagesSeen(int userID, int sessionID, int seenID);

	void messagesNotified(int userID, int sessionID, int notifiedid);

	DSChatUserData getUserData(int userID);

	void changeUserState(int userID, EnumStatus::status status,
	                     EnumCustomStatusIcon::customStatusIcon customStatusIcon, std::string customstatusText);

	void changeReachState(int userID, EnumStatus::status status);

	int newSession();

	void addUserToSession(int userID, int sessionID);

	void removeUserFromSession(int userID, int sessionID);

	void changeLeader(int userID, int sessionID);

	DSSessionList listSessions(int ID);

	DSSession getSession(int ID);

	zeitoon::usermanagement::DSUserList SessionUserList(int sessionID);

};
}
}
#endif //ZDS_CHAT_HPP
