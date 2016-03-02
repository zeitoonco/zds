/*
 * UMSessionManager.hpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_UMSESSIONMANAGER_HPP_
#define USERMANAGEMENT_UMSESSIONMANAGER_HPP_
#include <UserManagement/usergroupInfo.hpp>
#include <UserManagement/UMSession.hpp>
#include <string>
#include <map>


namespace zeitoon{
namespace usermanagement{
class UMCore;

class UMSessionManager {

public:
	~UMSessionManager();
	/**map sessionList, haaviye int e sessionID e makhsoos e har user va login information(UMSession)*/
	std::map<int, UMSession> sessionList;
	/**map usergroupCache haaviye int e ID e usergroup, va permissionhaa+state e har permission e motealegh be aan ast*/
	std::map<int, usergroupInfo> usergroupCache;
	/**map usergroupParentCache haaviye int e ID e group va int ID e parent aan group ast*/
	std::map<int, int> userGroupParentCache;
	/**map permissionParentCache haaviye Int e ID e permission va int e Id e parent e aan permission ast*/
	std::map<int, int> permissionParentCache;
	/**UMSessionManager() Default Constructor*/
	UMSessionManager(UMCore* instance);
	/**newSession(int userID), Id e user ra daryaaft mikonad va yek sessionID e UNIQUE be aan ekhtesaas midahad,
	 * login detail ra az database migirad  va dar map e sessionList zakhire mikonad
	 * @param userID int
	 * @return int sessionID e UNIQUE e sakhte shode */
	int newSession(int userID);
	/**removeSession(int ID), id e session ra migirad va aan ra az map e sessionList paak mikonad
	 * @param sessionID*/
	void removeSession(int id);

	void updateUsergroupCache();
	/**updateUsergroupCache(int id), id e usergroup raa migirad, sepas permission-haaye marboot be aan group ra
	 * az database migirad va dar map usergroupCache zakhire mikonad
	 * @param id int usergroupId*/
	void updateUsergroupCache(int id);
	int getSessionIDbyUserID(int userID);
	/**permissionParentCacheLoader, id e permission ra daryaaf mikonad va az database parent aan ra peydaa karde
		 * va dar map e permissionParentCache Zakhireh mikonad.
		 * @param permissionID int*/
		void permissionParentCacheLoader(int permissionID);


private:
UMCore* coreInstance;
	/**userGroupParentCacheLoader(int groupID), id e group ra daryaaft mikonad va
	 * bad az peyda kardan e parent aan group,, Id group va parent e aan raa dar map e userGroupParentCache
	 * zakhireh mikonad
	 * @param groupID int */
	void userGroupParentCacheLoader(int groupID);

	/**uniqueIdGenerator(), yek ID e Unique generate mikonad ke baraye sessionID estefaade mishavad*/
	int uniqueIdGenerator();
	/**
	 * getNameAndType baraye moshakhas kardan Owner e exception ast va kaarbord e ann faghat baraye exceptionEx hast
	 */
	std::string getNameAndType();
	/**
	 * Map<int,int> activeSessions, negah daarandeye ID e sessionhaaye active ba userID aan session ast <userID,sessionID>
	 */
	std::map<int, int> activeSessions;
};


}//usermanagement
}//zeitoon
#endif /* USERMANAGEMENT_UMSESSIONMANAGER_HPP_ */
