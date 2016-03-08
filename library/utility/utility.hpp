/*
 * utility
 *
 *  Created on: Jul 29, 2015
 *      Author: ajl
 */

#ifndef UTILITY_
#define UTILITY_

#include <string>
#include <string.h>
#include <sstream>
#include <unistd.h>

using namespace std;

typedef unsigned int uint;

namespace zeitoon {
namespace utility {

class JSONUtility {
protected:
	const static int specialCharsLen = 6;
	const static char specialChars[specialCharsLen][2];

public:

	/**getEndOfString()
	 *
	 * stringi ke migirad, '"' aval e string ra rad mikonad va makan e '"' badi ke neshan dahane ye payane
	 * string ast ra barmigardanad.
	 *
	 * @param str string voroodi
	 * @param pos makaani ke bayad az anjaa be bad donable '"' begardad.
	 *
	 * @return makaane '"' bad az pos.
	 *
	 */
	static size_t getEndOfString(string str, int pos) {
		int newPos = 1;
		while (newPos > 0) {
			newPos = str.find('"', pos + 1);
			if (newPos > 0) {
				int count = 0;
				for (int i = newPos - 1; str[i] == '\\'; i--) {
					if (str[i] == '\\')
						count++;
				}
				if (count % 2 == 1) {
					pos = newPos;
					continue;
				} else
					return newPos;
			}
		}
		return string::npos;
	}

	/**getEndOfString()
	 *
	 * stringi ke migirad, '"' aval e string ra rad mikonad va makan e '"' badi ke neshan dahane ye payane
	 * string ast ra barmigardanad.
	 *
	 * @param str string voroodi
	 * @param pos makaani ke bayad az anjaa be bad donable '"' begardad.
	 *
	 * @return makaane '"' bad az pos.
	 *
	 */
	static size_t getEndOfStruct(string str, int pos) {
		for (uint i = pos + 1; i < str.size(); i++) {
			if (str[i] == '"')
				i = getEndOfString(str, i);
			else if (str[i] == '{')
				i = getEndOfStruct(str, i);
			else if (str[i] == '[')
				i = getEndOfArray(str, i);
			else if (str[i] == '}')
				return i;
		}
		return string::npos;
	}

	/**getEndOfArray()
	 *
	 * in taabe baraye in ast ta yek string bgirad ke dar aan araye zakhire shode ast va makane payane arraye ra
	 * ba migardaanad. string ra migirand ke dar aan data zakhire shode ast va pos ke moshakhas mikonad az che
	 * makani dar string be baad bayad donbale payane arraye begardad.
	 *
	 * @param str stringi ke dar aan data(mesle arraye) zakhire shode ast.
	 * @param pos makani ke bayad bad az aan donbale payane arraye begardad.
	 *
	 * @return makani az string ke arraye dar anjaa tamam shode ast.
	 *
	 */
	static size_t getEndOfArray(string str, int pos) {
		for (uint i = pos + 1; i < str.size(); i++) {
			if (str[i] == '"')
				i = getEndOfString(str, i);
			else if (str[i] == '{')
				i = getEndOfStruct(str, i);
			else if (str[i] == '[')
				i = getEndOfArray(str, i);
			else if (str[i] == ']')
				return i;
		}
		return string::npos;
	}

	/**encodeString()
	 *
	 * in taabe baraye in tarahi shode ast ke stringha be soorate sahih zakhire shavand. baraye inkar caracterhaye
	 * khas ra barresi mikonad yani mesle '\t' ya '\n' ke nahveye zakhire aanha fargh mikonad. bayad dar hengame
	 * zakhire caractri mesle '\n' string "\n" zakhire shavad ta moshakhas shavad in caracter '\n' ast.
	 * in encod fghat baraye zakhire data dar string ast va baraye estefade az data bayad aan ra decode kard.
	 *
	 * @param str stringi ke bayad dar string digar zakhire shavad.
	 *
	 * @return stringi ke encode karde va amaade zakhire ast.
	 *
	 */
	static string encodeString(string str) {
		for (uint i = 0; i < str.size(); i++)
			for (uint j = 0; j < specialCharsLen; j++)
				if (str[i] == specialChars[j][0]) {
					string ns("\\ ");
					ns[1] = specialChars[j][1];
					str.replace(i, 1, ns);
					i++;
				}
		return str;
	}

	/**decodeString()
	 *
	 * in taabe kari baraxe encodeString anjam midahad. yaani dar string be donbale stringi manande "\n" migardad
	 * va be jaaye an caracter '\n' migozarad ta hengame parse kardan, data be tore monaseb parse shavad.
	 *
	 * @param str stringi ke bayad decode havad.
	 *
	 * @return stringi ke decode shode ast.
	 *
	 */
	static string decodeString(string str) {
		for (uint i = 0; i < str.size(); i++)
			if (str[i] == '\\') {
				for (uint j = 0; j < specialCharsLen; j++)
					if (str[i + 1] == specialChars[j][1])
						str.replace(i, 2, 1, specialChars[j][0]);
			}
		return str;
	}
};

/**class string
 *
 * in class baraye modiriate string ha va tavabe ie ke baraye taghir rooye string hastnad neveshte shode aast.
 */
class Strings {
protected:
	static const char *defIgnrChrs;
	static const char *xBaseDigit;

public:

