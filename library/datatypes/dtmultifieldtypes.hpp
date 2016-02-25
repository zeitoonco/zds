#ifndef DTMULTIFIELDTYPES_H
#define DTMULTIFIELDTYPES_H

#include <iostream>
#include <vector>
#include <stdio.h>

#include <datatypes/dtbase.hpp>
#include <datatypes/dtsingletypes.hpp>


using namespace ::std;
using zeitoon::utility::Strings;
using zeitoon::utility::JSONUtility;

namespace zeitoon {
namespace datatypes {

/**class DTMultifieldType piadesazi baraye object haa va moteghaeir haayi ke single nistand
 *
 * baraye mesal arraye ha va structure ha sinle nistand choon tarkibi az chand singleType hastand pas piadesaazi
 * jodagaane khahand daasht. in class az class e DTbase ers mibarad. va dar tempatei ke baraye aan tarif shode ast,
 * noe moteghaeir multi dar childT zakhire mishavad. az in kelass do class DTSet va DTStruct ers mibarand.
 *
 */
template<typename childT>
class DTMultiFieldType : public DTBase {
public:
	typedef typename vector<childT *>::iterator iterator;/**< vector az jense childT ke moteghaeir haaye single object ra dar khod zakhire mikonad*/

	/**constructor e DTMultiFieldType
	 *
	 * @param name name object ya haman moteghaeir ra moshakhas mikonad.
	 *
	 */
	DTMultiFieldType(string name) :
			DTBase(name) {
	}

	/**lenght ke tedad moteghaeir haye ke dar khod darad ra barmigardanad.
	 *
	 *@return size of list
	 *
	 */
	virtual size_t length() {
		return list.size();
	}

	/**add() baraye ezafe kardan yek moteghaeir e jadid be object.
	 *
	 * @param val objectti ke bayad be moteghair ha ezafe shavad.
	 *
	 */
	virtual void add(childT *val) {
		list.push_back(val);
	}

	/**remove() baraye hazf kardan yek object az list objecthaa ast.
	 *
	 * dar in taabe val ra dar voroodi migirad va aan ra dar liste object ha search mikonad va vaghti aan ra peida kard
	 * aan ra az list hazf mikonad. yek voroodi digare taabe del ast va baraaaye in manzoor ast ke momken ast
	 * objecti ke maa mikhahim hazf shavad ra khodeman haafeze be aan takhsis dadeiem(new kardim) va mikhahim
	 * faghat az list hazf shavad vali in object dar memory paak nashavad pas del ra false gharar midahim ta
	 * aan ra az memory paak delete nakonad. vali agar bekhahim object az memory niz hazf shavad del ra true
	 * gharar midahim.
	 *
	 * @param val objecti ke bayad az list hazf shavad.
	 * @param del boolian baraye moshakhas kardan delete kaamel e object ya na.
	 *
	 */
	virtual void remove(childT *val, bool del = false) {
		for (iterator i = list.begin(); i != list.end(); i++)
			if (*i == val) {
				list.erase(i);
				if (del)
					delete *i;
				break;
			}
	}

	/**clear list ra khali mikonad va tamame objecthaye list ra az list hazf mikonad.
	 *
	 * @param del boolian baraye moshakhas kardan delete kaamele object az memory ya na.
	 *
	 */
	virtual void clear(bool del = false) {
		if (del)
			for (iterator i = list.begin(); i != list.end(); i++)
				delete *i;
		list.clear();
	}

	/**overload operator [] baraye bargardandan e object ba tavajoh be shomare aan dar list.
	 *
	 * @param index shomare ye object
	 *
	 * @return object ba shomare moshakhas dar list.
	 *
	 */
	virtual childT *operator[](int index) {
		return list[index];
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
		return "DTMultiFieldType";
	}

protected:
	vector<childT *> list;
};

/**class DTSet
 *
 * class DTSet baraye piadesaazi type moteghaeir az noe arraye ast. be in soorat e noe arraye ra dar template gharar
 * midahand va az aan estefade mikonad. arraye mitavand az har noei bashad masaln az noe khode DTSet ya DTInteger bashad.
 * in class az class DTMultiType ers mibarad.
 *
 */
template<typename T>
class DTSet : public DTMultiFieldType<T> {
public:
	typedef typename vector<T *>::iterator iterator;
	typedef vector<T *> List;
protected:

