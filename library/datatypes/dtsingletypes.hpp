#ifndef DTSINGLETYPES_H
#define DTSINGLETYPES_H

#include <iostream>
#include <math.h>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstring>

#include <datatypes/dtbase.hpp>
#include <utility/exceptionex.hpp>


using namespace std;
using zeitoon::utility::Strings;

namespace zeitoon {

/**namespace datatypes ke dar aan kelass haye marboot be datataypes gharar migirad.
 *
 * hadaf e datatypes in ast ke yek standard baraye tarif va estefade az moteghayer haayi ke dar
 * code haye plugin ha core va tamam ghesmat proje zeitoon gharar darand estefade shavad.
 * kelass haye datatypes be in goone hastnad ke baraye type haye mokhtalef piadesazi jodagaane darand.
 * dar namespace datatype type haye integer va anvaa e an(long int,...), float va anvaa e an(double,...),
 * boolian, string, enum, array va structure gharar darad ke har moghe dar code niaz be tarif moteghaeri az in type ha
 * bashad, kellas e mored nazar seda zade mishavad.
 *
 */
namespace datatypes {

/**class DTSingleTypes baraye moteghaeir haayie k yki hastand.(hame be joz arraye va structure).
 *
 * in class az class DTBase ers mibarad va hamchnin kelass haye digari hastnad ke az in kellas ers mibarand.
 * dar in kellas tavabe va piadesazi haye marboot e motegheir haayi k sinle hastand yani faghat az aan no 1 adad tolid
 * shode ast gharar grefte ast.(aaraye va structure single nistand va multi hastnad).dar template ii ke tarif shode ast
 * dar T type moteghaeir zakhire mishavad. in type mitavanad integer(long ineteger...), float(long float...),boolian
 * string va enum bashad. va az T dar piadesazi kellas ha estefade mikoim.
 *
 */
template<typename T>
class DTSingleType: public DTBase {
protected:
	T _defaultValue;/**< meghdar e default baraye moteghaeri ke sakhte shode ast. az noe T, yani az noe moteghaeri sakhte shode*/
	T _value;/**< meghdar e moteghaeiri ke sakhte mishavad dar in moteghaeir zakhire mishavad */

	/**parseObject, objecti ra ke daryaaft mikonad ra parse mikond
	 *
	 * be tor daghightar, objecti ra ke daryaft mikonad raa meghdar e aan raa az aan biroon miavarad va barmigardanad.
	 * ebteda object raa be hamin objecti ke dar aan hastim cast mikonad, agar amal e cast anjam shod, yani noe
	 * 2 object ba ham barabar ast pas kaafi ast meghdar e object ra bargardaanad va parse kardan anjam mishavad.
	 * agar cast anjaam nashod, be in mani ast ke noe object voroodi va object ie k dar aan hastim(this) barabar nis
	 * va bayad meghdar e object ra tabdil be string kard va aan ra be parseString frestad ta dar anjaa amal e
	 * parse kardan anjaam shavad va meghdar e object bargardande shavad.
	 *
	 * @param dtvar object voroodi ke bayad parse shavad.
	 *
	 * @return meghdar value e object e voroodi
	 *
	 */
	virtual T parseObject(DTBase& dtvar) {
		DTSingleType<T> *temp;
		temp = dynamic_cast<DTSingleType<T>*>(&dtvar);
		if (temp) {
			return temp->getValue();
		} else {
			return this->parseString(temp->toString(RAW));
		}
	}

	/**parseString ke string voroodi ra parse mikonad va meghdar ezakhire shode ra barmigardand.
	 *
	 * in taabe be sooarat pure tarif shode ast choon piadesazi aan dar kellas haye mokhtalef motafavet ast.
	 *
	 * @param stringi ke bayad pasre shavad.
	 *
	 * @return meghdari k dar string zakhire shode ast ke az noe T ya haman objecte this ast.
	 */
	virtual T parseString(string str) = 0;

public:

	/**constructor e DTSingleType
	 *
	 * @param name name object ya haman moteghaeir ra moshakhas mikonad.
	 * @param def meghdar default baraye moteghaeir gharar midahad.
	 *
	 */
	DTSingleType(string name, T def) :
			DTBase(name) {
		_defaultValue = def;
		_value = def;
	}

	/**reset meghdar moteghaer ra hazf mikonad va mosavi meghdar default gharar midahad.
	 *
	 */
	virtual void reset() {
		_value = _defaultValue;
	}

	/**getDefualtValue meghdar default moteghaeir ra barmigardanad.
	 *
	 * @return _defaultValue.
	 *
	 */
	virtual T getDefualtValue() {
		return _defaultValue;
	}

