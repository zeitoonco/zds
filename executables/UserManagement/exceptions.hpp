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



#endif //ZEITOON_USERMANAGEMENT_EXCEPTIONS_HPP
