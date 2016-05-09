/*
 * DTStructs.hpp
 *
 *  Created on: Feb 22, 2016
 *      Author: inf
 */
#ifndef UTIL_DTSTRUCTS_HPP_
#define UTIL_DTSTRUCTS_HPP_

#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"
#include "datatypes/DTTableString.hpp"

namespace zeitoon {
namespace datatypes {

class DSString : public DTStruct {
public:
	DTString value = {"value"};

	static std::string getStructName() {
		return "DSString";
	}

	static int getStructVersion() {
		return 1;
	}

	DSString() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
		this->list.push_back(&value);
	}

	DSString(std::string data, bool isJSON) :
			DSString() {
		if (isJSON) {
			this->fromString(data);
		} else {
			value = data;
		}
	}
};

class DSInteger : public DTStruct {
public:
	DTInteger<> value = {"value"};

	static std::string getStructName() {
		return "DSInteger";
	}

	static int getStructVersion() {
		return 1;
	}

	DSInteger() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
		this->list.push_back(&value);
	}
};

class DSFloat : public DTStruct {
public:
	DTFloat<> value = {"value"};

	static std::string getStructName() {
		return "DSFloat";
	}

	static int getStructVersion() {
		return 1;
	}

	DSFloat() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
		this->list.push_back(&value);
	}
};

class DSBoolean : public DTStruct {
public:
	DTBoolean value = {"value"};

	static std::string getStructName() {
		return "DSBoolean";
	}

	static int getStructVersion() {
		return 1;
	}

	DSBoolean() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
		this->list.push_back(&value);
	}
};

class DSDBTable : public DTStruct {
public:
	DTTableString value = {"value"};

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
};

class EnmServiceType {
public:
	enum EnmServiceTypesEnum {
		other, Core, Database, UserManager, GUI, BMS, Accounting, Chat, Reception, __MAX
	};
	const static string typeString[];
};

class ServiceTypePrototypes {//todo:later
public:
	class ServiceTypeVersion {
	public:

	};
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
		DTString permissionRequired = {"permissionRequired"};

		DSCommandDetail(string sname) : DSCommandDetail("", "", 0, "", 0) {
		}

		DSCommandDetail(string iname, string inputDT, int inputDTVer, string outputDT, int outputDTVer,
		                string reqPermission = "") : DTStruct("DSCommandDetail", 0, 0, 0) {
			this->list.push_back(&name);
			this->list.push_back(&inputDatatype);
			this->list.push_back(&inputDatatypeVersion);
			this->list.push_back(&outputDatatype);
			this->list.push_back(&outputDatatypeVersion);
			this->list.push_back(&permissionRequired);
			name = iname;
			inputDatatype = inputDT;
			inputDatatypeVersion = inputDTVer;
			outputDatatype = outputDT;
			outputDatatypeVersion = outputDTVer;
			if (reqPermission == "")
				permissionRequired = iname;
			else
				permissionRequired = reqPermission;

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
			this->list.push_back(&name);
			this->list.push_back(&datatype);
			this->list.push_back(&datatypeVersion);
			name = iname;
			datatype = DT;
			datatypeVersion = DTVer;

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
			this->list.push_back(&name);
			this->list.push_back(&datatype);
			this->list.push_back(&datatypeVersion);
			name = iname;
			datatype = DT;
			datatypeVersion = DTVer;

		}
	};

	class DSInstallInfoCommandCallDetail : public DTStruct {
	public:
		DTString name = {"name"};
		DTString inputDatatype = {"inputDatatype"};
		DTInteger<> inputDatatypeVersion = {"inputDatatypeVersion"};
		DTString outputDatatype = {"outputDatatype"};
		DTInteger<> outputDatatypeVersion = {"inputDatatypeVersion"};

		DSInstallInfoCommandCallDetail(string sname) : DSInstallInfoCommandCallDetail("", "", 0, "", 0) {
		}

		DSInstallInfoCommandCallDetail(string iname, string inputDT, int inputDTVer, string outputDT, int outputDTVer) :
				DTStruct("DSInstallInfoCommandCallDetail", 0, 0, 0) {
			this->list.push_back(&name);
			this->list.push_back(&inputDatatype);
			this->list.push_back(&inputDatatypeVersion);
			this->list.push_back(&outputDatatype);
			this->list.push_back(&outputDatatypeVersion);
			name = iname;
			inputDatatype = inputDT;
			inputDatatypeVersion = inputDTVer;
			outputDatatype = outputDT;
			outputDatatypeVersion = outputDTVer;

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
			this->list.push_back(&name);
			this->list.push_back(&version);
			name = iname;
			version = iVer;

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
			this->list.push_back(&name);
			this->list.push_back(&version);
			name = iname;
			version = iVer;

		}
	};

	DTString name = {"name"};
	DTString name2 = {"name2"};
	DTInteger<> serviceVersion = {"version"};
	DTInteger<> minSupportedVersion = {"minSupportedVersion"};
	DTEnum<EnmServiceType> serviceType = {"serviceType"};
	//DTInteger<> serviceVersion = {"version"};

	DTSet<DSCommandDetail> commands = {"commands"};
	DTSet<DSEventDetail> events = {"events"};
	DTSet<DSHookDetail> hooks = {"hooks"};
	DTSet<DSInstallInfoCommandCallDetail> calls = {"calls"};
	DTSet<DSInstallInfoRequirementDetail> installRequirements = {"installRequirements"};
	DTSet<DSInstallInfoRequirementDetail> enableRequirements = {"enableRequirements"};
	DTSet<DSInstallInfoDatatypesDetail> datatypes = {"datatypes"};

	DSInstallInfo(string iname, string iname2, int iver, int iminSupVer, EnmServiceType::EnmServiceTypesEnum iSrvisType)
			: DTStruct("installInfo", 0, 0, 0) {
		name = iname;
		name2 = iname2;
		serviceVersion = iver;
		minSupportedVersion = iminSupVer;
		serviceType = iSrvisType;
		this->list.push_back(&name);
		this->list.push_back(&name2);
		this->list.push_back(&serviceVersion);
		this->list.push_back(&minSupportedVersion);
		this->list.push_back(&serviceType);

		this->list.push_back(&commands);
		this->list.push_back(&events);
		this->list.push_back(&hooks);
		this->list.push_back(&calls);
		this->list.push_back(&installRequirements);
		this->list.push_back(&enableRequirements);
		this->list.push_back(&datatypes);
	}

};


class DatatypesUtility {//todo: move to utility?
public:
	static zeitoon::datatypes::DTSet<zeitoon::datatypes::DTString> toDTStringSet(
			std::vector<std::string> iVector) {
		zeitoon::datatypes::DTSet<DTString> temp("");
		for (size_t iter = 0; iter < iVector.size(); iter++) {
			temp.add(new DTString("", iVector[iter]), true);
		}
		return temp;
	}

	static std::vector<zeitoon::datatypes::DTString> toDTStringList(std::vector<std::string> iVector) {
		std::vector<zeitoon::datatypes::DTString> temp;
		for (size_t iter = 0; iter < iVector.size(); iter++) {
			temp.push_back(DTString("", iVector[iter]));
		}
		return temp;
	}
};


}
}
#endif /* DTSTRUCTS_HPP_ */
