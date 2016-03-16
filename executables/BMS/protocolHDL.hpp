//
// Created by inf on 3/15/16.
//

#ifndef ZDS_PROTOCOLHDL_HPP
#define ZDS_PROTOCOLHDL_HPP
#define serverDevicetype 0xFE
#define serverDeviceID 0xFE
#define serverSubnetID 0xFE

#include <iostream>
#include <sstream>
#include "datatypes/dtsingletypes.hpp"
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define __BroadcastID 255 ///< well, Broadcast ID.!

/**
 * Structure of a Key config rule
 */
struct ConfigEntryKeyRuleDataStructure {
	uint8_t size; ///< size of config, including #size and #type vars
	uint8_t type; ///< type of rule. setchannel=0,disableKey=1,packet=2. see #KeyRuleType
	/***
	 * we use union to use only one of three structure.
	 */
	union Rule {
		struct ConfigEntryKeyRuleSetChannel {
			uint8_t targetNet;
			uint8_t targetID;
			uint8_t channelNo;
			uint8_t channelLevel;
			uint16_t runningTime;
		} setChannel;
		struct ConfigEntryKeyRuleDisableInput {
			uint8_t targetNet;
			uint8_t targetID;
			uint8_t channelNo;
			uint8_t disable;
		} disableKey;
		struct ConfigEntryKeyRulePacket {
			uint8_t data[];
		} packet;
	} rule;
	enum KeyRuleType {
		KRTSetChannel = 0, KRTDisableInput = 1, KRTPacket = 2
	};
};

struct ConfigEntryKeyDataStructure {
	uint8_t ruleCount; ///< number of ruels
	ConfigEntryKeyRuleDataStructure firstRule; ///< a place holder for first rule. next rules can accessed using address of this variable plus its size.
	/**
	 * Estimate size of this structure using counts of each rule (except pS). used for allocating memory.
	 * @param schN number of SetChannel rules
	 * @param dkN number of DisableKey rules
	 * @param pS total sum of length of RawPackets
	 * @return Calculated size
	 */
	static int estimateSize(int schN, int dkN, int pS) {
		return 1
		       + (sizeof(ConfigEntryKeyRuleDataStructure::Rule::ConfigEntryKeyRuleSetChannel)
		          + 2) * schN
		       + (sizeof(ConfigEntryKeyRuleDataStructure::Rule::ConfigEntryKeyRuleDisableInput)
		          + 2) * dkN + pS;
	}

	/**
	 * get current size of this structure.
	 * @return
	 */
	int getSize() {
		int size = 1; //1=size of ruleCount
		ConfigEntryKeyRuleDataStructure *p = &firstRule;
		for (int i = 0; i < ruleCount; i++) {
			size += p->size;
			p += p->size;
		}
		return size;
	}

	/**
	 * gets a pointer to n-th rule
	 * @param index rule number
	 * @return pointer to rule
	 */
	ConfigEntryKeyRuleDataStructure *getRule(int index) {
		uint8_t *p = (uint8_t *) &firstRule;
		for (int i = 0; i < index; i++)
			p += ((ConfigEntryKeyRuleDataStructure *) p)->size;
		return (ConfigEntryKeyRuleDataStructure *) p;
	}
};


/**
 * An array of specific bytes. this values are used to calculate Checksum.
 * @sa GenerateCrc
 */
extern const uint16_t CrcTable[256];

/**
 * Success value in communications. Used in packets to indicate that some task has beed successful or failed.
 */
extern const uint8_t success;
/**
 * Fail value in communications. Used in packets to indicate that some task has beed successful or failed.
 */
extern const uint8_t fail;

/**
 * Main Packet container and parser class. HDL Style!
 */
struct Packet {
	static const int HeaderSize = 2; ///< packet header size ( AA AA ).
	static const int CRCSize = 2; ///< crc size in packet
	static const int PacketWithHeaderSize = 13; ///< size of a packet with no content.
	static const int PacketWithoutHeaderSize = 11; ///< size of a packet with no content minus its header size. this is the value in third byte that indicate packet size
	static const uint8_t HeaderByte = 0xAA; ///< Value used in header bytes
	uint8_t DtHead[11]; ///< first part of packet data from beginning to content
	uint8_t *DtContent; ///< content bytes
	uint8_t DtCRC[2]; ///< Checksum
	int length; ///< Total packet length

