/*
 * DTTablePostgres.cpp
 *
 *  Created on: Jan 30, 2016
 *      Author: inf
 */

#include"DTTablePostgres.hpp"
#include <utility/exceptions.hpp>
#include "datatypes/dtmultifieldtypes.hpp"
#include"datatypes/dtsingletypes.hpp"
#include "pgutility.hpp"

using namespace zeitoon::datatypes;
using namespace zeitoon::utility;

namespace zeitoon {
namespace pgdatabase {

DTTablePostgres::~DTTablePostgres() {

}

DTTablePostgres::DTTablePostgres(PGconn *connection, PGresult *iResult, std::string name) :
		DTTable(name) {
	conn = connection;
	mapMaker(); //creates OID data type related maps.
	result = iResult;
	std::string desc;
	if (not zeitoon::pgdatabase::PGutility::isValidResult(result, desc)) {
		EXTDBError("SQL Query Failed. " + desc);
	}    /*if ((PQntuples(result)) < 1) {
		auto d = (PQresultStatus(result));
		if (d != PGRES_COMMAND_OK && d != PGRES_TUPLES_OK) {
			EXTDBError(errorSwitch(d));
		}
	}*/
}

DTTablePostgres::DTTablePostgres(PGconn *connection, std::string sql, std::string name) :
		zeitoon::datatypes::DTTable(name) {
	conn = connection;
	mapMaker();
	result = PQexec(conn, sql.c_str());
	std::string desc;
	if (not zeitoon::pgdatabase::PGutility::isValidResult(result, desc)) {
		EXTDBError("SQL Query Failed. " + desc);
	}/*
	if ((PQntuples(result)) < 1) {
		auto d = (PQresultStatus(result));
		if (d != PGRES_COMMAND_OK && d != PGRES_TUPLES_OK) {
			EXTDBError(std::string(PQerrorMessage(conn)) + "  " + PQresStatus(PQresultStatus(result)));
		}
	}*/
}

size_t DTTablePostgres::rowCount() const {
	return (size_t) PQntuples(result);
}

size_t DTTablePostgres::columnCount() const {

	return (size_t) PQnfields(result);
}

std::string DTTablePostgres::columnName(int columnNumber) const {//todo to be tested
	if (PQfname(result, columnNumber) == NULL) {
		EXToutOfRange("column number is out of range");
	}
	return PQfname(result, columnNumber);

}

ColumnDataType::columnDataType DTTablePostgres::columnDataType(int columnNumber) const {
	Oid columnOid;
	try {
		columnOid = columnODataType(columnNumber);
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTinvalidParameterI("Unable to fetch OID", err);
	}
	std::map<Oid, int>::iterator iter = mapPgCustomType.find(columnOid);
	if (iter == mapPgCustomType.end()) {
		EXTdataTypeMismatch("Data type not found");
	}
	return (ColumnDataType::columnDataType) iter->second;
}

Oid DTTablePostgres::columnODataType(int columnNumber) const {
	if (columnNumber >= columnCount() || columnNumber < 0) {
		EXToutOfRange("Column number is out of range");
	}
	return PQftype(result, columnNumber);
}

size_t DTTablePostgres::columnDataSize(int columnNumber) const { //returns -1 for varchar
	if (columnNumber >= columnCount() || columnNumber < 0) {
		EXToutOfRange("Column number is out of range");
	}
	return (size_t) PQfsize(result, columnNumber);
}

std::string DTTablePostgres::fieldValue(int tupleNumber, int columnNumber) const {//todo: to be tested
	if (PQgetvalue(result, tupleNumber, columnNumber) == NULL) {
		EXToutOfRange("Parameters out of range");
	}
	return PQgetvalue(result, tupleNumber, columnNumber);
}

std::string DTTablePostgres::fieldValue(int tuppleNumber, std::string columnName) const {
	size_t length = this->columnCount();
	try {
		for (size_t i = 0; i < length; i++) {
			if (this->columnName(i) == columnName) {
				return this->fieldValue(tuppleNumber, i);
			}
		}
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTinvalidNameI("Get field value by columnName failed", err);
	}
}


bool DTTablePostgres::fieldIsNull(int tupleNumber, int columnNumber) const {
	if ((tupleNumber ) >= PQntuples(result) || tupleNumber < 0 || (columnNumber) >= PQnfields(result) ||
	    columnNumber < 0) {
		EXToutOfRange("Parameters out of range");
	}
	return (bool) PQgetisnull(result, tupleNumber, columnNumber);
}

int DTTablePostgres::fieldSize(int tupleNumber, int columnNumber) const {
	if ((tupleNumber ) >= PQntuples(result) || tupleNumber < 0 || (columnNumber) >= PQnfields(result) ||
		columnNumber < 0) {
		EXToutOfRange("Parameters out of range");
	}
	return PQgetlength(result, tupleNumber, columnNumber);
}

std::map<int, std::vector<std::string>> DTTablePostgres::mapHardCodedTypes;
std::map<Oid, int> DTTablePostgres::mapPgCustomType;

void DTTablePostgres::mapMaker() { //this method only should be accessed via constructor

	/* Makes 1 map called mapHardCodedTypes which contains the desired OidTypenames:
	 * 1-Create the Hardcoded map
	 * 2-Make a query to fetch all pgtypenames and Oids
	 * 3-iterate throgh the hardcoded map
	 * 4-inserts into second map called mapPgCustomType, when it finds match for typenames;
	 * 5-makes mapPgCustomType_Status "TRUE" before the end and PQClear("The query result")
	 */
	static bool mapPgCustomType_Status = false;
	if (mapPgCustomType_Status) { //Check if map has already been created:
		return; //select oid, typname from pg_type"
	}
	PGresult *ress = PQexec(conn, "SELECT oid, typname FROM pg_type");
	if (PQresultStatus(ress) != PGRES_TUPLES_OK) {
		EXTDBError("Cannot fetch Oid Type @ mapMaker()");
	}

//Hardcoded map of Types with corresponding Postgres internal type names:

	mapHardCodedTypes[ColumnDataType::TEXT] = {"char", "text", "name", "_char", "_bpchar", "_varchar", "bpchar",
	                                           "varchar"};
	mapHardCodedTypes[ColumnDataType::INTEGER] = {"int8", "int2", "int2vector",
	                                              "int4", "_int2", "_int2vector", "_int4", "_int8",
	                                              "int4range", "_int4range", "int8range", "_int8range"};
	mapHardCodedTypes[ColumnDataType::FLOAT] = {"float4", "float8", "_float4",
	                                            "_float8"};
	mapHardCodedTypes[ColumnDataType::BINARY] = {"bytea"};
	mapHardCodedTypes[ColumnDataType::BOOLEAN] = {"bool", "_bool"};
	mapHardCodedTypes[ColumnDataType::DATE] = {"date", "time", "timestamp", "_timestamp", "_date", "_time",	"timestamptz", "_timestamptz"};
//Custom map of types with corresponding pg Oid:
	for (auto it = mapHardCodedTypes.begin(); it != mapHardCodedTypes.end(); it++) {
		for (auto iter = it->second.begin(); iter != it->second.end(); iter++) {
			for (int i = 0; i < PQntuples(ress); i++) {
				if (*iter == PQgetvalue(ress, i, 1)) {
					mapPgCustomType[atoi(PQgetvalue(ress, i, 0))] = it->first;
				}
			}
		}
	}

	mapPgCustomType_Status = true;
	PQclear(ress);

}

std::string DTTablePostgres::getNameAndType() const {
	return "[" + std::string(PQuser(this->conn)) + "DTTablePostgres] ";
}

/*
std::string DTTablePostgres::errorSwitch(int resStatusEnum) {
	switch (resStatusEnum) {
		case (PGRES_BAD_RESPONSE):
			return (PQresStatus(PGRES_BAD_RESPONSE) + std::string(PQresultErrorMessage(result)));

		case (PGRES_EMPTY_QUERY):
			return (PQresStatus(PGRES_EMPTY_QUERY) + std::string(PQresultErrorMessage(result)));

		case (PGRES_FATAL_ERROR):
			return ((PQresStatus(PGRES_FATAL_ERROR) + std::string(PQresultErrorMessage(result))));
		case (PGRES_COMMAND_OK):
			return ((PQresStatus(PGRES_COMMAND_OK) + std::string(PQresultErrorMessage(result))));
		case (PGRES_TUPLES_OK):
			return ((PQresStatus(PGRES_TUPLES_OK) + std::string(PQresultErrorMessage(result))));
		case (PGRES_SINGLE_TUPLE):
			return ((PQresStatus(PGRES_SINGLE_TUPLE) + std::string(PQresultErrorMessage(result))));
		default:
			EXTunknownException("Unknown PQresultStatus(result), " + std::string(PQresultErrorMessage(result)));
	}
}
*/

std::string DTTablePostgres::toString(int indent, std::string indentContent) const {
	stringstream buffer;
	const int numberOfColumns = PQnfields(result);
	const int numberOfTupples = PQntuples(result);

	buffer << "{\"columns\":[";
	for (int i = 0; i < numberOfColumns; i++) {
		buffer << (i == 0 ? "" : ", ") << "{\"name\":\"" << this->columnName(i) << "\", \"datatype\":\"" <<
		ColumnDataType::toString(this->columnDataType(i))
		<< "\", \"size\":" << columnDataSize(i) << "}";
	}
	buffer << "], \"rows\":[";
	for (int i = 0; i < numberOfTupples; i++) {
		buffer << (i != 0 ? ", " : "");
		for (int j = 0; j < numberOfColumns; j++) {
			buffer << (j == 0 ? "[" : ", ") <<
			(this->fieldIsNull(i, j) ? "NULL" : "\"" + std::string(PQgetvalue(result, i, j)) + "\"");
		}
		buffer << "]";
	}
	buffer << "]}";

	return buffer.str();
}

void DTTablePostgres::fromString(std::string data) {
	EXTnotImplemented("Not Implemented");

}

DTBase &DTTablePostgres::operator=(std::string str) {
	result = PQexec(conn, str.c_str());
	std::string desc;
	if (not zeitoon::pgdatabase::PGutility::isValidResult(result, desc)) {
		EXTDBError("SQL Query Failed. " + desc);
	}/*
	if ((PQntuples(result)) < 1) {
		auto d = (PQresultStatus(result));
		if (d != PGRES_COMMAND_OK && d != PGRES_TUPLES_OK) {
			EXTDBError(errorSwitch(d));
		}
	}*/
	return *this;
}

DTBase &DTTablePostgres::operator=(DTBase &dtvar) {
	auto tempDTS = dynamic_cast<DTString *>(&dtvar);
	if (tempDTS) {
		result = PQexec(conn, (tempDTS->getValue()).c_str());
		std::string desc;
		if (not zeitoon::pgdatabase::PGutility::isValidResult(result, desc)) {
			EXTDBError("SQL Query Failed. " + desc);
		}/*
		if ((PQntuples(result)) < 1) {
			auto d = (PQresultStatus(result));
			if (d != PGRES_COMMAND_OK && d != PGRES_TUPLES_OK) {
				EXTDBError(errorSwitch(d));
			}
		}*/
		return *this;;
	} else {
		auto tempDTT = dynamic_cast<DTTablePostgres *>(&dtvar);
		if (tempDTT) {
			result = tempDTT->result;
			std::string desc;
			if (not zeitoon::pgdatabase::PGutility::isValidResult(result, desc)) {
				EXTDBError("SQL Query Failed. " + desc);
			}/*
			if ((PQntuples(result)) < 1) {
				auto d = (PQresultStatus(result));
				if (d != PGRES_COMMAND_OK && d != PGRES_TUPLES_OK) {
					EXTDBError(errorSwitch(d));
				}
			}*/
			return *this;
		}
	}
	EXTdataTypeMismatch("Invalid Data input");
}

bool DTTablePostgres::operator==(DTBase &dtvar) {
	auto tempDTT = dynamic_cast<DTTable *>(&dtvar);
	if (tempDTT) {
		return (tempDTT->toString() == this->toString());
	}
	EXTdataTypeMismatch("Invalid Data input");
}

bool DTTablePostgres::operator!=(DTBase &dtvar) {
	return !this->operator==(dtvar);
}

std::string DTTablePostgres::toString(PGresult *res) {
	stringstream buffer;
	const int numberOfColumns = PQnfields(res);
	const int numberOfTupples = PQntuples(res);

	buffer << "{\"columns\":[";
	for (int i = 0; i < numberOfColumns; i++) {
		buffer << (i == 0 ? "" : ", ") << "{\"name\":\"" << this->columnName(i) << "\", \"datatype\":\"" <<
		ColumnDataType::toString(this->columnDataType(i))
		<< "\", \"size\":" << columnDataSize(i) << "}";
	}
	buffer << "], \"rows\":[";
	for (int i = 0; i < numberOfTupples; i++) {
		buffer << (i != 0 ? ", " : "");
		for (int j = 0; j < numberOfColumns; j++) {
			buffer << (j == 0 ? "[" : ", ") <<
			(this->fieldIsNull(i, j) ? "NULL" : "\"" + std::string(PQgetvalue(res, i, j)) + "\"");
		}
		buffer << "]";
	}
	buffer << "]}";

	return buffer.str();
}
}
}
