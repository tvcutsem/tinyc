#include "varref.h"

ostream& VariableRef::prettyPrint(ostream& os,int indent) const {
  return reference_->prettyPrint(os,indent);
}

string VariableRef::toString() const {
  return reference_->toString();
}

string* VariableRef::name() const {
  return reference_->name();
}

void VariableRef::setName(string* newName) {
  return reference_->setName(newName);
}

Type VariableRef::type() const {
  return reference_->type();
}

bool VariableRef::isGlobal() const {
  return reference_->isGlobal();
}

Variable* VariableRef::reference() const {
  return reference_;
}
