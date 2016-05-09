//
// Created by naeyrin on 5/9/16.
//

#ifndef ZDS_CHATCONFIG_HPP
#define ZDS_CHATCONFIG_HPP

#include "utility/ConfigManager.hpp"

namespace zeitoon {
    namespace chat {


        class ChatConfig : public zeitoon::utility::ConfigManager {
        public:
            DTString serviceID = {"serviceID"};
            DTString serverIP = {"serverIP"};
            DTString serverPort = {"serverPort"};

            ChatConfig() : ConfigManager("Chat", "ChatConfig") {
                this->add(&serviceID);
                this->add(&serverIP);
                this->add(&serverPort);
            }

            ~ChatConfig() {
                this->save();
            }
        };


    }
}
extern zeitoon::chat::ChatConfig Chatconfig;
#endif //ZDS_CHATCONFIG_HPP
