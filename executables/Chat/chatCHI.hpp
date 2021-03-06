//
// Created by naeyrin on 5/4/16.
//

#ifndef ZDS_CHATCHI_HPP
#define ZDS_CHATCHI_HPP


#include "mediator/CommunicationHandlerInterface.hpp"
#include "Chat.hpp"
#include "chatConfig.hpp"
#include "utility/DTStructs.hpp"
#include "DSStructs.hpp"

using namespace zeitoon::datatypes;
namespace zeitoon {
    namespace chat {
        class ChatCHI : public zeitoon::utility::CommunicationHandlerInterface {
            zeitoon::datatypes::DSInstallInfo insInfo;
            void setInstallInfo();
        public:
            chaT chatCore;

            ChatCHI() : CommunicationHandlerInterface(this), chatCore(this) , insInfo("Chat","Chat", 1, 1, EnmServiceType::Chat) {
                setInstallInfo();
            }

            bool onCommand(string node, string data, string id, string from, std::string &resultStr);

            void onCallback(string node, string data, string id, string from, string success);

            void onEvent(string node, string data, string from);

            void onInstall(string id);

            void onEnable();

            void onDisable();

            void onUninstall();

            void onConnect();

            void onDisconnect();

            string getInstallInfo();

            string getInstallID();

            string getServiceName();

            size_t getServiceVersion();

            string changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion,
                                         int &newVersion);

            void onError(string node, string id, string description);

            void onWarning(string level, string node, string id, string description);

            void pong(string id, int miliseconds);
        };
    }
}
#endif //ZDS_CHATCHI_HPP
