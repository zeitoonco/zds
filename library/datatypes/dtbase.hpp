#ifndef DTBASE_H
#define DTBASE_H

#include <string>

#include <utility/exceptionex.hpp>
#include "utility/utility.hpp"

using namespace zeitoon::utility;
using std::string;

namespace zeitoon {
namespace datatypes {

/** class DTBase
 *
 * class DTBase yek kelass parent ast ke baghie kelass ha az in kelass ers mibarand. do kelass DTSingleTypes va
 * DTMultitypes be soorat e mostaghim az in kelass ers mibarand.
 *
 */
class DTBase {
protected:

	/**isvalidname baraye check kardan valid boodn naami ke dar _name gharar migirad
	 *
	 * mavared mojaz : horoof e koochak, horoof ebozorg, underline, adaad magar dar ebteda bashand.
	 *
	 * @param name naami k bayad check shavad.
	 *
	 * @return agar name valid bood true va dar gheir in soorat false bargardande mishavad.
	 *
	 */
	bool isValidName(string name) {
		for (uint i = 0; i < name.length(); i++) {
			char c = name[i];
			if (!((c >= 65 && c <= 90) || //Lower case alphabet
			      (c >= 97 && c <= 122) || //Upper case alphabet
			      (c == 95) || // underline
			      (i > 0 && (c >= 48 && c < 57)))) //numbers, except in first place
				return false;
		}
		return true;
	}

public:

	/**constructor DTBase k name e moteghaeir ra migirad.
	 *
	 * stringi k daryaft mikond ra dar moteghaeir _name class gharar midahad.(dar soorati k valid bashad)
	 *
	 * @param name stringi ke bayad dar _name zakhire shavad. be sooarat pishfarz string khali ast.
	 */
	DTBase(string name = "") {
		name = utility::Strings::trim(name);
		if (isValidName(name))
			_name = name;
		else
			EXTinvalidName("'" + name +
			               "' contains invalid characters");/**< agar name valid nabashd exception inavalid name throw mishavad*/
	}

	/**constructor DTBase ke meghdar e moteghaeir va type aan ra migirad
	 *
	 * meghdar dar string zakhire shode ast ke in constructor voroodi haye grefte shode raa be taabe fromstring
	 * midahad ta in taabe string ra parse konad va meghdar e vaghie an ra dar moteghaeir zakhire konad.
	 *
	 * @param text stringi ke meghdare moteghaeir dar aan ast.
	 * @param type nahve zakhire shodane meghdar ra dar string neshan midahad(json, xml, raw...)
	 */
	DTBase(string name, string text) : DTBase(name) {
		this->fromString(text);
	}

	/**
	 * distructor DTBase.
	 */
	virtual ~DTBase() {
	}

	/**toString ke meghdar e moteghaeir ra be soorat string dar miavarad va barmigardand.
	 *
	 * dar in taabe ba tavajoh be typei ke vared mishavad, meghdar e taabe dar string zakhire mishavad va
	 * bargardande mishavad. in taabe be soorat pure tarif shode ast choon piadesazi aan dar kellas hayi ke az in
	 * kellas be ers miravand motafavet ast. yani nahve tabdil maghadir be string masalan dar arraye ba integer
	 * motafavet ast.
	 *
	 * @param type nahve tabdil e meghdar e moteghaeir be string.
	 *
	 * @return strinigi ke dar an meghdar moteghaeir zakhire shode ast.
	 */
	virtual string toString() = 0;

	/**fromString ke stringi raa ke migirad parse mikonad va aan string raa tabdil be meghdar moteghaeir mikond
	 *
	 * kaar e in taabe barax e toString ast va string va nahve zakhire maghadir ra migirad va ba estefade az an
	 * meghdar ghabel estefade ra az string voroodi mikhanad.in taabe ham pure ast choon piadesazi aan dar
	 * kelasaaye mokhtalef motafavet ast.
	 *
	 * @param data stringi ke dar aan meghdar e moteghaeir zakhire shode ast.
	 * @param type nahve zakhire maghadir(json, xml...)
	 *
	 */
	virtual void fromString(string data) = 0;

	/**overload operator = baraye vaghti yek moteghaeir mosavi string gharar girad.
	 *
	 * be in soorat ke har moghe moteghaei mosavi string gharar greft, aan string parse mishavad va meghdar e
	 * ghabel estefade aan dar moteghaeir zakhire mishavad.in taabe pure ast.
	 *
	 * @param str stringi ke mosavi moteghaeir gharar migirad.
	 *
	 * @return vaghti mosavi gharar dad hamin kelass ra(this) barmigadand.
	 */
	virtual DTBase &operator=(const string str) = 0;

	/**overload operator ==
	 *
	 * agar meghdar moteghaeir DTBase dar samte rast ba samt e chap barabar bashad, true va dar gheir in soorat
	 * false barmigardand.
	 *
	 * @param dtvar taraf e samte raste ==
	 *
	 * @retrun true or false
	 *
	 */
	virtual bool operator==(DTBase &dtvar) = 0;

	/**overload operator !=
	 *
	 * agar meghdar moteghaeir DTBase dar samte rast ba samt e chap nabarabar bashad, true va dar gheir in soorat
	 * false barmigardand.
	 *
	 * @param dtvar taraf e samte raste !=
	 *
	 * @retrun true or false
	 *
	 */
	virtual bool operator!=(DTBase &dtvar) = 0;

	/**
	 * getname naame moteghaeir ra barmigardand.
	 *
	 * @return naame moteghaeir.
	 */
	virtual string getName() {
		return _name;
	}

	/**
	 * setname string voroodi ra mosavi naame moteghaeir gharar midahad.
	 *
	 * @param name stringi ke bayad dar moteghaeir _name gharar dahad.
	 */
	virtual void setName(string name) {
		_name = name;
	}

	/**
	 * getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 */
	static string getTypeName() {
		return "DTBase";
	}

	/**
	 * in taabe mesl e taabe getType name amal mikonad ba in tafavot ke alave bar esm e kellasi ke dar hastim,
	 * esm e moteghaeir hame barmigardand ta dar exception.what zakhire shavad.
	 *
	 * @return name moteghaeir va naame kellas.
	 */
	virtual string getNameAndType() {
		return getName() + "[" + getTypeName() + "]";
	}

private:
	string _name;/**< naame moteghaeir */
};

}
}
#endif // DTBASE_H
