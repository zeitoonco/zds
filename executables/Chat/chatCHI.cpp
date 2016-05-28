#include "chatCHI.hpp"
#include "DSStructs.hpp"
#include "ChatUtility.hpp"
#include <string>

#include <mediator/CommunicationHandlerInterface.hpp>
#include "datatypes/dtsingletypes.hpp"
#include <fstream>

using namespace zeitoon::datatypes;
namespace zeitoon {
    namespace chat {


        void ChatCHI::onCommand(string node, string data, string id, string from) {
            if (!Strings::compare(node, CommandInfo::newMessage(), false)) {
                DSNewMessage input(data);
                DSInteger result;
                result.value = chatCore.newMessage(input.userID.getValue(), input.sessionID.getValue(),
                                                   input.msg.getValue(),
                                                   (EnumMsgType::msgType) input.type.getValue());
                sm.communication.runCallback(node, result.toString(true), id);
            }

            else if (!Strings::compare(node, CommandInfo::removeMessage(), false)) {
                DSInteger temp;
                temp.fromString(data);
                chatCore.removeMessage(temp.value.getValue());
            }

            else if (!Strings::compare(node, CommandInfo::checkNewMessages(), false)) {
                DSInteger temp;
                temp.fromString(data);
                DSCheckMessages result;
                result = chatCore.checkNewMessages(temp.value.getValue());
                sm.communication.runCallback(node, result.toString(true), id);
            }


            else if (!Strings::compare(node, CommandInfo::getMessages(), false)) {
                DSGetMessages temp(data);

                DSMessageList result = chatCore.getMessages(temp.userID.getValue(), temp.sessionID.getValue(),
                                     (EnumGetMsgType::getMsgType) temp.type.getValue(), temp.from.getValue(),
                                     temp.to.getValue());
                sm.communication.runCallback(node, result.toString(true), id);
            }
            else if (!Strings::compare(node, CommandInfo::messagesSeen(), false)) {
                DSSeen temp(data);
                chatCore.messagesSeen(temp.userID.getValue(), temp.sessionID.getValue(), temp.seenID.getValue());
            }
            else if (!Strings::compare(node, CommandInfo::messagesNotified(), false)) {
                DSSeen temp(data);
                chatCore.messagesNotified(temp.userID.getValue(), temp.sessionID.getValue(), temp.seenID.getValue());
            }
            else if (!Strings::compare(node, CommandInfo::getUserData(), false)) {
                DSInteger temp;
                temp.fromString(data);

                DSChatUserData result = chatCore.getUserData(temp.value.getValue());
                sm.communication.runCallback(node, result.toString(true), id);
            }
            else if (!Strings::compare(node, CommandInfo::changeUserState(), false)) {
                DSChangeUserState temp(data);
                chatCore.changeUserState(temp.userID.getValue(), (EnumStatus::status) temp.status.getValue(),
                                         (EnumCustomStatusIcon::customStatusIcon) temp.customStatusIcon.getValue(),
                                         temp.customStatus.getValue());

            }
            else if (!Strings::compare(node, CommandInfo::changeReachState(), false)) {
                DSChangeUserReachState temp(data);
                chatCore.changeReachState(temp.userID.getValue(), (EnumStatus::status) temp.status.getValue());
            }
            else if (!Strings::compare(node, CommandInfo::newSession(), false)) {
                DSInteger result;
                result.value = chatCore.newSession();
                sm.communication.runCallback(node, result.toString(true), id);
            }
            else if (!Strings::compare(node, CommandInfo::addUserToSession(), false)) {
                DSAddUserSession temp(data);
                chatCore.addUserToSession(temp.userID.getValue(), temp.sessionID.getValue());
            }
            else if (!Strings::compare(node, CommandInfo::removeUserFromSession(), false)) {
                DSAddUserSession temp(data);
                chatCore.addUserToSession(temp.userID.getValue(), temp.sessionID.getValue());
            }
            else if (!Strings::compare(node, CommandInfo::changeLeader(), false)) {
                DSAddUserSession temp(data);
                chatCore.addUserToSession(temp.userID.getValue(), temp.sessionID.getValue());
            }
            else if (!Strings::compare(node, CommandInfo::listSessions(), false)) {
                DSInteger temp;
                temp.fromString(data);
                DSSessionList result = chatCore.listSessions(
                        temp.value.getValue());//see @ajl: if we "DSSessionList result; result=..." raises segmentation fault. WHY????
                sm.communication.runCallback(node, result.toString(true), id);
            }
            else if (!Strings::compare(node, CommandInfo::getSession(), false)) {
                DSInteger temp;
                temp.fromString(data);
                DSSession result;
                result = chatCore.getSession(temp.value.getValue());
                sm.communication.runCallback(node, result.toString(true), id);
            }

        }

