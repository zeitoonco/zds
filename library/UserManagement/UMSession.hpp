/*
 * UMSession.hpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_UMSESSION_HPP_
#define USERMANAGEMENT_UMSESSION_HPP_

#include <map>
#include <vector>
#include <string>

namespace zeitoon{
namespace usermanagement{

class UMCore;
class UMSession {

public:
	/** Moteghayyer e haaviye username*/
	std::string username;
	/**moteghayyer e haviiye userID*/
	int userID;
	/**moteghayyere haaviye sessionID (UNIQUE)*/
	int sessionID;
	/**map permissionsCache haaviye Id e permission va State e permission-haayi hast ke mostaghiman aan user darad*/
	std::map<int, int> permissionsCache;//Permission---State
	/**vector usergroups haaviye ID usergroup-haayi ast ke user ozvi az aanha ast*/
	std::vector<int> usergruops;
	/**Default Constructor*/
	UMSession();
	/**Overloaded constructor.
	 *@param userIDIN int ID e user
	 *@param sessionIDIN e UNIQUE */
	UMSession(int userIDIN, int sessionIDIN, UMCore* instance);
	/**updatePermissionsCache() tamaamiye permissionhaaye marboot be user ra az Database mikhaanad
	 * va dar map permissions load mikonad*/
	void updatePermissionsCache();
	/**updatePermissionsCache(int permissionID) tanhaa ba daryaafte ID permission State marboot be aan raa az database migirad
	 *@param permissionID int */
	void updatePermissionsCache(int permissionID);
	/**clearPermissionCache() Kolle map e permissions ke marboot be yek user ast raa paak mikonad*/
	void clearPermissionsCache();
	/**clearUsergroups() Kolle vector marboot be usergroups e user raa paak mikonad*/
	void clearUsergroups();
	/**updateUsergroups() Tamaamie usergroups haayi ke user ozv e aan ast ra az database migirad va
	 * dar vector usergroups load mikonad*/
	void updateUsergroups();

private:
	UMCore* coreInstance;
	/**
	 * getNameAndType baraye moshakhas kardan Owner e exception ast va kaarbord e ann faghat baraye exceptionEx hast
	 */
	std::string getNameAndType();

};
}//usermanagement
}//zeitoon


#endif /* USERMANAGEMENT_UMSESSION_HPP_ */