	/**compare() do string ra migirad va check mikonad ba ham barabar and ya na.
	 *
	 * agar do string baham barabar bashand 0 va dar gheir insoorat adadi gheir sefr barmigardanad.
	 * agar caseSensetive true bood dar hengame moghaeyese be horoofe bozorg va koochak ham ahamiat midahad va
	 * agar false bashad nmidahad.
	 *
	 * @param a stringe aval
	 * @param b stringe dovom
	 * @param caseSensetive tasire bozorg ya koochak boodan horoof dar moghayese ra neshan midahad.
	 *
	 *@return integer
	 *
	 */
	static int compare(string a, string b, bool caseSensetive = true) {
		if (caseSensetive)
			return a.compare(b);
		else {
			return toLower(a).compare(toLower(b));
		}
	}

	/**toLower() tamame horoofe englisi e string ra koochak mikonad(agar bozorg bashad).
	 *
	 * @param a stringi ke bayad horoofe aan koochak shavad.
	 *
	 * @return stringi ke tamame horoofash koochak shode ast.
	 *
	 */
	static string toLower(string a) {
		string b(a);
		for (uint i = 0; i < b.length(); i++)
			b[i] = tolower(b[i]);
		return b;
	}

	/**cltrim() moteghaeire ignoreChars ra migirad va az avale string a be donbale aan migardad va har ja
	 * ke aan ra peida kard, yek substr az a ke az makane ignoreChars ta akhre a ast ra barmigardanad.
	 *
	 * @param a stringi ke bayad trim shavad.
	 * @param ignoreChars stringi(va harche ghabl az in string dar a boode) ke bayad az a hazf shavad.
	 *
	 * @return stringi ke trim shode ast.
	 *
	 */
	static string cltrim(string a, string ignoreChars = " ") {
		for (uint i = 0; i < a.length(); i++)
			if (ignoreChars.find(a[i]) == string::npos)
				return a.substr(i);
		return "";
	}

	/**crtrim() moteghaeire ignoreChars ra migirad va az akhare string a be donbale aan migardad va har ja
	 * ke aan ra peida kard, yek substr az a ke az makane ignoreChars ta avale a ast ra barmigardanad.
	 *
	 * @param a stringi ke bayad trim shavad.
	 * @param ignoreChars stringi(va harche bad az in string dar a boode) ke bayad az a hazf shavad.
	 *
	 * @return stringi ke trim shode ast.
	 *
	 */
	static string crtrim(string a, string ignoreChars = " ") {
		if (a.length() == 0)
			return "";
		for (uint i = a.length() - 1; i >= 0; i--)
			if (ignoreChars.find(a[i]) == string::npos)
				return a.substr(0, i + 1);
		return "";
	}

	/**ctrim() moteghaeire ignoreChars ra migirad va az do tarafe string a be donbale aan migardad va har ja
	 * ke aan ra peida kard, yek substr az a ke beine ignoreCharshaayei ke peida karde ra barmigardanad.
	 *
	 * @param a stringi ke bayad trim shavad.
	 * @param ignoreChars stringi(va harche ghabl va bad az in string dar a boode) ke bayad az a hazf shavad.
	 *
	 * @return stringi ke trim shode ast.
	 *
	 */
	static string ctrim(string a, string ignoreChars = " ") {
		return cltrim(crtrim(a, ignoreChars), ignoreChars);
	}

	/**ltrim()
	 * hamanande cltrim kaar mikonad vali ignoreChars dar inja meghDAR HAYE defalut hastnad ke tarif shode and(\r,\n,\t)
	 * va hamchenin ignoreChars iee ke az karbar migirad.
	 *
	 * @param a stringi ke bayad trim shavad.
	 * @param ignoreChars stringi(va harche ghabl az in string dar a boode) ke bayad az a hazf shavad.
	 *
	 * @return stringi ke trim shode ast.
	 *
	 */
	static string ltrim(string a, string ignoreChars = "") {
		return cltrim(a, defIgnrChrs + ignoreChars);
	}

	/**rtrim()
	 * hamanande crtrim kaar mikonad vali ignoreChars dar inja meghDAR HAYE defalut hastnad ke tarif shode and(\r,\n,\t)
	 * va hamchenin ignoreChars iee ke az karbar migirad.
	 *
	 * @param a stringi ke bayad trim shavad.
	 * @param ignoreChars stringi(va harche bad az in string dar a boode) ke bayad az a hazf shavad.
	 *
	 * @return stringi ke trim shode ast.
	 *
	 */
	static string rtrim(string a, string ignoreChars = "") {
		return crtrim(a, defIgnrChrs + ignoreChars);
	}

