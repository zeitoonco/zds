/*
 * pgmediator.hpp
 *
 *  Created on: Feb 14, 2016
 *      Author: inf
 */

#ifndef PGDATABASE_PGMEDIATOR_HPP_
#define PGDATABASE_PGMEDIATOR_HPP_
#include <mediator/CommunicationHandlerInterface.hpp>
#include "ConnectionManager.hpp"
#include "utility/DTStructs.hpp"

using namespace zeitoon;

namespace zeitoon {
namespace pgdatabase {

class PGmediator: public CommunicationHandlerInterface {
private:
	ConnectionManager conMgr;
	zeitoon::datatypes::DSInstallInfo insInfo;
public:

	/**
	 * constructor e class ke ba daryaft IP va PORT e server e core yek ertebaat ba Servermediator bargharar mikonad va ba daryafte parametr haaye LOGIN e
	 * database e postgresql az ba estefade az connection manager va admin-privileges be database connect mishavad ta har goone darkhast e sql(dartyafte az onCommand) ra ba estefade az naam e node e
	 * darkhaast konandeh, ejra  mikonad va result ra ba estefade az haman moshakhasaat e client e darkhaast konand "Callback" mikonad.
	 * @param serverIP string
	 * @param serverPort int
	 * @param pgAdminUserName string
	 * @param pgAdminPassWord string
	 * @param pgAdminHost string tavajoh dashte bashid ke host e postgresql address e ip e server e database ast, na esm e aan! mesaal: host != "localhost",, host =="127.0.0.1";
	 * @param pgAdminPort int
	 * @param pgAdminDbname string
	 */
	PGmediator(std::string pgAdminUserName, std::string pgAdminPassWord, std::string pgAdminHost, int pgAdminPort,
			std::string pgAdminDbname);

	~PGmediator() {
	}

	/**
	 * onCommand() ba tavajoh be node e voroodi, "query", "execute" ya "fieldValue", va ba estefadde az sql e voroodi, result ra be callback be darkhaast konande
	 * baaz migardanand, agar node e mored e nazar moteghaazi gheyr az "3 node  e zekr shode" baashad, yek peyghaam e motenaaseb ba error be server ersaal mishavad
	 * @param node string , naam e yeki az node haye zekr shode
	 * @param data string , farmaan e sql e darkhaast konande
	 * @param id string , id e makhsoos e in ertebaat.
	 * @param from  string, naam e node e darkhaast konandeh
	 */
	void onCommand(string node, string data, string id, string from);
	/**
	 * onCallback()
	 * NOT IMPLEMENTED
	 */
	void onCallback(string node, string data, string id, string from);
	/**
	 * onEvent()
	 * NOT IMPLEMENTED
	 */
	void onEvent(string node, string data, string from);
	/**
	 * onInstall()
	 * dar hangaam e nasb e service, yek serviceID e unique baraye service ersal mishavad ke dar in method in ID daroon e yek file be soorat e txt zakhir e mikonad.
	 * @param id string
	 */
	void onInstall(string id);
	/**
	 * onEnable()
	 * dar hangaam enable shodan e service, in method list e tamaami e function haaye e available e khod ra be server miferestad
	 */
	void onEnable();
	/**
	 * onDisable()
	 * dar hangaam disable shodan e service, in method list e function haye khod raa paak mikonad
	 */
	void onDisable();
	/**
	 * onUninstall()
	 * dar hangaam e unstall shodan, serviceID ra az configuration file paak mikonad
	 */
	void onUninstall();
	/**
	 * onConnect()
	 * NOT IMPLEMENTED
	 */
	void onConnect();
	/**
	 * onDisconnect()
	 * NOT IMPLEMENTED
	 */
	void onDisconnect();
	/**
	 * getInstallInfo()
	 * tamami e information e m arboot be service, az ghabil e:
	 * name, title, commands, events ra be soorat e yek reshte dar ghaalebe JSON barmigardaanad
	 * @return string instalationInformation
	 */
	string getInstallInfo();
	/**
	 * getInstallID()
	 * serviceID ra be ssoorat e yek reshte barmigardaanad
	 * @return string serviceID
	 */
	string getInstallID();
	/**
	 * getServiceName()
	 * naam e service ra bar migardaanad
	 * @return string service name
	 */
	string getServiceName();
	/**
	 * getServiceVersion()
	 * version e service ra bar migardaanad
	 * @return size_t service version
	 */

	size_t getServiceVersion();
	/**
	 *changeDatatypeVersion()
	 *NOT IMPLEMENTED
	 */
	string changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion, int &newVersion);
	/*
	 * onError()
	 * NOT IMPLEMENTED
	 */
	void onError(string node, string id, string description);
	/*
	 * onWarning()
	 * NOT IMPLEMENTED
	 */
	void onWarning(string level, string node, string id, string description);
	/*
	 * pong()
	 * be darkhaast e ping paasokh midahat
	 * @param id string
	 * @param miliseconds int
	 */
	void pong(string id, int miliseconds);
	void setInstallInfo();

	std::string getNameAndType() const{
		return "PGmediator";
	}

};
}
}

#endif /* PGDATABASE_PGMEDIATOR_HPP_ */
