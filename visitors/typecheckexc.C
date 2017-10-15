#include "typecheckexc.h"

#include <sstream>

TypeCheckError::~TypeCheckError() { }

int TypeCheckError::lineno() const {
   return node_->lineno();
}

string TypeCheckError::what() {
   return description();
}

ostream& TypeCheckError::report(ostream& errstream) const {
   errstream << "typecheck error at line " << lineno() << ": "
             << errname() << " - " << description() << endl;
   node_->prettyPrint(errstream,5);
   return errstream;
}

string TypeMismatchError::description() const {
   stringstream ss;
   ss << TypeCheckError::description();
   ss << ", expected " << type2str(expected_) << ", given ";
   ss << type2str(given_);
   return ss.str();
}

string OperatorTypeError::description() const {
   stringstream ss;
   ss << TypeCheckError::description();
   ss << ", given " << type2str(givenType_);
   return ss.str();
}

string ArityError::description() const {
   stringstream ss;
   ss << TypeCheckError::description();
   ss << ", expected " << expectedArgs_ << " arguments, given ";
   ss << givenArgs_ << " arguments.";
   return ss.str();
}

string IllegalIndexError::description() const {
   stringstream ss;
   ss << "In index " << nth_ << ": ";
   ss << TypeCheckError::description();
   ss << ", given " << type2str(givenType_) << ", requires integral value";
   return ss.str();
}

string ArrayDimensionError::description() const {
   stringstream ss;
   ss << TypeCheckError::description();
   ss << ", expected " << expectedDim_ << " indices, given ";
   ss << givenDim_;
   return ss.str();
}

string ParameterTypeError::description() const {
   stringstream ss;
   ss << "In argument " << nth_ << ": ";
   ss << TypeMismatchError::description();
   return ss.str();
}

string ConditionError::description() const {
   stringstream ss;
   ss << TypeCheckError::description();
   ss << ", given " << type2str(given_) << " type";
   return ss.str();
}

string ArrayBaseMismatch::description() const {
  stringstream ss;
  ss << TypeMismatchError::description();
  ss << " in passing argument " << nth_;
  return ss.str();
}

string DimensionMismatch::description() const {
  stringstream ss;
  ss << TypeCheckError::description();
  ss << " in passing argument " << nth_ << " of function call";
  return ss.str();
}
