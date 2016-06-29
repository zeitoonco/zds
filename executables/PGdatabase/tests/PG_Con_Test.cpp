//
// Created by inf on 6/22/16.
//

#include <executables/PGdatabase/Connection.hpp>
using namespace zeitoon;
int main(){
    zeitoon::pgdatabase::Connection x("postgres", "1234", "127.0.0.1", 5432, "zds");

}
