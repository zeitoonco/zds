/*
 * ExtensionManager
 *
 *  Created on: Sep 1, 2015
 *      Author: ajl
 */

#ifndef EXTENSIONMANAGER_HPP
#define EXTENSIONMANAGER_HPP

#include <string>
#include <string>
#include <mutex>
#include <thread>
#include <unordered_map>
#include "Profiles.hpp"
#include <vector>
#include "utility/utility.hpp"
#include "utility/DTStructs.hpp"
#include <database/sqliteDatabaseHub.h>
#include <map>

namespace zeitoon {
namespace _core {

using namespace std;

class ExtensionManager {
private:
	typedef vector<ExtensionProfile *> extList;
	extList extensionList;

	utility::sqliteDatabaseHub *db;

	void deleteList() {
		for (int i = 0; i < extensionList.size(); i++)
			delete extensionList[i];
		extensionList.clear();
	}

public:
	ExtensionManager(utility::sqliteDatabaseHub *dbHandler) {
		db = dbHandler;
	}

	~ExtensionManager() {
		save();
		deleteList();
	}

	ExtensionProfile *operator[](size_t index) {
		return extensionList[index];
	}

	ExtensionProfile *operator[](string name) {
		for (int i = 0; i < extensionList.size(); i++)
			if (streq(extensionList[i]->serviceInfo.name.getValue(), name))
				return extensionList[i];
		return NULL;
	}

	ExtensionProfile *getByNetID(size_t id) {
		for (int i = 0; i < extensionList.size(); i++)
			if (extensionList[i]->netClientId == id)
				return extensionList[i];
		return NULL;
	}

	vector<ExtensionProfile *> getByServiceType(datatypes::EnmServiceType::EnmServiceTypesEnum type) {
		vector<ExtensionProfile *> list;
		for (int i = 0; i < extensionList.size(); i++)
			if (extensionList[i]->serviceInfo.serviceType.getValue() == type)
				list.push_back(extensionList[i]);
		return list;
	}

	void add(ExtensionProfile *excpp) {
		if (excpp == NULL)
			EXTinvalidParameter("NULL input");
		if (this->operator[](excpp->serviceInfo.name.getValue()) != NULL)
			EXToutOfRange("operator add failed. extension '" + excpp->serviceInfo.name.getValue() +
			              "' exists");
		extensionList.push_back(excpp);
	}

	void remove(ExtensionProfile *excpp) {
		if (excpp == NULL)
			EXTinvalidParameter("NULL input");
		remove(excpp->serviceInfo.name.getValue());
	}

	void remove(std::string exName) {
		for (extList::iterator i = extensionList.begin(); i < extensionList.end(); i++)
			if (streq((*i)->serviceInfo.name.getValue(), exName)) {
				extensionList.erase(i);
				return;
			}
		/*return extensionList[i];
ExtensionProfile *e = this->operator[](exName);
if (e == NULL)
	EXTinvalidName("Remove failed. Extension with name '" + exName + "' dosnt exist.");
extensionList.erase(excpp);*/
	}

	size_t size() {
		return extensionList.size();
	}

	bool isReqSatisfied(datatypes::DTSet<datatypes::DSInstallInfo::DSInstallInfoRequirementDetail> &list) {
		for (int i = 0; i < list.length(); i++) {
			ExtensionProfile *ext = this->operator[](list[i]->name.getValue());
			if (ext == NULL)
				return false;
			if (ext->serviceInfo.minSupportedVersion.getValue() > list[i]->version.getValue())
				return false;
			if (ext->state != ExtensionProfile::extensionState::enabled)
				return false;
			//else its ok!
		}
		return true;
	}

	inline string getNameAndType() {
		return "ExtensionManager";
	}

	void save() {
		stringstream sql;
		sql << "BEGIN;\ndelete from services;\n";
		for (int i = 0; i < extensionList.size(); i++) {
			ExtensionProfile *e = extensionList[i];
			if (e->state == ExtensionProfile::extensionState::notInstalled ||
			    e->state == ExtensionProfile::extensionState::installing ||
			    e->state == ExtensionProfile::extensionState::unknown)
				continue;
			sql << "insert into services(name,version,installid,state,CEPReg,data) values('" <<
			e->serviceInfo.name.getValue()
			<< "'," << e->serviceInfo.serviceVersion.getValue() << ",'" << e->installID << "'," << (int) e->state <<
			"," << (e->CEPermissionsRegistered ? 1 : 0) << ",'" << e->serviceInfo.toString() << "');\n";
		}
		sql << "END;";
		db->execute(sql.str());
	}

	void load() {
		deleteList();
		datatypes::DTTableString dt = db->query("select name,version,installid,state,CEPReg,data from services");
		for (int c = 0; c < dt.rowCount(); c++) {
			ExtensionProfile *e = new ExtensionProfile(dt.fieldValue(c, 5),
			                                           (ExtensionProfile::extensionState) stoi(dt.fieldValue(c, 3)));
			e->installID = dt.fieldValue(c, 2);
			e->CEPermissionsRegistered = stoi(dt.fieldValue(c, 4));
			add(e);
		}
	}

};

}
}

#endif /* EXTENSIONMANAGER_HPP */
