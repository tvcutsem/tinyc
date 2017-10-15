#include "arraydecl.h"

#include <sstream>

ArrayDec::ArrayDec(string* name,Type base,Expressions* lengthExps, bool isGlobal):
	name_(name), baseType_(base), isGlobal_(isGlobal),
	lengthExps_(lengthExps), indexRegisters_(0) { }

ArrayDec::~ArrayDec() {
   for (Expressionit e=lengthExps_->begin();e!=lengthExps_->end();e++) {
      //*e is an Expression*
      delete *e;
   }
   delete lengthExps_;
   //check is necessary because this object may be deleted even
   //before the indexRegisters are generated (eg if parsing or typechecking fails)
   if (indexRegisters_)
     delete indexRegisters_;
}

int ArrayDec::dimension() const { return lengthExps_->size(); }

ostream& ArrayDec::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << type2str(baseType_) << " " << *name_ << "[";
   Expressionit exp = lengthExps_->begin();
   if (exp != lengthExps_->end()) {
      (*exp)->prettyPrint(os);
      exp++;
   }
   for (;exp != lengthExps_->end();exp++) {
      //*exp is an Expression*
      os << ",";
      (*exp)->prettyPrint(os);
   }
   os << "]";
   return os;
}

string ArrayDec::toString() const {
   stringstream ss;
   ss << "[ Array [" << type2str(baseType_) << "] [";
   ss << *name_ << "]" << "[";
   for (Expressionit exp=lengthExps_->begin();exp!=lengthExps_->end();exp++) {
      //*exp is an Expression*
      ss << "\n[ " << (*exp)->toString() << "]";
   }
   ss << "]]";
   return ss.str();
}

void ArrayDec::accept(TreeVisitor* v) {
   v->visitArray(this);
}

void ArrayDec::setIndices(Variables* indexlist) {
  indexRegisters_ = indexlist;
}

Variables* ArrayDec::getIndices() const {
  return indexRegisters_;
}
