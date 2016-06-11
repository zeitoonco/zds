#include "Chat.hpp"
#include "chatCHI.hpp"
#include "ChatUtility.hpp"

using namespace zeitoon::usermanagement;

namespace zeitoon {
namespace chat {
chaT::chaT(ChatCHI *ptr) : chatCHI(ptr) {

}

int chaT::newMessage(int userID, int sessionID, std::string msg, EnumMsgType::msgType type) {
	int result = chatCHI->sm.database.executeSync(
			"INSERT INTO Message ( userID, sessionID, msg, msgDate, type) VALUES (" + to_string(userID) + " ," +
			to_string(sessionID) + ", '" + msg + "', default," + to_string(type) + ") returning id");
	if (result > 0) {
		std::cerr << "MSG Sent!" << std::endl;
		chatCHI->sm.communication.runEvent(EventInfo::newMessageReceived(),
		                                   zeitoon::chat::DSNewMessage(userID, sessionID, msg, type).toString(
				                                   true));
		return result;
		//##Event Fired
		/*int result = stoi(chatCHI->sm.database.singleFieldQuerySync(
				"SELECT LAST(id) FROM Message"));      ORDER BY id DESC");*/

	}

}


void chaT::removeMessage(int id, int userID) {

	chatCHI->sm.database.execute(
			"delete from Message where id=" + std::to_string(id) + " AND userid = " + to_string(userID));
}


DSCheckMessages chaT::checkNewMessages(int userID) {
	zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
			"SELECT sessionid, seenid, notifiedid FROM sessionuser where userid =" + to_string(userID));

	DSCheckMessages temp;
	for (size_t i = 0; i < result.rowCount(); i++) {
		temp.mlist.add(new DSCheckMessageItem(std::stoi(result.fieldValue(i, 0)),
		                                      result.fieldIsNull(i, 1) ? -1 : std::stoi(
				                                      result.fieldValue(i, 1)),
		                                      result.fieldIsNull(i, 2) ? -1 : std::stoi(
				                                      result.fieldValue(i, 2))), true);
	}
	return temp;
}


DSMessageList chaT::getMessages(int userID, int sessionID, EnumGetMsgType::getMsgType type, string from,
                                string to) {
	//userid = " + to_string(userID) + " AND
	string FindMagUser = "";
	if (type == EnumGetMsgType::date) {
		if ((utility::Strings::trim(from).length() > 0) && ((utility::Strings::trim(to).length() <= 0) ||
		                                                    (utility::Strings::trim(to) == "null"))) {
			FindMagUser = "((sessionid = " + to_string(sessionID) +
			              ") AND (msgdate >= '" + from + "'))";
		}
		else if ((from.length() > 0) && (to.length() > 0)) {
			FindMagUser = "((sessionid = " + to_string(sessionID) +
			              ") AND (msgdate >= '" + from + "' AND msgdate<= '" + to + "))";
		}
	}

	else if (type == EnumGetMsgType::id) {
		if ((utility::Strings::trim(from).length() > 0) && ((utility::Strings::trim(to).length() <= 0) ||
		                                                    (utility::Strings::trim(to) == "null"))) {
			FindMagUser = "((sessionid = " + to_string(sessionID) +
			              ") AND (id >= " + from + "))";
		}
		else if ((from.length() > 0) && (to.length() > 0)) {
			FindMagUser = "((usessionid = " + to_string(sessionID) +
			              ") AND (id >= " + from + " AND id<= " + to + "))";
		}
	}

	else if (type == EnumGetMsgType::notified) {
		std::string result = chatCHI->sm.database.singleFieldQuerySync(
				"SELECT notifiedid FROM sessionuser WHERE sessionid = '" + to_string(sessionID) + "'");

		FindMagUser = "((sessionid = " + to_string(sessionID) +
		              ") AND (id > " + result + "))";

	} else if (type == EnumGetMsgType::seen) {
		std::string result = chatCHI->sm.database.singleFieldQuerySync(
				"SELECT seenid FROM sessionuser WHERE sessionid = '" + to_string(sessionID) + "'");
		FindMagUser = "((userid = " + to_string(userID) + " AND sessionid = " + to_string(sessionID) +
		              ") AND (id > " + result + "))";
	}

	zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
			"SELECT id,userid, msgdate, msg, type FROM message WHERE " + FindMagUser);

