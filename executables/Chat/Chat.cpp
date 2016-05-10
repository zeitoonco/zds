#include "Chat.hpp"
#include "chatCHI.hpp"

namespace zeitoon {
    namespace chat {
        chaT::chaT(ChatCHI *ptr) : chatCHI(ptr) {

        }

        void chaT::newMessage(int userID, int sessionID, std::string msg, EnumMsgType::msgType type) {
            chatCHI->sm.database.executeSync(
                    "INSERT INTO Message ( userID, sessionID, msg, msgDate, type) VALUES (userID ,sessionID, msg, current timestamp, type )");
        }


    }
}