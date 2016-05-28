//
// Created by naeyrin on 5/8/16.
//

#include "executables/Chat/DSStructs.hpp"
#include <iostream>

using namespace zeitoon::chat;
using namespace std;

int main() {
    try {
        DSCheckMessageItem a("");
        cout<<a.toString();


    }catch(zeitoon::utility::exceptionEx *rr){
        std::cerr<< "ERR: "<<rr->what()<<std::endl;
    }
}