	/**setDefualtValue meghdar e voroodi ra mosavi defaultvalue e motaghaeir gharar midahad.
	 *
	 * @param val meghdari ke bayad dar defaukt value zakhire shavad.
	 *
	 */
	virtual void setDefualtValue(T val) {
		_defaultValue = val;
	}

	/**getValue value e object ra barmigardand.
	 *
	 * @return _value
	 */
	virtual T getValue() {
		return _value;
	}

	/**setValue meghdar voroodi ra mosavi value e object gharar midahad.
	 *
	 * @param meghdari ke bayad dar value e object zakhire shavad.
	 *
	 */
	virtual void setValue(T val) {
		_value = val;
	}

	/**value getvalue() ra barmigardanad.
	 *
	 * @return getValue()
	 *
	 */
	virtual T value() {
		return getValue();
	}

	/**overload operator =
	 *
	 * object voroodi ra parse mikonad va meghdar e parse shode ra dar vlue moteghaeir gharar midahad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 */
	virtual DTBase& operator =(DTBase& dtvar) {
		setValue(parseObject(dtvar));
		return *this;
	}

	/**overload operator ==
	 *
	 * object voroodi ra parse mikonad, agar be excpetion barkhord nakard, check mikonad
	 * ke meghdar aan ba meghdar e moteghaeir barabar ast yaa na.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @return true or flase
	 *
	 */
	virtual bool operator ==(DTBase& dtvar) {
		T val;
		try {
			val = parseObject(dtvar);
		} catch (dataTypeMismatch *x) {
			return false;
		} catch (cantParseString *x) {
			return false;
		}
		return (_value == val);
	}

	/**overload operator !=
	 *
	 * @param dtvar object e voroodi
	 *
	 * @return true or flase
	 *
	 */
	virtual bool operator !=(DTBase& dtvar) {
		return !(*this == dtvar);
	}

	/**overload operator =
	 *
	 * string voroodi ra parse mikonad va meghdar e parse shode ra dar vlue moteghaeir gharar midahad.
	 *
	 * @param str string e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 */
	virtual DTBase& operator =(string str) {
		setValue(parseString(str));
		return *this;
	}

	/**overload operator ==
	 *
	 * string voroodi ra parse mikonad, agar be excpetion barkhord nakard, check mikonad
	 * ke meghdar aan ba meghdar e moteghaeir barabar ast yaa na.
	 *
	 * @param str string e voroodi
	 *
	 * @return true or flase
	 *
	 */
	virtual bool operator ==(string str) {
		T val;
		try {
			val = parseString(str);
		} catch (dataTypeMismatch *x) {
			return false;
		} catch (cantParseString *x) {
			return false;
		}
		return (_value == val);
	}

	/**overload operator !=
	 *
	 * @param str string e voroodi
	 *
	 * @return true or flase
	 *
	 */
	virtual bool operator !=(string str) {
		return !(*this == str);
	}

	/**
	 * getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 *
	 */
	static string getTypeName() {
		return "DTSingleType";
	}
};

/**class DTInteger
 *
 * in class az kelasse DTSingleType ers mibarad. hamchenin template ie ke tarif shode ast noe object
 * ra dar IntType zakhire mikonad ke dar injaa int va anvaa e an (long int, long long int,...)
 * zakhire mishavad.
 *
 */
template<typename IntType = int>
class DTInteger: public DTSingleType<IntType> {
public:

	/**constructor baraye DTInteger.
	 *
	 * @param name naame moteghaeir ke constructor DTSingleType miferstd.
	 * @param def meghdar default baraye _value e object.be sooarat e pishfarz 0 ast.
	 *
	 */
	DTInteger(string name, IntType def = 0) :
			DTSingleType<IntType>(name, def) {
		_min = 1;
		_max = 0;
	}

	/**constructor baraye DTInteger.
	 *
	 * @param name naame moteghaeir ke constructor DTSingleType miferstd.
	 * @param def meghdar default baraye _value e object.be sooarat e pishfarz 0 ast.
	 * @param min,max minimom va maximom meghdar mojaz baraye value ra moshakhs mikond.
	 *
	 */
	DTInteger(string name, IntType min, IntType max, IntType def = 0) :
			_min(min), _max(max), DTSingleType<IntType>(name, def) {
	}

	/**distructor baraye DTInteger.
	 *
	 */
	virtual ~DTInteger() {
	}

	/**setvalue meghdar e voroodi ra dar _value zakhire mikond.
	 *
	 * albate barresi mikonad k adad voroodi bein e min o max hast yaa na.agar nabood
	 * exception throw mikond.
	 *
	 * @param val ke bayad dar _value zakhire shavad.
	 *
	 */
	void setValue(IntType val) {
		if (_max > _min && (val < _min || val > _max)) {
			stringstream msg;
			msg << "Provided value is out of range. Min : " << this->_min << " Max : " << this->_max << " Value: " << val;
			EXToutOfRange(msg.str());
		}
		DTSingleType<IntType>::setValue(val);
	}

