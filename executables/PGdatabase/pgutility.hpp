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

	}
}

#endif /* PGDATABASE_PGUTILITY_HPP_ */
