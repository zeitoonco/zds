/*
 * pgutility.hpp
 *
 *  Created on: Mar 1, 2016
 *      Author: inf
 */

#ifndef PGDATABASE_PGUTILITY_HPP_
#define PGDATABASE_PGUTILITY_HPP_

#include"postgresql/libpq-fe.h"
#include <string>
#include <iostream>

namespace zeitoon {
namespace pgdatabase {

class eventInfo {
public:
	eventInfo() {
	}

	//-------------------Set Event Names
	static std::string newUser() { return "database.newUser"; }

	static std::string userLogin() { return "database.userLogin"; }

};

class commandInfo {
public:
	commandInfo() {
	}

	//------------------Set Command Names
	static std::string query() { return "database.query"; }

	static std::string execute() { return "database.execute"; }

	static std::string singlefieldquery() { return "database.singlefieldquery"; }
};

class PGutility {
public:
	static bool isValidResult(const PGresult *res, std::string &description) {
		description = "";
		switch (PQresultStatus(res)) {
			case (PGRES_FATAL_ERROR):
				description = std::string(PQresultErrorMessage(res)) + "PGRES_FATAL_ERROR";
				break;
			case (PGRES_BAD_RESPONSE):
				description = std::string(PQresultErrorMessage(res)) + "PGRES_BAD_RESPONSE";
				break;
			case (PGRES_COMMAND_OK):
				return true;
			case (PGRES_COPY_BOTH):
				break;
			case (PGRES_COPY_IN):
				break;
			case (PGRES_COPY_OUT):
				break;
			case (PGRES_EMPTY_QUERY):
				description = std::string(PQresultErrorMessage(res)) + "PGRES_EMPTY_QUERY";
				break;
			case (PGRES_NONFATAL_ERROR):
				description = std::string(PQresultErrorMessage(res)) + "PGRES_NONFATAL_ERROR";
				break;
			case (PGRES_SINGLE_TUPLE):
				description = "PGRES_SINGLE_TUPLE , not implemented";
				break;
			case (PGRES_TUPLES_OK):
				return true;
			default:
				break;

		}
	}

};
}
}

#endif /* PGDATABASE_PGUTILITY_HPP_ */
