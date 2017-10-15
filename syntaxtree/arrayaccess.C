#include "arrayaccess.h"

#include <sstream>

//constructor
ArrayAccess::ArrayAccess(Array* array,Expressions* indices):
   array_(array),indices_(indices) { }

//desctructor
ArrayAccess::~ArrayAccess() {
   delete array_;
   for (Expressionit e=indices_->begin();e!=indices_->end();e++) {
      //*e is an Expression*
      delete *e;
   }
   delete indices_;
}

void ArrayAccess::accept(TreeVisitor* v) {
   v->visitArrayAccess(this);
}

Type ArrayAccess::type() const {
   return array_->baseType();
}

ostream& ArrayAccess::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   os << *array_->name() << "[";
   Expressionit idx = indices_->begin();
   if (idx != indices_->end()) {
      (*idx)->prettyPrint(os);
      idx++;
   }
   for (;idx!=indices_->end();idx++) {
      //*idx is an Expression*
      os << ",";
      (*idx)->prettyPrint(os);
   }
   os << "]";
   return os;
}

string ArrayAccess::toString() const {
   stringstream ss;
   ss << "[ ArrayAccess [" << *array_->name() << "] [";
   for (Expressionit i=indices_->begin();i!=indices_->end();i++) {
      //*i is an Expression*
      ss << "\n[ " << (*i)->toString() << "]";
   }
   ss << "]]";
   return ss.str();
}
