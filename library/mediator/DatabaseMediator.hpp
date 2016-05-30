/*
 * DatabaseMediator.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#ifndef MEDIATOR_DATABASEMEDIATOR_HPP_
#define MEDIATOR_DATABASEMEDIATOR_HPP_

#include <utility/exceptions.hpp>
#include  "datatypes/DTTableString.hpp"

namespace zeitoon {
namespace utility {

class ServerMediator;

class DatabaseMediator {
	ServerMediator *sm;

	std::string wrapSqlCmd(std::string sql);

public:
	DatabaseMediator(ServerMediator *ism) :
			sm(ism) {
	}

	void query(std::string cmd);

	zeitoon::datatypes::DTTableString querySync(std::string cmd);

	void execute(std::string cmd);

	int executeSync(std::string cmd);

	void singleFieldQuery(std::string cmd);

	std::string singleFieldQuerySync(std::string cmd);

	virtual ~DatabaseMediator() {
	}
};

} /* namespace utility */
} /* namespace zeitoon */

#endif /* MEDIATOR_DATABASEMEDIATOR_HPP_ */
