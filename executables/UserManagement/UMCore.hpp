/*
 * UMCore.hpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_UMCORE_HPP_
#define USERMANAGEMENT_UMCORE_HPP_

#include <executables/UserManagement/UManagementUtility.hpp>
#include <executables/UserManagement/UMSessionManager.hpp>
#include "datatypes/DTTableString.hpp"
#include "datatypes/dtsingletypes.hpp"
#include "utility/logger.hpp"
#include "DTStructs.hpp"

namespace zeitoon {
namespace usermanagement {

class UmCHI;

class UMCore {
	UmCHI *umCHI;

public:
	logger systemLog;

	UMCore(UmCHI *instancePtr);

	~UMCore();

	/**login(), ba daryaaft e voroodi, eghdaaam be login mikonad,
	 * @param username string
	 * @param password string
	 * @param name string
	 * @param sessionID int& -> az reference estefaade shode ke betavaan ba estefade az aan meghdari ra baaz gardand
	 * @param desc string& -> az reference estefaade shode ke betavaan ba estefade az aan meghdari ra baaz gardand
	 * @return UMLoginResult vaziat e login e anjaam shode
	 * @return sessionID int , sessionId e generate shode, dar soorate adam e movafaghiat -1 baaz migardaanad
	 * @return desc string, tozihaat  e marboot be login dar soorat e adam movafaghiat!*/
	UMLoginResult::UMLoginResultEnum login(std::string username, std::string password,
	                                       int &sessionID, int &uID,    //UMLoginResult
	                                       std::string &desc);

	/**logout(), ba daryaaft e session Id logout mikonad (az map e sessionList paak mikonad)
	 * @param sessionID int*/
	void logout(int sessionID);

	/**checkPermission() ba daryaaft e sessionID va permissionID,, baressi mikonad ke user e morde nazar aya daraye
	 * permission e mored e nazar mibashad ya kheyr
	 * @param sessionID int
	 * @param permissionID int
	 * @return boolean (0 agar mojavez nadasht,, 1 agar mojavez dasht)*/
	bool checkPermission(int sessionID, int permissionID);

	bool checkPermissionByName(int sessionID, string permissionName);

	/**addUser() ba daryaft e naam va naam e karbari va password, yek user ba hamaan moshakhasat misazad
	 * va Id e marboot be  user ke dar database darj shode ra barmigardaanad
	 * @param username string
	 * @param password string
	 * @param name string
	 * @return int id e user dar DB*/
	int addUser(std::string username, std::string password, std::string name);

	/**removeUser(), id e user ra daryaaft karde va aan user raa az database hazv mikonadd
	 * @param userID int*/
	void removeUser(int userID);

	/**modifyUser(), moshakhasaat e user raa daryaafte karde va kolle moskhasaat e darj shode dar database ra ba tavajoh
	 * be id e user taghyir midahat
	 * @param userID int
	 * @param username string
	 * @param password string
	 * @param name string*/
	void modifyUser(int userID, std::string username, std::string password, std::string name);

	/**getUserInfo() baa daryaaft e Id e User, moshakhasat e marboot be user e mored e nazar ra (dar ghaaleb e yek class)
	 * barmigardanad.
	 * @param ID int UserID
	 * @return UMUserInfo user information*/
	UMUserInfo getUserInfo(int userID);

	/**registerPermission(), tamam e moshakhasaat e yek permission e jadid raagerefte va aan ra dar
	 * database zakhireh mikonad va ID e aan ra barmigardaanad
	 * @param name string
	 * @param title string
	 * @param desc string
	 * @param parentID int
	 * @return int permissionID*/
	int registerPermission(std::string name, std::string title, std::string desc, int parent);

	/**updatePermission(), tamam e moshakhast e yek permission ra migirad va baa tavajoh be ID e permission
	 * aan ra taghyir midahad
	 * @param permissionID int
	 * @param name string
	 * @param title string
	 * @param desc string
	 * @param parentID int*/
	void updatePermission(int permissionID, std::string name, std::string title, std::string desc, int parentID);

	/**removePermission(), id e permission ra gerefte va aan ra az database hazv mikonad
	 * @param permissionID int
	 */
	void removePermission(int permissionID);

	/** registerUsergroup(), moshakhasaat e marboot be yek usergroup e jadid ragerefte va
	 * aan ra dar database zakhireh mikonad va meghdaar e aan ra barmigardaanad.
	 * @param title string
	 * @param prentID int
	 * @param desc string
	 * @return int usergroupID
	 */
	int registerUsergroup(std::string title, int parentID, std::string desc);

	/**updateUsergroup(), tamam e moshakhast e marboot be usergroup ra daryaaft mikonad
	 * va baa tavajoh be Id e usergroup, aan ra taghyiir midahat.
	 * @param usergroupID int
	 * @param title string
	 * @param parentID int
	 * @param desc string
	 */
	void updateUsergroup(int usergroupID, std::string title, int parentID, std::string desc);

	/**removeUsergroup(), ID e usergroup ra daryaaf mikonad va aaan ra az database hazv mikonad
	 * @param usergroupID int
	 */
	void removeUsergroup(int usergroupID);

	/** listUsers(), tamami e userhaaye mojood dar database raa be soorat e yek vector e "zeitoon::datatypes::DTString" baaz migardaanad
	 * @return vector<"zeitoon::datatypes::DTString"> users
	 */
	DSUserList listUsers();

	/** listUsersByGroup(), baa daryaft e id yek group, list e naam e tamamiye
	 * user-haaye e ozv e aan group raa be soorate vector<"zeitoon::datatypes::DTString"> barmigardaanad
	 *@param groupID int
	 *@return vector<"zeitoon::datatypes::DTString">
	 */
	DSUserList listUsersByGroup(int groupID);

	/**listPermissions(), listi az naam e kolle permission haaye mojood dar database ra
	 * be soorat e vector<"zeitoon::datatypes::DTString"> bar-migardaanad
	 * @return vector<"zeitoon::datatypes::DTString">
	 */
	DSPermissionsList listPermissions();

	/** listi az kolle userGroup-haaye mojood dar database raa be soorat e yek vector<"zeitoon::datatypes::DTString"> barmigardanad
	 * @return vector<"zeitoon::datatypes::DTString">
	 */
	DSUserGroupsList listUsergroups();

	/** executeSync() baa daryaaft e string e haaviye "SQL" be soorat e Synchronize tedaad e tupple haay e ta'sir gerefte az farmaan
	 * raa baaz migardanad.
	 * @param string cmd Haaviye farmaan e sql
	 * @return int affected rows
	 */
	int executeSync(std::string cmd);

	/** querySync() baa daryafte farman e sql, result ra be soorat e class e DTTableString barmigardaanad
	 * @param string cmd farmaan e sql
	 * @return zeitoon::datatypes::DTTableString Nataayej e farman sql
	 */
	zeitoon::datatypes::DTTableString querySync(std::string sql);

	/** singleFieldQuerySync() ba daryaft e farman sql meghdar e field e 0,0 ra barmigardaanad,
	 * @param string query Farmaan e SQL
	 * @return string Meghdaar e tak field
	 */
	std::string singleFieldQuerySync(std::string query);

	void addUserUsergroup(int userID, int usergroupID);

	void removeUserUsergroup(int userID, int usergroupID);

	void addUserPermission(int userID, int permissionID, int state);

	void removeUserPermission(int userID, int permissionID);

	DSUserPermissionList listUserPermissions(int userID);

	void addUsergroupPermission(int usergroupID, int permissionID, int state);

	void removeUsergroupPermission(int usergroupID, int permissionID, int state = 0);

	DSUsergroupPermissionList listUsergroupPermissions(int usergroupID);

	/**
    * bool isOnline returns user stats
	 **/
	bool isOnline(int userID);

