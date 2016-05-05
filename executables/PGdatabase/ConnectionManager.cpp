/*
 * ConnectionManager.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: inf
 */

#include <executables/PGdatabase/PGmediator.hpp>
#include"ConnectionManager.hpp"
#include "utility/exceptionex.hpp"

using namespace zeitoon::utility;

#define CREATE_DATABASE "CREATE DATABASE "
#define CREATE_SCHEMA_FOR_EXTENSIONS "CREATE SCHEMA __local"
#define CREATE_EXTENSIONS_INFO_TABLE "CREATE TABLE __local.extension(  id serial NOT NULL,  name text, username text,  password text,  schema text,  blocked boolean,  CONSTRAINT extension_pkey PRIMARY KEY (id),  CONSTRAINT extension_name_key UNIQUE (name),  CONSTRAINT extension_password_key UNIQUE (password),  CONSTRAINT extension_schema_key UNIQUE (schema),  CONSTRAINT extension_username_key UNIQUE (username))"

namespace zeitoon {
namespace pgdatabase {

ConnectionManager::ConnectionManager(std::string adminUserName, std::string adminPassWord,
                                     std::string adminHost, int adminPort, std::string adminDbname,
                                     PGmediator *instanceptr) :
		pgMediator(instanceptr) {

	host = adminHost;
	port = adminPort;
	dbname = adminDbname;
	try {
		adminConnection.connect(adminUserName, adminPassWord, adminHost, adminPort, adminDbname);

	} catch (exceptionEx *errorInfo) {
		if (!adminConnection.isConnected())
			adminConnection.connect(adminUserName, adminPassWord, adminHost, adminPort, "");
		if (adminConnection.isConnected()) {
			try {
				this->createDatabase(adminUserName, adminPassWord, adminPort, adminHost);
			} catch (exceptionEx *_errorInfo) {
				EXTDBErrorI("Admin Database Create Failed", _errorInfo);
			}
		} else {
			EXTconnectionErrorI("Admin Connection Failed", errorInfo);
		}
	}
	if (adminConnection.isConnected() == false) {
		EXTconnectionError("Admin Connection Failed");
	}
	if (adminConnection.isConnected())
		std::cerr << "CONNECTED" << endl;
}

ConnectionManager::~ConnectionManager() {
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);
	std::map<std::string, Connection>::iterator iter;
	for (iter = connectionList.begin(); iter != connectionList.end(); iter++) {
		iter->second.disconnect();
	}
	connectionList.clear();
	adminConnection.disconnect();
}

int ConnectionManager::execute(std::string extension, std::string sql) {
	int returnAmont = 0;
	if (checkIfExtensionRegistered(extension) != true) {
		registerNewExtension(extension);
	}
	if (connectionList.find(extension) == connectionList.end()) {
		connectionMaker(extension);
	}
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);

	try {
		returnAmont = connectionList.at(extension).execute(sql);

	} catch (exceptionEx *errorInfo_ConMgr) {
		std::cerr << "ConMGR65" << errorInfo_ConMgr->what() << endl;
		//	EXTDBError(errorInfo_ConMgr->what());
	}
	return returnAmont;
}

DTTablePostgres ConnectionManager::query(std::string extension, std::string sql) {

	if (checkIfExtensionRegistered(extension) != true) {
		registerNewExtension(extension);
	}
	if (connectionList.find(extension) == connectionList.end()) {
		connectionMaker(extension);
	}
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);
	try {

		return this->connectionList.at(extension).query(sql);
		//return sqlQuery;

	} catch (exceptionEx *errorInfo) {
		EXTDBErrorIO("Unable to perform the query", this->getNameAndType(), errorInfo);
	}

}

std::string ConnectionManager::singleFieldQuery(std::string extension, std::string sql) {
	std::string val = "";
	if (checkIfExtensionRegistered(extension) != true) {
		registerNewExtension(extension);
	}
	if (connectionList.find(extension) == connectionList.end()) {
		connectionMaker(extension);
	}
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);
	try {
		val = connectionList.at(extension).getValue(sql);
		return val;

	} catch (exceptionEx *errorInfo) {
		EXTDBErrorIO("Unable to perform the query", this->getNameAndType(), errorInfo);
	}

}

void ConnectionManager::registerNewExtension(std::string extensionName) {
	/* Registering a new extension:
	 *1- Create a no-superuser role with Rolename and Pw same as extension name.
	 *2- Create a schema, named the same as extension name.
	 *3- Grants all privileges on the schema to extension role.
	 *4- Registers extension in "__local.extension" table for(list of registered extensions).
	 */
	try {
		adminConnection.execute(
				"BEGIN;CREATE ROLE \"" + extensionName + "\" nosuperuser  login password '" + extensionName +
				"' valid until 'infinity';" +
				"CREATE SCHEMA \"" + extensionName + "\";" +
				"GRANT ALL ON SCHEMA \"" + extensionName + "\" TO \"" + extensionName + "\";" +
				"INSERT INTO __local.extension (id,name, username, password, schema, blocked)"
						" values(default, '" + extensionName + "','" + extensionName + "','" + extensionName +
				"','" + extensionName + "', false);" +
				"ALTER ROLE \"" + extensionName + "\" SET search_path TO \"" + extensionName + "\";"
						"END");
	} catch (exceptionEx *errorInfo) {
		EXTDBErrorI("unable to register new extension", errorInfo);
	}
	pgMediator->sm.communication.runEvent("database.newUser", "{\"value\":\"" + extensionName + "\"}");
	//##Event Fired
}

void ConnectionManager::connectionMaker(std::string extensionName) {
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);
	this->connectionList.insert(std::pair<string, Connection>(extensionName,
	                                                          Connection(extensionName, extensionName, host,
	                                                                     port, dbname)));
	pgMediator->sm.communication.runEvent("database.userLogin", "{\"value\":\"" + extensionName + "\"}");
	//##Event Fired

}

bool ConnectionManager::checkIfExtensionRegistered(std::string extension) {
	return adminConnection.query("select id from __local.extension where name =\'" + extension + "\'").rowCount() == 1;
}

std::string ConnectionManager::getNameAndType() {
	return "ConnectionManager@ ";
}

void ConnectionManager::createDatabase(std::string userName, std::string passWord, int port, std::string host) {
	try {
		adminConnection.execute(CREATE_DATABASE + dbname);
		adminConnection.disconnect();
		adminConnection.connect(userName, passWord, host, port, dbname);
		adminConnection.execute(CREATE_SCHEMA_FOR_EXTENSIONS);
		adminConnection.execute(CREATE_EXTENSIONS_INFO_TABLE);
	} catch (exceptionEx *errorInfo) {
		EXTDBErrorI("Admin createDatabase Failed", errorInfo);

	}

}
}
}