	DSMessageList temp;
	for (int i = 0; i < result.rowCount(); i++)
		temp.mmlist.add(new DSMessageItem(std::stoi(result.fieldValue(i, 0)),
		                                  std::stoi(result.fieldValue(i, 1)),
		                                  result.fieldValue(i, 2),
		                                  result.fieldValue(i, 3),
		                                  (EnumMsgType::msgType) std::stoi(result.fieldValue(i, 4))), true);
	return temp;
}


void chaT::messagesSeen(int userID, int sessionID, int seenID) {
	int result = chatCHI->sm.database.executeSync(
			"UPDATE sessionuser SET seenID = " + to_string(seenID) + " WHERE userID = '" + to_string(userID) +
			"' AND sessionID = '" + to_string(sessionID) + "'");
}

void chaT::messagesNotified(int userID, int sessionID, int notifiedid) {
	int result = chatCHI->sm.database.executeSync(
			"UPDATE sessionuser SET notifiedid = " + to_string(notifiedid) + " WHERE userID = '" +
			to_string(userID) +
			"' AND sessionID = '" + to_string(sessionID) + "'");
}

DSChatUserData chaT::getUserData(int userID) {
	chatCHI->sm.database.execute("INSERT INTO userdata (userid) SELECT " + to_string(userID) +
	                             " WHERE NOT EXISTS ("
			                             "SELECT (userid) FROM userdata WHERE userid = " + to_string(userID) + ");");

	zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
			"SELECT reachstate, status, customstatusicon, customstatustext FROM userdata WHERE userid = " +
			to_string(userID));
	int reachState = 0, status = 0, customStatusIcon = 0;
	std::string customStatus = "";
	if (result.rowCount() > 0) {
		reachState = stoi(result.fieldValue(0, 0));
		status = stoi(result.fieldValue(0, 1));
		customStatusIcon = stoi(result.fieldValue(0, 2));
		customStatus = (result.fieldValue(0, 3));
	}
	DSChatUserData temp((EnumReachState::reachState) reachState, (EnumStatus::status) status,
	                    (EnumCustomStatusIcon::customStatusIcon) customStatusIcon, customStatus, userID);

	return temp;

	/*else{

		int result = stoi(chatCHI->sm.database.singleFieldQuerySync(
				"INSERT INTO userdata ( userid, status, customstatustext) VALUES ("+ to_string(userID) +" , Normal, Normal"));
	}*/
}

void chaT::changeUserState(int userID, EnumStatus::status status,
                           EnumCustomStatusIcon::customStatusIcon customStatusIcon,
                           std::string customStatus) {
	chatCHI->sm.database.executeSync(
			"UPDATE userdata SET status = " + to_string(status) + ", customstatusicon =" +
			to_string(customStatusIcon) + ", customstatusText ='" +
					customStatus + "' WHERE userid =" + to_string(userID));

	chatCHI->sm.communication.runEvent(EventInfo::userStateChanged(),
	                                   zeitoon::chat::DSChangeUserState(userID, status, customStatusIcon,
	                                                                    customStatus).toString(true));
}

void chaT::changeReachState(int userID, EnumStatus::status status) {
	chatCHI->sm.database.executeSync(
			"UPDATE userdata SET reachstate = " + to_string(status) + " WHERE userid =" + to_string(userID));
}

int chaT::newSession(DSListUserID *userIDList) {

	int sessionID = stoi(chatCHI->sm.database.singleFieldQuerySync(
			"INSERT INTO session ( id,creationdate,leader) VALUES (default,default," +
			std::to_string(userIDList->userIDlist[0]->value()) + ") returning id"));
	/*if (sessionID <= 0)
		EXTDBError("Creating session failed");*/
//=======<< Added UserToSession >>=======
	for (int i = 0; userIDList->userIDlist.length() > i; i++) {
		int result = chatCHI->sm.database.executeSync(
				"INSERT INTO sessionuser ( userid, sessionid, joined, seenid, notifiedid) VALUES (" +
				to_string(userIDList->userIDlist[i]->value()) + " ," +
				to_string(sessionID) + ",Default,null,null)");

		chatCHI->sm.communication.runEvent(EventInfo::sessionUserAdded(),
		                                   zeitoon::chat::DSAddUserSession(userIDList->userIDlist[i]->value(),
		                                                                   sessionID).toString(
				                                   true));
	}
	return sessionID;
}

