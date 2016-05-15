//
// Created by naeyrin on 5/4/16.
//
#include <iostream>
#include "DSStructs.hpp"
#include "chatCHI.hpp"
int main(){
    std::cout << "Hello World";

    try{
        zeitoon::chat::ChatCHI test1;
        test1.connect("127.0.0.1",5458);
        test1.sm.joinNet();
    }
    catch(zeitoon::utility::exceptionEx *err) {
        std::cerr<<"Error: " << err->what() <<endl;

    }

}