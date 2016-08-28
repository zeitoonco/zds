/*
 * Connection.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: inf
 */
#include "Connection.hpp"
#include <utility/exceptions.hpp>
#include "pgutility.hpp"

using namespace zeitoon::utility;

namespace zeitoon {
namespace pgdatabase {

Connection::Connection() {
	mtxLock = new mutex;
	conn = nullptr;
}

Connection::Connection(std::string USERin, std::string PASSin, std::string HOSTin, int PORTin,
                       std::string DBNAMEin) : Connection() {
	USER = USERin;
	PASS = PASSin;
	HOST = HOSTin;
	DBNAME = DBNAMEin;
	PORT = PORTin;

	std::string connInfoStr = ("hostaddr=" + HOST + " port=" + std::to_string(PORT) + " user=" + USER +
	                           " password=" + PASS + " dbname=" + DBNAME);

	conn = PQconnectdb(connInfoStr.c_str());
	if (PQstatus(conn) != CONNECTION_OK) {
		EXTconnectionError(string(PQerrorMessage(conn)));
	}

}

Connection::~Connection() {
	if (isConnected()) {
		try {
			this->disconnect();
		} catch (zeitoon::utility::exceptionEx &err) {
			EXTconnectionErrorI("Disconnect FAILED", err);
		}
	}
}

void Connection::connect() {

	if (isConnected()) {
		EXTconnectionError("Already Connected");
	}

	std::string connInfoStr = ("hostaddr=" + HOST + " port=" + std::to_string(PORT) + " user=" + USER +
	                           " password=" + PASS + " dbname=" + DBNAME);
	conn = PQconnectdb(connInfoStr.c_str());
	if (PQstatus(conn) != CONNECTION_OK) {
		EXTconnectionError(PQerrorMessage(conn));
	}
}

void Connection::connect(std::string USERin, std::string PASSin, std::string HOSTin, int PORTin,
                         std::string DBNAMEin) {
	USER = USERin;
	PASS = PASSin;
	HOST = HOSTin;
	DBNAME = DBNAMEin;
	PORT = PORTin;
	if (isConnected()) {
		EXTconnectionErrorO("Already Connected!", this->getNameAndType());
	}
	std::string connInfoStr = ("hostaddr=" + HOSTin + " port=" + std::to_string(PORTin) + " user=" + USERin +
	                           " password=" + PASSin + " dbname=" + DBNAMEin);
	conn = PQconnectdb(connInfoStr.c_str());
	if (PQstatus(conn) != CONNECTION_OK) {
		std::string msg = PQerrorMessage(conn);
		EXTconnectionError(msg);

	}
}

void Connection::disconnect() {
	if (not isConnected()) {
		return;
	}
//	conn = nullptr;
	PQfinish(conn);

}

std::string Connection::getValue(std::string command) {
	if (not isConnected()) {
		EXTconnectionErrorO("No Connection.  " + std::string(PQerrorMessage(conn)), this->getNameAndType());
	}
	lock_guard<mutex> lck(*mtxLock);
	PGresult *commandResult = PQexec(conn, command.c_str());
	std::string desc;
	if (not zeitoon::pgdatabase::PGutility::isValidResult(commandResult, desc)) {
		EXTDBError("SQL GetSingleFieldValue Failed. " + desc);
	}
	if (PQntuples(commandResult) < 1)
		EXTDBError("SQL GetSingleFieldValue Failed. No tuples returned from database");
	return PQgetvalue(commandResult, 0, 0);
}

DTTablePostgres Connection::query(std::string command) {
	if (not isConnected()) {
		EXTconnectionErrorO("No Connection.  " + std::string(PQerrorMessage(conn)), this->getNameAndType());
	}
	try {
		lock_guard<mutex> lck(*mtxLock);
		DTTablePostgres queryObj(conn, command, "Query");
		return queryObj;
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("SQL QUERY FAILED", errorInfo);
	}

}

bool Connection::isConnected() {
	if (conn == nullptr) {
		return false;
	}
	return (PQstatus(conn) == CONNECTION_OK);

}

int Connection::execute(std::string command) {
	if (not isConnected()) {
		EXTconnectionErrorO("No Connection.  " + std::string(PQerrorMessage(conn)), this->getNameAndType());
	}
	lock_guard<mutex> lck(*mtxLock);
	PGresult *commandResult = PQexec(conn, command.c_str());
	std::string desc;
	if (not zeitoon::pgdatabase::PGutility::isValidResult(commandResult, desc)) {
		EXTDBError("SQL Execute Failed. " + desc);
	}
	return atoi(PQcmdTuples(commandResult));
}


std::string Connection::getNameAndType() {

	return isConnected() ? ("PGConnection" + HOST + ":" + std::to_string(PORT) + "@" + USER) : (USER +
	                                                                                            "@PGConnection ");

}
}
}