	/**
	 * Caption of each byte in packet
	 */
	enum ByteName {
		CNHeader = 0,            //!< CNHeader Header bytes. its constant [2 byte]
		CNLength = 2,            //!< CNLength Size of packet from here to end [1 byte]
		CNOriginalSubnetID = 3,  //!< CNOriginalSubnetID Sender Subnet [1 byte]
		CNOriginalDeviceID = 4,  //!< CNOriginalDeviceID Sender ID [1 byte]
		CNOriginalDeviceType = 5,  //!< CNOriginalDeviceType Sender Device type [2 byte]
		CNOperationCode = 7, //!< CNOperationCode Operation code. check reference for codes. [2 byte]
		CNTragetSubnetID = 9,    //!< CNTragetSubnetID Receiver Subnet [1 byte]
		CNTragetDeviceID = 10,   //!< CNTragetDeviceID Receiver ID [1 byte]
		CNContent = 11           //!< CNContent Content bytes start here [n byte]
	};

	/**
	 * Constructor. Initialize an empty packet.
	 */
	Packet() {
		DtContent = NULL;
		length = 13;
		for (int i = 0; i < PacketWithoutHeaderSize; i++)
			DtHead[i] = 0;
		FixHeader();
		FixLength();
	}

	/**
	 * Constructor. Initialize packet with given data.
	 * @param arraybytes data array that contains packet
	 * @param len Length of arraybytes parameter
	 */
	Packet(uint8_t *arraybytes, int len) {
		DtContent = NULL;
		Load(arraybytes, len);
	}

	/**
	 * Initialize packet with given data.
	 * @param arraybytes data array that contains packet
	 * @param len Length of arraybytes parameter
	 */
	void Load(uint8_t *arraybytes, int len) {
		if (DtContent != NULL) delete[] DtContent;
		DtContent = new uint8_t[len - 13];
		memcpy(DtHead, arraybytes, 11);
		memcpy(DtContent, arraybytes + 11, len - 13);
		memcpy(DtCRC, arraybytes + (len - 2), 2);
		length = len;
	}

	/**
	 * Deconstructor. frees allocated memory.
	 */
	~Packet() {
		if (DtContent != NULL) delete[] DtContent;
	}

	/**
	 * get an array containing header bytes.
	 * @return an array with size of #HeaderSize ,and its values must be #HeaderByte
	 */
	uint8_t *GetHeader() {
		return DtHead;
	}

	/**
	 * Fill header bytes with correct values.
	 */
	void FixHeader() {
		DtHead[0] = HeaderByte;
		DtHead[1] = HeaderByte;
	}

	/**
	 * Get Sender device type. check reference for a list of device types.
	 * @return Device type value.
	 */
	uint16_t GetOriginalDeviceType() {
		return (uint16_t) (
				DtHead[CNOriginalDeviceType] * 256 + DtHead[CNOriginalDeviceType + 1]);
	}

	/**
	 * Set Sender device type. check reference for a list of device types.
	 * @param DeviceType Device type value.
	 */
	void SetOriginalDeviceType(uint16_t DeviceType) {
		DtHead[CNOriginalDeviceType] = (uint8_t) (DeviceType >> 8);
		DtHead[CNOriginalDeviceType + 1] = (uint8_t) (DeviceType & 0x0ff);
		FixCRC();
	}

	/**
	 * Get Sender Subnet ID
	 * @return Subnet value
	 */
	uint8_t GetOriginalSubnetID() {
		return DtHead[CNOriginalSubnetID];
	}

	/**
	 * Set Sender Subnet ID
	 * @param value Subnet value
	 */
	void SetOriginalSubnetID(uint8_t value) {
		DtHead[CNOriginalSubnetID] = value;
		FixCRC();
	}

	/**
	 * Get Sender device ID
	 * @return Device ID value
	 */
	uint8_t GetOriginalDeviceID() {
		return DtHead[CNOriginalDeviceID];
	}

	/**
	 * Set Sender device ID
	 * @param value Device ID value
	 */
	void SetOriginalDeviceID(uint8_t value) {
		DtHead[CNOriginalDeviceID] = value;
		FixCRC();
	}

	/**
	 * Get Receiver Subnet ID
	 * @return Subnet value
	 */
	uint8_t GetTargetSubnetID() {
		return DtHead[CNTragetSubnetID];
	}

	/**
	 * Set Receiver Subnet ID
	 * @param value Subnet value
	 */
	void SetTargetSubnetID(uint8_t value) {
		DtHead[CNTragetSubnetID] = value;
		FixCRC();
	}

