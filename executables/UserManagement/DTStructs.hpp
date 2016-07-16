/*
 * DSStructs.hpp
 *
 *  Created on: Feb 23, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_DTSTRUCTS_HPP_
#define USERMANAGEMENT_DTSTRUCTS_HPP_

#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"
#include "datatypes/DTTableString.hpp"
#include <vector>
#include <string>

namespace zeitoon {
namespace usermanagement {

using namespace zeitoon::datatypes;

class DSLoginInfo : public DTStruct {
public:
    DTString username = {"username"};
    DTString password = {"password"};

    static std::string getStructName() {
        return "DSLoginInfo";
    }

    static int getStructVersion() {
        return 1;
    }

    DSLoginInfo() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&username);
        this->list.push_back(&password);
    }

    DSLoginInfo(std::string data) :
            DSLoginInfo() {
        this->fromString(data);
    }

    DSLoginInfo(std::string iusername, std::string ipassword) :
            DSLoginInfo() {
        username = iusername;
        password = ipassword;

    }
};

class DSChkPermission : public DTStruct {
public:
    DTInteger<int> sessionID = {"sessionID"};
    DTInteger<int> permissionID = {"permissionID"};

    static std::string getStructName() {
        return "DSChkPermission";
    }

    static int getStructVersion() {
        return 1;
    }

    DSChkPermission() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&permissionID);
        this->list.push_back(&sessionID);
    }

    DSChkPermission(std::string data) :
            DSChkPermission() {
        this->fromString(data);
    }

    DSChkPermission(int isessionID, int ipermissionID) :
            DSChkPermission() {
        permissionID = ipermissionID;
        sessionID = isessionID;

    }
};

class DSChkPermissionByName : public DTStruct {
public:
    DTInteger<int> sessionID = {"sessionID"};
    DTString pname = {"name"};

    static std::string getStructName() {
        return "DSChkPermissionByName";
    }

    static int getStructVersion() {
        return 1;
    }

    DSChkPermissionByName() : DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&pname);
        this->list.push_back(&sessionID);
    }

    DSChkPermissionByName(std::string data) : DSChkPermissionByName() {
        this->fromString(data);
    }

    DSChkPermissionByName(int isessionID, string permName) : DSChkPermissionByName() {
        pname = permName;
        sessionID = isessionID;
    }
};

class DSAddUser : public DTStruct {
public:
    DTString username = {"username"};
    DTString password = {"password"};
    DTString name = {"name"};

    static std::string getStructName() {
        return "DSAddUser";
    }

    static int getStructVersion() {
        return 1;
    }

    DSAddUser() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&username);
        this->list.push_back(&password);
        this->list.push_back(&name);
    }

    DSAddUser(std::string data) :
            DSAddUser() {
        this->fromString(data);
    }

    DSAddUser(std::string iusername, std::string ipassword, std::string iname) :
            DSAddUser() {
        username = iusername;
        password = ipassword;
        name = iname;
    }
};

class DSModifyUser : public DTStruct {
public:
    DTInteger<int> userID = {"userID"};
    DTString username = {"username"};
    DTString password = {"password"};
    DTString name = {"name"};

    static std::string getStructName() {
        return "DSModifyUser";
    }

    static int getStructVersion() {
        return 1;
    }

    DSModifyUser() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&userID);
        this->list.push_back(&username);
        this->list.push_back(&password);
        this->list.push_back(&name);
    }

    DSModifyUser(std::string data) :
            DSModifyUser() {
        this->fromString(data);
    }

    DSModifyUser(int iuserID, std::string iusername, std::string ipassword, std::string iname) :
            DSModifyUser() {
        userID = iuserID;
        username = iusername;
        password = ipassword;
        name = iname;

    }
};

class DSUserInfo : public DTStruct {
public:
    DTInteger<int> userID = {"userID"};
    DTString username = {"username"};
    DTString name = {"name"};
    DTBoolean banned = {"banned"};
    DTString banReason = {"banReason"};
    DTBoolean isOnline = {"isOnline"};

    static std::string getStructName() {
        return "DSUserInfo";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUserInfo() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&userID);
        list.push_back(&username);
        list.push_back(&banReason);
        list.push_back(&banned);
        list.push_back(&name);
        list.push_back(&isOnline);
    }

    DSUserInfo(std::string data) :
            DSUserInfo() {
        this->fromString(data);
    }

    DSUserInfo(int iuserID, std::string iusername, std::string iname, bool ibanned, std::string ibanReason,
               bool iOnline) :
            DSUserInfo() {
        userID = iuserID;
        username = iusername;
        banReason = ibanReason;
        banned = ibanned;
        name = iname;
        isOnline = iOnline;

    }
};

class DSRegPermission : public DTStruct {
public:
    DTString name = {"name"};
    DTString title = {"title"};
    DTString description = {"description"};
    DTInteger<int> parentID = {"parentID"};

    static std::string getStructName() {
        return "DSRegPermission";
    }

    static int getStructVersion() {
        return 1;
    }

    DSRegPermission() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&name);
        list.push_back(&title);
        list.push_back(&description);
        list.push_back(&parentID);
    }

    DSRegPermission(std::string data) :
            DSRegPermission() {
        this->fromString(data);
    }

    DSRegPermission(std::string iname, std::string ititle, std::string idescription, int iparentID) :
            DSRegPermission() {
        name = iname;
        title = ititle;
        description = idescription;
        parentID = iparentID;

    }
};

class DSUpdatePermission : public DTStruct {
public:
    DTInteger<int> permissiosnID = {"permissionID"};
    DTString name = {"name"};
    DTString title = {"title"};
    DTString description = {"description"};
    DTInteger<int> parentID = {"parentID"};

    static std::string getStructName() {
        return "DSUpdatePermission";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUpdatePermission() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&permissiosnID);
        list.push_back(&name);
        list.push_back(&title);
        list.push_back(&description);
        list.push_back(&parentID);
    }

    DSUpdatePermission(std::string data) :
            DSUpdatePermission() {
        this->fromString(data);
    }

    DSUpdatePermission(int ipermissionID, std::string iname, std::string ititle, std::string idescription,
                       int iparentID) :
            DSUpdatePermission() {
        permissiosnID = ipermissionID;
        name = iname;
        title = ititle;
        description = idescription;
        parentID = iparentID;
    }
};

class DSRegUsrGrp : public DTStruct {
public:
    DTInteger<int> parentID = {"parentID"};
    DTString title = {"title"};
    DTString description = {"description"};

    static std::string getStructName() {
        return "DSRegUsrGrp";
    }

    static int getStructVersion() {
        return 1;
    }

    DSRegUsrGrp() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&title);
        list.push_back(&parentID);
        list.push_back(&description);
    }

    DSRegUsrGrp(std::string JSONdata) :
            DSRegUsrGrp() {
        this->fromString(JSONdata);
    }

    DSRegUsrGrp(std::string ititle, int iparentID, std::string idescription) :
            DSRegUsrGrp() {
        title = ititle;
        parentID = iparentID;
        description = idescription;

    }
};

class DSUpdateUsrGrp : public DTStruct {
public:
    DTInteger<int> usergroupID = {"usergroupID"};
    DTString title = {"title"};
    DTInteger<int> parentID = {"parentID"};
    DTString description = {"description"};

    static std::string getStructName() {
        return "DSUpdateUsrGrp";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUpdateUsrGrp() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&usergroupID);
        list.push_back(&title);
        list.push_back(&parentID);
        list.push_back(&description);
    }

    DSUpdateUsrGrp(std::string JSONdata) :
            DSUpdateUsrGrp() {
        this->fromString(JSONdata);
    }

    DSUpdateUsrGrp(int iusergroupID, std::string ititle, int iparentID, std::string idescription) :
            DSUpdateUsrGrp() {
        usergroupID = iusergroupID;
        title = ititle;
        parentID = iparentID;
        description = idescription;

    }

};

class DSUserList : public DTStruct {
public:
    DTSet<DSUserInfo> usersList = {"userList"};

    static std::string getStructName() {
        return "DSUserList";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUserList() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&usersList);
    }

    DSUserList(std::string data) :
            DSUserList() {
        this->fromString(data);
    }

};

class DSPermissionsList : public DTStruct {
public:
    DTSet<DSUpdatePermission> permissionsList = {"listPermissions"};

    static std::string getStructName() {
        return "DSlistPermissions";
    }

    static int getStructVersion() {
        return 1;
    }

    DSPermissionsList() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&permissionsList);
    }

    DSPermissionsList(std::string data) :
            DSPermissionsList() {
        this->fromString(data);
    }
};

class DSUserGroupsList : public DTStruct {
public:
    DTSet<DSUpdateUsrGrp> userGrpsList = {"userGroupsList"};

    static std::string getStructName() {
        return "DSUserGroupsList";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUserGroupsList() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&userGrpsList);
    }

    DSUserGroupsList(std::string data) :
            DSUserGroupsList() {
        this->fromString(data);
    }
};

class DSUserUsergroup : public DTStruct {
public:
    DTInteger<int> userID = {"userID"};
    DTInteger<int> groupID = {"groupID"};

    static std::string getStructName() {
        return "DSUserUsergroup";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUserUsergroup() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&userID);
        list.push_back(&groupID);
    }

    DSUserUsergroup(std::string data) :
            DSUserUsergroup() {
        this->fromString(data);
    }

    DSUserUsergroup(int iuserID, int igroupID) :
            DSUserUsergroup() {
        userID = iuserID;
        groupID = igroupID;

    }
};

class DSUserPermission : public DTStruct {
public:
    DTInteger<int> userID = {"userID"};
    DTInteger<int> permissionID = {"permissionID"};
    DTInteger<int> permissionState = {"state"};

    static std::string getStructName() {
        return "DSUserPermission";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUserPermission() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&userID);
        list.push_back(&permissionID);
        list.push_back(&permissionState);
    }

    DSUserPermission(std::string data) :
            DSUserPermission() {
        this->fromString(data);
    }

    DSUserPermission(int iuserID, int ipermissionID, int ipermissionState) :
            DSUserPermission() {
        permissionID = ipermissionID;
        userID = iuserID;
        permissionState = ipermissionState;

    }
};

class DSPermissionState : public DTStruct {
public:
    DTInteger<int> permissionID = {"id"};
    DTInteger<int> permissionState = {"state"};

    static std::string getStructName() {
        return "DSPermissionState";
    }

    static int getStructVersion() {
        return 1;
    }

    DSPermissionState() :
            DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&permissionID);
        list.push_back(&permissionState);
    }

    DSPermissionState(std::string data) :
            DSPermissionState() {
        this->fromString(data);
    }

    DSPermissionState(int ipermissionID, int ipermissionState) :
            DSPermissionState() {
        permissionID = ipermissionID;
        permissionState = ipermissionState;

    }
};

class DSUserPermissionList : public DTStruct {
public:
    DTSet<DSPermissionState> permissionsList = {"listPermissions"};

    static std::string getStructName() {
        return "DSUserPermissionList";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUserPermissionList() : DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&permissionsList);
    }

    DSUserPermissionList(std::string data) : DSUserPermissionList() {
        this->fromString(data);
    }
};

class DSUsergroupPermission : public DTStruct {
public:
    DTInteger<int> usergroupID = {"usergroupID"};
    DTInteger<int> permissionID = {"permissionID"};
    DTInteger<int> permissionState = {"state"};

    static std::string getStructName() {
        return "DSUsergroupPermission";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUsergroupPermission() : DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        list.push_back(&usergroupID);
        list.push_back(&permissionID);
        list.push_back(&permissionState);
    }

    DSUsergroupPermission(std::string data) : DSUsergroupPermission() {
        this->fromString(data);
    }

    DSUsergroupPermission(int iusergroupID, int ipermissionID, int ipermissionState) : DSUsergroupPermission() {
        permissionID = ipermissionID;
        usergroupID = iusergroupID;
        permissionState = ipermissionState;
    }
};

class DSUsergroupPermissionList : public DTStruct {
public:
    DTSet<DSPermissionState> permissionsList = {"listPermissions"};

    static std::string getStructName() {
        return "DSUsergroupPermissionList";
    }

    static int getStructVersion() {
        return 1;
    }

    DSUsergroupPermissionList() : DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&permissionsList);
    }

    DSUsergroupPermissionList(std::string data) : DSUsergroupPermissionList() {
        this->fromString(data);
    }
};

class DSBrfPermission : public DTStruct {
public:
    DTInteger<int> permissionID = {"permissionID"};
    DTString name = {"name"};
    DTInteger<int> parentID = {"parentID"};
    DTBoolean state = {"state"};

    static std::string getStructName() {
        return "DSBrfPermission";
    }

    static int getStructVersion() {
        return 1;
    }

    DSBrfPermission() : DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&permissionID);
        this->list.push_back(&name);
        this->list.push_back(&parentID);
        this->list.push_back(&state);

    }

    DSBrfPermission(int ipermissionID, std::string iname, int iparentID, bool istate)
            : DSBrfPermission() {
        this->permissionID = ipermissionID;
        this->name = iname;
        this->parentID = iparentID;
        this->state = istate;
    }

    DSBrfPermission(std::string data) : DSBrfPermission() {
        this->fromString(data);
    }
};

class DSLoginResult : public DTStruct {
public:
    DTString UMLoginResult = {"UMLoginResult"};
    DTString description = {"description"};
    DTInteger<int> sessionID = {"sessionID"};
    DSUserInfo userInfo;
    DTSet<DSBrfPermission> permissions = {"permissions"};

    static std::string getStructName() {
        return "DSLoginResult";
    }

    static int getStructVersion() {
        return 1;
    }

    DSLoginResult() : DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
        this->list.push_back(&UMLoginResult);
        this->list.push_back(&description);
        this->list.push_back(&sessionID);
        this->list.push_back(&userInfo);
        this->list.push_back(&permissions);
    }

    DSLoginResult(std::string data) :
            DSLoginResult() {
        this->fromString(data);
    }

    DSLoginResult(std::string iUMLoginResult, std::string idescription, int isessionID) :
            DSLoginResult() {
        UMLoginResult = iUMLoginResult;
        description = idescription;
        sessionID = isessionID;
    }

    string toString(bool includeVersion, int indent = -1, std::string indentContent = defaultIndentContent) const {
        vector<string> *skipf = NULL;
        if (this->sessionID.getValue() == -1)
            skipf = new vector<string>{userInfo.getName(), permissions.getName()};
        return DTStruct::toString(includeVersion, indent, indentContent, skipf);
    }

};


} //namespace: zeitoon
} //namespace: datatypes

#endif /* USERMANAGEMENT_DSSTRUCTS_HPP_ */