	/**
	 * dar inja yek vector az noe T ijad mikonim. hadafe in vecrtor in ast ke hargah yek object ra be soorate
	 * dasti be an hafeze dadim(new kardim), alave bar zakhire kardane aan object dar list kelass DTMultitype,
	 * aan ra dar vector allocatedOnes ham zakhire mikonim taa yek list az object haayi ke be soorate dasti
	 * new shodeand ra dashte bashim.
	 *
	 */
	List allocatedOnes;

	/**isAllocated() object ra migirad va search mikonad ke dar vector allocatedOnes ast ya na.
	 *
	 * @param item objecti ke bayad search shavad.
	 *
	 * @return agar dar vector bood true va agar nabood false barmigardanad.
	 *
	 */
	bool isAllocated(T *item) {
		for (iterator i = allocatedOnes.begin(); i != allocatedOnes.end(); i++)
			if (*i == item)
				return true;
		return false;
	}

	/**clearAllocatedOnes()
	 *
	 * tamame object haayi ke khodeman new karde iim ra az liste kelass DTMultitype hazf mikonad, hamchin az
	 * memory paak mikonad va sepas vector allocatedOnes ra ham khali mikonad.
	 *
	 */
	void clearAllocatedOnes() {
		for (iterator i = allocatedOnes.begin(); i != allocatedOnes.end(); i++)
			DTMultiFieldType<T>::remove(*i, true);
		allocatedOnes.clear();
	}

	/**tryRemoveAllocated() yek object khaas ra ham az list DTMultiType hazf mikonad ham az allocatedOnes.
	 *
	 * @param d objecti ke bayad kollan hazf shavad.
	 *
	 * @return agar amaliat e hazf anjaam shod true va agar object ra peida nakard false barmigardanad.
	 *
	 */
	bool tryRemoveAllocated(T *item) {
		for (iterator i = allocatedOnes.begin(); i != allocatedOnes.end(); i++)
			if (*i == item) {
				DTMultiFieldType<T>::remove(*i, true);
				allocatedOnes.erase(i);
				return true;
			}
		return false;
	}

	/**addAllocated() objecte jadid be vector AllocatedOnes ezafe mikonad.
	 *
	 *@param item objecti ke bayad be vector allocatedOnes ezafe shavad.
	 *
	 */
	void addAllocated(T *item) {
		allocatedOnes.push_back(item);
	}

public:

	/**constructor baraye DTSet.
	 *
	 *@param name naame moteghaeir ke be constructor DTMultiType miferstd.
	 *
	 */
	DTSet(string name) :
			DTMultiFieldType<T>(name) {
	}

	/**
	 * distructor baraye DTSet
	 *
	 */
	virtual ~DTSet() {
		clearAllocatedOnes();
	}

	/**add() objecte jadid be list DTMultiType ezafe mikonad.
	 *
	 * @param val objecti ke bayad ezaafe shavad.
	 *
	 */
	virtual void add(T *val) {
		DTMultiFieldType<T>::add(val);
	}

	/**add() dade ie ke be soorate string vared mishavad ra dar list zakhire mikonad.
	 *
	 * dar inja string vared mishavad ke dar aan daade be soorat RAW zakhire shode ast. taabe yek object misazad
	 * va meghdare daade ra dar aan zakhire mikonad va sepas object ra be list DTMultiType va hamchin
	 * allocatedOnes ezafe mikonad.
	 *
	 * @param stringi ke daade be soorat RAW dar an zakhire shode ast.
	 *
	 */
	virtual void add(string str) {
		T *temp = new T("");
		temp->fromString(str, RAW);
		add(temp);
		addAllocated(temp);
	}

	/**insert baraye ezafe kardane objecte voroodi dar makaane moshakhas dar liste DTMultiType
	 *
	 * @param val objecti ke bayad be list ezafe shavad.
	 * @param index makaan ya haman shomare ie ke bayad val dar list gharar girad.
	 *
	 */
	virtual void insert(T *val, int index) {
		if (index < 0 || index > this->list.size())
			EXToutOfRange(
					"Provided index(" + Strings::itoa(index)
					+ ") is out of range(0-"
					+ Strings::itoa(this->list.size()) + ")");
		this->list.insert(this->list.begin() + index, val);
	}