	/**
	 * Get Receiver device ID
	 * @return Device ID value
	 */
	uint8_t GetTargetDeviceID() {
		return DtHead[CNTragetDeviceID];
	}

	/**
	 * Set Receiver device ID
	 * @param value Device ID value
	 */
	void SetTargetDeviceID(uint8_t value) {
		DtHead[CNTragetDeviceID] = value;
		FixCRC();
	}

	/**
	 * Get Operation code. check reference for a list of Operation codes.
	 * @return Operation code value
	 */
	uint16_t GetOperationCode() {
		return (uint16_t) (DtHead[CNOperationCode] * 256 + DtHead[CNOperationCode + 1]);
	}

	/**
	 * Set Operation code. check reference for a list of Operation codes.
	 * @param value Operation code value
	 */
	void SetOperationCode(uint16_t value) {
		DtHead[CNOperationCode] = (uint8_t) (value >> 8);
		DtHead[CNOperationCode + 1] = (uint8_t) (value & 0x0ff);
		FixCRC();
	}

	/**
	 * Get Packet Content bytes.
	 * @sa GetContentLength, DtContent
	 * @return Returns a pointer to an array containing Content bytes. array size can be determined using GetContentLength()
	 */
	uint8_t *GetContent() {
		return DtContent;
	}

	/**
	 * Set Packet Content bytes. This function automatically calls FixLength() to fix length and CRC values.
	 * @param data a pointer to an array containing Content bytes
	 * @param len Length of data array.
	 */
	void SetContent(uint8_t *data, int len) {
		length -= GetContentLength(); // reduce previous content size from total size, then add new size to it.
		if (DtContent != NULL) delete[] DtContent;
		DtContent = new uint8_t[len];
		memcpy(DtContent, data, len);
		length += len;
		FixLength();
	}

	/**
	 * Get length of content data.
	 * @return Length of Content data
	 * @sa GetContent, DtContent
	 */
	int GetContentLength() {
		return DtHead[CNLength] - PacketWithoutHeaderSize;
	}

	/**
	 * Get length of packet without header.
	 * @return Length of packet without header.
	 */
	int GetLength() {
		return DtHead[CNLength];
	}

	/**
	 * Fix value of Length byte (#CNLength) in Packet. This sould be called when content array`s size changes.
	 * @return New value of Length.
	 * @sa SetContent
	 */
	int FixLength() {
		DtHead[CNLength] = (uint8_t) (length - HeaderSize);
		FixCRC();
		return DtHead[CNLength];
	}

	/**
	 * Generates Checksum values.
	 *
	 * Generates Checksum values using Packet data. Resulting values are returned using two received parameters.
	 * this function use #pgm_read_word_near array to calculate checksum. This is HDL style!.
	 *
	 * @param byte1	First byte of CRC.
	 * @param byte2 Second byte of CRC.
	 */
	void GenerateCrc(uint8_t &byte1, uint8_t &byte2) {
		uint16_t crc = 0;
		uint8_t dat;
		for (int i = 2; i < PacketWithoutHeaderSize; i++) {
			dat = (uint8_t) (crc >> 8);
			crc <<= 8;
			crc ^= CrcTable[(dat ^ DtHead[i])];
			//crc ^= CrcTable[(dat ^ DtContent[i])]; //pgm_read_word_near(CrcTable + (dat ^ DtHead[i]));
		}
		for (int i = 0; i < GetContentLength(); i++) {
			dat = (uint8_t) (crc >> 8);
			crc <<= 8;
			crc ^= CrcTable[(dat ^ DtContent[i])];
			//crc ^= CrcTable[(dat ^ DtContent[i])]; //pgm_read_word_near(CrcTable + (dat ^ DtHead[i]));
		}
		byte1 = (uint8_t) (crc >> 8);
		byte2 = (uint8_t) (crc & 0x00ff);
	}

	std::string toString() {
		FixCRC();
		auto d = Generate();
		std::stringstream temp;
		temp << std::hex << std::uppercase;
		for (int i = 0; i < length; i++) {
			if (d[i] < 16) { temp << 0; }
			temp << (int) (d[i]) << " ";
		}
		temp << std::dec << std::nouppercase;

		return temp.str();
	}

	/**
	 * Get checksum values.
	 * @return a pointer to an array of Checksum bytes. Array size is determined using #CRCSize .
	 */
	uint8_t *GetCRC() {
		return DtCRC;
	}