/*void chaT::addUserToSession(int userID, int sessionID) {
	int result = chatCHI->sm.database.executeSync(
			"INSERT INTO sessionuser ( userid, sessionid, joined, seenid, notifiedid) VALUES (" +
			to_string(userID) + " ," +
			to_string(sessionID) + ",Default,null,null)");

	chatCHI->sm.communication.runEvent(EventInfo::sessionUserAdded(),
	                                   zeitoon::chat::DSAddUserSession(userID, sessionID).toString(
			                                   true));
}
*/
void chaT::removeUserFromSession(int userID, int sessionID) {

//If the user in the 'Session' was another of the 'Session' is deleted if the 'Leader' is not
	int result = stoi(chatCHI->sm.database.singleFieldQuerySync(
			"SELECT leader FROM session WHERE id = " + to_string(sessionID)  + "")); //Search UserID Leader

	int result2=0;

	int temp = stoi(chatCHI->sm.database.singleFieldQuerySync("SELECT count(userid) FROM sessionuser WHERE sessionid = " + to_string(sessionID)));
	//if (result > 0) {

	if ((temp >1) && (result == userID)) {
		result2 = stoi(chatCHI->sm.database.singleFieldQuerySync(
				"SELECT userid FROM sessionuser WHERE sessionid = " + to_string(sessionID) + " AND NOT(userid = " +
				to_string(result) + ")"));
		chatCHI->sm.database.executeSync(
				"UPDATE session SET Leader = " + to_string(result2) + " WHERE id =" + to_string(sessionID));
	}
	//EXTDBError("user is leader. cannot remove leader from a session");
	//}

	chatCHI->sm.database.execute(
			"DELETE FROM sessionuser WHERE userid=" + to_string(userID) + " AND sessionid=" +
			to_string(sessionID));

	if (temp == 1)
	{
		chatCHI->sm.database.execute(
				"DELETE FROM session WHERE id=" + to_string(sessionID));

		chatCHI->sm.database.execute(
				"DELETE FROM message WHERE userid=" + to_string(userID) + " AND sessionid = " +
				to_string(sessionID));

	}
	chatCHI->sm.communication.runEvent(EventInfo::sessionUserRemoved(),
	                                   zeitoon::chat::DSAddUserSession(userID, sessionID).toString(
			                                   true));
}


void chaT::changeLeader(int userID, int sessionID) {
	chatCHI->sm.database.executeSync(
			"UPDATE sessionuser SET Leader = 'False'  WHERE Leader = 'True' AND sessionID = '" +
			to_string(sessionID) + "' AND NOT(userID = '" + to_string(userID) + "')");


	int result = chatCHI->sm.database.executeSync(
			"UPDATE sessionuser SET Leader ='True'  WHERE userID = '" + to_string(userID) +
			"' AND sessionID = '" + to_string(sessionID) + "'");

	chatCHI->sm.communication.runEvent(EventInfo::sessionLeaderChanged(),
	                                   zeitoon::chat::DSAddUserSession(userID, sessionID).toString(
			                                   true));
}


DSSessionList chaT::listSessions(int ID) {
	zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
			"SELECT (sessionid) FROM sessionuser WHERE userid=" +
			to_string(ID));

	DSSessionList res;
	for (int i = 0; i < result.rowCount(); i++) {
		DTInteger<> *tempInt = new DTInteger<>("", stoi(result.fieldValue(i, 0)));
		res.mnlist.add(tempInt, true);
	}
	std::cerr << "SESSION LIST: " << res.toString(true);
	return (res);
}

DSSession chaT::getSession(int sessionID) {
	zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
			"SELECT (id,creationdate) FROM session WHERE id =" + to_string(sessionID));

	return DSSession(std::stoi(result.fieldValue(0, 0)), result.fieldValue(0, 1));
}


zeitoon::usermanagement::DSUserList chaT::SessionUserList(int sessionID) {
	DSUserList temp;
	zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
			"SELECT userid FROM sessionuser WHERE sessionid = " + to_string(sessionID));
	for (auto i = 0; i < result.rowCount(); i++) {
		DSInteger userID;
		userID.value = std::stoi(result.fieldValue(i, 0));
		std::string a = chatCHI->sm.communication.runCommandSync(zeitoon::usermanagement::commandInfo::getUserInfo(),
		                                                         userID);
		temp.usersList.addJson(a);
	}
	return temp;
}


}
}