	/**remove() ebteda val ra az allocatedOnes hazf mikonad sepas aan ra az list DTMultiType hazf mikonad.
	 *
	 * @param val objecti ke bayad hazf shavad.
	 * @param del moshakhas mikonad ke val be soorat e kaamel az memory hazf shavad ya na.
	 *
	 */
	virtual void remove(T *val, bool del = false) {
		if (!tryRemoveAllocated(val))
			DTMultiFieldType<T>::remove(val, del);
	}

	/**clear har do vector allocatedOnse va list DTMultyType ra khali mikonad.
	 *
	 * @param del moshakhas mikonad ke object ha be soorat e kaamel az memory hazf shavad ya na.
	 *
	 */
	virtual void clear(bool del = false) {
		clearAllocatedOnes();
		DTMultiFieldType<T>::clear(del);
	}

	/**removeAt() object moshakhasi ra az har do vector paak mikonad.
	 *
	 * in tabe shomareye objecti ke dar list vojood darad va bayad hazf shavad ra migirad va aan object ra peida
	 * mikonad va az har do vectore allocatedOnes va list hazf mikonad.
	 *
	 * @param shomare objecti ke bayad hazf shavad.
	 * @param del moshakhas mikonad ke object be soorat e kaamel az memory hazf shavad ya na.
	 *
	 */
	virtual void removeAt(int index, bool del = false) {
		if (index < 0 || index >= this->list.size())
			EXToutOfRange(
					"Provided index(" + Strings::itoa(index)
					+ ") is out of range(0-"
					+ Strings::itoa(this->list.size()) + ")");
		this->remove(*(this->list.begin() + index), del);
	}

	/**toString ke _value objectha ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...), dar string zakhire mikonad.taabe toString har yek az object haye list
	 * ra seda mizanad va khorooji aan ha ra dar string zakhire mikonad.
	 *
	 * @param type SerializationType(json,xml,....)
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type = SerializationType::JSON) {
		stringstream str;
		switch (type) {
			case JSON:
				str << "[";
				for (iterator i = this->list.begin(); i != this->list.end(); i++) {
					str << (**i).toString(JSON);
					if (i + 1 != this->list.end())
						str << ",";
				}
				str << ']';
				break;
			case XML:
				str << '<' << this->getName() << '>';
				for (iterator i = this->list.begin(); i != this->list.end(); i++)
					str << "<item>" << (**i).toString(XML) << "</item>";
				str << "</" << this->getName() << ">";
				break;
			case RAW:
				for (iterator i = this->list.begin(); i != this->list.end(); i++) {
					str << (**i).toString(RAW);
					if (i + 1 != this->list.end())
						str << ",";
				}
				break;
			default:
				throw new invalidParameter(
						"You've provided invalid type for string serialization type parameter.");
		}
		return str.str();
	}

	/**fromstring baraye khandan dade haye objecthaa az stringi k daryaft mikonad.
	 *
	 * string ra parse mikonad va daade ha ra az aan mikhanad va dar _value objectha zakhire mikond.dar ghesmate
	 * json string RAW ra joda mikonad va be taabe pareseRawstring miferestad.
	 *
	 * @param data stringi k bayad az aan dade bekhanad
	 * @param type SerializationType(json,xml,...)
	 *
	 */
	void fromString(string data, SerializationType type) {
		size_t pos, rePos;
		List tempList;
		switch (type) {
			case JSON:
				pos = data.find('[');
				rePos = data.rfind(']');
				if (pos == string::npos || rePos == string::npos)
					EXTcantParseString("can't find brackets");
				parseRawString(data.substr(pos + 1, rePos - pos - 1), RAW,
				               tempList);
				break;
			case RAW:
				parseRawString(data, RAW, tempList);
				break;
			case XML:
				EXTnotImplemented("XML not implemented yet");
			default:
				EXTinvalidParameter(
						"You've provided invalid type for string serialization type parameter");
		}
		clear();
		for (iterator i = tempList.begin(); i != tempList.end(); i++) {
			add(*i);
			addAllocated(*i);
		}
	}

