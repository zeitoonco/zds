//
// Created by naeyrin on 5/8/16.
//

#include "executables/Chat/chatCHI.hpp"
#include <iostream>


int main() {
  //  std::cout <<"HELLO";
    zeitoon::chat::ChatCHI m;
    m.connect("127.0.0.1",5458);
m.sm.joinNet();

}