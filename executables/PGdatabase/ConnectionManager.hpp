/*
 * ConnectionManager.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: inf
 */

#ifndef CONNECTIONMANAGER_HPP_
#define CONNECTIONMANAGER_HPP_

#include"Connection.hpp"

#include<mutex>

/**
 * Class e ConnectionManager baraye modiriat e userhaaye postgresql mibaashad.
 * bayad be in nokte tavajoh dasht ke jozv e malzoomat baraye estefade az in class
 * ijad e yek database ba naam e "__local" va yek Schema be naam e extension mibashad
 * dar gheyr e in soorat va dar har halat gheyr e matloob ba exception e aan movajeh mishavid.
 * mesal: PGRES_FATAL_ERRORERROR:  relation "__local.extension" does not exist
 */

namespace zeitoon {
namespace pgdatabase {

class PGmediator;

class ConnectionManager {
	PGmediator *pgMediator;
public:
	/**ConnectionManager() constructor e class
	 * @param adminUserName string
	 * @param adminPassWord string
	 * @param adminHost string
	 * @param adminPort string
	 * @param adminDbname string
	 */
	ConnectionManager(std::string adminUserName, std::string adminPassWord, std::string adminHost, int adminPort,
	                  std::string adminDbname,
	                  PGmediator *instancePtr);

	/**~ConnectionManager() Destructor
	 * 1- map e connectionList ra lock mikonad,
	 * 2- ba estefaade az iterator dar map migardad va tamami e connection haaye mojood ra ebtedaa disconnect() va sepat delete mikonad
	 */
	~ConnectionManager();

	/**execute() ba estefaade az connection e motenaaseb baa parametr e naame  extension
	 * az function execude class Connection estefaade mikonad va farmaan e sql e daryaafty
	 * raa ejraa mikonad.
	 * @param extension string
	 * @param sql string
	 * @return meghdaar e satr-haaye tasir gerefte az farmaan e sql
	 */
	int execute(std::string extension, std::string sql);

	/**query() ba estefaade az connection e motenaaseb baa parametr e naame  extension
	 * az function query ee class Connection estefaade mikonad va farmaan e sql e daryaafty
	 * raa ejra mikonad, bad az aan mitavaan az kole method-haaye ghabele dastresi e class e
	 * QueryResult estefaade kard.
	 * @param extension string
	 * @param sql string
	 * @return QueryResult bastegi be noe method entekhaabi ast.
	 */
	DTTablePostgres query(std::string extension, std::string sql);

	/**singleFieldQuery()
	 * ba estefaade az connection e motenaaseb baa parametr e naame  extension
	 * az function getValue class Connection estefaade mikonad va farmaan e sql e daryaafty
	 * raa ejraa mikonad.
	 * @param extension string
	 * @param sql string
	 * @return string meghdaar e field e mored e nazar
	 */

	std::string singleFieldQuery(std::string extension, std::string sql);

	void removeExtension(std::string serviceName);

private:
	/**registerNewExtension() sabt e extension e jadid .
	 *@param extensionName string
	 * 1- Create Role mikonad User = extension Pass= extension
	 * 2- Create schema, Schema name = extension
	 * 3- grant all privileges on schema to extension
	 * 4- sabt e naam e va moshakhasaat e extension dar __local.extension
	 */
	void registerNewExtension(std::string extensionName);

	/** connectionMaker() be map e connectionList meghdaar midahad.
	 * @param extensionName string
	 * naam e extension ra az voroodi daryaaft va ba estefaade az host, port, dbname
	 * ke dar constructor meghdar gereftand, yek object az class e Connection misaazad
	 */
	void connectionMaker(std::string extensionName);

	/**checkIfExtensionRegistered()
	 * @param extensionName string
	 * Check Mikonad ke connection e extension e mored e nazar, dar __local.extension vojood daraad.
	 * @return bool .Agar extension sabt shode baashad, 1 (TRUE) barmigardaanad;
	 *
	 */
	bool checkIfExtensionRegistered(std::string extension);

	/**getNameAndType() in function mas'ool e jam-aavari etela'aat baraye estefaade dar hangaam throw kardan e
	 * exception ast (Hooviaye scopy ke error run-time error midahad.
	 * @return string e etela'aat taa jayi ke mitavaanad.
	 */
	std::string getNameAndType();

	std::string host, dbname;
	/** dar constructor meeghdaar migirad va bad be onvaan e
		 *parametr e moshtarek baraye connection e har extension estefaade mishavafd */

	int port;
	/*dar constructor meghdar migirad va baraye extension ha be ssoorate moshtarek estefaade mishavad*/

	Connection adminConnection;
	/**dar constructor pas az daryaaft e parametr ha in connection baraye modiriat, saakhte mishavad*/

	std::mutex mapGuard;
	/** baraye lock kardan e map e connectionList  dar hangaam e estefaade az map*/

	std::map<std::string, Connection> connectionList; //list connection haye mojood baraaye extension haye mokhtalef. dar void connectionMaker() meghdaar migirad

	void createDatabase(std::string userName, std::string passWord, int port, std::string host);
};

}
}
#endif /* CONNECTIONMANAGER_HPP_ */
