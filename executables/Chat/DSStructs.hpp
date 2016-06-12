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
        class EnumGetMsgType {
        public:
            enum getMsgType {
                date, id, seen, notified, __MAX
            };
            const static string typeString[];
        };

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

            DSCheckMessageItem(std::string name) : DSCheckMessageItem() {

            }

            DSCheckMessageItem(int isessionID, int iseenID, int inotifiedID) : DSCheckMessageItem() {
                sessionID = isessionID;
                seenID = iseenID;
                notifiedID = inotifiedID;
            }
        };

        class DSCheckMessages : public DTStruct {
        public:
            DTSet<DSCheckMessageItem> mlist = {"list"};

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

            DSCheckMessages(DTSet<DSCheckMessageItem> ilist) : DSCheckMessages() {
                mlist = ilist;
            }
        };

        class DSGetMessages : public DTStruct {
        public:
            DTInteger<int> userID = {"userID"};
            DTInteger<int> sessionID = {"sessionID"};
            DTEnum<EnumGetMsgType> type = {"type"};
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
                this->list.push_back(&userID);
                this->list.push_back(&sessionID);
                this->list.push_back(&type);
                this->list.push_back(&from);
                this->list.push_back(&to);
            }

            DSGetMessages(std::string data) : DSGetMessages() {
                this->fromString(data);
            }

            DSGetMessages(int iuserID, int isessionID, EnumMsgType::msgType iEnumMsgType, std::string ifrom,
                          std::string ito)
                    : DSGetMessages() {
                userID = iuserID;
                sessionID = isessionID;
                type = iEnumMsgType;
                from = ifrom;
                to = ito;
            }
        };

        class DSMessageItem : public DTStruct {
        public:
            DTInteger<int> id = {"id"};
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
                this->list.push_back(&id);
                this->list.push_back(&userID);
                this->list.push_back(&date);
                this->list.push_back(&msg);
                this->list.push_back(&type);
            }

            DSMessageItem(std::string data) : DSMessageItem() {
                this->fromString(data);
            }

            DSMessageItem(int iid,int iuserID, std::string idate, std::string imsg, EnumMsgType::msgType iEnumMsgType)
                    : DSMessageItem() {
                id = iid;
                userID = iuserID;
                date = idate;
                msg = imsg;
                type = iEnumMsgType;
            }
        };

        class DSMessageList : public DTStruct {
        public:
            DTSet<DSMessageItem> mmlist = {"list"};

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

            DSMessageList(DTSet<DSMessageItem> ilist) : DSMessageList() {
                mmlist = ilist;
            }
        };

        class DSChatUserData : public DTStruct {
        public:
            DTEnum<EnumReachState> reachState = {"reachState"};
            DTEnum<EnumStatus> status = {"status"};
            DTEnum<EnumCustomStatusIcon> customStatusIcon = {"customStatusIcon"};
            DTString customStatusText = {"customStatusText"};
            DTInteger<int> userID = {"userID"};

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
                this->list.push_back(&customStatusText);
                this->list.push_back(&userID);
            }

            DSChatUserData(std::string data) : DSChatUserData() {
                this->fromString(data);
            }

            DSChatUserData(EnumReachState::reachState ireachState, EnumStatus::status istatus,
                           EnumCustomStatusIcon::customStatusIcon icustomStatusIcon,
                           std::string icustomStatusText,int iuserID)
                    : DSChatUserData() {
                reachState = ireachState;
                status = istatus;
                customStatusIcon = icustomStatusIcon;
                customStatusText = icustomStatusText;
                userID = iuserID;
            }

        };

        class DSChangeUserState : public DTStruct {
        public:
            DTInteger<int> userID = {"userID"};
            DTEnum<EnumReachState> reachState = {"reachState"};
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
                this->list.push_back(&userID);
                this-> list.push_back(&reachState);
                this->list.push_back(&status);
                this->list.push_back(&customStatusIcon);
                this->list.push_back(&customStatus);
            }

            DSChangeUserState(std::string data) : DSChangeUserState() {
                this->fromString(data);
            }

            DSChangeUserState(int iuserID,EnumReachState::reachState ireachState,
                              EnumStatus::status istatus,
                              EnumCustomStatusIcon::customStatusIcon icustomStatusIcon,
                              std::string icustomStatus)
                    : DSChangeUserState() {
                userID = iuserID;
                reachState = ireachState;
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
            DTSet<DTInteger<int>> mnlist = {"list"};

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

            DSSession(int iid, std::string icreationDate)
                    : DSSession() {
                id = iid;
                creationDate = icreationDate;
            }

        };

        class DSSeen : public DTStruct {
        public:
            DTInteger<int> userID = {"userID"};
            DTInteger<int> sessionID = {"sessionID"};
            DTInteger<int> seenID = {"seenID"};

            static std::string getStructName() {
                return "DSSeen";
            }

            static int getStructVersion() {
                return 1;
            }

            DSSeen() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&userID);
                this->list.push_back(&sessionID);
                this->list.push_back(&seenID);
            }

            DSSeen(std::string data) : DSSeen() {
                this->fromString(data);
            }

            DSSeen(int iuserID, int isessionID, int iseenID) : DSSeen() {
                userID = iuserID;
                sessionID = isessionID;
                seenID = iseenID;
            }
        };

        class DSChangeUserReachState : public DTStruct {
        public:
            DTInteger<int> userID = {"userID"};
            DTEnum<EnumStatus> status = {"status"};

            static std::string getStructName() {
                return "DSChangeUserReachState";
            }

            static int getStructVersion() {
                return 1;
            }

            DSChangeUserReachState() :
                    DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
                this->list.push_back(&userID);
                this->list.push_back(&status);
            }

            DSChangeUserReachState(std::string data) : DSChangeUserReachState() {
                this->fromString(data);
            }

            DSChangeUserReachState(int iuserID, EnumStatus::status istatus)
                    : DSChangeUserReachState() {
                userID = iuserID;
                status = istatus;
            }

        };
    class DSListUserID : public DTStruct {
    public:
        DTSet<DTInteger<int>> userIDlist = {"list"};

        static std::string getStructName() {
            return "DSListUserID";
        }

        static int getStructVersion() {
            return 1;
        }

        DSListUserID() :
                DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
            this->list.push_back(&userIDlist);
        }

        DSListUserID(std::string data) : DSListUserID() {
            this->fromString(data);
        }

    };


    }
}

#endif //ZDS_DSSTRUCTS_HPP