	/**overload operator = ke type string voroodi ra peida mikonad va be taave fromString midahad ta aan ra parse
	 * konad va data ra dar objecthaa gharar dahad.
	 *
	 * @param str stringi ke dar aan dataha zakhire shode ast.
	 *
	 * @return khode object bargardande mishavad.
	 *
	 */
	DTBase &operator=(string str) {
		str = Strings::trim(str);
		SerializationType type;
		switch (str[0]) {
			case '[':
				type = JSON;
				break;
			case '<':
				type = XML;
				break;
			default:
				type = RAW;
				break;
		}
		fromString(str, type);
		return *this;
	}

	/**overload operator =
	 *
	 * ebteda dtvar ra cast mikonim ta bbinim dtvar az noe DTSet ba template T hast ya na. vagar bood, list ra
	 * clear mikonim ta khali shavad sepas liste objecte dtvar ra dar list khodash zakhire mikonad.
	 *
	 * @dtvar objecte voroodi
	 *
	 * @return khode object
	 *
	 */
	DTBase &operator=(DTBase &dtvar) {
		DTSet<T> *temp;
		temp = dynamic_cast<DTSet<T> *>(&dtvar);
		if (temp) {
			this->clear();
			this->list = temp->list;
			return *this;
		} else {
			EXTdataTypeMismatch(
					"Can't cast " + dtvar.getNameAndType() + " to type DTSet<"
					+ T::getNameAndType() << ">.");
		}
	}

	/**overload operator ==
	 *
	 * object haaye dtvar ra ba objecthaaye this check mikonad agar tamam e object ha baham barabar boodand,
	 * true va dar gheir insoorat false barmigardanad.
	 *
	 * @param dtvar objecte voroodi
	 *
	 * @return tru or false.
	 *
	 */
	bool operator==(DTBase &dtvar) {
		DTSet<T> *temp;
		temp = dynamic_cast<DTSet<T> *>(&dtvar);
		if (temp) {
			if (this->length() != temp->length())
				return false;
			try {
				for (int i = 0; i < this->length(); i++) {
					if ((*this)[i] != (*temp)[i])
						return false;
				}
			} catch (cantParseString *ex) {
				return false;
			} catch (namesMismatch *ex) {
				return false;
			} catch (dataTypeMismatch *ex) {
				return false;
			}
			return true;
		} else
			return false;
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 * @param objecte voroodi
	 *
	 * @return true or false
	 *
	 */
	bool operator!=(DTBase &dtvar) {
		return !(*this == dtvar);
	}

	/**
	 * getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 */
	static string getTypeName() {
		return "DTSet";
	}

private:

	/**parseString()
	 *
	 *stringi ke migirad ra parse mikonad va datahayi ke dar string vojood darad ra az aan joda mikonad va dar yek
	 *stringi mobject temp zakhire mikonad va sepas temp ra be akhare listi ke migirad ezafe mikonad.
	 *
	 *@param str stringi ke bayad parse shavad.
	 *@param type nype string(josn,xml,...)
	 *@param list lsiti ke bayad object haayi ke data heye parse shode dar anha gharar darand ra zakhire konad.
	 *
	 */
	void parseRawString(string str, SerializationType type, List &list) {
		string parsed;
		size_t q = 0, j = 0;
		bool ready = false;
		for (size_t i = 0; i < str.size(); i++) {
			if (str[i] == '"') {
				j = JSONUtility::getEndOfString(str, i);
			} else if (str[i] == '{') {
				j = JSONUtility::getEndOfStruct(str, i);
			} else if (str[i] == '[') {
				j = JSONUtility::getEndOfArray(str, i);
			} else if (str[i] == ',') {
				parsed = str.substr(q, i - q);
				ready = true;
				q = i + 1;
			}
			if (!ready) {
				if (j == string::npos)
					EXTcantParseString(
							"syntax error at column" + Strings::itoa(i)
							+ " : can't find end of region");
				else
					i = j + 1;
				j = 0;
			}
			if (i >= str.size()) {
				parsed = str.substr(q);
				ready = true;
			}
			if (ready) {
				T *temp = new T("");
				try {
					temp->fromString(parsed, type);
				} catch (exceptionEx *ex) {
					EXTcantParseStringI(
							"Failed to parse string:\"" + parsed + "\" as '"
							+ temp->getNameAndType() + "'", ex);
				}
				list.push_back(temp);
				ready = false;
			}
		}
	}
};

/**class DTStruct
 *
 * class DTStruct baraye piadesaazi type moteghaeir az noe structure ast
 * structure mitavand az har noei bashad masaln az noe DTSet ya DTInteger bashad, yani dar listi ke dar
 * DTMultiType gharar darad object haayi ba type haye mokhtalef mitavan zakhire kard .in class az class DTMultiType
 * ers mibarad.
 *
 */

class DTStruct : public DTMultiFieldType<DTBase> {
public:
	typedef typename vector<DTBase *>::iterator iterator;
	typedef vector<DTBase *> List;

