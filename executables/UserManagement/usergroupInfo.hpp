/*
 * usergroupInfo.hpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_USERGROUPINFO_HPP_
#define USERMANAGEMENT_USERGROUPINFO_HPP_

#include <map>
#include <string>
#include <vector>

namespace zeitoon{
namespace usermanagement{

class UMCore;
class usergroupInfo {
public:
	int parentID = -1;
	int groupID;
	/**map e permissions, haavi ye <int permissionID, int permissionState> */
	std::map<int, int> permissions;
	/**usergroupInfo() default constructor*/
	usergroupInfo();
	/**usergroupInfo() constructor va tanha member function e in class ke
	 * @param groupID int ra migirad va baa estefaadde az aan permission haaye marboot be
	 * aan usergroup ra load mikonad dar std::map<int, int> permissions*/
	usergroupInfo(int groupIDn, UMCore* instance);

	usergroupInfo(int groupID, int iparentID);
private:
	UMCore* coreInstance;
	/**moteghayyer e haavi e Id e group*/
	std::string getNameAndType();
};


}//usermanagement
}//zeitoon
#endif /* USERMANAGEMENT_USERGROUPINFO_HPP_ */
