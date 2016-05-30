// Created by naeyrin on 5/8/16.
//

#include "executables/Chat/Chat.hpp"
#include "executables/Chat/chatCHI.hpp"
#include <iostream>

using namespace zeitoon::chat;

int main() {
	try {
		Chatconfig.load();
		zeitoon::chat::ChatCHI *dummy = new zeitoon::chat::ChatCHI;
		dummy->connect("127.0.0.1", 5458);
		std::cout << "==========<< Welcome to Programming Chat >>==========" << endl;
		std::cout << "1- NewSession" << endl;
		std::cout << "2- NewMessage" << endl;
		std::cout << "3- RemoveMessage" << endl;
		std::cout << "4- AddUserToSession" << endl;
		std::cout << "5- CheckNewMessages" << endl;
		std::cout << "6- GetMessages" << endl;
		std::cout << "7- MessagesSeen" << endl;
		std::cout << "8- MessagesNotified" << endl;
		std::cout << "9- GetUserData" << endl;
		std::cout << "10- ChangeUserState" << endl;
		std::cout << "11- ChangeReachState" << endl;
		std::cout << "12- RemoveUserFromSession" << endl;
		std::cout << "13- ChangeLeader" << endl;
		std::cout << "14- ListSessions" << endl;
		std::cout << "15- GetSession" << endl;
		//std::cout << "12- RemoveUserFromSession" << endl;
		int op = 0;
		try {
			do {
				std::cout << "Enter option index(Enter 3 to display menu & 1212 to exit\n#->" << std::endl;
				std::cin >> op;
				try {
					switch (op) {
						case (1): {
							std::cout << "==========<< NewSession >>==========" << endl;
							int d = dummy->chatCore.newSession();
							break;
						}
						case (2): {
							std::cout << "==========<< newMessage >>==========" << endl;
							int userID;
							int sessionID;
							string msg = "";
							string t = "";
							cout << "Enter userID massage: " << endl;
							cin >> userID;

							cout << "Enter sessionID massage: " << endl;
							cin >> sessionID;

							cout << "Enter msg massage: " << endl;
							cin >> msg;

							cout << "Enter type massage: " << endl;
							cin >> t;
							DTEnum<EnumMsgType> type("");
							type.fromString(t);
							auto d = dummy->chatCore.newMessage(userID, sessionID, msg,
							                                    (EnumMsgType::msgType) type.getValue());
							break;
						}
						case (3): {
							std::cout << "==========<< RemoveMessage >>==========" << endl;
							int id;
							cout << "Enter ID massage: " << endl;
							cin >> id;
							dummy->chatCore.removeMessage(id);

							break;
						}
						case (4): {
							std::cout << "==========<< AddUserToSession >>==========" << endl;
							int userID;
							int sessionID;
							cout << "Enter userID massage: " << endl;
							cin >> userID;
							cout << "Enter sessionID massage: " << endl;
							cin >> sessionID;
							dummy->chatCore.addUserToSession(userID, sessionID);

							break;
						}
						case (5): {
							std::cout << "==========<< CheckNewMessages >>==========" << endl;
							int userID;
							cout << "Enter userID massage: " << endl;
							cin >> userID;
							DSCheckMessages a = dummy->chatCore.checkNewMessages(userID);
							cout << a.toString(0) << endl;
							break;
						}
						case (6): {
							std::cout << "==========<< GetMessages >>==========" << endl;
							int userID;
							int sessionID;
							string form = "";
							string to = "";
							string t = "";
							cout << "Enter userID massage: " << endl;
							cin >> userID;

							cout << "Enter sessionID massage: " << endl;
							cin >> sessionID;

							cout << "Enter type (Date,ID,Notified or Seen) massage: " << endl;
							cin >> t;
							if ((seq(t, "date")) || (seq(t, "id"))) {
								cout << "Enter form " + t + " massage: " << endl;
								cin >> form;

								cout << "Enter to " + t + "  massage: " << endl;
								cin >> to;
							}

							DTEnum<EnumGetMsgType> type("");
							type.fromString(t);
							auto a = dummy->chatCore.getMessages(userID, sessionID,
							                                     (EnumGetMsgType::getMsgType) type.getValue(), form,
							                                     to);
							cout << a.toString(0) << endl;
							break;
						}
						case (7): {
							std::cout << "==========<< MessagesSeen >>==========" << endl;
							int userID;
							int sessionID;
							int seenID;
							cout << "Enter userID massage: " << endl;
							cin >> userID;

							cout << "Enter sessionID massage: " << endl;
							cin >> sessionID;

							cout << "Enter seenID massage: " << endl;
							cin >> seenID;
							dummy->chatCore.messagesSeen(userID, sessionID, seenID);
							break;
						}
						case (8): {
							std::cout << "==========<< MessagesNotified >>==========" << endl;
							int userID;
							int sessionID;
							int notifiedid;
							cout << "Enter userID massage: " << endl;
							cin >> userID;

							cout << "Enter sessionID massage: " << endl;
							cin >> sessionID;

							cout << "Enter notifiedID massage: " << endl;
							cin >> notifiedid;
							dummy->chatCore.messagesNotified(userID, sessionID, notifiedid);
							break;
						}
						case (9): {
							std::cout << "==========<< GetUserData >>==========" << endl;
							int userID;
							cout << "Enter userID massage: " << endl;
							cin >> userID;
							auto a = dummy->chatCore.getUserData(userID);
							cout << a.toString(0) << endl;
							break;
						}
						case (10): {
							std::cout << "==========<< ChangeUserState >>==========" << endl;
							int userID;
							string t, t2, customStatusText;
							cout << "Enter userID massage: " << endl;
							cin >> userID;
							cout << "Enter status (normal, custom, invisible or busy) massage: " << endl;
							cin >> t;
							cout << "Enter customStatusIcon (online, offline, busy or away) massage: " << endl;
							cin >> t2;
							cout << "Enter customStatusText massage: " << endl;
							cin >> customStatusText;

							DTEnum<EnumStatus> status("");
							status.fromString(t);
							DTEnum<EnumCustomStatusIcon> customStatusIcon("");
							customStatusIcon.fromString(t2);
							dummy->chatCore.changeUserState(userID, (EnumStatus::status) status.getValue(),
							                                (EnumCustomStatusIcon::customStatusIcon) customStatusIcon.getValue(),
							                                customStatusText);
							break;
						}
						case (11): {
							std::cout << "==========<< ChangeReachState >>==========" << endl;
							int userID;
							string t;
							cout << "Enter userID massage: " << endl;
							cin >> userID;
							cout << "Enter status (normal, custom, invisible or busy) massage: " << endl;
							cin >> t;
							DTEnum<EnumStatus> status("");
							status.fromString(t);
							dummy->chatCore.changeReachState(userID, (EnumStatus::status) status.getValue());
							break;
						}

						case (12): {
							std::cout << "==========<< RemoveUserFromSession >>==========" << endl;
							int userID;
							int sessionID;
							cout << "Enter userID massage: " << endl;
							cin >> userID;

							cout << "Enter sessionID massage: " << endl;
							cin >> sessionID;
							dummy->chatCore.removeUserFromSession(userID, sessionID);
							break;
						}
						case (13): {
							std::cout << "==========<< ChangeLeader >>==========" << endl;
							int userID;
							int sessionID;
							cout << "Enter userID massage: " << endl;
							cin >> userID;

							cout << "Enter sessionID massage: " << endl;
							cin >> sessionID;
							dummy->chatCore.changeLeader(userID, sessionID);
							break;
						}
						case (14): {
							std::cout << "==========<< ListSessions >>==========" << endl;
							int id;
							cout << "Enter ID massage: " << endl;
							cin >> id;
							auto a = dummy->chatCore.listSessions(id);
							cout << a.toString(0) << endl;
							break;
						}
						case (15): {
							std::cout << "==========<< GetSession >>==========" << endl;
							int id;
							cout << "Enter ID massage: " << endl;
							cin >> id;
							auto a = dummy->chatCore.getSession(id);
							cout << a.toString(0) << endl;
							break;
						}
						case (1212):
							return 0;
						default:
							break;

					}//END_Switch
				} catch (exceptionEx *err) {
					std::cerr << "Error: " << err->what() << endl;
				}

			} while (op != 0);
		} catch (exception &err) {
			std::cerr << "Error: " << err.what() << std::endl;
		}


		dummy->sm.joinNet();
	} catch (zeitoon::utility::exceptionEx *rr) {
		std::cerr << "ERR: " << rr->what() << std::endl;
	}
}