	/**constructor baraye DTStruct.
	 *
	 *@param name naame moteghaeir ke be constructor DTMultiType miferstd.
	 *
	 */
	DTStruct(string name) :
			DTMultiFieldType<DTBase>(name) {
	}

	/**getVersion() versione DTStruct ra barmigardand.
	 *
	 * @return version DTSruct.
	 *
	 */
	virtual size_t getVersion() = 0;

	/**getMinSupportedVersion() minimom versione DTStruct ra barmigardand.
	 *
	 * @return minimom version DTSruct.
	 *
	 */
	virtual size_t getMinSupportedVersion() = 0;

	/**
	 * getMinSupportedVersionWOConversation() kamterin versioni ke lazem ast taa tavabe seda zade shavand ra
	 * barmigardanad
	 */
	virtual size_t getMinSupportedVersionWOConversation() = 0;

	/**remove() yek naam daryaft mikonad va dar list object ba in naam ra peida mikonad va aan ra az list hazf mikonad.
	 *
	 * @param name esme objecti ke bayad paak shavad.
	 *
	 */
	void remove(string name) {
		for (iterator i = this->list.begin(); i != list.end(); i++) {
			if (!Strings::compare((**i).getName(), name, false)) {
				DTMultiFieldType<DTBase>::remove(*i);
				break;
			}
		}
	}

	/**contains() yek naam daryaft mikonad va dar list object ba in naam ra peida mikonad.
	 * agar peida kard true va age nakard false barmigardanad.
	 *
	 * @param name esme objecti ke bayad peida konad.
	 *
	 */
	bool contains(string name) {
		for (iterator i = this->list.begin(); i != list.end(); i++)
			if (!Strings::compare((**i).getName(), name, false))
				return true;
		return false;
	}

	/**toString ke _value objectha ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...), dar string zakhire mikonad.taabe toString har yek az object haye list
	 * ra seda mizanad va khorooji aan ha ra dar string zakhire mikonad.
	 *
	 * @param type SerializationType(json,xml,....)
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 */
	string toString(SerializationType type = SerializationType::JSON) {
		return this->toString(false, type);
	}

	string toString(bool includeVersion = false, SerializationType type = SerializationType::JSON) {
		stringstream str;
		switch (type) {
			case JSON:
				str << "{";
				if (includeVersion)
					str << "\"_version\":" << this->getVersion() << ",";
				for (iterator i = this->list.begin(); i != this->list.end(); i++) {
					str << '"' << (*i)->getName() << "\":" << (*i)->toString(JSON);
					if (i + 1 != list.end())
						str << ',';
				}
				str << '}';
				break;
			case XML:
				str << '<' << this->getName() << '>';
				if (includeVersion)
					str << "<_version>" << this->getVersion() << "</_version>";
				for (iterator i = this->list.begin(); i != this->list.end(); i++) {
					str << '<' << (*i)->getName() << '>' << (*i)->toString(XML)
					<< "</" << (*i)->getName() << '>';
					if (i + 1 != list.end())
						str << ',';
				}
				str << "</" << this->getName() << '>';
				break;
			case RAW:
				str << this->getVersion() << ",";
				for (iterator i = this->list.begin(); i != this->list.end(); i++) {
					str << (**i).toString(RAW);
					if (i + 1 != this->list.end())
						str << ",";
				}
				break;
			default:
				throw new invalidParameter(
						"You've provided invalid type for string serialization type parameter.");
		}
		return str.str();
	}

