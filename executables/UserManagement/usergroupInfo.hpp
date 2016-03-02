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

namespace zeitoon{
namespace usermanagement{

class UMCore;
class usergroupInfo {
public:
	/**map e permissions, haavi ye <int permissionID, int permissionState> */
	std::map<int, int> permissions;
	/**usergroupInfo() default constructor*/
	usergroupInfo();
	/**usergroupInfo() constructor va tanha member function e in class ke
	 * @param groupID int ra migirad va baa estefaadde az aan permission haaye marboot be
	 * aan usergroup ra load mikonad dar std::map<int, int> permissions*/
	usergroupInfo(int groupIDn, UMCore* instance);
private:
	UMCore* coreInstance;
	/**moteghayyer e haavi e Id e group*/
	int groupID;
	std::string getNameAndType();
};


}//usermanagement
}//zeitoon
#endif /* USERMANAGEMENT_USERGROUPINFO_HPP_ */
