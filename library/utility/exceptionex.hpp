#ifndef EXCEPTIONEX_HPP
#define EXCEPTIONEX_HPP

#include <exception>
#include <string>
#include <sstream>
#include <memory>

namespace zeitoon {
namespace utility {

class exceptionEx : public std::exception {
protected:
	std::string _message;
	std::string _title;
	std::string _owner;
	int _line;
	std::string _file;
	std::string _function;
	std::string tempStr;
public:

	exceptionEx() : exceptionEx(getDefTitle(), "", "", "", "", -1) {
	}

	exceptionEx(std::string imessage) : exceptionEx(getDefTitle(), imessage, "", "", "", -1) {
	}

	exceptionEx(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	exceptionEx(std::string ititle, std::string imessage, std::string owner, std::string file, std::string function,
	            int line) {
		_title = ititle;
		_message = imessage;
		_owner = owner;
		_line = line;
		_file = file;
		_function = function;
	}

	virtual ~exceptionEx() throw() {
	}

	virtual std::string getDefTitle() {
		return "Exception occurred";
	}

	virtual std::string getName() {
		return "exceptionEx";
	}

	virtual std::string getNameAndType() {
		return getName();
	}

	virtual const char *what() throw() {
		tempStr = this->toString();
		return tempStr.c_str();
	}

	virtual std::string toString() const {
		std::stringstream msg;
		msg << getTitle();
		if (_owner.length() > 0 || _file.length() > 0 || _line >= 0)
			msg << " ( From " << (_owner.length() > 0 ? "'" + _owner + "'" : "")
			<< (_function.length() > 0 ? ":" + _function : "")
			<< (_file.length() > 0 || _line >= 0 ? "[" : "")
			<< (_file.length() > 0 ? _file : "")
			<< (_line >= 0 ? "#" + std::to_string(_line) : "")
			<< (_file.length() > 0 || _line >= 0 ? "]" : "")
			<< " )";
		msg << " : " << getMessage();
		return msg.str();
	}

	virtual void setMessage(std::string msg) {
		_message = msg;
	}

	virtual std::string getMessage() const {
		return _message;
	}

	virtual void setTitle(std::string str) {
		_title = str;
	}

	virtual std::string getTitle() const {
		return _title;
	}

	virtual void setLineNumber(int n) {
		_line = n;
	}

	virtual int getLineNumber() {
		return _line;
	}

	virtual void setFilename(std::string str) {
		_file = str;
	}

	virtual std::string getFilename() {
		return _file;
	}

	virtual void setOwner(std::string str) {
		_owner = str;
	}

	virtual std::string getOwner() {
		return _owner;
	}

	virtual void setFunction(std::string str) {
		_function = str;
	}

	virtual std::string getFunction() {
		return _function;
	}

};

class exceptionExWithInner : public exceptionEx {
protected:
	std::exception_ptr _innerException;

public:
	exceptionExWithInner(std::string ititle, std::string imessage, std::string owner, std::string file,
	                     std::string function, int line,
	                     std::exception_ptr &innerException) :
			exceptionExWithInner(ititle, imessage, owner, file, function, line) {
		setInnerException(innerException);
	}

	exceptionExWithInner(std::string ititle, std::string imessage, std::string owner, std::string file,
	                     std::string function, int line,
	                     std::exception_ptr &&innerException) :
			exceptionExWithInner(ititle, imessage, owner, file, function, line) {
		setInnerException(innerException);
	}

	exceptionExWithInner(std::string ititle, std::string imessage, std::string owner, std::string file,
	                     std::string function, int line) :
			exceptionEx(ititle, imessage, owner, file, function, line) {
	}

	virtual ~exceptionExWithInner() throw() {
	}

	virtual std::string getDefTitle() {
		return "exceptionExWithInner";
	};

	virtual std::string getName() {
		return "exceptionExWithInner";
	};

	virtual std::string toString() const {
		std::string str = exceptionEx::toString();
		if (hasInnerException()) {
			try {
				std::rethrow_exception(_innerException);
			} catch (const std::exception &e) {
				str += "\n\tInnerException : " + std::string(e.what());
			}
		}
		return str;
	}