	/**fromstring baraye khandan dade haye objecthaa az stringi k daryaft mikonad.
	 *
	 * string ra parse mikonad va daade ha ra az aan mikhanad va dar _value objectha zakhire mikond.dar ghesmate
	 * json va RAW string ra be taabe pareseRawstring va pareseJsonstring miferestad va aan tabe parse mikonad va
	 * dar list gharar midahad.
	 *
	 * @param data stringi k bayad az aan dade bekhanad
	 * @param type SerializationType(json,xml,...)
	 *
	 */
	void fromString(string data, SerializationType type) {
		string backup = toString(RAW);
		exceptionEx *ex = NULL;
		size_t ver;
		switch (type) {
			case JSON:
			case RAW:
				try {
					if (type == RAW)
						ver = parseRawString(data);
					else
						ver = parseJSONString(data);
				} catch (exception &excp) {
					ex = EXcantParseStringI("Failed.",
					                        EXunknownExceptionI("", excp));
				} catch (exceptionEx *excp) {
					ex = EXcantParseStringI("Failed.", excp);
				} catch (...) {
					ex = EXcantParseString("Unknown error occurred.");
				}
				if (ver < getMinSupportedVersion())
					ex =
							EXobsolete(
									"This version (" + Strings::itoa(ver) + ") of '"
									+ this->getNameAndType()
									+ ") struct, is not supported anymore. Minimum supported version : "
									+ Strings::itoa(
											this->getMinSupportedVersion()));
				break;
			case XML:
				throw new notImplemented("XML not implemented yet.");
			default:
				throw new invalidParameter(
						"You've provided invalid type for string serialization type parameter.");
		}
		if (ex != NULL) {
			fromString(backup, RAW);
			throw ex;
		}
	}

	/**overlaod operator []
	 *
	 * yek naam daryaft mikonad va object ba in naam ra peida mikonad va bermigardand.
	 *
	 * @param name naame objecti ke bayad bargardanad.
	 *
	 * @retrun objecti ke ke peida mikonad.
	 *
	 */
	DTBase *operator[](string name) {
		for (iterator i = this->list.begin(); i != list.end(); i++) {
			if (!Strings::compare((**i).getName(), name, false)) {
				return *i;
			}
		}
		return NULL;
	}

	/**overload operator =
	 *
	 * ebteda type stringi ke migirad ra check mikonad(json,xml,...) sepas string hamraah ba typash ra be taabe
	 * fromString midahad ta dar anjaa parse konad va mosavi objecthaayash gharar dahad.
	 *
	 * @param str stringi ke data dar aan zakhire shode ast.
	 *
	 * @return khode objecte this.
	 *
	 */
	DTBase &operator=(string str) {
		str = Strings::trim(str);
		SerializationType type;
		switch (str[0]) {
			case '{':
				type = JSON;
				break;
			case '<':
				type = XML;
				break;
			default:
				type = RAW;
				break;
		}
		fromString(str, type);
		return *this;
	}