	/**
	 * Fix Checksum values in Packet using packet data.
	 * @return a pointer to an array of Checksum bytes. Array size is determined using #CRCSize .
	 */
	uint8_t *FixCRC() {
		uint8_t b1, b2;
		GenerateCrc(b1, b2);
		DtCRC[0] = b1;
		DtCRC[1] = b2;
		return DtCRC;
	}

	/**
	 * Check that if Packet`s structure, Header, Length values and Checksum is valid or not.
	 * @return Resulting boolean value.
	 */
	int IsValid() {
		return IsValidHeader() && IsValidStructure() && IsValidLength() && IsValidCrc();
	}

	/**
	 * Check that if Packet structure is valid.
	 * @return Resulting boolean value.
	 */
	int IsValidStructure() {
		return length >= PacketWithHeaderSize;
	}

	/**
	 * Check that if Packet Header is valid.
	 * @return Resulting boolean value.
	 */
	int IsValidHeader() {
		return HeaderByte == DtHead[0] && HeaderByte == DtHead[1];
	}

	/**
	 * Check that if Packet Length is valid.
	 * @return Resulting boolean value.
	 */
	int IsValidLength() {
		if (length < PacketWithHeaderSize) return false;
		if (length == DtHead[CNLength] + HeaderSize) return true;
		return false;
	}

	/**
	 * Check that if Packet Checksum is valid.
	 * @return Resulting boolean value.
	 */
	int IsValidCrc() {
		uint8_t b1, b2;
		GenerateCrc(b1, b2);
		return DtCRC[0] == b1 && DtCRC[1] == b2;
	}

	/**
	 * Generate and array containing Packet bytes.
	 * @return Array of packet bytes. Size of array can be determined using #length
	 */
	uint8_t *Generate() {
		uint8_t *res = new uint8_t[length];
		memcpy(res, DtHead, PacketWithoutHeaderSize);
		memcpy(res + PacketWithoutHeaderSize, DtContent, GetContentLength());
		memcpy(res + (length - 2), DtCRC, 2);
		return res;
	}

};

/**
 * Generate a Packet using given data.
 * Original Subnet, Device ID and Device Type values will be set using hardware`s defined values.
 * @param oc Operation code
 * @param si Target Subnet ID
 * @param di Target Device ID
 * @param data Array containing content data
 * @param dataLen Content array length
 * @return Generated packet.
 */
Packet *BuildPacket(uint16_t oc, uint8_t si, uint8_t di, uint8_t *data, int dataLen);

/**
 * Generate a Packet using given data.
 * Original Subnet, Device ID and Device Type values will be set using hardware`s defined values.
 * This version generate a Broadcast packet.
 * @param oc Operation code
 * @param data Array containing content data
 * @param dataLen Content array length
 * @return Generated packet.
 */
Packet *BuildPacket(uint16_t oc, uint8_t *data, int dataLen);

/**
 * a Packet content type that contains Device remarks.
 * Used is Ping and Read Device Remarks.
 */
struct PCReadDeviceRemarkResponse {
	uint8_t remarks[20]; ///< i have no idea what is this twenty bytes!!
};

/**
 * a Packet content type that contains a MAC address.
 */
struct PCMACAddress {
	uint8_t MAC[8]; ///< Eight bytes of MAC Address.
};

/**
 * a Packet content type that contains a Response.
 * Response is either Fail or Success, Presented using #success and #fail values in a single byte.
 */
struct PCSuccesFailResponse {
	uint8_t result; ///< Result of an operation. Containing #success or #fail value.
	/**
	 * Set result to #success
	 */
	void Success() {
		result = success;
	}

	/**
	 * Set result to #fail
	 */
	void Fail() {
		result = fail;
	}

	/**
	 * Checks that if result was Successful or not.
	 * In other words converts #result values to an boolean style (0&1).
	 * @return result in boolean.
	 */
	int WasSuccessful() {
		return (result == success);
	}

	/**
	 * Generate a Packet with it`s Content Set to a PCSuccesFailResponse with #success value.
	 * @param oc Operation Code
	 * @param si Target Subnet ID
	 * @param di Target Device ID
	 * @return Generated Packet.
	 */
	static Packet *buildSuccessPacket(uint16_t oc, uint8_t si, uint8_t di) {
		PCSuccesFailResponse c;
		c.Success();
		return BuildPacket(oc, si, di, (uint8_t *) &c, sizeof(c));
	}

