#ifndef FLOW_EXC_H
#define FLOW_EXC_H

#include "../syntaxtree/identifier.h"
#include "../syntaxtree/variable.h"
#include "../syntaxtree/function.h"

#include <string>
#include <list>
#include <iostream>

using namespace std;

/**
 * Describes errors reported by a livenesschecker.
 *
 * Their meaning is that usually, a variable is used
 * (or unused) in a wrong way (eg used before it is initialized).
 * These errors are discovered by checking the 'flow of control'
 * of the program, hence the name.
 */

class ControlError {

  public:

    /**
     * Constructs a new ControlError with the given description,
     * and the given identifier as the source of the error.
     */
    ControlError(const string& description, const Identifier& id);

    /**
     * Reports this error to the given error stream
     */
    void report(ostream& errstream) const;

  public:

    const string& description_;
    const Identifier& id_;

};


typedef list<ControlError> ControlErrors;
typedef ControlErrors::iterator ControlErrorit;


/**
 * Error raised when control flow reaches the end of a function,
 * which is declared to have a non-void return type.
 */

class MissingReturnError: public ControlError {
  public:

    MissingReturnError(const Function& f): ControlError(ERR_MSG,f) { }

  private:

    static string ERR_MSG;

};

/**
 * Raised when a variable is declared but not used in a function.
 */

class UnusedVariableError: public ControlError {

  public:

    UnusedVariableError(const Variable& v):
      ControlError(ERR_MSG,v) { }

  private:

    static string ERR_MSG;

};

/**
 * Raised when a variable is used before it is initialized.
 * Sometimes, the variable MIGHT have been initialized, but
 * the flow of control allows no guarantees (eg initialization
 * in only one of two if-branches). In these cases, these
 * errors are still reported.
 */

class UninitializedVariableError: public ControlError {

  public:

    UninitializedVariableError(const Variable& v):
      ControlError(ERR_MSG,v) { }

  private:

    static string ERR_MSG;

};

#endif
