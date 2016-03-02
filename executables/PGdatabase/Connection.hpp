/*
 * Connection.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: inf
 */

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include"DTTablePostgres.hpp"
namespace zeitoon{
namespace pgdatabase{
class Connection {
public:
	Connection();
	/**Connection() Constructor e kelass ke ba gereftan e parametr-haa yek connection admin misaazad.
	 * @param USER string Username-admin
	 * @param PASS string Password-admin
	 * @param HOST string Host-Address
	 * @param PORT int Port
	 * @param DBNAME string Naame Database
	 */
	Connection(std::string USER, std::string PASS, std::string HOST, int PORT, std::string DBNAME);
	/**~Connection() Destructor
	 * function e disconnect() raa ejra mikonad
	 */
	~Connection();
	/**connect() ba estefaadeh az parametr haye mojood dar ghesmat e private e class connect mikonad
	 */
	void connect();
	/**connect()
	 * baa gereftan e parametr ha yek connection misazad. ebteda check mikonad ke agar connection
	 * az ghabll vojood darad, Throw->"already connected"
	 * @param USER string Username-admin
	 * @param PASS string Password-admin
	 * @param HOST string Host-Address
	 * @param PORT int Port
	 * @param DBNAME string Naame Database
	 */

	void connect(std::string USER, std::string PASS, std::string HOST, int PORT, std::string DBNAME);
	/**disconnect()
	 * connection ra rahaa mikonad,
	 * ebteda check mikonad , agar pointer e connection nullptr bood, faghat az function
	 * disconnect kharej mishavad
	 */
	void disconnect();
	/**getvalue() tanha meghdaare yek field raa bar migardaanad.
	 * baraye estefade mesl e "SELECT MAX (x) FROM y" estefade mishavad.
	 * @param command String e farmany ke tanha yek field javab darad
	 */
	std::string getValue(std::string command);
	/**query() string query ra daryaaft mikonad va be hamraah e Connection e mojood,
	 * har 2 parametr ra be constructor e class QueryResult miferestad. va dar nahaayat
	 * yek object az noe e QueryResult bar migardaanad.
	 * @param command string
	 * @return QueryResult yek object az noe natijeye query
	 */
	DTTablePostgres query(std::string command);
	/**isConnected()
	 * check mikonad ke "PGconn * conn" agar null baashad, FALSE barmigardaanad
	 * dar gheyr e az in soorat, agar ( * conn == CONNECTION_OK ) bashad TRUE bar migardaanad
	 */
	bool isConnected();
	/**execute() string e farmaan ra daryaaft mikonad (SELECT, CREATE TABLE AS, INSERT, UPDATE,
	 * DELETE, MOVE, FETCH, or COPY) va tedaad e satr-haaye tasir gerefteh raa bar migardaanad
	 * @param command string
	 * @return int tedaad e satr-haaye tasir gerefte
	 */
	int execute(std::string command);

private:
	/**getNameAndType() in function mas'ool e jam-aavari etela'aat baraye estefaade dar hangaam throw kardan e
		 * exception ast (Hooviaye scopy ke error run-time error midahad.
		 * agar connection mojood basshaaad maghaadir   HOST  va PORT va USER raa barmigardaanad.
		 * dar gheir az in soorat ??? chi bargardaanad?
		 * @return string e etela'aat taa jayi ke mitavaanad.
		 */
		std::string getNameAndType();
	PGconn *conn; /**Pointer e connection*/
	std::string USER = "";/**moteghayyer baraye user name  */
	std::string PASS = "";/**moteghayyer baraye password  */
	std::string HOST = "";/**moteghayyer baraye host address  */
	int PORT = 0;			/**moteghayyer baraye shomaareye port  */
	std::string DBNAME = "";/**moteghayyer baraye database name  */
};
}
}


#endif /* CONNECTION_HPP_ */
