#ifndef EXCEPTIONEX_HPP
#define EXCEPTIONEX_HPP

#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include "utility/utility.hpp"

using namespace std;

namespace zeitoon {
namespace utility {

class exceptionEx: public exception {
protected:
	exceptionEx *_innerException;
	string _message;
	string _title;
	string _owner;
	int _line;
	string _file;
public:
	exceptionEx(string imessage, string owner = "", string file = "", int line =
			-1, exceptionEx *iinnerException =
	NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	exceptionEx(string ititle, string imessage, string owner = "", string file =
			"", int line = -1, exceptionEx *iinnerException = NULL) {
		_title = ititle;
		_message = imessage;
		_innerException = iinnerException;
		_owner = owner;
		_line = line;
		_file = file;
	}
	virtual ~exceptionEx() throw(){
		delete _innerException;
	}

	virtual string getDefTitle() {
		return "Exception occurred";
	}
	virtual const char * what() const throw () {
		stringstream msg;
		msg << getTitle();
		if (_owner.length() > 0 || _file.length() > 0 || _line >= 0)
			msg << "(from " << (_owner.length() > 0 ? _owner : "")
					<< (_file.length() > 0 ? "file:" + _file : "")
					<< (_line >= 0 ?
							string("#") + utility::Strings::itoa(_line) : "")
					<< ")";
		msg << " : " << getMessage();
		if (getInnerException() != NULL)
			msg << endl << "Innerexception : " << getInnerException()->what();
		return msg.str().c_str();
	}
	virtual void setMessage(string msg) {
		_message = msg;
	}
	virtual string getMessage() const {
		return _message;
	}
	virtual void setInnerException(exceptionEx* excp) {
		_innerException = excp;
	}
	virtual exceptionEx* getInnerException() const {
		return _innerException;
	}
	virtual void setTitle(string str) {
		_title = str;
	}
	virtual string getTitle() const {
		return _title;
	}
	virtual void setLineNumber(int n) {
		_line = n;
	}
	virtual int getLineNumber() {
		return _line;
	}
	virtual void setFilename(string str) {
		_file = str;
	}
	virtual string getFilename() {
		return _file;
	}
	virtual void setOwner(string str) {
		_owner = str;
	}
	virtual string getOwner() {
		return _owner;
	}

};

class invalidParameter: public exceptionEx {
public:
	invalidParameter(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Input parameter is invalid";
	}
};
class namesMismatch: public exceptionEx {
public:
	namesMismatch(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Variable names doesn't match";
	}
};
class dataTypeMismatch: public exceptionEx {
public:
	dataTypeMismatch(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Can not convert between DataTypes";
	}
};
class cantParseString: public exceptionEx {
public:
	cantParseString(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Unreadable string format";
	}
};
class outOfRange: public exceptionEx {
public:
	outOfRange(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Value is out of range";
	}
};
class notImplemented: public exceptionEx {
public:
	notImplemented(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Not Implemented";
	}
};
class invalidName: public exceptionEx {
public:
	invalidName(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Invalid Name";
	}
};
class obsolete: public exceptionEx {
public:
	obsolete(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Obsolete";
	}
};
class unknownException: public exceptionEx {
protected:
	bool hasEx;
	exception _ex;
public:
	unknownException(exception ex, string imessage = "", string owner = "",
			string file = "", int line = -1) :
			exceptionEx(getDefTitle(), imessage, owner, file, line, NULL) {
		_ex = ex;
		hasEx = true;
	}
	unknownException(string imessage = "", string owner = "", string file = "",
			int line = -1) :
			exceptionEx(getDefTitle(), imessage, owner, file, line, NULL) {
		hasEx = false;
	}

	string getDefTitle() {
		return "Unknown exception occurred.";
	}

