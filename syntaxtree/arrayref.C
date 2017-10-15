#include "arrayref.h"

Type ArrayRef::baseType() const {
   return reference_->baseType();
}

Expressions&  ArrayRef::lengths() const {
   return reference_->lengths();
}

int ArrayRef::dimension() const {
   return reference_->dimension();
}

string* ArrayRef::name() const {
   return reference_->name();
}

void ArrayRef::setName(string* newName) {
   reference_->setName(newName);
}

ostream& ArrayRef::prettyPrint(ostream& os,int indent) const {
   return reference_->prettyPrint(os,indent);
}

string ArrayRef::toString() const {
   return reference_->toString();
}

void ArrayRef::accept(TreeVisitor* v) {
  v->visitArray(this);
}

void ArrayRef::setIndices(Variables* indexlist) {
  reference_->setIndices(indexlist);
}

Variables* ArrayRef::getIndices() const {
  return reference_->getIndices();
}

bool ArrayRef::isGlobal() const {
  return reference_->isGlobal();
}
