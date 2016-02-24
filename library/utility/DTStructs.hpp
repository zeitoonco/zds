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

namespace zeitoon { namespace datatypes {

class DSString : public DTStruct {
public:
	DTString value = {"value"};

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

class DSInteger : public DTStruct {
public:
	DTInteger<int> value = {"value"};

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

class DSFloat : public DTStruct {
public:
	DTFloat<float> value = {"value"};

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

class DSBoolean : public DTStruct {
public:
	DTBoolean value = {"value"};

	DSBoolean() :
			DTStruct("DSBoolean") {
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

class DSDBTable : public DTStruct {
public:
	DTTableString value = {"value"};

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


class DSInstallInfo : public DTStruct {
public:
	class DSCommandDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTString inputDatatype = {"inputDatatype"};
		DTInteger inputDatatypeVersion = {"inputDatatypeVersion"};
		DTString outputDatatype = {"outputDatatype"};
		DTInteger outputDatatypeVersion = {"inputDatatypeVersion"};

		DSCommandDetail(string iname, string inputDT, int inputDTVer, string outputDT, int outputDTVer) {
			name = iname;
			inputDatatype = inputDT;
			inputDatatypeVersion = inputDTVer;
			outputDatatype = outputDT;
			outputDatatypeVersion = outputDTVer;
			this->list.push_back(name);
			this->list.push_back(inputDatatype);
			this->list.push_back(inputDatatypeVersion);
			this->list.push_back(outputDatatype);
			this->list.push_back(outputDatatypeVersion);
		}
	};

	class DSEventDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTString datatype = {"datatype"};
		DTInteger datatypeVersion = {"datatypeVersion"};

		DSEventDetail(string iname, string DT, int DTVer) {
			name = iname;
			datatype = DT;
			datatypeVersion = DTVer;
			this->list.push_back(name);
			this->list.push_back(datatype);
			this->list.push_back(datatypeVersion);
		}
	};

	class DSHookDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTString datatype = {"datatype"};
		DTInteger datatypeVersion = {"datatypeVersion"};

		DSHookDetail(string iname, string DT, int DTVer) {
			name = iname;
			datatype = DT;
			datatypeVersion = DTVer;
			this->list.push_back(name);
			this->list.push_back(datatype);
			this->list.push_back(datatypeVersion);
		}
	};

	class DSInstallInfoCommandCallDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTInteger inputDatatypeVersion = {"inputDatatypeVersion"};
		DTInteger outputDatatypeVersion = {"outputDatatypeVersion"};

		DSCommandCallDetail(string
		iname,
		int inDTVer,
		int outDTVer
		) {
			name = iname;
			inputDatatypeVersion = inDTVer;
			outputDatatypeVersion = outDTVer;
			this->list.push_back(name);
			this->list.push_back(inputDatatypeVersion);
			this->list.push_back(outputDatatypeVersion);
		}
	};

	class DSInstallInfoRequirementDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTInteger version = {"version"};

		DSInstallInfoRequirementDetail(string iname, int iVer) {
			name = iname;
			version = iVer;
			this->list.push_back(name);
			this->list.push_back(version);
		}
	};

	class DSInstallInfoDatatypesDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTInteger version = {"version"};

		DSInstallInfoDatatypesDetail(string iname, int iVer) {
			name = iname;
			version = iVer;
			this->list.push_back(name);
			this->list.push_back(version);
		}
	};

	DTString name = {"name"};
	DTString title = {"title"};
	DTInteger version = {"version"};
	DTSet<DSCommandDetail> commands = {"commands"};
	DTSet<DSEventDetail> events = {"events"};
	DTSet<DSHookDetail> hooks = {"hooks"};
	DTSet<DSInstallInfoCommandCallDetail> calls = {"calls"};
	DTSet<DSInstallInfoRequirementDetail> requirements = {"requirements"};
	DTSet<DSInstallInfoDatatypesDetail> datatypes = {"datatypes"};

	DSInstallInfo() : DTStruct("installInfo") {

	}

	size_t getVersion() {
		return 1;
	}

	size_t getMinSupportedVersion() {
		return 1;
	}

	size_t getMinSupportedVersionWOConversation() {
		return 1;
	}
};

}}
#endif /* DTSTRUCTS_HPP_ */
