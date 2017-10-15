#include "livenessexc.h"

ControlError::ControlError(const string& description, const Identifier& id):
  description_(description), id_(id) { }

void ControlError::report(ostream& errstream) const {
  errstream << description_ << ": " << *id_.name() << endl;
}

string MissingReturnError::ERR_MSG(
    "Reaching the end of function with non-void return type");

string UnusedVariableError::ERR_MSG("Unused variable");

string UninitializedVariableError::ERR_MSG( 
    "Variable used before it might have been initialized");
