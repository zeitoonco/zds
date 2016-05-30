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
protected:
	typedef typename vector<childT *>::iterator iterator;
	typedef typename vector<childT *>::const_iterator citerator;
	typedef vector<childT *> List;

	List list;
	List allocatedOnes;

	/**isAllocated() object ra migirad va search mikonad ke dar vector allocatedOnes ast ya na.
	 *
	 * @param item objecti ke bayad search shavad.
	 *
	 * @return agar dar vector bood true va agar nabood false barmigardanad.
	 *
	 */
	bool isAllocated(childT *item) {
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
			delete (*i);
		allocatedOnes.clear();
	}

	/**tryRemoveAllocated() yek object khaas ra ham az list DTMultiType hazf mikonad ham az allocatedOnes.
	 *
	 * @param d objecti ke bayad kollan hazf shavad.
	 *
	 * @return agar amaliat e hazf anjaam shod true va agar object ra peida nakard false barmigardanad.
	 *
	 */
	bool tryRemoveAllocated(childT *item) {
		for (iterator i = allocatedOnes.begin(); i != allocatedOnes.end(); i++)
			if (*i == item) {
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
	void addAllocated(childT *item) {
		allocatedOnes.push_back(item);
	}

public:
	/**constructor e DTMultiFieldType
	 *
	 * @param name name object ya haman moteghaeir ra moshakhas mikonad.
	 *
	 */
	DTMultiFieldType(string name) :
			DTBase(name) {
	}

	virtual ~DTMultiFieldType() {
		clear();
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

	/**add() objecte jadid be list DTMultiType ezafe mikonad.
 *
 * @param val objecti ke bayad ezaafe shavad.
 *
 */
	virtual void add(childT *val, bool unManagedMemory) {
		add(val);
		if (unManagedMemory)
			addAllocated(val);
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
	virtual void removeAt(int index) {
		if (index < 0 || index >= this->list.size())
			EXToutOfRange(
					"Provided index(" + Strings::itoa(index)
					+ ") is out of range(0-"
					+ Strings::itoa(this->list.size()) + ")");
		this->remove(*(this->list.begin() + index));
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
	virtual void remove(childT *val) {
		if (tryRemoveAllocated(val))
			delete val;
		for (iterator i = list.begin(); i != list.end(); i++)
			if (*i == val) {
				list.erase(i);
				break;
			}
	}

	/**clear list ra khali mikonad va tamame objecthaye list ra az list hazf mikonad.
	 *
	 * @param del boolian baraye moshakhas kardan delete kaamele object az memory ya na.
	 *
	 */
	virtual void clear() {
		clearAllocatedOnes();
		list.clear();
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
	typedef vector<T *> List;
	typedef typename vector<T *>::iterator iterator;
	typedef typename vector<T *>::const_iterator citerator;
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
	}

	/**addJson() dade ie ke be soorate string vared mishavad ra dar list zakhire mikonad.
*
* dar inja string vared mishavad ke dar aan daade be soorat RAW zakhire shode ast. taabe yek object misazad
* va meghdare daade ra dar aan zakhire mikonad va sepas object ra be list DTMultiType va hamchin
* allocatedOnes ezafe mikonad.
*
* @param stringi ke daade be soorat RAW dar an zakhire shode ast.
*
*/
	virtual void addJson(string str) {
		T *temp = new T("");
		temp->fromString(str);
		this->add(temp, true);
	}

	virtual void addRange(std::vector<T> iVector) {
		for (size_t iter = 0; iter < iVector.size(); iter++) {
			T *temp = new T("");
			(*temp) = iVector[iter];
			this->add(temp, true);
		}
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

	/**toString ke _value objectha ra dar yek string zakhire mikond.
	 *
	 * ba tavajoh be type(json,xml,...), dar string zakhire mikonad.taabe toString har yek az object haye list
	 * ra seda mizanad va khorooji aan ha ra dar string zakhire mikonad.
	 *
	 * @param type SerializationType(json,xml,....)
	 *
	 * @return stringi k dar aan _value zakhire mishavad.
	 *
	 *///todo:: Mr, Bakhshi: cmake dependencies to be automaticaly installed when compiled
	string toString(int indent = -1,
	                std::string indentContent = defaultIndentContent
	) const {//TODO: args changed--CHECK TOSTRING for bugs
		std::string newLine = "";
		if (indent != -1) {
			indent++;
			newLine = "\n";
		}
		stringstream str;
		str << "[";
		for (citerator i = this->list.begin(); i != this->list.end(); i++) {
			str << newLine << (**i).toString(indent, indentContent);
			if (i + 1 != this->list.end())
				str << ",";
		}
		str << newLine << Strings::indentMaker((indent - 1), indentContent) << ']';
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
	void fromString(string data) {
		size_t pos, rePos;
		List tempList;

		pos = data.find('[');
		rePos = data.rfind(']');
		if (pos == string::npos || rePos == string::npos)
			EXTcantParseString("can't find brackets");
		parseRawString(data.substr(pos + 1, rePos - pos - 1), tempList);
		DTMultiFieldType<T>::clear();
		for (iterator i = tempList.begin(); i != tempList.end(); i++) {
			this->add(*i, true);
		}
	}

	/**overload operator [] baraye bargardandan e object ba tavajoh be shomare aan dar list.
 *
 * @param index shomare ye object
 *
 * @return object ba shomare moshakhas dar list.
 *
 */
	virtual T *operator[](int index) {
		return this->list[index];
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
		fromString(str);
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
	void parseRawString(string str, List &list) {
		string parsed;
		size_t q = 0, j = string::npos - 1;
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
				else if (j != string::npos - 1) {
					i = j;
					j = string::npos - 1;
				}
			}
			if (i >= str.size() - 1) {
				parsed = str.substr(q);
				ready = true;
			}
			if (ready) {
				T *temp = new T("");
				try {
					temp->fromString(parsed);
				} catch (exceptionEx &ex) {
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
protected:
	size_t version, minVersion, minVersionWOC;

	void setVersionValues(size_t iversion, size_t iminVersion, size_t iminVersionWOC) {
		version = iversion;
		minVersion = iminVersion;
		minVersionWOC = -iminVersionWOC;
	}

public:
	typedef typename vector<DTBase *>::iterator iterator;
	typedef typename vector<DTBase *>::const_iterator citerator;
	typedef vector<DTBase *> List;

	/**constructor baraye DTStruct.
	 *
	 *@param name naame moteghaeir ke be constructor DTMultiType miferstd.
	 *
	 */
	DTStruct(string name, size_t iversion, size_t iminVersion, size_t iminVersionWOC) :
			DTMultiFieldType<DTBase>(name) {
		setVersionValues(iversion, iminVersion, iminVersionWOC);
	}

	/**getVersion() versione DTStruct ra barmigardand.
	 *
	 * @return version DTSruct.
	 *
	 */
	virtual size_t getVersion() const { return this->version; };

	/**getMinSupportedVersion() minimom versione DTStruct ra barmigardand.
	 *
	 * @return minimom version DTSruct.
	 *
	 */
	virtual size_t getMinSupportedVersion() { return this->minVersion; };

	/**
	 * getMinSupportedVersionWOConversation() kamterin versioni ke lazem ast taa tavabe seda zade shavand ra
	 * barmigardanad
	 */
	virtual size_t getMinSupportedVersionWOConversation() { return this->minVersionWOC; };

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
	string toString(int indent = -1, std::string indentContent = defaultIndentContent) const {
		return toString(false, indent, indentContent);
	}


	string toString(bool includeVersion, int indent = -1,
	                std::string indentContent = defaultIndentContent) const {//TODO: args changed--CHECK TOSTRING for bugs
		stringstream str;
		std::string newLine = "";
		if (indent < 0) {
			str << Strings::indentMaker(indent, indentContent) << "{";

		} else {
			newLine = "\n";
			str << Strings::indentMaker(indent++, indentContent) << "{";
		}
		if (includeVersion)
			str << newLine << Strings::indentMaker(indent, indentContent) << "\"_version\":" <<
			this->getVersion() <<
			",";
		for (citerator i = this->list.begin(); i != this->list.end(); i++) {
			str << newLine << Strings::indentMaker(indent, indentContent) << '"' << (*i)->getName() << "\":" <<
			(*i)->toString(indent);
			if (i + 1 != list.end())
				str << ",";
		}
		str << newLine << Strings::indentMaker(--indent, indentContent) << '}';
		std::string terr = str.str();
		return str.str();
	}

	virtual void fromString(string data) {
		if (data.length() == 0)
			return;
		fromString(data, false);
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
	void fromString(string data, bool createIfNotExist) {
		string backup = toString();
		size_t ver;

		try {
			ver = parseJSONString(data, createIfNotExist);
		} catch (exceptionEx &excp) {
			fromString(backup);
			EXTcantParseStringI("Failed", excp);
		} catch (exception &excp) {
			fromString(backup);
			EXTcantParseString(std::string("Failed:") + excp.what());
		} catch (...) {
			fromString(backup);
			EXTcantParseString("Unknown error occurred.");
		}
		if (ver != string::npos && ver < getMinSupportedVersion()) {
			fromString(backup);
			EXTobsolete(
					"This version (" + Strings::itoa(ver) + ") of '" + this->getNameAndType()
					+ ") struct, is not supported anymore. Minimum supported version : "
					+ Strings::itoa(this->getMinSupportedVersion()));
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

	/**overload operator [] baraye bargardandan e object ba tavajoh be shomare aan dar list.
 *
 * @param index shomare ye object
 *
 * @return object ba shomare moshakhas dar list.
 *
 */
	virtual DTBase *operator[](int index) {
		return list[index];
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
		fromString(str);
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
		string backup = this->toString();
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
						try {
							(*j) = (*i);
						} catch (exceptionEx &excp) {
							fromString(backup);
							EXTdataTypeMismatchI("Assignment between '" + (*i)->getNameAndType() + "' and '" +
							                     (*i)->getNameAndType() + "' failed", excp);
						} catch (exception &excp) {
							fromString(backup);
							EXTdataTypeMismatch("Assignment between '" + (*i)->getNameAndType() + "' and '" +
							                    (*i)->getNameAndType() + "' failed: " + excp.what());
						} catch (...) {
							fromString(backup);
							EXTdataTypeMismatch("Assignment between '" + (*i)->getNameAndType() + "' and '" +
							                    (*i)->getNameAndType() + "' failed. Unknown error occurred.");
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
	size_t parseJSONString(string str, bool createIfNotExist = false) {
		size_t pos, rePos, ver = string::npos, j = string::npos - 1;
		pos = str.find('{');
		rePos = str.rfind('}');
		if ((pos == string::npos) || (rePos == string::npos)) {
			EXTcantParseString("can't find brackets.");
		}
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
			} //else { nothing!;			}
			if (!ready) {
				if (j == string::npos)
					EXTcantParseString(
							"syntax error at column" + Strings::itoa(i) + " : can't find end of region:\"" +
							str +
							"\"");
				else if (j != string::npos - 1) {
					i = j;
					j = string::npos - 1;
				}
			}
			if (i >= (str.size() - 1)) {
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
						(*this)[name]->fromString(value);
					} catch (exceptionEx &excp) {
						EXTcantParseStringI(
								"Parsing string '" + value + "' as '" + (*this)[name]->getNameAndType() + "' failed",
								excp);
					} catch (exception &excp) {
						EXTcantParseString(
								"Parsing string '" + value + "' as '" + (*this)[name]->getNameAndType() + "' failed: " +
								excp.what());
					} catch (...) {
						EXTcantParseString(
								"Parsing string '" + value + "' as '" + (*this)[name]->getNameAndType() + "' failed");
					}
				} else if (createIfNotExist) {
					DTString *temp = new DTString(name, "");
					temp->fromString(value);
					this->add(temp, true);
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
