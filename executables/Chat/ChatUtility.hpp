//
// Created by naeyrin on 5/8/16.
//

#ifndef ZDS_CHATUTILITY_HPP
#define ZDS_CHATUTILITY_HPP

#include "string"
#include "utility/logger.hpp"
namespace zeitoon {
namespace chat {
class CommandInfo {
public:
	static std::string newMessage() { return "chat.newMessage"; }

	static std::string removeMessage() { return "chat.removeMessage"; }

	static std::string checkNewMessages() { return "chat.checkNewMessages"; }

	static std::string getMessages() { return "chat.getMessages"; }

	static std::string messagesSeen() { return "chat.messagesSeen"; }

	static std::string messagesNotified() { return "chat.messagesNotified"; }

	static std::string getUserData() { return "chat.getUserData"; }

	static std::string changeUserState() { return "chat.changeUserState"; }

	static std::string changeReachState() { return "chat.changeReachState"; }

	static std::string newSession() { return "chat.newSession"; }

	static std::string addUserToSession() { return "chat.addUserToSession"; }

	static std::string removeUserFromSession() { return "chat.removeUserFromSession"; }

	static std::string changeLeader() { return "chat.changeLeader"; }

	static std::string listSessions() { return "chat.listSessions"; }

	static std::string getSession() { return "chat.getSession"; }

	static std::string SessionUserList() { return "chat.SessionUserList"; }

	static std::string removeSession() { return "chat.removeSession"; }


};

class EventInfo {
public:
	static std::string newMessageReceived() { return "chat.newMessageReceived"; }

	static std::string sessionUserAdded() { return "chat.sessionUserAdded"; }

	static std::string sessionUserRemoved() { return "chat.sessionUserRemoved"; }

	static std::string sessionLeaderChanged() { return "chat.sessionLeaderChanged"; }

	static std::string userStateChanged() { return "chat.userStateChanged"; }
};

}
}


#endif //ZDS_CHATUTILITY_HPP