	/**
	 * Generate a Packet with it`s Content Set to a PCSuccesFailResponse with #fail value.
	 * @param oc Operation Code
	 * @param si Target Subnet ID
	 * @param di Target Device ID
	 * @return Generated Packet.
	 */
	static Packet *buildFailPacket(uint16_t oc, uint8_t si, uint8_t di) {
		PCSuccesFailResponse c;
		c.Fail();
		return BuildPacket(oc, si, di, (uint8_t *) &c, sizeof(c));
	}
};

/**
 * a Packet content type that contains data to change Device ID and Subnet ID
 */
struct PCSetDeviceID {
	void setMac(zeitoon::datatypes::MACAddress MacAddr) {
		(*((uint64_t *) MAC)) = MacAddr.getValue();
	}

	uint8_t MAC[8];
	///< Eight bytes of Target MAC Address.
	uint8_t SubnetID;
	///< New Subnet ID
	uint8_t DeviceID;///< New Device ID
};

/**
 * a Packet content type that contains a single byte of data.
 * This is a general Packet that can be used when there's just one byte of data to transmit.
 */
struct PCSingleByte {
	uint8_t data;
};

/**
 * a Packet content type that contains device info
 */
struct PCInfo {
	uint16_t deviceType;
	///< Device Type
	uint16_t HWVersion;
	///< Hardware Version
	uint16_t SWVersion;
	///< Software Version
	uint8_t inputCount;
	///< Number of input channels
	uint8_t outputCount;
	///< Number of output channels
	uint8_t locked;///< lock state
};

/**
 * a Packet content type that contains data for controlling a single channel.
 */
struct PCSingleChannelControl { //0031
	uint8_t _ChannelNo;
	///< Channel number
	uint8_t _ChannelLevel;
	///< Desired channel level. max 100
	uint16_t _RunningTime;
	///< Dimming duration. it its Dimmable!
	uint8_t _ButtonNo;///<Number of button that has sent the command
	/**
	 * Allocate memory for this structure
	 * @param len returns length of allocated memory
	 * @return returns allocated memory
	 */
	static uint8_t *GetMemory(int &len) {
		len = 5;
		return new uint8_t[len];
	}
};

/**
 * a Packet content type that contains Response of SingleChannelControl
 */
struct PCSingleChannelControlResponse { //0032
	uint8_t _ChannelNo;
	///<Channel Number
	uint8_t _Success;
	///<Success state
	uint8_t _ChannelLevel;
	///<current level
	uint8_t _TotalChannelNo;
	///<Total number of channels
	uint8_t _ChannelStatus[];///<State of all channels as bits

	/**
	 * Set all #_ChannelStatus bits to specified value
	 * @param value Desired boolean value
	 */
	void SetAllChannels(int value) {
		int bytes = std::max(2, (int) ceil(_TotalChannelNo / 8.0));
		for (int i = 0; i < bytes && ((i * 8) < _TotalChannelNo); i++)
			//for (int bi = 0; bi < 8 && ((i * 8 + bi) < _TotalChannelNo); bi++)
			_ChannelStatus[i] = value ? 255 : 0; //((byte)pow(2, bi) & _ChannelStatus[i]) > 0;* 8 + bi
	}

	/**
	 * Set a channel state bit in #_ChannelStatus
	 * @param channelIndex Channel number
	 * @param value Channel state (boolean)
	 */
	void SetChannel(int channelIndex, int value) {
		int iByte = (int) floor(channelIndex / 8);
		int iBit = channelIndex % 8;
		int p2 = 1;
		for (int ii = 0; ii < iBit; p2 = p2 * 2, ii++);
		if (value)
			_ChannelStatus[iByte] |= p2;
		else if (_ChannelStatus[iByte] & p2) {
			_ChannelStatus[iByte] &= ~p2;
		}
	}

	/**
	 * Get a channel state
	 * @param channelIndex Channel number
	 * @return Boolean state of channel
	 */
	int GetChannel(int channelIndex) {
		int iByte = (int) floor(channelIndex / 8);
		int iBit = channelIndex % 8;
		return (_ChannelStatus[iByte] & (uint8_t) pow(2, iBit)) > 0;
	}

