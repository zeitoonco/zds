//
// Created by inf on 3/7/16.
//

#ifndef ZDS_GUIUTILITY_HPP
#define ZDS_GUIUTILITY_HPP

#include <string>

namespace zeitoon {
namespace BMS {

class eventInfo {
public:
	static std::string zoneCreated() { return "BMS.zoneCreated"; }

	static std::string zoneRemoved() { return "BMS.zoneRemoved"; }

	static std::string zoneModified() { return "BMS.zoneModified"; }

	static std::string gatewayCreated() { return "BMS.gatewayCreated"; }

	static std::string gatewayRemoved() { return "BMS.gatewayRemoved"; }

	static std::string gatewayModified() { return "BMS.gatewayModified"; }

	static std::string gatewayMovedToGateway() { return "BMS.gatewayMovedToGateway"; }

	static std::string deviceCreated() { return "BMS.deviceCreated"; }

	static std::string deviceRemoved() { return "BMS.deviceRemoved"; }

	static std::string deviceModified() { return "BMS.deviceModified"; }

	static std::string deviceMovedToGateway() { return "BMS.deviceMovedToGateway"; }

	static std::string deviceMovedToZone() { return "BMS.deviceMovedToZone"; }

	static std::string gatewayMovedToZone() { return "BMS.gatewayMovedToZone"; }

	static std::string deviceRemovedFromZone() { return "BMS.deviceRemovedFromZone"; }

	static std::string gatewayRemovedFromZone() { return "BMS.gatewayRemovedFromZone"; }

	static std::string PacketReceived() { return "BMS.PacketReceived"; }

	static std::string PacketSent() { return "BMS.PacketSent"; }
};

class commandInfo {
public:
	static std::string newZone() { return "BMS.newZone"; }

	static std::string removeZone() { return "BMS.removeZone"; }

	static std::string editZone() { return "BMS.editZone"; }

	static std::string newGateway() { return "BMS.newGateway"; }

	static std::string removeGateway() { return "BMS.removeGateway"; }

	static std::string editGateway() { return "BMS.editGateway"; }

	static std::string moveGatewayToGateway() { return "BMS.moveGatewayToGateway"; }

	static std::string newDevice() { return "BMS.newDevice"; }

	static std::string removeDevice() { return "BMS.removeDevice"; }

	static std::string editDevice() { return "BMS.editDevice"; }

	static std::string moveDeviceToGateway() { return "BMS.moveDeviceToGateway"; }

	static std::string moveDeviceToZone() { return "BMS.moveDeviceToZone"; }

	static std::string moveGatewayToZone() { return "BMS.moveGatewayToZone"; }

	static std::string removeDeviceFromZone() { return "BMS.removeDeviceFromZone"; }

	static std::string removeGatewayFromZone() { return "BMS.removeGatewayFromZone"; }
};

}
}
#endif //ZDS_GUIUTILITY_HPP
