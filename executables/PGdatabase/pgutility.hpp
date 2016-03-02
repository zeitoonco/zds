/*
 * pgutility.hpp
 *
 *  Created on: Mar 1, 2016
 *      Author: inf
 */

#ifndef PGDATABASE_PGUTILITY_HPP_
#define PGDATABASE_PGUTILITY_HPP_

#include <string>

namespace zeitoon {
namespace pgdatabase {

class eventInfo {
public:
	eventInfo() {
	}
	static std::string newUser;
	static std::string userLogin;

};
class commandInfo {
public:
	commandInfo() {
	}
	static std::string query;
	static std::string execute;
	static std::string singlefieldquery;
};

}
}

#endif /* PGDATABASE_PGUTILITY_HPP_ */
