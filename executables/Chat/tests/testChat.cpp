//
// Created by naeyrin on 5/8/16.
//

#include "executables/Chat/Chat.hpp"
#include "executables/Chat/chatCHI.hpp"
#include <iostream>


int main() {
    try {
        zeitoon::chat::ChatCHI *dummy = new zeitoon::chat::ChatCHI;
        dummy->connect("127.0.0.1",5458);


        dummy->chatCore.newMessage(1, 2, "ssss", zeitoon::chat::EnumMsgType::user);
    }catch(zeitoon::utility::exceptionEx *rr){
        std::cerr<< "ERR: "<<rr->what()<<std::endl;
    }
}