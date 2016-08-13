/*
 * ConnectionManager.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: inf
 */

#include <executables/PGdatabase/PGmediator.hpp>
#include"ConnectionManager.hpp"
#include <utility/exceptions.hpp>
#include "utility/logger.hpp"
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

	} catch (exceptionEx &errorInfo) {
		if (!adminConnection.isConnected())
			adminConnection.connect(adminUserName, adminPassWord, adminHost, adminPort, "");
		if (adminConnection.isConnected()) {
			try {
				this->createDatabase(adminUserName, adminPassWord, adminPort, adminHost);
			} catch (exceptionEx &_errorInfo) {
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
		lNote("CONNECTED TO DATABASE");
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
	int returnAmount = 0;
	try {
		if (checkIfExtensionRegistered(extension) != true) {
			registerNewExtension(extension);
		}
		if (connectionList.find(extension) == connectionList.end()) {
			connectionMaker(extension);
		}
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTDBErrorI("DB Connection Error for execute ", err);
	}
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);
	try {
		returnAmount = connectionList.at(extension).execute(sql);

	} catch (exceptionEx &errorInfo_ConMgr) {
		EXTDBErrorI("SQL Execution Error: ", errorInfo_ConMgr);
	}
	return returnAmount;
}

DTTablePostgres ConnectionManager::query(std::string extension, std::string sql) {
	try {
		if (checkIfExtensionRegistered(extension) != true) {
			registerNewExtension(extension);
		}
		if (connectionList.find(extension) == connectionList.end()) {
			connectionMaker(extension);
		}
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTDBErrorI("DB Connection Error for query ", err);
	}
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);
	try {
		return this->connectionList.at(extension).query(sql);
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("SQL Query Error: ", errorInfo);
	}

}

std::string ConnectionManager::singleFieldQuery(std::string extension, std::string sql) {
	std::string val = "";
	try {
		if (checkIfExtensionRegistered(extension) != true) {
			registerNewExtension(extension);
		}
		if (connectionList.find(extension) == connectionList.end()) {
			connectionMaker(extension);
		}
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTDBErrorI("DB Connection Error for singleFieldQuery ", err);
	}
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);
	try {
		val = connectionList.at(extension).getValue(sql);
		return val;
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("SQL SingleFieldQuery Error: ", errorInfo);
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
		adminConnection.execute(//todo: check-> create role if not exist
				"BEGIN;CREATE ROLE \"" + extensionName + "\" nosuperuser  login password '" + extensionName +
				"' valid until 'infinity';" +
				"CREATE SCHEMA \"" + extensionName + "\";" +
				"GRANT ALL ON SCHEMA \"" + extensionName + "\" TO \"" + extensionName + "\";" +
				"INSERT INTO __local.extension (id,name, username, password, schema, blocked)"
						" values(default, '" + extensionName + "','" + extensionName + "','" + extensionName +
				"','" + extensionName + "', false);" +
				"ALTER ROLE \"" + extensionName + "\" SET search_path TO \"" + extensionName + "\";"
						"END");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Register new extension FAILED", errorInfo);
	}
	pgMediator->sm.communication.runEvent("database.newUser", "{\"value\":\"" + extensionName + "\"}");
	lNote("New extension added. Name: "+extensionName);
	//##Event Fired
}

void ConnectionManager::connectionMaker(std::string extensionName) {
	std::lock_guard<std::mutex> connectionListGuard(mapGuard);
	auto insertResult = this->connectionList.insert(
			std::pair<string, Connection>(extensionName, Connection(extensionName, extensionName, host, port, dbname)));
	if (!insertResult.second)//false if not successfully inserted into the map
		EXTDBError("Create Connection for \'" + extensionName + "\' FAILED");

	pgMediator->sm.communication.runEvent("database.userLogin", "{\"value\":\"" + extensionName + "\"}");
	lNote("Extension logged in. Name: "+extensionName);
	//##Event Fired
}


void ConnectionManager::dropConnection(std::string extensionName) {
this->connectionList.erase(extensionName);
}


bool ConnectionManager::checkIfExtensionRegistered(std::string extension) {
	return adminConnection.query("SELECT id FROM __local.extension WHERE name =\'" + extension + "\'").rowCount() == 1;
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
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Admin createDatabase Failed", errorInfo);
	}
	lNote("Admin Role,Schema,Tables created. Name: "+userName);

}

void pgdatabase::ConnectionManager::removeExtension(std::string serviceName) {
	try {
		lWarnig("Removing DB schema for user: " + serviceName + " .... " );
		int a = adminConnection.execute("DROP SCHEMA IF EXISTS \"" + serviceName + "\" CASCADE");
		lWarnig("Removing service name from DB for user: " + serviceName + " .... ");
		int b = adminConnection.execute("DELETE FROM __local.extension WHERE name = '" + serviceName + "'");
		lWarnig("Removing role "+ serviceName + " ....  ");
		int c = adminConnection.execute("DROP ROLE IF EXISTS \"" + serviceName + "\"");
		lWarnig("terminating DB connection for "+serviceName);
		this->dropConnection(serviceName);
		lNote("Done");

	} catch (zeitoon::utility::exceptionEx &err) {
		EXTDBErrorI("Removing Extention FAILED", err);
	}
}
}
}
