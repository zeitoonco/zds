/** ***   WARNING   ***
  * This file has been generated using CodeGenerator.
  *   any manual modification will be erased in next cmake build.
  *
  * By ajl.
  */

#ifndef ZEITOON_USERMANAGEMENT_EXCEPTIONS_HPP
#define ZEITOON_USERMANAGEMENT_EXCEPTIONS_HPP

#include <utility/exceptionex.hpp>

namespace zeitoon {
namespace usermanagement {

class loginFail : public exceptionExWithInnerEx {
public:
	loginFail(std::string imessage) :
			loginFail(imessage, "", "", "", -1) {
	}

	loginFail(std::string imessage, exceptionEx &innerException) :
			loginFail(imessage, "", "", "", -1, innerException) {
	}

	loginFail(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	loginFail(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Login failed";
	}

	virtual std::string getName() {
		return "loginFail";
	}

};

class checkPermissionFail : public exceptionExWithInnerEx {
public:
	checkPermissionFail(std::string imessage) :
			checkPermissionFail(imessage, "", "", "", -1) {
	}

	checkPermissionFail(std::string imessage, exceptionEx &innerException) :
			checkPermissionFail(imessage, "", "", "", -1, innerException) {
	}

	checkPermissionFail(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	checkPermissionFail(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Check-Permission failed";
	}

	virtual std::string getName() {
		return "checkPermissionFail";
	}

};

class addContactFailed : public exceptionExWithInnerEx {
public:
	addContactFailed(std::string imessage) :
			addContactFailed(imessage, "", "", "", -1) {
	}

	addContactFailed(std::string imessage, exceptionEx &innerException) :
			addContactFailed(imessage, "", "", "", -1, innerException) {
	}

	addContactFailed(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	addContactFailed(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Add-Contact failed";
	}

	virtual std::string getName() {
		return "addContactFailed";
	}

};

class removeContactFailed : public exceptionExWithInnerEx {
public:
	removeContactFailed(std::string imessage) :
			removeContactFailed(imessage, "", "", "", -1) {
	}

	removeContactFailed(std::string imessage, exceptionEx &innerException) :
			removeContactFailed(imessage, "", "", "", -1, innerException) {
	}

	removeContactFailed(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	removeContactFailed(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Remove-Contact failed";
	}

	virtual std::string getName() {
		return "removeContactFailed";
	}

};

class listContactsFailed : public exceptionExWithInnerEx {
public:
	listContactsFailed(std::string imessage) :
			listContactsFailed(imessage, "", "", "", -1) {
	}

	listContactsFailed(std::string imessage, exceptionEx &innerException) :
			listContactsFailed(imessage, "", "", "", -1, innerException) {
	}

	listContactsFailed(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	listContactsFailed(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "List-Contacts failed";
	}

	virtual std::string getName() {
		return "listContactsFailed";
	}

};

class modifyContactFailed : public exceptionExWithInnerEx {
public:
	modifyContactFailed(std::string imessage) :
			modifyContactFailed(imessage, "", "", "", -1) {
	}

	modifyContactFailed(std::string imessage, exceptionEx &innerException) :
			modifyContactFailed(imessage, "", "", "", -1, innerException) {
	}

	modifyContactFailed(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	modifyContactFailed(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Modify-Contact failed";
	}

	virtual std::string getName() {
		return "modifyContactFailed";
	}

};


} // zeitoon
} // usermanagement


//Macro here -----------------------------------------------------------------

//loginFail
#define EXTloginFail(msg) throw zeitoon::usermanagement::loginFail(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTloginFailO(msg, owner) throw zeitoon::usermanagement::loginFail(msg,owner,__FILE__,__func__,__LINE__)
#define EXTloginFailI(msg, inner) throw zeitoon::usermanagement::loginFail(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTloginFailIO(msg, owner, inner) throw zeitoon::usermanagement::loginFail(msg,owner,__FILE__,__func__,__LINE__,inner)

//checkPermissionFail
#define EXTcheckPermissionFail(msg) throw zeitoon::usermanagement::checkPermissionFail(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTcheckPermissionFailO(msg, owner) throw zeitoon::usermanagement::checkPermissionFail(msg,owner,__FILE__,__func__,__LINE__)
#define EXTcheckPermissionFailI(msg, inner) throw zeitoon::usermanagement::checkPermissionFail(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTcheckPermissionFailIO(msg, owner, inner) throw zeitoon::usermanagement::checkPermissionFail(msg,owner,__FILE__,__func__,__LINE__,inner)

//addContactFailed
#define EXTaddContactFailed(msg) throw zeitoon::usermanagement::addContactFailed(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTaddContactFailedO(msg, owner) throw zeitoon::usermanagement::addContactFailed(msg,owner,__FILE__,__func__,__LINE__)
#define EXTaddContactFailedI(msg, inner) throw zeitoon::usermanagement::addContactFailed(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTaddContactFailedIO(msg, owner, inner) throw zeitoon::usermanagement::addContactFailed(msg,owner,__FILE__,__func__,__LINE__,inner)

//removeContactFailed
#define EXTremoveContactFailed(msg) throw zeitoon::usermanagement::removeContactFailed(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTremoveContactFailedO(msg, owner) throw zeitoon::usermanagement::removeContactFailed(msg,owner,__FILE__,__func__,__LINE__)
#define EXTremoveContactFailedI(msg, inner) throw zeitoon::usermanagement::removeContactFailed(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTremoveContactFailedIO(msg, owner, inner) throw zeitoon::usermanagement::removeContactFailed(msg,owner,__FILE__,__func__,__LINE__,inner)

//listContactsFailed
#define EXTlistContactsFailed(msg) throw zeitoon::usermanagement::listContactsFailed(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTlistContactsFailedO(msg, owner) throw zeitoon::usermanagement::listContactsFailed(msg,owner,__FILE__,__func__,__LINE__)
#define EXTlistContactsFailedI(msg, inner) throw zeitoon::usermanagement::listContactsFailed(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTlistContactsFailedIO(msg, owner, inner) throw zeitoon::usermanagement::listContactsFailed(msg,owner,__FILE__,__func__,__LINE__,inner)

//modifyContactFailed
#define EXTmodifyContactFailed(msg) throw zeitoon::usermanagement::modifyContactFailed(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTmodifyContactFailedO(msg, owner) throw zeitoon::usermanagement::modifyContactFailed(msg,owner,__FILE__,__func__,__LINE__)
#define EXTmodifyContactFailedI(msg, inner) throw zeitoon::usermanagement::modifyContactFailed(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTmodifyContactFailedIO(msg, owner, inner) throw zeitoon::usermanagement::modifyContactFailed(msg,owner,__FILE__,__func__,__LINE__,inner)



#endif //ZEITOON_USERMANAGEMENT_EXCEPTIONS_HPP