	/**
	 * Allocate memory for this structure
	 * @param channelNo Number of Channels
	 * @param len returns Length of allocated memory
	 * @return Returns Allocated memory
	 */
	static uint8_t *GetMemory(int channelNo, int &len) {
		len = 4 + std::max(2, (int) ceil(channelNo / 8.0));
		return new uint8_t[len];
	}

	/**
	 * Checks that if setting channel was successful
	 * @return Boolean state of operation
	 */
	int IsSuccessful() {
		return _Success == success;
	}
};

/**
 * a Packet content type that contains Channels status
 */
struct PCChannelStatusResponse { //0034<-33
	uint8_t ChannelNumber;
	///< number of channels
	uint8_t Channels[];///< channels states as bytes.
	/**
	 * Allocate memory for this class
	 * @param channelNo Number of channels
	 * @param len returns length of allocated memory
	 * @return returns allocated memory
	 */
	static uint8_t *GetMemory(int channelNo, int &len) {
		len = 1 + channelNo;
		return new uint8_t[len];
	}
};

//AC
/**
 * AC States
 */
enum ACMode {
	ACModeCooling = 0, ACModeHeating = 1, ACModeFan = 2, ACModeAuto = 3, ACModeDry = 4
};
/**
 * AC Fan speeds
 */
enum ACFanSpeed {
	ACFanAuto = 0, ACFanHigh = 1, ACFanMedium = 2, ACFanLow = 3
};

/**
 * a Packet content type that contains AC data. Used for Controlling and logging.
 */
struct PCACData { //193A->193B
	uint8_t No; ///< idk!
	uint8_t TempType; ///< Temperature display type. Celsius=0, Fahrenheit=1
	uint8_t CurrentTemp; ///< Current Temperature
	uint8_t CoolingTempPoint; ///< Temperature for cooling mode
	uint8_t HeatingTempPoint; ///< Temperature for heating mode
	uint8_t AutoTempPoint; ///< Temperature for auto mode
	uint8_t DryTempPoint; ///< Temperature for dry mode
	uint8_t ModeXFan; ///< a state byte containing #ACMode in lower 4 bit and #ACFanSpeed in higher 4 bit
	uint8_t ACState; ///< Power state(On,Off)
	uint8_t SetupMode; ///< idk!
	uint8_t SetupSpeed; ///< idk!
	uint8_t CurrentMode; ///< idk!
	uint8_t Sweep; ///< Sweep state

	ACMode GetMode() {
		return (ACMode) (ModeXFan & 0x0F);
	}

	ACMode GetSetupMode() {
		return (ACMode) (SetupMode);
	}

	ACFanSpeed GetFanSpeed() {
		return (ACFanSpeed) ((ModeXFan & 0xF0) >> 4);
	}

	ACFanSpeed GetSetupSpeed() {
		return (ACFanSpeed) (SetupSpeed);
	}

	void SetTypeCelsius() {
		TempType = 0;
	}

	void SetTypeFahrenheit() {
		TempType = 1;
	}

	void SetMode(ACMode mode) {
		ModeXFan &= 0xF0;
		ModeXFan |= (uint8_t) mode;
	}

	void SetSetupMode(ACMode mode) {
		SetupMode = (uint8_t) mode;
	}

	void SetFanSpeed(ACFanSpeed spd) {
		ModeXFan &= 0x0F;
		ModeXFan |= ((uint8_t) spd) << 4;
	}

	void SetSetupSpeed(ACFanSpeed spd) {
		ModeXFan = (uint8_t) spd;
	}

	void SetSweep(int Enable, int sweep) {
		Sweep = (Enable ? 1 : 0) + (sweep ? 16 : 0);
	}
};

//Mix
/**
 * a Packet content type that contains Mix module configuration parameters
 */
struct PCMixConfig {
	uint8_t locked; ///< Lock state
	uint8_t curtainMode; ///< Curtain mode enable
};
/**
 * a Packet content type that contains a key config
 */
struct PCKeyConfig {
	uint8_t keyID; ///< Key number
	ConfigEntryKeyDataStructure data; ///< key config data
};
/**
 * a Packet content type that contains a key disable command
 */
struct PCKeyDisable { //0xABCA
	uint8_t keyID; ///< Key number
	uint8_t Disable; ///< Key state (boolean)
};
/**
 * a Packet content type that contains response to key disable
 */
struct PCKeyDisableResponse { //0xABCB
	uint8_t keyNumber; ///< number of keys
	uint8_t keyDisableState[]; ///< key states as and array of booleans
};


#endif //ZDS_PROTOCOLHDL_HPP
