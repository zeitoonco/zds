#include "Chat.hpp"
#include "chatCHI.hpp"
#include "ChatUtility.hpp"

namespace zeitoon {
    namespace chat {
        chaT::chaT(ChatCHI *ptr) : chatCHI(ptr) {

        }

        int chaT::newMessage(int userID, int sessionID, std::string msg, EnumMsgType::msgType type) {
            int result = chatCHI->sm.database.executeSync(
                    "INSERT INTO Message ( userID, sessionID, msg, msgDate, type) VALUES (" + to_string(userID) + " ," +
                    to_string(sessionID) + ", " + msg + ", Delault," + to_string(type) + ") returning id");
            if (result > 0) {
                std::cerr << "MSG Sent!" << std::endl;
                chatCHI->sm.communication.runEvent(EventInfo::newMessage(),
                                                   zeitoon::chat::DSNewMessage(userID, sessionID, msg, type).toString(
                                                           true));
                //##Event Fired
                /*int result = stoi(chatCHI->sm.database.singleFieldQuerySync(
                        "SELECT LAST(id) FROM Message"));      ORDER BY id DESC");*/

            }

        }


        void chaT::removeMessage(int id) {
            chatCHI->sm.database.execute("delete from Message where id=" + std::to_string(id));
        }


        DSCheckMessages chaT::checkNewMessages(int userID) {
            size_t i = 0;
            zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
                    "SELECT (sessionid, seenid, notifiedid) FROM sessionuser  WHERE userid = " + to_string(userID));
            vector<DSCheckMessageItem> tempVec;
            do {
                tempVec.push_back(
                        DSCheckMessageItem(std::stoi(result.fieldValue(i, 0)), std::stoi(result.fieldValue(i, 1)),
                                           std::stoi(result.fieldValue(i, 2))));
                i++;
            } while (i < result.rowCount());
            DSCheckMessages temp(tempVec);
            return temp;
        }


        DSMessageList chaT::getMessages(int userID, int sessionID, EnumGetMsgType::getMsgType type, string from,
                                        string to) {
            string FindMagUser = "";
            if (!Strings::compare(EnumGetMsgType::typeString[type], EnumGetMsgType::typeString[EnumGetMsgType::date],
                                  false)) {
                FindMagUser = "userid = '" + to_string(userID) + "' AND sessionid = '" + to_string(sessionID) +
                              "' AND msgdate >= " + from + " AND msgdate<= " + to;

            } else if (!Strings::compare(EnumGetMsgType::typeString[type],
                                         EnumGetMsgType::typeString[EnumGetMsgType::id], false)) {

                FindMagUser = "userid = '" + to_string(userID) + "' AND sessionid = '" + to_string(sessionID) +
                              "' AND id >= " + from + " AND id<= " + to;

            } else if (!Strings::compare(EnumGetMsgType::typeString[type],
                                         EnumGetMsgType::typeString[EnumGetMsgType::notified], false)) {
                std::string result = chatCHI->sm.database.singleFieldQuerySync(
                        "SELECT notifiedid FROM sessionuser WHERE userid = '" + to_string(userID) +
                        "' AND sessionid = '" + to_string(sessionID) + "'");

                FindMagUser = "userid = '" + to_string(userID) + "' AND sessionid = '" + to_string(sessionID) +
                              "' AND AND id > " + result;

            } else if (!Strings::compare(EnumGetMsgType::typeString[type],
                                         EnumGetMsgType::typeString[EnumGetMsgType::seen], false)) {
                std::string result = chatCHI->sm.database.singleFieldQuerySync(
                        "SELECT seenid FROM sessionuser WHERE userid = '" + to_string(userID) +
                        "' AND sessionid = '" + to_string(sessionID) + "'");
                FindMagUser = "userid = '" + to_string(userID) + "' AND sessionid = '" + to_string(sessionID) +
                              "' AND AND id > " + result;
            }

            zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
                    "SELECT (userid, msgdate, msg, type) FROM message WHERE " + FindMagUser);

            vector<DSMessageItem> tempVec;
            int i = 0;
            do {

                tempVec.push_back(
                        DSMessageItem(std::stoi(result.fieldValue(i, 0)), result.fieldValue(i, 1),
                                      result.fieldValue(i, 2),
                                      (EnumMsgType::msgType) std::stoi(result.fieldValue(i, 3))));
                i++;
            } while (i < result.rowCount());
            DSMessageList temp(tempVec);
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

