#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "syntaxtree/constant.h"

#include <string>

using namespace std;

//apparently inherting from runtime_error implies writing empty
//throw clauses, my gcc 3.1 gets confused all the time
//we'll have to do without inheritance

class Exception {
   public:
      Exception(string description): description_(description) { }
      virtual ~Exception() { }
      virtual string what() { return description_; }
   private:
      string description_;
};

/**
 * Common superclass for exceptions regarding variables.
 */
class VariableException: public Exception {
   public:
      VariableException(string description,string name):
	 Exception(description),name_(name) { }
      string what() { return Exception::what() + name_; }
   private:
      string name_;
};


/**
 * Thrown when a variable was found undeclared.
 */
class UndefException: public VariableException {
   public:
      UndefException(string name):
	 VariableException("undefined variable reference: ",name) { }
};


/**
 * thrown when a variable was found to have already been declared.
 */
class AlreadyDefException: public VariableException {
   public:
      AlreadyDefException(string name):
	 VariableException("variable already declared in current scope: ",name) { }
};


/**
 * thrown when a function or array was found where a variable had to be 
 */
class VariableExpected: public VariableException {
   public:
      VariableExpected(string name):
	 VariableException("primitive required, found array or function ",name) { }
};


/**
 * thrown when a variable was found where a function had to be 
 */
class FunctionExpected: public VariableException {
   public:
      FunctionExpected(string name):
	 VariableException("function required, found array or primitive ",name) { }
};

/**
 * thrown when a variable was found where an array should be 
 */
class ArrayExpected: public VariableException {
   public:
      ArrayExpected(string name):
	 VariableException("array required, found function or primitive ",name) { }
};


/**
 * Thrown when a compile-time computation executes a division by zero
 */
class DivisionByZeroError: public Exception {
  public:
    DivisionByZeroError(Constant* c):
      Exception("Division by zero error: "+c->toString()+" / 0") { }
};


/**
 * Thrown when a variable is declared as being void 
 */
class IllegalTypeException: public Exception {
  public:
    IllegalTypeException(string identifier):
      Exception("Illegal type declaration for "+identifier+
	        ": cannot specify it's type as void") { }
};

/**
 * Thrown when an error occured during the compilation process
 * and the process has to be 'aborted'
 */
class ProcessError: public Exception {
  public:
    ProcessError(string description):
      Exception(description) { }
};

#endif
