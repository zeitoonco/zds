//
// Created by naeyrin on 5/4/16.
//

#ifndef ZDS_DSSTRUCTS_HPP
#define ZDS_DSSTRUCTS_HPP

#include "datatypes/dtbase.hpp"
#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"
#include <string>
#include <vector>

using namespace zeitoon::datatypes;

namespace zeitoon {
    namespace chat {

        class EnumMsgType {
        public:
            enum msgType {
                user, userLog, system, __MAX
            };
            const static string typeString[];
        };

        class EnumReachState {
        public:
            enum reachState {
                normal, noSearch, noChart, oneWay, noChartSearch, __MAX
            };
            const static string typeString[];
        };

        class EnumStatus {
        public:
            enum status {
                normal, custom, invisible, busy, away, __MAX
            };
            const static string typeString[];
        };

        class EnumCustomStatusIcon {
        public:
            enum customStatusIcon {
                online, offline, busy, away, __MAX
            };
            const static string typeString[];
        };

        class DSNewMessage : public DTStruct {
        public:
            DTInteger<int> userID = {"userID"};
            DTInteger<int> sessionID = {"sessionID"};
            DTString msg = {"msg"};
            DTEnum<EnumMsgType> type = {"type"};

            static std::string getStructName() {
                return "DSNewMessage";
            }

            static int getStructVersion() {
                return 1;
            }

            DSNewMessage() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&userID);
                this->list.push_back(&sessionID);
                this->list.push_back(&msg);
                this->list.push_back(&type);
            }

            DSNewMessage(std::string data) : DSNewMessage() {
                this->fromString(data);
            }

