//
// Created by inf on 3/7/16.
//

#ifndef ZDS_GUIUTILITY_HPP
#define ZDS_GUIUTILITY_HPP

#include <string>

namespace zeitoon {
namespace GUI {
class commandInfo {
public:
	//------------------Set Command Names

	static std::string getList() { return "gui.getList"; }

	static std::string getListOfLists() { return "gui.getListOfLists"; }

	static std::string registerList() { return "gui.registerList"; }

	static std::string updateList() { return "gui.updateList"; }

	static std::string removeList() { return "gui.removeList"; }

	static std::string getTemplate() { return "gui.getTemplate"; }

	static std::string registerTemplate() { return "gui.registerTemplate"; }

	static std::string updateTemplate() { return "gui.updateTemplate"; }

	static std::string removeTemplate() { return "gui.removeTemplate"; }

	static std::string getTemplateFilesList() { return "gui.getTemplateFilesList"; }

	static std::string getTemplatesList() { return "gui.getTemplatesList"; }

};


}
}
#endif //ZDS_GUIUTILITY_HPP