        void ChatCHI::onCallback(string node, string data, string id, string from) {

        }

        void ChatCHI::onEvent(string node, string data, string from) {

        }

        void ChatCHI::onInstall(string id) {
            string cpath = FileSystemUtility::getAppPath();
            //Addressing the file and checking Database tables
            string temp = cpath + "DBTableScripts.sql";
            std::cout << temp << endl;
            std::ifstream t(temp);
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            int res;
            try {
                res = sm.database.executeSync(str);
            } catch (exceptionEx *errorInfo) {
                EXTDBErrorI("Unable to create default tables for Chat", errorInfo);
            }
            if (res == -1)
                EXTDBError("Sql error");

            //set serviceID in confMgr

            Chatconfig.serviceID = id;
            Chatconfig.save();
        }

        void ChatCHI::onEnable() {
            //--------register cmds
            std::string temp = "";
            size_t length = insInfo.commands.length();
            for (size_t i = 0; i < length; i++) {
                temp += (i == 0 ? "" : ",") + insInfo.commands[i]->name.toString();
            }
            sm.communication.registerCommand(temp);
            std::cout << temp << endl;

            //--------register events
            length = insInfo.events.length();
            temp = "";
            for (size_t i = 0; i < length; i++) {
                temp += (i == 0 ? "" : ",") + insInfo.events[i]->name.toString();
            }
            sm.communication.registerEvent(temp);
            std::cout << temp << endl;

            //--------register hooks
            length = insInfo.hooks.length();
            temp = "";
            for (size_t i = 0; i < length; i++) {
                temp += (i == 0 ? "" : ",") + insInfo.hooks[i]->name.toString();
            }
            sm.communication.registerHook(temp);
            std::cout << temp << endl;

        }

        void ChatCHI::onDisable() {

        }

        void ChatCHI::onUninstall() {
            //TODO Chatconfig.serviceID = "";
            Chatconfig.serviceID = "";
            Chatconfig.save();
        }

        void ChatCHI::onConnect() {
            std::cerr << "\n+Chat Connected to server\n";
        }

        void ChatCHI::onDisconnect() {
            std::cerr << "\n-Chat Disconnected from server\n";
        }

        string ChatCHI::getInstallInfo() {
            return insInfo.toString(true);//todo insInfo Bayad takmil bashe...
        }

        string ChatCHI::getInstallID() {
            return Chatconfig.serviceID.getValue();
        }

        string ChatCHI::getServiceName() {
            return "Chat";
        }

        size_t ChatCHI::getServiceVersion() {
            return 1;
        }

        string ChatCHI::changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion,
                                              int &newVersion) {
            return "";
        }

        void ChatCHI::onError(string node, string id, string description) {
            std::cerr << "Error:\t" << description << std::endl;
        }

        void ChatCHI::onWarning(string level, string node, string id, string description) {
            std::cerr << "Warning:\t" << description << std::endl;
        }

        void ChatCHI::pong(string id, int miliseconds) {
            sm.send(
                    "{\"type\" : \"internal\" , \"node\" : \"ping\" , \"name\" : \"" + getServiceName() +
                    "\" , \"version\" : \"5\""
                    + (id.length() > 0 ? " , \"id\" : \"" + getInstallID() + "\"" : "") + "}");
        }


