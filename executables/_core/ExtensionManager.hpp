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
#include <map>

namespace zeitoon {
namespace _core {

using namespace std;

class ExtensionManager {
private:
	typedef map<size_t, ExtensionProfile *> extMap;
	extMap extensionList;

public:
	ExtensionProfile *operator[](size_t index) {
		auto i = extensionList.find(index);
		if (i == extensionList.end())
			return NULL;
		//EXToutOfRange("operator[] failed. id '" + std::to_string(index) + "' is not valid");
		return i->second;
	}

	void add(size_t id, ExtensionProfile *excpp = NULL) {
		auto i = extensionList.find(id);
		if (i != extensionList.end())
			EXToutOfRange("operator add failed. id '" + std::to_string(id) + "' exists");
		extensionList.emplace(id, excpp);
	}

	void remove(size_t id) {
		auto i = extensionList.find(id);
		if (i == extensionList.end())
			EXToutOfRange("operator remove failed. id '" + std::to_string(id) + "' is not valid");
		extensionList.erase(i);
	}

	size_t idOf(std::string extension) {
		for (extMap::iterator i = extensionList.begin(); i != extensionList.end(); i++)
			if (i->second != NULL && utility::Strings::compare(i->second->name, extension, false) == 0)
				return i->first;
	}

	inline string getNameAndType() {
		return "ExtensionManager";
	}

};

}
}

#endif /* EXTENSIONMANAGER_HPP */