private:
	/*
	 typedef  void (UMCore::*BPTR) (std::string);
	 void (UMCore::*queryFuncPtr) (std::string);// &UMCore::query();
	 void (UMCore::*Executes)(std::string);// &UMCore::Executes;
	 */
	/**
	 * struct userAllInfo{} baraye negah daari e info e marboot be login e useri mmibaashad ke hata yek baar password e khod
	 * raa eshtebah mizanad,
	 * @param vector<int> logAttemptTimes haavi e zaman e har eghdaam be login e  eshtebah mibaashad
	 * @param int numberOfAttempts yek counter baraye tedad e dafa'aat e login e eshtebah ast
	 * @param int Bannedtime baraye sabt zaman e ban shodan e movaghat mibaashad
	 * @param bool banned baraye moshakhas kardannn vaziat e ban e user ast
	 */
	struct userLogInfoStruct {

		std::vector<int> logAttemptTimes;
		int Banedtime = 0;
		bool banned = false;
	};
	/**
	 * map usreLogInfo baraye negah-daari e etelaa@ e login har user dar marhaleye authentication hast!(makhsoos e False attempts)
	 *  std::map<std::string (username), userAllInfo>
	 */
	std::map<std::string, userLogInfoStruct> userLogInfo;

	/** Object e USMcoreUser baraye estefaadeye class e UMCore az class e UMSessionManager sakhte shode
	 */
	UMSessionManager sessionManager;

	/** hashingProccess(), yek string daryaft mikonad va pas az hash kardan, aan ra barmigardaanad
	 * @param STRING sting
	 * @return string HASHED-STRING
	 */
	std::string hashingProccess(std::string STRING);

	/**checkUserPermissionState(), ba daryafte sessionID va Permission e mored e nazar, baresi mikonad
	 * agar permission e mored e nazar dar map e permissionCache e user mojood bashad,
	 * State e aan ra barmigardaanad
	 * @param sessionID int
	 * @param permissionID int
	 * @return int permission State
	 */
	int checkUserPermissionState(int sessionID, int permissionID);

	/**checkUserPermissionParentState(), ba daryafte sessionID va Permission e mored e nazar, baresi mikonad
	 * agar permission parent  e permission e  mored e nazar dar map e permissionCache e user mojood bashad,
	 * State e aan ra barmigardaanad
	 * @param sessionID int
	 * @param permissionID int
	 * @return int permission State
	 */
	int checkUserPermissionParentState(int sessionID, int permissionID);

	/**checkUsergroupPermission(), ba daryafte sessionID va Permission e mored e nazar, baresi mikonad
	 * agar permission e mored e nazar dar map e yeki az usergroup haaye user mojood bashad,
	 * State e aan ra barmigardaanad
	 * @param sessionID int
	 * @param permissionID int
	 * @return int permission State
	 */
	int checkUsergroupPermission(int sessionID, int PermissionID);

	/**checkUsergroupParentPermission(), ba daryafte sessionID va Permission e mored e nazar, baresi mikonad
	 * agar permission e mored e nazar dar map e yeki az usergroupParent haaye user mojood bashad,
	 * State e aan ra barmigardaanad
	 * @param sessionID int
	 * @param permissionID int
	 * @return int permission State
	 */
	int checkUsergroupParentPermission(int sessionID, int permissionID);

	/**getPermissionParent() baraye daryaft e parent e permission dar mavagheye khaas(maanand vaghty ke parent dar Cache vojood nadarad)
	 * @param permissionID int
	 * @return int Permission Parent ID
	 */
	int getPermissionParent(int permissionID);

	/**getUsergroupParent() baraye daryaft e parent e usergroup dar mavagheye khaas(maanand vaghty ke parent dar Cache vojood nadarad)
	 * @param permissionID int
	 * @return int usergroup parent ID
	 */
	int getUsergroupParent(int groupID);

	/**
	 * getNameAndType baraye moshakhas kardan Owner e exception ast va kaarbord e ann faghat baraye exceptionEx hast
	 */
	std::string getNameAndType();


};

}//usermanagement
}//zeitoon
#endif /* USERMANAGEMENT_UMCORE_HPP_ */
