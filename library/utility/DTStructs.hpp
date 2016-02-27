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

	DSString() : DTStruct("DSString", 0, 0, 0) {
		this->list.push_back(&value);
	}
};

class DSInteger : public DTStruct {
public:
	DTInteger<> value = {"value"};

	DSInteger() :
			DTStruct("DSInteger", 0, 0, 0) {
		this->list.push_back(&value);
	}
};

class DSFloat : public DTStruct {
public:
	DTFloat<> value = {"value"};

	DSFloat() :
			DTStruct("DSFloat", 0, 0, 0) {
		this->list.push_back(&value);
	}
};

class DSBoolean : public DTStruct {
public:
	DTBoolean value = {"value"};

	DSBoolean() :
			DTStruct("DSBoolean", 0, 0, 0) {
		this->list.push_back(&value);
	}
};

class DSDBTable : public DTStruct {
public:
	DTTableString value = {"value"};

	DSDBTable() :
			DTStruct("DSDBTable", 0, 0, 0) {
		this->list.push_back(&value);
	}
};


class DSInstallInfo : public DTStruct {
public:
	class DSCommandDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTString inputDatatype = {"inputDatatype"};
		DTInteger<> inputDatatypeVersion = {"inputDatatypeVersion"};
		DTString outputDatatype = {"outputDatatype"};
		DTInteger<> outputDatatypeVersion = {"inputDatatypeVersion"};

		DSCommandDetail(string sname) : DSCommandDetail("", "", 0, "", 0) {
		}

		DSCommandDetail(string iname, string inputDT, int inputDTVer, string outputDT, int outputDTVer) :
				DTStruct("DSCommandDetail", 0, 0, 0) {
			name = iname;
			inputDatatype = inputDT;
			inputDatatypeVersion = inputDTVer;
			outputDatatype = outputDT;
			outputDatatypeVersion = outputDTVer;
			this->list.push_back(&name);
			this->list.push_back(&inputDatatype);
			this->list.push_back(&inputDatatypeVersion);
			this->list.push_back(&outputDatatype);
			this->list.push_back(&outputDatatypeVersion);
		}
	};

	class DSEventDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTString datatype = {"datatype"};
		DTInteger<> datatypeVersion = {"datatypeVersion"};

		DSEventDetail(string sname) : DSEventDetail("", "", 0) {
		}

		DSEventDetail(string iname, string DT, int DTVer) :
				DTStruct("DSEventDetail", 0, 0, 0) {
			name = iname;
			datatype = DT;
			datatypeVersion = DTVer;
			this->list.push_back(&name);
			this->list.push_back(&datatype);
			this->list.push_back(&datatypeVersion);
		}
	};

	class DSHookDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTString datatype = {"datatype"};
		DTInteger<> datatypeVersion = {"datatypeVersion"};

		DSHookDetail(string sname) : DSHookDetail("", "", 0) {
		}

		DSHookDetail(string iname, string DT, int DTVer) :
				DTStruct("DSHookDetail", 0, 0, 0) {
			name = iname;
			datatype = DT;
			datatypeVersion = DTVer;
			this->list.push_back(&name);
			this->list.push_back(&datatype);
			this->list.push_back(&datatypeVersion);
		}
	};

	class DSInstallInfoCommandCallDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTInteger<> inputDatatypeVersion = {"inputDatatypeVersion"};
		DTInteger<> outputDatatypeVersion = {"outputDatatypeVersion"};

		DSInstallInfoCommandCallDetail(string sname) : DSInstallInfoCommandCallDetail("", 0, 0) {
		}

		DSInstallInfoCommandCallDetail(string iname, int inDTVer, int outDTVer) :
				DTStruct("DSInstallInfoCommandCallDetail", 0, 0, 0) {
			name = iname;
			inputDatatypeVersion = inDTVer;
			outputDatatypeVersion = outDTVer;
			this->list.push_back(&name);
			this->list.push_back(&inputDatatypeVersion);
			this->list.push_back(&outputDatatypeVersion);
		}
	};

	class DSInstallInfoRequirementDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTInteger<> version = {"version"};

		DSInstallInfoRequirementDetail(string sname) : DSInstallInfoRequirementDetail("", 0) {
		}

		DSInstallInfoRequirementDetail(string iname, int iVer) :
				DTStruct("DSInstallInfoRequirementDetail", 0, 0, 0) {
			name = iname;
			version = iVer;
			this->list.push_back(&name);
			this->list.push_back(&version);
		}
	};

	class DSInstallInfoDatatypesDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTInteger<> version = {"version"};

		DSInstallInfoDatatypesDetail(string sname) : DSInstallInfoDatatypesDetail("", 0) {
		}

		DSInstallInfoDatatypesDetail(string iname, int iVer) :
				DTStruct("DSInstallInfoDatatypesDetail", 0, 0, 0) {
			name = iname;
			version = iVer;
			this->list.push_back(&name);
			this->list.push_back(&version);
		}
	};

	DTString name = {"name"};
	DTString title = {"title"};
	DTInteger<> serviceVersion = {"version"};
	DTSet<DSCommandDetail> commands = {"commands"};
	DTSet<DSEventDetail> events = {"events"};
	DTSet<DSHookDetail> hooks = {"hooks"};
	DTSet<DSInstallInfoCommandCallDetail> calls = {"calls"};
	DTSet<DSInstallInfoRequirementDetail> requirements = {"requirements"};
	DTSet<DSInstallInfoDatatypesDetail> datatypes = {"datatypes"};

	DSInstallInfo(string iname, string ititle, int iver) : DTStruct("installInfo", 0, 0, 0) {
		name = iname;
		title = ititle;
		serviceVersion = iver;
		this->list.push_back(&name);
		this->list.push_back(&title);
		this->list.push_back(&serviceVersion);
		this->list.push_back(&commands);
		this->list.push_back(&events);
		this->list.push_back(&hooks);
		this->list.push_back(&calls);
		this->list.push_back(&requirements);
		this->list.push_back(&datatypes);
	}

};

}}
#endif /* DTSTRUCTS_HPP_ */
