/*
 * DatabaseMediator.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#ifndef MEDIATOR_DATABASEMEDIATOR_HPP_
#define MEDIATOR_DATABASEMEDIATOR_HPP_

#include "utility/exceptionex.hpp"

namespace zeitoon {
namespace utility {

class ServerMediator;

class DatabaseMediator {
	ServerMediator *sm;
	public:
	DatabaseMediator(ServerMediator *ism) :
			sm(ism) {
	}
	virtual ~DatabaseMediator() {
	}
};

} /* namespace utility */
} /* namespace zeitoon */

#endif /* MEDIATOR_DATABASEMEDIATOR_HPP_ */