	virtual const char * what() const throw () {
		stringstream msg;
		msg << getTitle() << " : " << getMessage();
		if (hasEx)
			msg << "\nInner std::exception : " << _ex.what();
		return msg.str().c_str();
	}

};
class connectionError: public exceptionEx {
public:
	connectionError(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "connection failed.";
	}
};
class DBError: public exceptionEx {
public:
	DBError(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "DBError";
	}
};
class duplicateName: public exceptionEx {
public:
	duplicateName(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Duplicate Name";
	}
};
class nullValue: public exceptionEx {
public:
	nullValue(string imessage = "", string owner = "", string file = "",
			int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
					iinnerException) {
	}
	string getDefTitle() {
		return "Duplicate Name";
	}
};
class networkFailure: public exceptionEx {
public:
	networkFailure(string imessage = "", string owner = "", string file = "",
	          int line = -1, exceptionEx *iinnerException = NULL) :
			exceptionEx(getDefTitle(), imessage, owner, file, line,
			            iinnerException) {
	}
	string getDefTitle() {
		return "Network Failure";
	}
};
//exceptionEx
#define EXexceptionEx(msg) new exceptionEx(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTexceptionEx(msg) throw EXexceptionEx(msg)
#define EXexceptionExO(msg,owner) new exceptionEx(msg,owner,__FILE__,__LINE__)
#define EXTexceptionExO(msg,owner) throw EXexceptionExO(msg,owner)
#define EXexceptionExI(msg,inner) new exceptionEx(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTexceptionExI(msg,inner) throw EXexceptionExI(msg,inner)
#define EXexceptionExIO(msg,owner,inner) new exceptionEx(msg,owner,__FILE__,__LINE__,inner)
#define EXTexceptionExIO(msg,owner,inner) throw EXexceptionExIO(msg,owner,inner)
//invalidParameter
#define EXinvalidParameter(msg) new invalidParameter(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTinvalidParameter(msg) throw EXinvalidParameter(msg)
#define EXinvalidParameterO(msg,owner) new invalidParameter(msg,owner,__FILE__,__LINE__)
#define EXTinvalidParameterO(msg,owner) throw EXinvalidParameterO(msg,owner)
#define EXinvalidParameterI(msg,inner) new invalidParameter(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTinvalidParameterI(msg,inner) throw EXinvalidParameterI(msg,inner)
#define EXinvalidParameterIO(msg,owner,inner) new invalidParameter(msg,owner,__FILE__,__LINE__,inner)
#define EXTinvalidParameterIO(msg,owner,inner) throw EXinvalidParameterIO(msg,owner,inner)
//namesMismatch
#define EXnamesMismatch(msg) new namesMismatch(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTnamesMismatch(msg) throw EXnamesMismatch(msg)
#define EXnamesMismatchO(msg,owner) new namesMismatch(msg,owner,__FILE__,__LINE__)
#define EXTnamesMismatchO(msg,owner) throw EXnamesMismatchO(msg,owner)
#define EXnamesMismatchI(msg,inner) new namesMismatch(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTnamesMismatchI(msg,inner) throw EXnamesMismatchI(msg,inner)
#define EXnamesMismatchIO(msg,owner,inner) new namesMismatch(msg,owner,__FILE__,__LINE__,inner)
#define EXTnamesMismatchIO(msg,owner,inner) throw EXnamesMismatchIO(msg,owner,inner)
//dataTypeMismatch
#define EXdataTypeMismatch(msg) new dataTypeMismatch(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTdataTypeMismatch(msg) throw EXdataTypeMismatch(msg)
#define EXdataTypeMismatchO(msg,owner) new dataTypeMismatch(msg,owner,__FILE__,__LINE__)
#define EXTdataTypeMismatchO(msg,owner) throw EXdataTypeMismatchO(msg,owner)
#define EXdataTypeMismatchI(msg,inner) new dataTypeMismatch(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTdataTypeMismatchI(msg,inner) throw EXdataTypeMismatchI(msg,inner)
#define EXdataTypeMismatchIO(msg,owner,inner) new dataTypeMismatch(msg,owner,__FILE__,__LINE__,inner)
#define EXTdataTypeMismatchIO(msg,owner,inner) throw EXdataTypeMismatchIO(msg,owner,inner)
//cantParseString
#define EXcantParseString(msg) new cantParseString(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTcantParseString(msg) throw EXcantParseString(msg)
#define EXcantParseStringO(msg,owner) new cantParseString(msg,owner,__FILE__,__LINE__)
#define EXTcantParseStringO(msg,owner) throw EXcantParseStringO(msg,owner)
#define EXcantParseStringI(msg,inner) new cantParseString(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTcantParseStringI(msg,inner) throw EXcantParseStringI(msg,inner)
#define EXcantParseStringIO(msg,owner,inner) new cantParseString(msg,owner,__FILE__,__LINE__,inner)
#define EXTcantParseStringIO(msg,owner,inner) throw EXcantParseStringIO(msg,owner,inner)
//outOfRange
#define EXoutOfRange(msg) new outOfRange(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXToutOfRange(msg) throw EXoutOfRange(msg)
#define EXoutOfRangeO(msg,owner) new outOfRange(msg,owner,__FILE__,__LINE__)
#define EXToutOfRangeO(msg,owner) throw EXoutOfRangeO(msg,owner)
#define EXoutOfRangeI(msg,inner) new outOfRange(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXToutOfRangeI(msg,inner) throw EXoutOfRangeI(msg,inner)
#define EXoutOfRangeIO(msg,owner,inner) new outOfRange(msg,owner,__FILE__,__LINE__,inner)
#define EXToutOfRangeIO(msg,owner,inner) throw EXoutOfRangeIO(msg,owner,inner)
//notImplemented
#define EXnotImplemented(msg) new notImplemented(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTnotImplemented(msg) throw EXnotImplemented(msg)
#define EXnotImplementedO(msg,owner) new notImplemented(msg,owner,__FILE__,__LINE__)
#define EXTnotImplementedO(msg,owner) throw EXnotImplementedO(msg,owner)
#define EXnotImplementedI(msg,inner) new notImplemented(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTnotImplementedI(msg,inner) throw EXnotImplementedI(msg,inner)
#define EXnotImplementedIO(msg,owner,inner) new notImplemented(msg,owner,__FILE__,__LINE__,inner)
#define EXTnotImplementedIO(msg,owner,inner) throw EXnotImplementedIO(msg,owner,inner)
//invalidName
#define EXinvalidName(msg) new invalidName(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTinvalidName(msg) throw EXinvalidName(msg)
#define EXinvalidNameO(msg,owner) new invalidName(msg,owner,__FILE__,__LINE__)
#define EXTinvalidNameO(msg,owner) throw EXinvalidNameO(msg,owner)
#define EXinvalidNameI(msg,inner) new invalidName(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTinvalidNameI(msg,inner) throw EXinvalidNameI(msg,inner)
#define EXinvalidNameIO(msg,owner,inner) new invalidName(msg,owner,__FILE__,__LINE__,inner)
#define EXTinvalidNameIO(msg,owner,inner) throw EXinvalidNameIO(msg,owner,inner)
//obsolete
#define EXobsolete(msg) new obsolete(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTobsolete(msg) throw EXobsolete(msg)
#define EXobsoleteO(msg,owner) new obsolete(msg,owner,__FILE__,__LINE__)
#define EXTobsoleteO(msg,owner) throw EXobsoleteO(msg,owner)
#define EXobsoleteI(msg,inner) new obsolete(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTobsoleteI(msg,inner) throw EXobsoleteI(msg,inner)
#define EXobsoleteIO(msg,owner,inner) new obsolete(msg,owner,__FILE__,__LINE__,inner)
#define EXTobsoleteIO(msg,owner,inner) throw EXobsoleteIO(msg,owner,inner)
//unknownException
#define EXunknownException(msg) new unknownException(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTunknownException(msg) throw EXunknownException(msg)
#define EXunknownExceptionO(msg,owner) new unknownException(msg,owner,__FILE__,__LINE__)
#define EXTunknownExceptionO(msg,owner) throw EXunknownExceptionO(msg,owner)
#define EXunknownExceptionI(msg,inner) new unknownException(inner,msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTunknownExceptionI(msg,inner) throw EXunknownExceptionI(msg,inner)
#define EXunknownExceptionIO(msg,owner,inner) new unknownException(inner,msg,owner,__FILE__,__LINE__)
#define EXTunknownExceptionIO(msg,owner,inner) throw EXunknownExceptionIO(msg,owner,inner)
//connectionError
#define EXconnectionError(msg) new connectionError(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTconnectionError(msg) throw EXconnectionError(msg)
#define EXconnectionErrorO(msg,owner) new connectionError(msg,owner,__FILE__,__LINE__)
#define EXTconnectionErrorO(msg,owner) throw EXconnectionErrorO(msg,owner)
#define EXconnectionErrorI(msg,inner) new connectionError(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTconnectionErrorI(msg,inner) throw EXconnectionErrorI(msg,inner)
#define EXconnectionErrorIO(msg,owner,inner) new connectionError(msg,owner,__FILE__,__LINE__,inner)
#define EXTconnectionErrorIO(msg,owner,inner) throw EXconnectionErrorIO(msg,owner,inner)
//DBError
#define EXDBError(msg) new DBError(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTDBError(msg) throw EXDBError(msg)
#define EXDBErrorO(msg,owner) new DBError(msg,owner,__FILE__,__LINE__)
#define EXTDBErrorO(msg,owner) throw EXDBErrorO(msg,owner)
#define EXDBErrorI(msg,inner) new DBError(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTDBErrorI(msg,inner) throw EXDBErrorI(msg,inner)
#define EXDBErrorIO(msg,owner,inner) new DBError(msg,owner,__FILE__,__LINE__,inner)
#define EXTDBErrorIO(msg,owner,inner) throw EXDBErrorIO(msg,owner,inner)
//duplicateName
#define EXduplicateName(msg) new duplicateName(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTduplicateName(msg) throw EXduplicateName(msg)
#define EXduplicateNameO(msg,owner) new duplicateName(msg,owner,__FILE__,__LINE__)
#define EXTduplicateNameO(msg,owner) throw EXduplicateNameO(msg,owner)
#define EXduplicateNameI(msg,inner) new duplicateName(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTduplicateNameI(msg,inner) throw EXduplicateNameI(msg,inner)
#define EXduplicateNameIO(msg,owner,inner) new duplicateName(msg,owner,__FILE__,__LINE__,inner)
#define EXTduplicateNameIO(msg,owner,inner) throw EXduplicateNameIO(msg,owner,inner)
//nullValue
#define EXnullValue(msg) new nullValue(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTnullValue(msg) throw EXnullValue(msg)
#define EXnullValueO(msg,owner) new nullValue(msg,owner,__FILE__,__LINE__)
#define EXTnullValueO(msg,owner) throw EXnullValueO(msg,owner)
#define EXnullValueI(msg,inner) new nullValue(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTnullValueI(msg,inner) throw EXnullValueI(msg,inner)
#define EXnullValueIO(msg,owner,inner) new nullValue(msg,owner,__FILE__,__LINE__,inner)
#define EXTnullValueIO(msg,owner,inner) throw EXnullValueIO(msg,owner,inner)
//networkFailure
#define EXnetworkFailure(msg) new networkFailure(msg,this->getNameAndType(),__FILE__,__LINE__)
#define EXTnetworkFailure(msg) throw EXnetworkFailure(msg)
#define EXnetworkFailureO(msg,owner) new networkFailure(msg,owner,__FILE__,__LINE__)
#define EXTnetworkFailureO(msg,owner) throw EXnetworkFailureO(msg,owner)
#define EXnetworkFailureI(msg,inner) new networkFailure(msg,this->getNameAndType(),__FILE__,__LINE__,inner)
#define EXTnetworkFailureI(msg,inner) throw EXnetworkFailureI(msg,inner)
#define EXnetworkFailureIO(msg,owner,inner) new networkFailure(msg,owner,__FILE__,__LINE__,inner)
#define EXTnetworkFailureIO(msg,owner,inner) throw EXnetworkFailureIO(msg,owner,inner)

}
}
#endif // EXCEPTIONEX_HPP