	/**trim()
	 * hamanande ctrim kaar mikonad vali ignoreChars dar inja meghDAR HAYE defalut hastnad ke tarif shode and(\r,\n,\t)
	 * va hamchenin ignoreChars iee ke az karbar migirad.
	 *
	 * @param a stringi ke bayad trim shavad.
	 * @param ignoreChars stringi(va harche bad az in string dar a boode) ke bayad az a hazf shavad.
	 *
	 * @return stringi ke trim shode ast.
	 *
	 */
	static string trim(string a, string ignoreChars = "") {
		return ctrim(a, defIgnrChrs + ignoreChars);
	}

	/**itoa() integer ra be string tabdil mikonad.
	 *
	 * @param val moteghaeire integeri ke bayad be string tabdil shavad.
	 *
	 * @return stringi ke dar aan integere voroodi zakhire shode ast.
	 *
	 */
	static string itoa(long long int val) {
		return toString(val);
	}

	static string itoa(long int val) {
		return toString(val);
	}

	static string itoa(int val) {
		return toString(val);
	}

	static string itoa(unsigned long long int val) {
		return toString(val);
	}

	static string itoa(unsigned long int val) {
		return toString(val);
	}

	static string itoa(unsigned int val) {
		return toString(val);
	}

	static string toString(int val, unsigned int base = 10) {
		return toString((long long int) val, base);
	}

	static string toString(long int val, unsigned int base = 10) {
		return toString((long long int) val, base);
	}

	static string toString(long long int val, unsigned int base = 10) {
		string n = toString((unsigned long long int) val, base);
		return (val < 0 ? "-" + n : n);
	}

	static string toString(unsigned int val, unsigned int base = 10) {
		return toString((unsigned long long int) val, base);
	}

	static string toString(unsigned long int val, unsigned int base = 10) {
		return toString((unsigned long long int) val, base);
	}

	static string toString(unsigned long long int val, unsigned int base = 10) {
		if (base == 10) {
			return to_string(val);
		} else {
			char num[65];
			memset(num, 0, 65);
			int i;
			for (i = 0; i < 64 && val >= base; i++) {
				num[i] = xBaseDigit[val % base];
				val = val / base;
			}
			num[i++] = xBaseDigit[val];
			char t;
			for (int j = 0; j < (i / 2); j++) {
				t = num[j];
				num[j] = num[i - j - 1];
				num[i - j - 1] = t;
			}
			return string(num);
		}
	}

	static string toString(long double val) {
		return to_string(val);
	}

	static string toString(double val) {
		return to_string(val);
	}

	static string toString(float val) {
		return to_string(val);
	}

	static string toString(bool val) {
		return string(val ? "true" : "false");
	}
};

class MessageTypes {
public:
	enum MessageTypes_ {
		MTFire, MTCall, MTCallback, __MAX
	};
	static const char *typeString[__MAX];

	static const char *toString(MessageTypes_ value) {
		return typeString[value];
	}
};

class CommunicationUtility {
public:
	static string getRandomID() {
		srand(time(NULL));
		return Strings::itoa(rand());
	}

	static string makeCommand(string node, string data) {
		return makeCommand(node, "", "", data);
	}

	static string makeCommand(string node, string id, string data) {
		return makeCommand(node, id, "", data);
	}

	static string makeCommand(string node, string id, string from, string data) {
		return ("{\"type\" : \"call\" , \"node\" : \"" + node + "\" " +
		        (id.length() > 0 ? ", \"id\" : \"" + id + "\" " : "") +
		        (from.length() > 0 ? ", \"from\" : \"" + from + "\" " : "") +
		        (data.length() > 0 ? ", \"data\" : " + data + " " : "") +
		        "}");
	}

	static string makeCallback(string node, string data) {
		return makeCallback(node, "", "", data);
	}

	static string makeCallback(string node, string id, string data) {
		return makeCallback(node, id, "", data);
	}

	static string makeCallback(string node, string id, string from, string data) {
		return ("{\"type\" : \"callback\" , \"node\" : \"" + node + "\" " +
		        (id.length() > 0 ? ", \"id\" : \"" + id + "\" " : "") +
		        (from.length() > 0 ? ", \"from\" : \"" + from + "\" " : "") +
		        (data.length() > 0 ? ", \"data\" : " + data + " " : "") +
		        "}");
	}

	static string makeEvent(string node, string from, string data) {
		return ("{\"type\" : \"fire\" , \"node\" : \"" + node + "\" " +
		        (from.length() > 0 ? ", \"from\" : \"" + from + "\" " : "") +
		        (data.length() > 0 ? ", \"data\" : " + data + " " : "") +
		        "}");
	}
};

class FileSystem {
public:
	static string getAppPath() {
		char buf[1000];
		ssize_t s = readlink("/proc/self/exe", buf, 1000);
		buf[s] = 0;
		std::string temp = buf;
		size_t lastSlash = temp.find_last_of("/");
		temp.erase(lastSlash + 1, std::string::npos);
		return temp;
	}
};

}
}

#endif /* UTILITY_ */
