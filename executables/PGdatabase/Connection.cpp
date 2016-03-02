/*
 * Connection.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: inf
 */
#include "Connection.hpp"
#include "utility/exceptionex.hpp"
using namespace zeitoon::utility;

namespace zeitoon {
namespace pgdatabase {

Connection::Connection() {
	conn = nullptr;

}
Connection::Connection(std::string USERin, std::string PASSin, std::string HOSTin, int PORTin, std::string DBNAMEin) {
	USER = USERin;
	PASS = PASSin;
	HOST = HOSTin;
	DBNAME = DBNAMEin;
	PORT = PORTin;

	std::string connInfoStr = ("hostaddr=" + HOST + " port=" + std::to_string(PORT) + " user=" + USER + " password=" + PASS + " dbname=" + DBNAME);

	conn = PQconnectdb(connInfoStr.c_str());
	if (PQstatus(conn) != CONNECTION_OK) {
		EXTconnectionError(string(PQerrorMessage(conn)));
	}

}
Connection::~Connection() {
	this->disconnect();
}
void Connection::connect() {

	if (isConnected() == true) {
		EXTconnectionError("Already Connected");
	}

	std::string connInfoStr = ("hostaddr=" + HOST + " port=" + std::to_string(PORT) + " user=" + USER + " password=" + PASS + " dbname=" + DBNAME);
	conn = PQconnectdb(connInfoStr.c_str());
	if (PQstatus(conn) != CONNECTION_OK) {
		EXTconnectionError(PQerrorMessage(conn));
	}
}

void Connection::connect(std::string USERin, std::string PASSin, std::string HOSTin, int PORTin, std::string DBNAMEin) {
	USER = USERin;
	PASS = PASSin;
	HOST = HOSTin;
	DBNAME = DBNAMEin;
	PORT = PORTin;
	if (isConnected()) {
		EXTconnectionErrorO("Already Connected!", this->getNameAndType());
	}
	std::string connInfoStr = ("hostaddr=" + HOSTin + " port=" + std::to_string(PORTin) + " user=" + USERin + " password=" + PASSin + " dbname=" + DBNAMEin);
	conn = PQconnectdb(connInfoStr.c_str());
	if (PQstatus(conn) != CONNECTION_OK) {
		EXTconnectionErrorO(PQerrorMessage(conn), this->getNameAndType());

	}
}
void Connection::disconnect() {
	if (not isConnected()) {
		return;
	}
	conn = nullptr;
	PQfinish(conn);

}
std::string Connection::getValue(std::string command) {
	if (not isConnected()) {
		EXTconnectionErrorO("No Connection!", this->getNameAndType());
	}
	return query(command).fieldValue(0, 0);
}

DTTablePostgres Connection::query(std::string command) {
	if (not isConnected()) {
		EXTconnectionErrorO("No Connection!", this->getNameAndType());
	}
	try {
		DTTablePostgres queryObj(conn, command, "");
		return queryObj;
	} catch (exceptionEx *errorInfo) {
		cerr << "Connection 84 Unable to passs exception to ConMgr\n" << errorInfo->what() << endl;
		EXTDBErrorI("unable to perform the query", errorInfo);
	}

}
bool Connection::isConnected() { //todo: returns false???
	if (conn == nullptr) {
		return false;
	}
	return (PQstatus(conn) == CONNECTION_OK);

}
int Connection::execute(std::string command) {
	if (not isConnected()) {
		EXTconnectionErrorO("No Connection!", this->getNameAndType());
	}
	PGresult * commandResult = PQexec(conn, command.c_str());
	if (PQresultStatus(commandResult) == PGRES_COMMAND_OK || PQresultStatus(commandResult) == PGRES_TUPLES_OK) {
		return atoi(PQcmdTuples(commandResult));
	}
	std::cerr << "Connection100(problem with passing exception between connection and con-MGR):\n" << "\t" << PQresultErrorMessage(commandResult) << endl;

	EXDBError(std::string(PQresultErrorMessage(commandResult)).c_str()); //TODO:Injas
	return -1;
}
std::string Connection::getNameAndType() {

	return isConnected() ? ("PGConnection" + HOST + ":" + std::to_string(PORT) + "@" + USER) : (USER + "@PGConnection ");

}
}
}