	/**overload operator =
	 *
	 * ebteda dtvar ra cast mikonad ta moshakhas shavad az noe DTStruct ast ya na. agar cast be dorosti anjam shavad,
	 * objecthaaye dtvar ba objecthaayi this moghayese mishavand va dar objecthaye this data e objecte hamnameshan dar
	 * dtvar gharar migirad.
	 *
	 * @param dtvr objecte voroodi
	 *
	 * @return this.
	 *
	 */
	DTBase &operator=(DTBase &dtvar) {
		DTStruct *temp;
		string backup = this->toString(RAW);
		temp = dynamic_cast<DTStruct *>(&dtvar);
		if (temp) {
			if (temp->getVersion() < this->getMinSupportedVersion())
				EXTobsolete(
						"This version (" + Strings::itoa(temp->getVersion())
						+ ") of '" + this->getNameAndType()
						+ "' struct, is not supported anymore. Minimum supported version : "
						+ Strings::itoa(
								this->getMinSupportedVersion()));
			for (iterator i = temp->list.begin(); i != temp->list.end(); i++)
				for (iterator j = this->list.begin(); j != this->list.end();
				     j++)
					if (!Strings::compare((*i)->getName(), (*j)->getName())) {
						exceptionEx *ex = NULL;
						try {
							(*j) = (*i);
						} catch (exception &excp) {
							ex = EXdataTypeMismatchI(
									"Assignment between '"
									+ (*i)->getNameAndType() + "' and '"
									+ (*i)->getNameAndType()
									+ "' failed",
									EXunknownExceptionI("", excp));
						} catch (exceptionEx *excp) {
							ex = EXdataTypeMismatchI(
									"Assignment between '"
									+ (*i)->getNameAndType() + "' and '"
									+ (*i)->getNameAndType()
									+ "' failed", excp);
						} catch (...) {
							ex =
									EXdataTypeMismatch(
											"Assignment between '"
											+ (*i)->getNameAndType()
											+ "' and '"
											+ (*i)->getNameAndType()
											+ "' failed. Unknown error occurred.");
						}
						if (ex) {
							fromString(backup, RAW);
							throw ex;
						}
					}
			return *this;
		} else {
			EXTdataTypeMismatch(
					"Can't cast '" + dtvar.getNameAndType()
					+ "' to type DTStruct.");
		}
	}

	/**overload opertor ==
	 *
	 * ebteda dtvar ra cast mikonad agar cast anjaam shod barrasi mikonad ke tamam e objecthaaye temp,
	 * hamnaameshan ra dar this peida konand, agar peida kardand barresi mikonad ke data anha ba ham barabar
	 * bashad ta true ya false bargardanad va agar peida nakard false barmigardanad. yani dar 1 halat true
	 * barmigardanad ke tamame objecthaa yek object hamnaam dashte bashand va meghdar anaa ba ham barabar nashad.
	 *
	 * @param dtvar object voroodi
	 *
	 * @return true or false.
	 *
	 */
	bool operator==(DTBase &dtvar) {
		DTStruct *temp;
		temp = dynamic_cast<DTStruct *>(&dtvar);
		if (temp) {
			for (iterator i = temp->list.begin(); i != temp->list.end(); i++) {
				bool found = false;
				for (iterator j = this->list.begin(); j != this->list.end();
				     j++)
					if (!Strings::compare((*i)->getName(), (*j)->getName())) {
						try {
							if ((**i) != (**j))
								return false;
						} catch (...) {
							return false;
						}
						found = true;
						break;
					}
				if (!found)
					return false;
			}
			return true;
		} else
			return false;
	}

	/**overload operator !=
	 *
	 * mokhalefe operator == ra barmigardanad.
	 *
	 */
	bool operator!=(DTBase &dtvar) {
		return !(*this == dtvar);
	}

	/**
	 * getTypename naame kellasi ke dar aan gharar darim ra barmigardand.az in taabe dar hengame excption ha estefade
	 * mishavad ta vaghti excption rokh dad esm e kellasi ke dar aan excption rokh dade ast dar esception.what
	 * zakhire mishavad va sepas namayesh dade mishavad taa marahel debug kardn va eibyaabi barnaame rahat tar bashad.
	 *
	 *@return naame kellas.
	 */
	static string getTypeName() {
		return "DTStruct";
	}

private:

