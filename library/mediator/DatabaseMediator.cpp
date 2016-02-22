/*
 * DatabaseMediator.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#include "DatabaseMediator.hpp"

namespace zeitoon {
namespace utility {



void DatabaseMediator::query(std::string cmd) {
	this->sm->communication.runCommandSync("database.query", cmd);
}
zeitoon::datatypes::DTTableString DatabaseMediator::querySync(std::string cmd) {
	std::cout <<"DBMEDIATOR QUERYSYNC"<<endl;
	return zeitoon::datatypes::DTTableString(this->sm->communication.runCommandSync("database.query", cmd));
}

void DatabaseMediator::execute(std::string cmd) {
	sm->communication.runCommandSync("database.execute", cmd);
}

int DatabaseMediator::executeSync(std::string cmd) {
	return std::stoi(this->sm->communication.runCommandSync("database.execute",cmd));
	sm->dataReceivedRouter(this,cmd);
}

void DatabaseMediator::singleFieldQuery(std::string cmd) {/// from? which node? applies to all these methods
	sm->communication.runCommand("database.singlefieldquery",cmd);
}

std::string DatabaseMediator::singleFieldQuerySync(std::string cmd) { //َUse for a single-tupple/row(singleton) result only
return sm->communication.runCommandSync("database.singlefieldquery",cmd);

}


} /* namespace utility */
} /* namespace zeitoon */
