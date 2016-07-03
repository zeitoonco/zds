//
// Created by inf on 7/3/16.
//

#ifndef ZDS_CORECONFIG_HPP
#define ZDS_CORECONFIG_HPP

#include "utility/ConfigManager.hpp"

namespace zeitoon {
    namespace _core {
        class CoreConfig : public ConfigManager {
        public:
            DTString listenPort = {"serverPort"};

            CoreConfig() : ConfigManager("Core", "CoreConfig") {
                this->add(&listenPort);
            }

            ~CoreConfig() {
                this->save();
            }
        };


    }
}
extern zeitoon::_core::CoreConfig coreConfig;

#endif //ZDS_CORECONFIG_HPP