        void ChatCHI::setInstallInfo() {
            //---------------------Commands
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::newMessage(), DSNewMessage::getStructName(),
                                                       DSNewMessage::getStructVersion(),
                                                       DSInteger::getStructName(), DSInteger::getStructVersion()),
                    true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::removeMessage(), DSInteger::getStructName(),
                                                       DSInteger::getStructVersion(),
                                                       "", 0), true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::checkNewMessages(), DSInteger::getStructName(),
                                                       DSInteger::getStructVersion(),
                                                       DSCheckMessages::getStructName(),
                                                       DSCheckMessages::getStructVersion()),
                    true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::getMessages(), DSGetMessages::getStructName(),
                                                       DSGetMessages::getStructVersion(),
                                                       DSMessageList::getStructName(),
                                                       DSMessageList::getStructVersion()),
                    true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::messagesSeen(), DSInteger::getStructName(),
                                                       DSInteger::getStructVersion(),
                                                       "", 0), true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::messagesNotified(), DSInteger::getStructName(),
                                                       DSInteger::getStructVersion(),
                                                       "", 0), true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::getUserData(), DSGetMessages::getStructName(),
                                                       DSGetMessages::getStructVersion(),
                                                       DSChatUserData::getStructName(),
                                                       DSChatUserData::getStructVersion()),
                    true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::changeUserState(),
                                                       DSChangeUserState::getStructName(),
                                                       DSChangeUserState::getStructVersion(),
                                                       "", 0), true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::changeReachState(),
                                                       DSChangeUserState::getStructName(),
                                                       DSChangeUserState::getStructVersion(),
                                                       "", 0), true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::newSession(), "",
                                                       0,
                                                       DSInteger::getStructName(),
                                                       DSInteger::getStructVersion()),
                    true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::addUserToSession(),
                                                       DSAddUserSession::getStructName(),
                                                       DSAddUserSession::getStructVersion(),
                                                       "", 0), true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::removeUserFromSession(), DSInteger::getStructName(),
                                                       DSInteger::getStructVersion(),
                                                       "", 0), true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::changeLeader(), DSAddUserSession::getStructName(),
                                                       DSAddUserSession::getStructVersion(),
                                                       "", 0), true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::listSessions(), DSInteger::getStructName(),
                                                       DSInteger::getStructVersion(),
                                                       DSSessionList::getStructName(),
                                                       DSSessionList::getStructVersion()),
                    true);
            insInfo.commands.add(
                    new DSInstallInfo::DSCommandDetail(CommandInfo::getSession(), DSInteger::getStructName(),
                                                       DSInteger::getStructVersion(),
                                                       DSSession::getStructName(),
                                                       DSSession::getStructVersion()),
                    true);


            //-------------------EVENTS
            insInfo.events.add(
                    new DSInstallInfo::DSEventDetail(EventInfo::newMessage(), DSNewMessage::getStructName(),
                                                     DSNewMessage::getStructVersion()), true);
            insInfo.events.add(
                    new DSInstallInfo::DSEventDetail(EventInfo::sessionUserAdded(), DSAddUserSession::getStructName(),
                                                     DSAddUserSession::getStructVersion()), true);
            insInfo.events.add(
                    new DSInstallInfo::DSEventDetail(EventInfo::sessionUserRemoved(), DSAddUserSession::getStructName(),
                                                     DSAddUserSession::getStructVersion()), true);
            insInfo.events.add(
                    new DSInstallInfo::DSEventDetail(EventInfo::sessionLeaderChanged(),
                                                     DSAddUserSession::getStructName(),
                                                     DSAddUserSession::getStructVersion()), true);
            insInfo.events.add(
                    new DSInstallInfo::DSEventDetail(EventInfo::userStateChanged(), DSChangeUserState::getStructName(),
                                                     DSChangeUserState::getStructVersion()), true);

        }


    }
}
