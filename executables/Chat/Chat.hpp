//
// Created by naeyrin on 5/8/16.
//

#ifndef ZDS_CHAT_HPP
#define ZDS_CHAT_HPP

#include "DSStructs.hpp"

#include <string>

namespace zeitoon {
    namespace chat {

        class ChatCHI;

        class chaT {
            ChatCHI *chatCHI;
        public:
            chaT(ChatCHI *ptr);

            void newMessage(int userID, int sessionID, std::string msg, EnumMsgType::msgType type);


        };
    }
}
#endif //ZDS_CHAT_HPP
