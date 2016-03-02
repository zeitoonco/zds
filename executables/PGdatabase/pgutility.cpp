/*
 * pgutility.cpp
 *
 *  Created on: Mar 1, 2016
 *      Author: inf
 */

#include "pgutility.hpp"


namespace zeitoon {
namespace pgdatabase {


//-------------------Set Event Names
std::string eventInfo::newUser = "database.newUser";
std::string eventInfo::userLogin = "database.userLogin";

//------------------Set Command Names
std::string commandInfo::query = "database.query";
std::string commandInfo::execute = "databasse.execute";
std::string commandInfo::singlefieldquery = "database.singlefieldquery";

}
}
