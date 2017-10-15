#include "funcall.h"

#include <sstream>

typedef Expressions::iterator ExpressionIt;

Funcall::~Funcall() {
   delete fun_;
   //delete all expressions which constitute the actual arguments
   for (ExpressionIt i=args_->begin();i!=args_->end();i++) {
      // *i is an Expression*
      delete *i;
   }
   delete args_;
}

ostream& Funcall::prettyPrint(ostream& os,int indent) const {
   while(indent--)
      os << " ";
   os << *fun_->name() << "(";
   ExpressionIt i=args_->begin();
   if (i!=args_->end()) {
      (*i)->prettyPrint(os);
      i++;
   }
   for (;i!=args_->end();i++) {
      // *i is an Expression*
      os << ",";
      (*i)->prettyPrint(os);
   }
   os << ")";
   return os;
}

string Funcall::toString() const {
   stringstream ss;
   ss << "[ funcall [ " << *fun_->name() << " ] [";
   for (ExpressionIt i=args_->begin();i!=args_->end();i++) {
      // *i is an Expression*
      ss << "\n[" << (*i)->toString() << "]";
   }
   ss << "]]";
   return ss.str();
}
