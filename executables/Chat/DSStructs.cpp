//
// Created by naeyrin on 5/4/16.
//
#include "DSStructs.hpp"

namespace zeitoon {
    namespace chat {
        const std::string EnumMsgType::typeString[] = {"user", "userLog", "system"};
        const std::string EnumReachState::typeString[] = {"normal", "noSearch", "noChart", "oneWay", "noChartSearch"};
        const std::string EnumStatus::typeString [] ={"normal", "custom", "invisible", "busy", "away"};
        const std::string EnumCustomStatusIcon::typeString [] = {"online", "offline", "busy", "away"};
    }
    }