/*
 * DTTablePostgres.hpp
 *
 *  Created on: Jan 31, 2016
 *      Author: inf
 */

#ifndef DTTABLEPOSTGRES_HPP_
#define DTTABLEPOSTGRES_HPP_

#include<string>
#include<map>
#include<vector>
#include"postgresql/libpq-fe.h"
#include "datatypes/DTTable.hpp"

using namespace zeitoon::datatypes;

namespace zeitoon {
namespace pgdatabase {

class DTTablePostgres : public DTTable {
public:
	/**~QueryResult() Destructor
	 * Result raa khali mikonad
	 */
	~DTTablePostgres();

	/**QueryResult() constructor e class e QueryResult ba daryaaft e 2 parametr
	 * "Command" va "PGconn * conn", yek PGresult misazad.
	 * yek coppy az pointer ba naam e conn dorst karde va sepas void mapmaker() ra seda mizanad;
	 * @param PGconn * connection
	 * @param const char * command
	 */
	DTTablePostgres(PGconn *connection, PGresult *iResult, std::string name = "");

	/**rowCount()
	 * @return int tedaad e satr-haay e mojood dar "PGresult * result" ra barmigardaanad.
	 */
	DTTablePostgres(PGconn *connection, std::string sql, std::string name = "");

	size_t rowCount() const;

	/**columnCount()
	 * @return int tedaad e Sotoon-haay e mojood dar "PGresult * result" ra barmigardaanad.
	 */
	size_t columnCount() const;

	/**columnName()
	 * @param fieldNumber int shomareye sotoon e mored nazar dar result."az 0 shoroo mishavad"
	 * @return string naam sotoon e entekhab shode ra barmigardaanad.
	 */
	std::string columnName(int fieldNumber) const;

	/**columnDataType() ba estefaade az function e columnODataType(), Noe (Data-Type) barmigardaanad.
	 *@param fieldNumber int shomareye column e mored e nazar.
	 * @return PG_Data-Type az noe string
	 */

	ColumnDataType::columnDataType columnDataType(int columnNumber) const;

	/**columnODataType() Oid e  marboot be sotoon e mored e nazar raa bar migardaanad
	 * @param fieldNumber shomare sotoon e mored e nazar
	 * @return Oid e sotoon e mored e nazar
	 */
	Oid columnODataType(int columnNumber) const;

	/**columnDataSize() Haafezzeye eshghaal shode dar database tavasote "field" ra barmigardaanad.
	 * agar data-type e field e mored e nazar "varchar" bashad, "-1" barmigardaanad
	 * @param fieldNumber int
	 * @return int size
	 */
	size_t columnDataSize(int columnNumber) const;

	/**fieldValue() meghdaar e field e morede nazar az *result ra be soorat e string barmigardaanad.
	 * @param tupleNumber int shomaareye satr (az sefr)
	 * @param fieldNumber int shomareye sotoon mored e nazar
	 * @return string e meghdaar e field
	 */
	std::string fieldValue(int tupleNumber, int columnNumber) const;

	/*
	 * Overloaded function of fieldValue
	 * returns field's value by receiving tupple's number and column name.
	 */
	std::string fieldValue(int tuppleNumber, std::string columnName) const;

	/**fieldIsNull()  field e mored e nazar raa baraye NULL boodan check mikonad.
	 * @param tupleNumber int shomaareye satr (az sefr)
	 * @param fieldNumber int shomareye sotoon mored e nazar
	 * @return agar null baashad, true bar migardaanad
	 */
	bool fieldIsNull(int tupleNumber, int columnNumber) const;

	/** fieldSize() tool e field raa ba meghiyaas e Byte barmigardaanad
	 * @param tupleNumber int shomaareye satr (az sefr)
	 * @param fieldNumber int shomareye sotoon mored e nazar
	 * @return int tool e daadeye field e mored e nazar
	 */
	int fieldSize(int tupleNumber, int columnNumber) const;

	/** toString ba estefaade az SerializationType e voroodi.  PGresult * result raa be std::string result tabdil mikonad
	 * @param type SerializationType
	 * @return  string resultData
	 */
	std::string toString(int indent = -1, std::string indentContent = defaultIndentContent) const;

	/**fromString()
	 * not implemented
	 *
	 */

	void fromString(std::string data);

	/**operator= (string) yek farmaan e sql ra daryaaft mikonad, aan ra ejra karde va "result" e aan ra dar result gharaaar midahad
	 * @param  sql string
	 * @return DTBase&
	 */

	DTBase &operator=(std::string sql);

	/** operator= (DTBase&)
	 * @param dtvar DTBase&
	 * ebtedaa meghdaar e voroodi ra be DTString cast mikonad, agar natijeh sahih bud, az string haasel yek query misaazad va natijeye aan
	 * raa dar PGresult * result zakhire mikonad
	 * dar gheyr az in soorat
	 * meghdaar e voroodi ra be DTTablePostgres cast mikonad, agar shod, natijeye cast ra be result mirizad
	 * @return DTBase&
	 */
	DTBase &operator=(DTBase &dtvar);

	/** operator==
	 * @param  DTBase& dtvar
	 * ebtedaa dtvar raa be DTString cast mikonad, agar shod, baa estefade az aan string yek query migirad va result e haasel ra ba
	 * this->result moghaayese mikonad
	 * @return DTBase&
	 */
	bool operator==(DTBase &dtvar);

	/**operator !=
	 * @param DTBase dtvar
	 * natijeye hasel az TBase& operator =(DTBase& dtvar) ra NOT mikonad va barmigardaanad
	 * @return bool
	 */
	bool operator!=(DTBase &dtvar);

private:


	/**getNameAndType() in function mas'ool e jam-aavari etela'aat baraye estefaade dar hangaam throw kardan e
	 * exception ast (Hooviaye scopy ke error run-time error midahad.
	 * @return string e etela'aat taa jayi ke mitavaanad.
	 */

	std::string getNameAndType() const;

	static std::map<Oid, int> mapPgCustomType;
	/**in map baraye gharaar daadan e kole data type-haaye motenaaseb ba mape HardcodedTypes mibasash */

	static std::map<int, std::vector<std::string> > mapHardCodedTypes; /** Map e Hard Code shode- baraye negahdaari e anvaye data type e
	 Postgresql va naam e delkhaah baraye anvaae anhaa, i.e: text = char , _bpchar, varchar, name, text, ect...*/

	/** mapMaker() in function 2 map baraye tabdil e OID be data-type mortabet baa aan misazaad.
	 */
	void mapMaker();

	std::string toString(PGresult *res);

	const PGresult *result;
	/**dar constructor meghdaar migirad*/
	PGconn *conn; /**dar constructor az rooye *connection copy mishavad*/
};
}
}

#endif /* DTTABLEPOSTGRES_HPP_ */
