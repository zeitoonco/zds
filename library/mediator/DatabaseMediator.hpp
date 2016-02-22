/*
 * DatabaseMediator.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#ifndef MEDIATOR_DATABASEMEDIATOR_HPP_
#define MEDIATOR_DATABASEMEDIATOR_HPP_

#include "utility/exceptionex.hpp"
#include  "datatypes/DTTableString.hpp"

namespace zeitoon {
namespace utility {

class ServerMediator;

class DatabaseMediator {
	ServerMediator *sm;
	public:
	DatabaseMediator(ServerMediator *ism) :
			sm(ism) {
	}
	void query(std::string cmd);
	zeitoon::datatypes::DTTableString querySync(std::string cmd);
	void execute(std::string cmd);
	int executeSync(std::string cmd);
	void singleFieldQuery(std::string cmd);
	std::string singleFieldQuerySync(std::string cmd);//single field result Only
	virtual ~DatabaseMediator() {
	}
};

} /* namespace utility */
} /* namespace zeitoon */

#endif /* MEDIATOR_DATABASEMEDIATOR_HPP_ */