	/**toString ke _value object ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...),adad ra dar mabnaaye 10 dar string zakhire mikond va
	 * string ra barmigardanad.
	 *
	 * @param type SerializationType(json,xml,....)
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type) {
		return toString(type, 10);
	}

	/**toString ke _value object ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...), va base ke mabnaa ra moshakhs mikond dar string
	 * zakhire mikonad.
	 *
	 * @param type SerializationType(json,xml,....)
	 * @param base adad dar che mabnaayei dar string zakhire shavad.
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type, int base) {
		stringstream str;
		string data;
		//char based[64];
		//memset(based, 0, 64);
		string based = Strings::toString((long long int)this->getValue());		//, based, base);
		switch (type) {
		case RAW:
		case JSON:
			str << based;
			break;
		case XML:
			str << '<' << this->getName() << '>' << based << "</" << this->getName() << '>';
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		return str.str();
	}

	/**fromstring baraye khoondan adad az stringi k daryaft mikonad.
	 *
	 * string ra parse mikonad va adad ra az aan mikhanad va dar _value object zakhire mikond.
	 *
	 * @param data stringi k bayad az aan dade bekhanad
	 * @param type SerializationType(json,xml,...)
	 *
	 */
	void fromString(string data, SerializationType type, bool checkName = true) {
		int i = 0;
		IntType val;
		switch (type) {
		case JSON:
			val = parseString(data);
			break;
		case XML:
			EXTnotImplemented("XML not implemented yet.");
			break;
		case RAW:
			val = parseString(data);
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		this->setValue(val);
	}

	/**overload operator =

	 * meghdar voroodi ra dar _value zakhire mikond.
	 *
	 * @param var IntType e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 *
	 */
	DTBase& operator =(IntType var) {
		setValue(var);
		return *this;
	}

	/**overload operator ==
	 *
	 *agar meghdar e voroodi ba value e object barabar bood true va dar gheir in sooart false
	 *barmigardanad.
	 *
	 * @param var IntType e voroodi
	 *
	 * @return true or flase
	 *
	 */
	bool operator ==(IntType var) {
		return (this->getValue() == var);
	}

	/**
	 * mokhalefe operator == barmigardanad.
	 *
	 * @param var IntType
	 *
	 */
	bool operator !=(IntType var) {
		return !(*this == var);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 */
	virtual DTBase& operator =(DTBase& dtvar) {
		return DTSingleType<IntType>::operator =(dtvar);
	}

	/**overload operator ==
	 *
	 * object voroodi ra parse mikonad, agar be excpetion barkhord nakard, check mikonad
	 * ke meghdar aan ba meghdar e moteghaeir barabar ast yaa na.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @return true or flase
	 *
	 */
	virtual bool operator ==(DTBase& dtvar) {
		IntType val;
		bool exact;
		try {
			val = parseObject(dtvar, exact);
		} catch (dataTypeMismatch *x) {
			return false;
		} catch (cantParseString *x) {
			return false;
		}
		return exact && (this->getValue() == val);
	}

	/**overload operator !=
	 *
	 * @param dtvar object e voroodi
	 *
	 * @return true or flase
	 *
	 */
	virtual bool operator !=(DTBase& dtvar) {
		return !(*this == dtvar);
	}

	/**operator =
	 *
	 * operator = DTSingleType raa seda mizand va meghadri k midahad ra barmigardanad.
	 *
	 * @param str stringi ke bayad parse shavad va meghdar e daroon e an dar object zakhire shavad.
	 *
	 * @return object bargardande mishavad.
	 *
	 */
	virtual DTBase& operator =(string str) {
		return DTSingleType<IntType>::operator =(str);
	}

	/**overload operator ==
	 *
	 * string voroodi ra parse mikonad, agar be excpetion barkhord nakard, check mikonad
	 * ke meghdar aan ba meghdar e moteghaeir barabar ast yaa na.
	 *
	 * @param str string e voroodi
	 *
	 * @return true or flase
	 *
	 */
	virtual bool operator ==(string str) {
		IntType val;
		bool exact;
		try {
			val = parseString(str, exact);
		} catch (dataTypeMismatch *x) {
			return false;
		} catch (cantParseString *x) {
			return false;
		}
		return exact && (this->getValue() == val);
	}

	/**overload operator !=
	 *
	 * @param str string e voroodi
	 *
	 * @return true or flase
	 *
	 */
	virtual bool operator !=(string str) {
		return !(*this == str);
	}

	/**
	 * getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 */
	static string getTypeName() {
		return "DTInteger";
	}
protected:
	IntType _min;
	IntType _max;

	/*parseOnject
	 *
	 * hamanand parseObject DTSingleType amal mikonad va tafavoti ke darad in ast yek moteghaeir e boolian isExact
	 * daryaft mikond. baraye in manzoor ke momken ast dar object adadi az noe float zakhire shode bashad vali bayad
	 * be soorat e integer parse shavad ta az aan estefade kard shavad. isExact moshakhas mikonad ke adadi ke
	 *  parse mikonad daghighan haman adadiast ke dar object zakhire shode ast ya na.
	 *
	 *@param dtvar objecti dar aan value gharar darad va bayad parse shavad.
	 *@param isExact true or false
	 *
	 *@return adad parse shode az noe IntType.
	 */
	IntType parseObject(DTBase &dtvar, bool& isExact) {
		IntType val;
		DTSingleType<IntType> *temp;
		temp = dynamic_cast<DTSingleType<IntType>*>(&dtvar);
		if (temp) {
			val = temp->getValue();
			isExact = true;
		} else {
			val = this->parseString(temp->toString(RAW), isExact);
		}
		return val;
	}

	/**parseString
	 *
	 * parse strring DTInteger raa seda mizanad ta vaziat isExact ra barresi konad.
	 *
	 * @param stringi ke bayad parse shavad.
	 *
	 * @return adadi ke parse shode ast.
	 *
	 */
	IntType parseString(string str) {
		bool isExact;
		return parseString(str, isExact);
	}

	/**parseString
	 *
	 * dar in taabe string voroodi ra daryaft mioknad va adad dakhel aan ra az aan joda mikonad va barmigardanad.
	 * taabe trim dar ketabkhane utility ra seda mizanad ta space va digar caracter haye pishfarz ra az aval va
	 * akhare string hazf mikonad va barmigardanad. sepas agar caracter haye string valid boodand(adad, va harf e ya E(baraye tavan
	 * ,' ','.','-','+'))
	 * moteghaeir ra mikhanad va aan ra barmigardanad.
	 *
	 * @param str stringi ke bayad parse shavad.
	 * @param ba meghdar dadan be in moteghaeir moshakhs mishavad ke adad e parse shode daghighan haman chizi ast
	 * ke dar string vojood dard ya inke round shode ast.
	 *
	 * @return adad e hasel az parse kardan.
	 *
	 */
	IntType parseString(string str, bool& isExact) {
		IntType x;
		long double y;
		str = Strings::trim(str);
		for (int i = 0; i < str.size(); i++) {
			if ((str[i] < 48 || str[i] > 57)	//Numbers
			&& str[i] != 32 && str[i] != 45 && str[i] != 46 && str[i] != 43	// ' ','.','-','+'
			&& str[i] != 101 && str[i] != 69)	//e & E
				EXTcantParseString("Can't parse this string as Integer : \"" + str + "\"");
		}
		stringstream data(str);
		data >> y;
		x = round(y);
		isExact = (x == y ? true : false);
		return x;
	}
};

/**class DTFloat
 *
 * in class az kelasse DTSingleType ers mibarad. hamchenin template ie ke tarif shode ast noe object
 * ra dar FloatType zakhire mikonad ke dar injaa int va anvaa e an (double,long double,...)
 * zakhire mishavad.
 *
 */
template<typename FloatType = float>
class DTFloat: public DTSingleType<FloatType> {
public:

	/**constructor baraye DTFloat.
	 *
	 *@param name naame moteghaeir ke constructor DTSingleType miferstd.
	 * @param def meghdar default baraye _value e object.be sooarat e pishfarz 0 ast.
	 *
	 */
	DTFloat(string name, FloatType def = 0) :
			DTSingleType<FloatType>(name, def) {
		_min = 1;
		_max = 0;
	}

	/**constructor baraye DTFloat.
	 *
	 * @param name naame moteghaeir ke constructor DTSingleType miferstd.
	 * @param def meghdar default baraye _value e object.be sooarat e pishfarz 0 ast.
	 * @param min,max minimom va maximom meghdar mojaz baraye value ra moshakhs mikond.
	 *
	 */
	DTFloat(string name, FloatType min, FloatType max, FloatType def = 0) :
			_min(min), _max(max), DTSingleType<FloatType>(name, def) {
	}

	/**
	 * distructor baraye DTFloat
	 *
	 */
	virtual ~DTFloat() {
	}

	/**setvalue meghdar e voroodi ra dar _value zakhire mikond.
	 *
	 * albate barresi mikonad k adad voroodi bein e min o max hast yaa na.agar nabood
	 * exception throw mikond.
	 *
	 * @param val ke bayad dar _value zakhire shavad.
	 *
	 */
	void setValue(FloatType val) {
		if (_max > _min && (val < _min || val > _max)) {
			stringstream msg;
			msg << "Provided value is out of range. Min : " << this->_min << " Max : " << this->_max << " Value: " << val;
			EXToutOfRange(msg.str());
		}
		DTSingleType<FloatType>::setValue(val);
	}

	/**toString ke _value object ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...), dar string zakhire mikonad.
	 *
	 * @param type SerializationType(json,xml,....)
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type) {
		stringstream str;
		string data;
		switch (type) {
		case RAW:
		case JSON:
			str << setprecision(std::numeric_limits<double>::digits10 + 1) << this->getValue();
			break;
		case XML:
			str << '<' << this->getName() << '>' << setprecision(std::numeric_limits<FloatType>::digits10 + 1) << this->getValue() << "</" << this->getName() << '>';
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		return str.str();
	}

	/**fromstring baraye khandan adad az stringi k daryaft mikonad.
	 *
	 * string ra parse mikonad va adad ra az aan mikhanad va dar _value object zakhire mikond.
	 *
	 * @param data stringi k bayad az aan dade bekhanad
	 * @param type SerializationType(json,xml,...)
	 *
	 */
	void fromString(string data, SerializationType type, bool checkName = true) {
		int i = 0;
		FloatType val;
		string name;
		stringstream str(data);
		switch (type) {
		case JSON:
			val = parseString(data);
			break;
		case XML:
			EXTnotImplemented("XML not implemented yet.");
			break;
		case RAW:
			val = parseString(data);
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		this->setValue(val);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 */
	virtual DTBase& operator =(DTBase& dtvar) {
		return DTSingleType<FloatType>::operator =(dtvar);
	}

	/**overload operator ==
	 *
	 * operator == kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn true or false.
	 *
	 */
	virtual bool operator ==(DTBase& dtvar) {
		return DTSingleType<FloatType>::operator ==(dtvar);
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @return true or false
	 */
	virtual bool operator !=(DTBase& dtvar) {
		return !(*this == dtvar);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param str string voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 *
	 */
	virtual DTBase& operator =(string str) {
		return DTSingleType<FloatType>::operator =(str);
	}

	/**overload operator ==
	 *
	 * operator == kelass e DTSingletype ra seda mizanad.
	 *
	 * @param str string voroodi
	 *
	 * @rerturn true or false.
	 *
	 */
	virtual bool operator ==(string str) {
		return DTSingleType<FloatType>::operator ==(str);
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param str string e voroodi
	 *
	 * @return true or false
	 */
	virtual bool operator !=(string str) {
		return !(*this == str);
	}

	/**overload operator =
	 *
	 * meghdar voroodi ra dar _value zakhire mikond.
	 *
	 * @param var FloatType e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 *
	 */
	DTBase& operator =(FloatType var) {
		setValue(var);
		return *this;
	}

	/**overload operator ==
	 *
	 *agar meghdar e voroodi ba value e object barabar bood true va dar gheir in sooart false
	 *barmigardanad.
	 *
	 * @param var FloatType e voroodi
	 *
	 * @return true or flase
	 *
	 */
	bool operator ==(FloatType var) {
		return (this->getValue() == var);
	}

	/**overload operator !=
	 *
	 *mokhalefe operator == ra barmigardanad.
	 *
	 * @param var FloatType
	 *
	 * @return true or flase
	 *
	 */
	bool operator !=(FloatType var) {
		return !(*this == var);
	}

	/**
	 *getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 *
	 */
	static string getTypeName() {
		return "DTFloat";
	}
protected:
	FloatType _min;
	FloatType _max;

	/**parseString
	 *
	 * dar in taabe string voroodi ra daryaft mioknad va adad dakhel aan ra az aan joda mikonad va barmigardanad.
	 * taabe trim dar ketabkhane utility ra seda mizanad ta space va digar caracter haye pishfarz ra az aval va
	 * akhare string hazf mikonad va barmigardanad. sepas agar caracter haye string valid boodand(adad, va harf e ya E(baraye tavan)
	 * ,' ','.','-','+')
	 * moteghaeir ra mikhanad va aan ra barmigardanad.
	 *
	 * @param str stringi ke bayad parse shavad.
	 * @param ba meghdar dadan be in moteghaeir moshakhs mishavad ke adad e parse shode daghighan haman chizi ast
	 * ke dar string vojood dard ya inke round shode ast.
	 *
	 * @return adad e hasel az parse kardan.
	 *
	 */
	FloatType parseString(string str) {
		FloatType val;
		str = Strings::trim(str);
		for (int i = 0; i < str.size(); i++) {
			if ((str[i] < 48 || str[i] > 57)	//Numbers
			&& str[i] != 32 && str[i] != 45 && str[i] != 46 && str[i] != 43	// ' ','.','-','+'
			&& str[i] != 101 && str[i] != 69)	//e & E
				EXTcantParseString("Can't parse this string as Integer : \"" + str + "\"");
		}
		stringstream strs(str);
		strs >> val;
		return val;
	}
};

/**class DTBoolean
 *
 * in class az kelasse DTSingleType ers mibarad.
 *
 */

class DTBoolean: public DTSingleType<bool> {
public:

	/**enum boolTxType
	 *
	 * dar in enum halat haye mokhtalefe zakhire dade dar string moshakhas mishavad.masalan agar dade true bood,
	 * dar string mitavanad true,one,on,yes zakhire shavad va baraye false ham be hamin manzoor.
	 *
	 */
	enum boolTxType {
		TrueFalse, OneZero, OnOff, YesNo
	};

protected:
	int boolStrLen = 4;
	string boolStr[4][2] = { { "false", "true" }, { "0", "1" }, { "off", "on" }, { "no", "yes" } };
	boolTxType _stringType;

public:

	/**constructor baraye DTBoolian
	 *
	 * @param name,def baraye gharar dadan dar naam e object va meghdar default aan._stringType be soorat e pishfarz
	 * TrueFlase dar nazar gerefte mishavad.
	 *
	 */
	DTBoolean(string name, bool def = false) :
			DTSingleType<bool>(name, def), _stringType(TrueFalse) {
	}

	/**setValue baraye meghdardehi kardan e _value moteghaeir ya haman object
	 *
	 * @param val agar 0 bood false va dar gheir in soorat true dar _value zakhire mishavad.
	 *
	 */
	void setValue(int val) {
		setValue(!(val == 0));
	}
	void setStringType(boolTxType type) {
		_stringType = type;
	}
	boolTxType getStringType() {
		return _stringType;
	}

	/**
	 * getInt meghdar e object ra be soorat e 0 ya 1 barmigardanad.
	 */
	int getInt() {
		return this->getValue() ? 1 : 0;
	}

	/**toString ke _value object ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...),adad ra dar string zakhire mikond va
	 * string ra barmigardanad.
	 *
	 * @param type SerializationType(json,xml,....)
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type) {
		return toString(type, getStringType());
	}

	/**toString ke _value object ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...), va txType ke foramt e zakhire ra moshakhas mikond(truefalse,...) dar string
	 * zakhire mikonad.
	 *
	 * @param type SerializationType(json,xml,....)
	 * @param txType format e zakhire ra moshakhas mikonad.
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type, boolTxType txType) {
		stringstream str;
		switch (type) {
		case RAW:
			str << boolStr[txType][getInt()];
			break;
		case JSON:
			if (_stringType == OneZero)
				str << boolStr[txType][getInt()];
			else
				str << "\"" << boolStr[txType][getInt()] << "\"";
			break;
		case XML:
			str << '<' << this->getName() << '>' << boolStr[txType][getInt()] << "</" << this->getName() << '>';
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		return str.str();
	}

	/**fromstring baraye khoondan daade az stringi k daryaft mikonad.
	 *
	 * string ra parse mikonad va daade ra az aan mikhanad va dar _value object zakhire mikond.
	 *
	 * @param data stringi k bayad az aan dade bekhanad
	 * @param type SerializationType(json,xml,...)
	 *
	 */
	void fromString(string data, SerializationType type, bool checkName = true) {
		bool val;
		string name;
		switch (type) {
		case RAW:
		case JSON:
			val = parseString(data);
			break;
		case XML:
			EXTnotImplemented("XML not implemented yet.");
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		this->setValue(val);
	}

	void on() {
		this->setValue(true);
	}
	void off() {
		this->setValue(false);
	}
	void disable() {
		this->setValue(false);
	}
	void enable() {
		this->setValue(true);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 */
	virtual DTBase& operator =(DTBase& dtvar) {
		return DTSingleType<bool>::operator =(dtvar);
	}

	/**overload operator ==
	 *
	 * operator == kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn true or false.
	 *
	 */
	virtual bool operator ==(DTBase& dtvar) {
		return DTSingleType<bool>::operator ==(dtvar);
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @return true or false
	 */
	virtual bool operator !=(DTBase& dtvar) {
		return !(*this == dtvar);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param str string voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 *
	 */
	virtual DTBase& operator =(string str) {
		return DTSingleType<bool>::operator =(str);
	}

	/**overload operator ==
	 *
	 * operator == kelass e DTSingletype ra seda mizanad.
	 *
	 * @param str string voroodi
	 *
	 * @rerturn true or false.
	 *
	 */
	virtual bool operator ==(string str) {
		return DTSingleType<bool>::operator ==(str);
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param str string e voroodi
	 *
	 * @return true or false
	 */
	virtual bool operator !=(string str) {
		return !(*this == str);
	}
	DTBoolean& operator =(bool val) {
		this->setValue(val);
		return *this;
	}
	DTBoolean& operator =(int val) {
		this->setValue(val);
		return *this;
	}
	bool operator ==(bool val) {
		return (this->getValue() == val);
	}
	bool operator !=(bool val) {
		return !(*this == val);
	}
	bool operator ==(int val) {
		return (!(val == 0)) == this->getValue();
	}
	bool operator !=(int val) {
		return !(*this == val);
	}

	/**
	 * getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 */
	static string getTypeName() {
		return "DTBoolean";
	}
protected:

	/**parseString
	 *
	 * dar in taabe string voroodi ra daryaft mioknad va data dakhel aan ra az aan joda mikonad va barmigardanad.
	 * taabe trim dar ketabkhane utility ra seda mizanad ta space va digar caracter haye pishfarz ra az aval va
	 * akhare string hazf mikonad va barmigardanad.sepas daade ra mikhanad va aan ra barmigardanad.
	 *
	 * @param str stringi ke bayad parse shavad.
	 *
	 * @return dade e hasel az parse kardan.
	 *
	 */
	bool parseString(string str) {
		str = Strings::trim(str, "\"");
		for (int i = 0; i < boolStrLen; i++)
			if (!Strings::compare(str, boolStr[i][0], false))
				return false;
			else if (!Strings::compare(str, boolStr[i][1], false))
				return true;
		EXTcantParseString("invalid string for boolean");
	}
};

/**class DTString
 *
 * in class az kelasse DTSingleType ers mibarad.
 *
 */
class DTString: public DTSingleType<string> {
public:

	/**
	 * enum ke moshakhas mikonad data be che formati dar string zakhire shavad.
	 *
	 */
	enum StringEncoding {
		Ascii, UTF8, UTF16, UTF32
	};

public:

	/**constructor baraye DTString.
	 *
	 *@param name naame moteghaeir ke constructor DTSingleType miferstd.
	 * @param def meghdar default baraye _value e object.be sooarat e pishfarz khali ast.
	 *
	 */
	DTString(string name, string def = "") :
			DTSingleType<string>(name, def) {
	}

	/**toString ke _value object ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...),adad ra dar string zakhire mikond vastring ra barmigardanad.dar ghesmate
	 * json ebteda taabe encodestring az ketabkhane utility ra seda mizanad ta caracter haayi mesl '\t' ya '\n'
	 * ke caracter haye khas system hastand ra be soorat e sahih dar string zakhire konad.
	 *
	 * @param type SerializationType(json,xml,....)
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type) {
		stringstream str;
		switch (type) {
		case JSON:
			str << '"' << utility::JSONUtility::encodeString(this->getValue()) << '"';
			break;
		case XML:
			str << '<' << this->getName() << '>' << this->getValue() << "</" << this->getName() << '>';
			break;
		case RAW:
			str << this->getValue();
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		return str.str();
	}

	/**fromstring baraye khoondan daade az stringi k daryaft mikonad.
	 *
	 * string ra parse mikonad va daade ra az aan mikhanad va dar _value object zakhire mikond.dar ghesmat
	 * json nmitavan az trim dar ketabkhane utility estefade kard choon momken ast harf aval stringi ke dar string voroodi
	 * zakhire shode ast '"' bashad va trim aan ra hazf khahad kard.pas be soorat e dasti '"'ra peida mikonad va
	 * meghdar beine '"' ra joda mikonad va barmigardanad.sepas aan ra decode mikonad ta caracter haayi mesl e
	 * '\n' ya '\t' be soorat sahih bashand. va baad az aan string be dast aamdade ra dar _value zakhire mikonad.
	 *
	 * @param data stringi k bayad az aan dade bekhanad
	 * @param type SerializationType(json,xml,...)
	 *
	 */
	void fromString(string data, SerializationType type, bool checkName = true) {
		string str;
		switch (type) {
		case JSON: {
			//we can't use trim because first character in string can be '"'
			size_t x = data.find('"');
			size_t y = data.rfind('"');
			if (x == string::npos || y == string::npos)
				EXTcantParseString("provided string is not enclosed with '\"'.");
			str = data.substr(x + 1, y - x - 1);
			str = parseString(utility::JSONUtility::decodeString(data));
			break;
		}
		case XML:
			EXTnotImplemented("XML not implemented yet.");
			break;
		case RAW:
			str = parseString(data);
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		this->setValue(str);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 */
	virtual DTBase& operator =(DTBase& dtvar) {
		return DTSingleType<string>::operator =(dtvar);
	}

	/**overload operator ==
	 *
	 * operator == kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn true or false.
	 *
	 */
	virtual bool operator ==(DTBase& dtvar) {
		return DTSingleType<string>::operator ==(dtvar);
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @return true or false
	 */
	virtual bool operator !=(DTBase& dtvar) {
		return !(*this == dtvar);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param str string voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 *
	 */
	virtual DTBase& operator =(string str) {
		return DTSingleType<string>::operator =(str);
	}

	/**overload operator ==
	 *
	 * operator == kelass e DTSingletype ra seda mizanad.
	 *
	 * @param str string voroodi
	 *
	 * @rerturn true or false.
	 *
	 */
	virtual bool operator ==(string str) {
		return DTSingleType<string>::operator ==(str);
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param str string e voroodi
	 *
	 * @return true or false
	 */
	virtual bool operator !=(string str) {
		return !(*this == str);
	}

	/**
	 * getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 */
	static string getTypeName() {
		return "DTString";
	}
protected:
	string parseString(string str) {
		return str;
	}
};

template<typename EnumType>
class DTEnum: public DTSingleType<int> {
public:

	/**constructor baraye DTEnum.
	 *
	 *@param name naame moteghaeir ke constructor DTSingleType miferstd.
	 * @param def meghdar default baraye _value e object.be sooarat e pishfarz khali ast.
	 *
	 */
	DTEnum(string name, int def = 0) :
			DTSingleType(name, def) {
	}

	/**toString ke _value object ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...),adad ra dar string zakhire mikond vastring ra barmigardanad.
	 *
	 * @param type SerializationType(json,xml,....)
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type) {
		stringstream str;
		switch (type) {
		case JSON:
			str << '"' << EnumType::typeString[this->getValue()] << '"';
			break;
		case XML:
			str << '<' << this->getName() << '>' << EnumType::typeString[this->getValue()] << "</" << this->getName() << '>';
			break;
		case RAW:
			str << EnumType::typeString[this->getValue()];
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		return str.str();
	}

	/**fromstring baraye khoondan daade az stringi k daryaft mikonad.
	 *
	 * string ra parse mikonad va daade ra az aan mikhanad va dar _value object zakhire mikond.
	 *
	 * @param data stringi k bayad az aan dade bekhanad
	 * @param type SerializationType(json,xml,...)
	 *
	 */
	void fromString(string data, SerializationType type, bool checkName = true) {
		int val;
		switch (type) {
		case RAW:
		case JSON:
			val = parseString(data);
			break;
		case XML:
			EXTnotImplemented("XML not implemented yet.");
			break;
		default:
			EXTinvalidParameter("You've provided invalid type for string serialization type parameter.");
		}
		this->setValue(val);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 */
	virtual DTBase& operator =(DTBase& dtvar) {
		return DTSingleType<int>::operator =(dtvar);
	}

	/**overload operator ==
	 *
	 * operator == kelass e DTSingletype ra seda mizanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @rerturn true or false.
	 *
	 */
	virtual bool operator ==(DTBase& dtvar) {
		return DTSingleType<int>::operator ==(dtvar);
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param dtvar object e voroodi
	 *
	 * @return true or false
	 */
	virtual bool operator !=(DTBase& dtvar) {
		return !(*this == dtvar);
	}

	/**overload operator =
	 *
	 * operator = kelass e DTSingletype ra seda mizanad.
	 *
	 * @param str string voroodi
	 *
	 * @rerturn object e this. khode moteghaeir.
	 *
	 */
	virtual DTBase& operator =(string str) {
		return DTSingleType<int>::operator =(str);
	}

	virtual DTBase& operator =(int val) {
		this->setValue(val);
		return *this;
	}

	/**overload operator ==
	 *
	 * operator == kelass e DTSingletype ra seda mizanad.
	 *
	 * @param str string voroodi
	 *
	 * @rerturn true or false.
	 *
	 */
	virtual bool operator ==(string str) {
		return DTSingleType<int>::operator ==(str);
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param str string e voroodi
	 *
	 * @return true or false
	 */
	virtual bool operator !=(string str) {
		return !(*this == str);
	}

	/**
	 * getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 */
	static string getTypeName() {
		return "DTEnum";
	}
protected:
	int parseString(string str) {
		str = Strings::trim(str, "\"");
		for (int i = 0; i < EnumType::__MAX; i++)
			if (!Strings::compare(str, EnumType::typeString[i], false))
				return i;
		EXTcantParseString("Unknown constant passed to Enum :" + str);
	}
};

}
}

#endif // DTSINGLETYPES_H