	/**parseRawString()
	 *
	 * dar in taabe stringi ke migirad ra parse mikonad va data haayi ra ke be dast miavarad ra be
	 * fromString objecthaa midahad ta aan data ha ra dar khod zakhire konand.
	 *
	 * @param str stringi ke data dar khod zakhire shode ast.
	 *
	 */
	size_t parseRawString(string str) {
		string parsed;
		size_t q = 0, cc = 0, ver = 0, j = 0;
		bool ready = false;
		for (size_t i = 0; i < str.size(); i++) {
			if (str[i] == '"') {
				j = JSONUtility::getEndOfString(str, i);
			} else if (str[i] == '{') {
				j = JSONUtility::getEndOfStruct(str, i);
			} else if (str[i] == '[') {
				j = JSONUtility::getEndOfArray(str, i);
			} else if (str[i] == ',') {
				parsed = str.substr(q, i - q);
				ready = true;
				q = i + 1;
			}
			if (!ready) {
				if (j == string::npos)
					EXTcantParseString(
							"syntax error at column" + Strings::itoa(i)
							+ " : can't find end of region");
				else
					i = j + 1;
				j = 0;
			}
			if (i >= str.size()) {
				parsed = str.substr(q);
				ready = true;
			}
			if (ready) {
				if (cc == 0) //version
					ver = stoi(parsed);
				else {
					try {
						this->list[cc - 1]->fromString(parsed, RAW);
					} catch (exception &excp) {
						EXTcantParseStringI(
								"Parsing string '" + parsed + "' as '"
								+ this->list[cc - 1]->getNameAndType()
								+ "' failed",
								EXunknownExceptionI("", excp));
					} catch (exceptionEx *excp) {
						EXTcantParseStringI(
								"Parsing string '" + parsed + "' as '"
								+ this->list[cc - 1]->getNameAndType()
								+ "' failed", excp);
					} catch (...) {
						EXTcantParseString(
								"Parsing string '" + parsed + "' as '"
								+ this->list[cc - 1]->getNameAndType()
								+ "' failed");
					}
					cc++;
					ready = false;
				}
			}
		}
		return ver;
	}

	/**parseRawString()
	 *
	 * dar in taabe stringi ke migirad ra parse mikonad va data haayi ra ke be dast miavarad ra be
	 * fromString objecthaa midahad ta aan data ha ra dar khod zakhire konand.
	 *
	 * @param str stringi ke data dar khod zakhire shode ast.
	 *
	 */
	size_t parseJSONString(string str) {
		size_t pos, rePos, ver = 0, j = 0;
		pos = str.find('{');
		rePos = str.rfind('}');
		if (pos == string::npos || rePos == string::npos)
			throw new cantParseString("can't find brackets.");
		str = str.substr(pos + 1, rePos - pos - 1);
		string parsed, name, value;
		int q = 0;
		bool check = true, ready = false;
		for (size_t i = 0; i < str.size(); i++) {
			if (str[i] == '"') {
				if (check) {
					q = i;
					check = false;
				}
				j = JSONUtility::getEndOfString(str, i);
			} else if (str[i] == '{') {
				j = JSONUtility::getEndOfStruct(str, i);
			} else if (str[i] == '[') {
				j = JSONUtility::getEndOfArray(str, i);
			} else if (str[i] == ',') {
				parsed = str.substr(q, i - q);
				ready = true;
				check = true;
				q = i + 1;
			}
			if (!ready) {
				if (j == string::npos)
					EXTcantParseString(
							"syntax error at column" + Strings::itoa(i)
							+ " : can't find end of region");
				else
					i = j + 1;
				j = 0;
			}
			if (i >= str.size()) {
				parsed = str.substr(q);
				ready = true;
			}
			if (ready) {
				size_t x = 0, y = 0;
				y = parsed.find('"', x + 1);
				if (y == string::npos)
					EXTcantParseString(
							"Can't find character '\"' for name part of JSON field in '"
							+ parsed + "'");
				name = parsed.substr(1, y - 1);
				x = parsed.find(':', y + 1);
				if (x == string::npos)
					EXTcantParseString(
							"Can't find character ':' for JSON field in '"
							+ parsed + "'");
				value = parsed.substr(x + 1);
				if (!Strings::compare(name, "_version", false))
					ver = stoi(value);
				else if ((*this).contains(name)) {
					try {
						(*this)[name]->fromString(value, JSON);
					} catch (exception &excp) {
						EXTcantParseStringI(
								"Parsing string '" + value + "' as '"
								+ (*this)[name]->getNameAndType()
								+ "' failed",
								EXunknownExceptionI("", excp));
					} catch (exceptionEx *excp) {
						EXTcantParseStringI(
								"Parsing string '" + value + "' as '"
								+ (*this)[name]->getNameAndType()
								+ "' failed", excp);
					} catch (...) {
						EXTcantParseString(
								"Parsing string '" + value + "' as '"
								+ (*this)[name]->getNameAndType()
								+ "' failed");
					}
				}
				ready = false;
			}
		}
		return ver;
	}
};

}
}
#endif // DTMULTIFIELDTYPES_H