	virtual void setInnerException(std::exception_ptr &excp) {
		_innerException.swap(excp);
	}

	virtual void setInnerException(std::exception_ptr &&excp) {
		_innerException.swap(excp);
	}

	virtual std::exception_ptr getInnerException() const {
		return _innerException;
	}

	virtual bool hasInnerException() const {
		return (bool) _innerException;
	}//todo:move to base. add a getInnerWhat() and use it in base toString. remove drived toString()s
};

class exceptionExWithInnerEx : public exceptionEx {
protected:
	std::shared_ptr<exceptionEx> _innerException;

public:
	exceptionExWithInnerEx(std::string ititle, std::string imessage, std::string owner, std::string file,
	                       std::string function, int line,
	                       exceptionEx &iinnerException) :
			exceptionExWithInnerEx(ititle, imessage, owner, file, function, line) {
		setInnerException(iinnerException);
	}

	exceptionExWithInnerEx(std::string ititle, std::string imessage, std::string owner, std::string file,
	                       std::string function, int line) :
			exceptionEx(ititle, imessage, owner, file, function, line) {
	}

	virtual ~exceptionExWithInnerEx() throw() {
	}

	virtual std::string getDefTitle() {
		return "exceptionExWithInnerEx";
	};

	virtual std::string getName() {
		return "exceptionExWithInnerEx";
	};

	virtual std::string toString() const {
		std::string str = exceptionEx::toString();
		if (hasInnerException())
			str += "\n\tInnerException : " + getInnerException().toString();
		return str;
	}

	/*virtual void setInnerException(exceptionEx &&excp) {
		_innerException.reset((exceptionExWithInnerEx *) &excp);//new exceptionExWithInnerEx);
		//*_innerException = excp;
	}*/

	virtual void setInnerException(exceptionEx &excp) {
		exceptionExWithInner *a = dynamic_cast<exceptionExWithInner *>(&excp);
		exceptionExWithInnerEx *b = dynamic_cast<exceptionExWithInnerEx *>(&excp);
		if (a) {
			exceptionExWithInner *ex = new exceptionExWithInner("", "", "", "", "", 0);
			*ex = (exceptionExWithInner &) excp;
			_innerException.reset(ex);
		} else if (b) {
			exceptionExWithInnerEx *ex = new exceptionExWithInnerEx("", "", "", "", "", 0);
			*ex = (exceptionExWithInnerEx &) excp;
			_innerException.reset(ex);
		} else {
			exceptionEx *ex = new exceptionEx();
			*ex = excp;
			_innerException.reset(ex);
		}
		//_innerException.reset(&excp);//new exceptionExWithInnerEx);

		//*_innerException = (exceptionExWithInnerEx)excp;
	}

	virtual exceptionEx &getInnerException() const {
		return *_innerException;
	}

	virtual bool hasInnerException() const {
		return (bool) _innerException;
	}

};

//unknown exception
class unknownException : public exceptionExWithInner {
public:
	unknownException(std::string imessage) :
			unknownException(imessage, "", "", "", -1) {
	}

	unknownException(std::string imessage, std::exception_ptr &innerException) :
			unknownException(imessage, "", "", "", -1, innerException) {
	}

	unknownException(std::string imessage, std::exception_ptr &&innerException) :
			unknownException(imessage, "", "", "", -1, innerException) {
	}

	unknownException(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 std::exception_ptr &innerException) :
			exceptionExWithInner(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	unknownException(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 std::exception_ptr &&innerException) :
			exceptionExWithInner(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	unknownException(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInner(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Unknown exception occurred";
	}

	virtual std::string getName() {
		return "unknownException";
	}

};

}
}

//Macro here -----------------------------------------------------------------

//unknownException
#define EXTunknownException(msg) throw zeitoon::utility::unknownException(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTunknownExceptionO(msg, owner) throw zeitoon::utility::unknownException(msg,owner,__FILE__,__func__,__LINE__)
#define EXTunknownExceptionI(msg) throw zeitoon::utility::unknownException(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,std::current_exception())
#define EXTunknownExceptionIO(msg, owner) throw zeitoon::utility::unknownException(msg,owner,__FILE__,__func__,__LINE__,std::current_exception())


#endif // EXCEPTIONEX_HPP
