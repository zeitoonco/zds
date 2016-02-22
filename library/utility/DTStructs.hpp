/*
 * DTStructs.hpp
 *
 *  Created on: Feb 22, 2016
 *      Author: inf
 */
#ifndef DTSTRUCTS_HPP_
#define DTSTRUCTS_HPP_

#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"
#include "datatypes/DTTableString.hpp"

namespace zeitoon {
namespace datatypes {

class DSString: public DTStruct {
public:
	DTString value = { "value" };
	DSString() :
			DTStruct("DSString") {
		this->list.push_back(&value);
	}

	size_t getVersion() {
		return 0;
	}
	size_t getMinSupportedVersion() {
		return 0;
	}
	size_t getMinSupportedVersionWOConversation() {
		return 0;
	}
};

class DSInteger: public DTStruct {
public:
	DTInteger<int> value = { "value" };
	DSInteger() :
			DTStruct("DSInteger") {
		this->list.push_back(&value);
	}

	size_t getVersion() {
		return 0;
	}
	size_t getMinSupportedVersion() {
		return 0;
	}
	size_t getMinSupportedVersionWOConversation() {
		return 0;
	}
};

class DSFloat: public DTStruct {
public:
	DTFloat<float> value = { "value" };
	DSFloat() :
			DTStruct("DSFloat") {
		this->list.push_back(&value);
	}

	size_t getVersion() {
		return 0;
	}
	size_t getMinSupportedVersion() {
		return 0;
	}
	size_t getMinSupportedVersionWOConversation() {
		return 0;
	}
};

class DSDBTable: public DTStruct {
public:
	DTTableString value = { "value" };
	DSDBTable() :
			DTStruct("DSDBTable") {
		this->list.push_back(&value);
	}

	size_t getVersion() {
		return 0;
	}
	size_t getMinSupportedVersion() {
		return 0;
	}
	size_t getMinSupportedVersionWOConversation() {
		return 0;
	}
};


}
}
#endif /* DTSTRUCTS_HPP_ */
