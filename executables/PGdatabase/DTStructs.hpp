/*
 * DTStructs.hpp
 *
 *  Created on: Feb 22, 2016
 *      Author: inf
 */
#ifndef PGDTSTRUCTS_HPP_
#define PGDTSTRUCTS_HPP_

#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"
#include "datatypes/DTTableString.hpp"

namespace zeitoon {
namespace pgdatabase {

using namespace zeitoon::datatypes;

class DSDBQuery: public DTStruct {
public:
	DTString query = { "query" };
	static std::string getStructName() {
		return "DSDBQuery";
	}

	static int getStructVersion() {
		return 1;
	}
	DSDBQuery() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
		this->list.push_back(&query);
	}
	DSDBQuery(std::string data, bool isJSON) :
			DSDBQuery() {
		if (isJSON) {
			this->fromString(data);
		} else {
			query = data;
		}
	}
};

class DSDBTable: public DTStruct {
public:
	DTTableString value = { "value" };
	static std::string getStructName() {
		return "DSDBTable";
	}
	static int getStructVersion() {
		return 1;
	}
	DSDBTable() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
		this->list.push_back(&value);
	}
	DSDBTable(std::string data, bool isJSON) :
			DSDBTable() {
		if (isJSON) {
			this->fromString(data);
		} else {
			value = data;
		}
	}
};

}
}

#endif /* DTSTRUCTS_HPP_ */
