#include "funref.h"

ostream& FunctionRef::prettyPrint(ostream& os,int indent) const {
   return reference_->prettyPrint(os,indent);
}

string FunctionRef::toString() const {
   return reference_->toString();
}

ostream& FunctionRef::prettyPrintBody(ostream& os,int indent) const {
   return reference_->prettyPrintBody(os,indent);
}

string FunctionRef::toStringBody() const {
   return reference_->toStringBody();
}

string* FunctionRef::name() const {
   return reference_->name();
}

void FunctionRef::setName(string* newName) {
   return reference_->setName(newName);
}

Type FunctionRef::returnType() const {
   return reference_->returnType();
}

void FunctionRef::accept(TreeVisitor* v) {
  v->visitFunction(this);
}


void FunctionRef::parameters(Identifiers& output) const { 
   reference_->parameters(output);
}

Block* FunctionRef::body() const {
   return reference_->body();
}

SymbolTable* FunctionRef::paramScope() const {
   return reference_->paramScope();
}

void FunctionRef::setBody(Block* newBody) {
  reference_->setBody(newBody);
}

bool FunctionRef::operator==(Function& other) const {
   FunctionRef* otherref = dynamic_cast<FunctionRef*>(&other);
   if (!otherref)
      return false;
   else
      return (reference_ == otherref->reference_);
}

void FunctionRef::setIntermediateCode(Instructions* code) {
  reference_->setIntermediateCode(code);
}

Instructions* FunctionRef::intermediateCode() const {
  return reference_->intermediateCode();
}


void FunctionRef::setUniqueDeclarations(Identifiers* ids) {
  reference_->setUniqueDeclarations(ids);
}

Identifiers* FunctionRef::uniqueDeclarations() const {
  return reference_->uniqueDeclarations();
}
