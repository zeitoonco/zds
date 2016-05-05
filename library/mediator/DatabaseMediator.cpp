/*
 * DatabaseMediator.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#include "DatabaseMediator.hpp"
#include "ServerMediator.hpp"
#include "utility/DTStructs.hpp"

namespace zeitoon {
namespace utility {

std::string DatabaseMediator::wrapSqlCmd(std::string sql) {
	return "{\"query\":\"" + utility::JSONUtility::encodeString(sql) + "\"}";
}

void DatabaseMediator::query(std::string cmd) {
	this->sm->communication.runCommand("database.query", wrapSqlCmd(cmd));
}

zeitoon::datatypes::DTTableString DatabaseMediator::querySync(std::string cmd) {
	return zeitoon::datatypes::DTTableString(
			this->sm->communication.runCommandSync("database.query", wrapSqlCmd(cmd),
			                                       CommunicationUtility::getRandomID()), "");
}

void DatabaseMediator::execute(std::string cmd) {
	sm->communication.runCommand("database.execute", wrapSqlCmd(cmd));
}

int DatabaseMediator::executeSync(std::string cmd) {
	datatypes::DSInteger res;
	res.fromString(this->sm->communication.runCommandSync("database.execute", wrapSqlCmd(cmd),
	                                                      CommunicationUtility::getRandomID()));
	return res.value.getValue();
}

void DatabaseMediator::singleFieldQuery(std::string cmd) {/// from? which node? applies to all these methods
	sm->communication.runCommand("database.singlefieldquery", wrapSqlCmd(cmd));
}

//Use for a single-tupple/row(singleton) result only
std::string DatabaseMediator::singleFieldQuerySync(std::string cmd) {
	datatypes::DSString res;
	res.fromString(sm->communication.runCommandSync("database.singlefieldquery", wrapSqlCmd(cmd),
	                                                CommunicationUtility::getRandomID()));
	return res.value.getValue();
}


} /* namespace utility */
} /* namespace zeitoon */
