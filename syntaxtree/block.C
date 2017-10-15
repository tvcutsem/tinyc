#include <sstream>

#include "block.h"



Block::Block(SymbolTable* scope,Statements* statements):
	scope_(scope),statements_(statements) { }


Block::~Block() {
   //delete all statements as well
   for(Statementit s=statements_->begin();s!=statements_->end();s++) {
      // *s is a Statement*
      delete *s;
   }
   delete statements_;
   delete scope_;
}


ostream& Block::prettyPrint(ostream& os,int indent) const {
   while (indent--)
      os << " ";
   Identifiers decls;
   //insert scope declarations into decls
   scope_->declarations(decls);
   for(Identifierit i=decls.begin();i!=decls.end();i++) {
      // *i is an Identifier*
      (*i)->prettyPrint(os);
      os << ";";
   }
   os << ")" << endl << " { ";
   for(Statementit i=statements_->begin();i!=statements_->end();i++) {
      // *i is a Statement*
      (*i)->prettyPrint(os,indent+1);
      os << ";";
   }
   os << " } " << endl;
   return os;
}


string Block::toString() const {
   stringstream ss;
   ss << "[ Block [ "; 
   Identifiers decls;
   //insert scope declarations into decls
   scope_->declarations(decls);
   for(Identifierit v=decls.begin();v!=decls.end();v++) {
      // *v is an Identifier*
      ss << "\n[" << (*v)->toString() << "]";
   }
   ss << "] [ ";
   for(Statementit s=statements_->begin();s!=statements_->end();s++) {
      // *s is a Statement*
      ss << "\n[" << (*s)->toString() << "]\n";
   }
   ss << " ] ]";
   return ss.str();
}