            DSNewMessage(int iuserID, int isessionID, std::string imsg, EnumMsgType::msgType iEnumMsgType)
                    : DSNewMessage() {
                userID = iuserID;
                sessionID = isessionID;
                msg = imsg;
                type = iEnumMsgType;
            }

        };

        class DSCheckMessageItem : public DTStruct {
        public:
            DTInteger<int> sessionID = {"sessionID"};
            DTInteger<int> seenID = {"seenID"};
            DTInteger<int> notifiedID = {"notifiedID"};

            static std::string getStructName() {
                return "DSCheckMessageItem";
            }

            static int getStructVersion() {
                return 1;
            }

            DSCheckMessageItem() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&sessionID);
                this->list.push_back(&seenID);
                this->list.push_back(&notifiedID);
            }

            DSCheckMessageItem(std::string data) : DSCheckMessageItem() {
                this->fromString(data);
            }

            DSCheckMessageItem(int isessionID, int iseenID, int inotifiedID) : DSCheckMessageItem() {
                sessionID = isessionID;
                seenID = iseenID;
                notifiedID = inotifiedID;
            }
        };

        class DSCheckMessages : public DTStruct {
        public:
            DTSet<DSCheckMessageItem> mlist = {"mlist"};

            static std::string getStructName() {
                return "DSCheckMessages";
            }

            static int getStructVersion() {
                return 1;
            }

            DSCheckMessages() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&mlist);
            }

            DSCheckMessages(std::string data) : DSCheckMessages() {
                this->fromString(data);
            }

            DSCheckMessages(vector<DSCheckMessageItem> ilist) : DSCheckMessages() {
                mlist.addRange(ilist);
            }

            DSCheckMessages(DTSet<DSCheckMessageItem> ilist) : DSCheckMessages() {
                mlist = ilist;
            }
        };

        class DSGetMessages : public DTStruct {
        public:
            DTInteger<int> sessionID = {"sessionID"};
            DTEnum<EnumMsgType> type = {"type"};
            DTString from = {"from"};
            DTString to = {"to"};

            static std::string getStructName() {
                return "DSGetMessages";
            }

            static int getStructVersion() {
                return 1;
            }

            DSGetMessages() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&sessionID);
                this->list.push_back(&type);
                this->list.push_back(&from);
                this->list.push_back(&to);
            }

            DSGetMessages(std::string data) : DSGetMessages() {
                this->fromString(data);
            }

            DSGetMessages(int isessionID, EnumMsgType::msgType iEnumMsgType, std::string ifrom, std::string ito)
                    : DSGetMessages() {
                sessionID = isessionID;
                type = iEnumMsgType;
                from = ifrom;
                to = ito;
            }
        };

        class DSMessageItem : public DTStruct {
        public:
            DTInteger<int> userID = {"userID"};
            DTString date = {"date"};
            DTString msg = {"msg"};
            DTEnum<EnumMsgType> type = {"type"};

            static std::string getStructName() {
                return "DSMessageItem";
            }

            static int getStructVersion() {
                return 1;
            }

            DSMessageItem() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&userID);
                this->list.push_back(&date);
                this->list.push_back(&msg);
                this->list.push_back(&type);
            }

            DSMessageItem(std::string data) : DSMessageItem() {
                this->fromString(data);
            }

            DSMessageItem(int iuserID, std::string idate, std::string imsg, EnumMsgType::msgType iEnumMsgType)
                    : DSMessageItem() {
                userID = iuserID;
                date = idate;
                msg = imsg;
                type = iEnumMsgType;
            }
        };

        class DSMessageList : public DTStruct {
        public:
            DTSet<DSMessageItem> mmlist = {"mmlist"};

            static std::string getStructName() {
                return "DSMessageList";
            }

            static int getStructVersion() {
                return 1;
            }

            DSMessageList() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&mmlist);
            }

            DSMessageList(std::string data) : DSMessageList() {
                this->fromString(data);
            }

            DSMessageList(vector<DSMessageItem> ilist) : DSMessageList() {
                mmlist.addRange(ilist);
            }

            DSMessageList(DTSet<DSMessageItem> ilist) : DSMessageList() {
                mmlist = ilist;
            }
        };

        class DSChatUserData : public DTStruct {
        public:
            DTEnum<EnumReachState> reachState = {"reachState"};
            DTEnum<EnumStatus> status = {"status"};
            DTEnum<EnumCustomStatusIcon> customStatusIcon = {"customStatusIcon"};
            DTString customStatus = {"customStatus"};

            static std::string getStructName() {
                return "DSChatUserData";
            }

            static int getStructVersion() {
                return 1;
            }

            DSChatUserData() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&reachState);
                this->list.push_back(&status);
                this->list.push_back(&customStatusIcon);
                this->list.push_back(&customStatus);
            }

            DSChatUserData(std::string data) : DSChatUserData() {
                this->fromString(data);
            }

            DSChatUserData(EnumReachState::reachState ireachState, EnumStatus::status istatus,
                           EnumCustomStatusIcon::customStatusIcon icustomStatusIcon,
                           std::string icustomStatus)
                    : DSChatUserData() {
                reachState = ireachState;
                status = istatus;
                customStatusIcon = icustomStatusIcon;
                customStatus = icustomStatus;
            }

        };

        class DSChangeUserState : public DTStruct {
        public:
            DTEnum<EnumStatus> status = {"status"};
            DTEnum<EnumCustomStatusIcon> customStatusIcon = {"customStatusIcon"};
            DTString customStatus = {"customStatus"};

            static std::string getStructName() {
                return "DSChangeUserState";
            }

            static int getStructVersion() {
                return 1;
            }

            DSChangeUserState() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&status);
                this->list.push_back(&customStatusIcon);
                this->list.push_back(&customStatus);
            }

            DSChangeUserState(std::string data) : DSChangeUserState() {
                this->fromString(data);
            }

            DSChangeUserState(EnumStatus::status istatus,
                              EnumCustomStatusIcon::customStatusIcon icustomStatusIcon,
                              std::string icustomStatus)
                    : DSChangeUserState() {
                status = istatus;
                customStatusIcon = icustomStatusIcon;
                customStatus = icustomStatus;
            }

        };

        class DSAddUserSession : public DTStruct {
        public:
            DTInteger<int> userID = {"userID"};
            DTInteger<int> sessionID = {"sessionID"};

            static std::string getStructName() {
                return "DSAddUserSession";
            }

            static int getStructVersion() {
                return 1;
            }

            DSAddUserSession() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&userID);
                this->list.push_back(&sessionID);
            }

            DSAddUserSession(std::string data) : DSAddUserSession() {
                this->fromString(data);
            }

            DSAddUserSession(int iuserID, int isessionID) : DSAddUserSession() {
                userID = iuserID;
                sessionID = isessionID;
            }
        };

        class DSSessionList : public DTStruct {
        public:
            DTSet<DTInteger<int>> mnlist = {"mnlist"};

            static std::string getStructName() {
                return "DSSessionList";
            }

            static int getStructVersion() {
                return 1;
            }

            DSSessionList() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&mnlist);
            }

            DSSessionList(std::string data) : DSSessionList() {
                this->fromString(data);
            }

            DSSessionList(vector<DTInteger<int>> ilist) : DSSessionList() {
                mnlist.addRange(ilist);
            }

            DSSessionList(DTSet<DTInteger<int>> ilist) : DSSessionList() {
                mnlist = ilist;
            }
        };

        class DSSession : public DTStruct {
        public:
            DTInteger<int> id = {"id"};
            DTString creationDate = {"creationDate"};


            static std::string getStructName() {
                return "DSSession";
            }

            static int getStructVersion() {
                return 1;
            }

            DSSession() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&id);
                this->list.push_back(&creationDate);
            }

            DSSession(std::string data) : DSSession() {
                this->fromString(data);
            }

            DSSession(int iuserID, int iid, std::string icreationDate)
                    : DSSession() {
                id = iid;
                creationDate = icreationDate;
            }

        };

    }
}

#endif //ZDS_DSSTRUCTS_HPP