            size_t i = 0;
            zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
                    "SELECT (reachstate, status, customstatusicon, customstatustex)FROM userdata WHERE userid = " +
                    to_string(userID));
            int reachState = stoi(result.fieldValue(0, 0));
            int status = stoi(result.fieldValue(0, 1));
            int customStatusIcon = stoi(result.fieldValue(0, 2));
            std::string customStatus = (result.fieldValue(0, 3));

            DSChatUserData temp((EnumReachState::reachState) reachState, (EnumStatus::status) status,
                                (EnumCustomStatusIcon::customStatusIcon) customStatusIcon, customStatus);
            return temp;
        }

        void chaT::changeUserState(int userID, EnumStatus::status status,//todo Ask MR.Ali
                                   EnumCustomStatusIcon::customStatusIcon customStatusIcon,
                                   std::string customstatusText) {
            chatCHI->sm.database.executeSync(
                    "UPDATE userdata SET status = " + to_string(status) + "customstatusicon =" +
                    to_string(customStatusIcon) + "customstatusText =" +
                    customstatusText + " WHERE userid =" + to_string(userID));
        }

        void chaT::changeReachState(int userID, EnumStatus::status status) {
            chatCHI->sm.database.executeSync(
                    "UPDATE userdata SET reachstate = " + to_string(status) + " WHERE userid =" + to_string(userID));
        }

//todo creationdate Data Rooz Default DataBase (MR.Ali_Navidi)
        int chaT::newSession() {

            int result = stoi(chatCHI->sm.database.singleFieldQuerySync(
                    "INSERT INTO session ( id,creationdate) VALUES (default,default) returning id"));
            if (result > 0) {
                /*  std::cerr << "MSG Sent!" << std::endl; //todo MR.Aliiiiiiiiiiiiiiiiiiii
                  chatCHI->sm.communication.runEvent(EventInfo::sessionUserAdded(),
                                                     zeitoon::chat::DSAddUserSession(user,default).toString(
                                                             true));
                  //##Event Fired
                  /* int result = stoi(chatCHI->sm.database.singleFieldQuerySync(
                          "SELECT LAST(id) FROM session"));     */

            }
        }

//todo MR.Ali
        void chaT::addUserToSession(int userID, int sessionID) {
            int result = chatCHI->sm.database.executeSync(
                    "INSERT INTO sessionuser ( userid, sessionid, joined, seenid, notifiedid) VALUES (" +
                    to_string(userID) + " ," +
                    to_string(sessionID) + ",Default,null,null)");

        }

        void chaT::removeUserFromSession(int userID, int sessionID) {

            chatCHI->sm.database.execute(
                    "DELETE FROM sessionuser WHERE userid=" + to_string(userID) + " AND sessionid=" +
                    to_string(sessionID));

//If the user in the 'Session' was another of the 'Session' is deleted if the 'Leader' is not
            std::string result = chatCHI->sm.database.singleFieldQuerySync(
                    "SELECT (sessionid) FROM sessionuser WHERE userid =" + to_string(userID) + " AND Leader = 'False'");
            if (result.length() == 0) {
                std::string result = chatCHI->sm.database.singleFieldQuerySync(
                        "SELECT (Leader) FROM sessionuser WHERE userid =" + to_string(userID));
                if (result == "False") {
                    chatCHI->sm.database.execute(
                            "DELETE FROM session WHERE id=" + to_string(userID));
                }
            }
        }


    void chaT::changeLeader(int userID, int sessionID, bool Leader) {
        chatCHI->sm.database.executeSync(
                "UPDATE sessionuser SET Leader = 'False'  WHERE Leader = 'True' AND sessionID = '" +
                to_string(sessionID) + "' AND NOT(userID = '" + to_string(userID) + "')");


        int result = chatCHI->sm.database.executeSync(
                "UPDATE sessionuser SET Leader ='True'  WHERE userID = '" + to_string(userID) +
                "' AND sessionID = '" + to_string(sessionID) + "'");
    }


    DSSessionList chaT::listSessions(int ID) {
        zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
                "SELECT (sessionid) FROM sessionuser WHERE userid=" +
                to_string(ID));

        vector<DTInteger<int>> tempVec;
        DTInteger<> tempInt = {"sessionID"};
        int i = 0;
        do {
            tempInt = stoi(result.fieldValue(i, 0));
            tempVec.push_back(tempInt);
            i++;
        } while (i < result.rowCount());
        return DSSessionList(tempVec);
    }

    DSSession chaT::getSession(int sessionID) {
        zeitoon::datatypes::DTTableString result = chatCHI->sm.database.querySync(
                "SELECT (id,creationdate) FROM session WHERE id =" + to_string(sessionID));

        return DSSession(std::stoi(result.fieldValue(0, 0)), result.fieldValue(0, 1));
    }


}
}