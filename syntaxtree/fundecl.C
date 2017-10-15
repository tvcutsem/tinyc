#include "fundecl.h"

#include <sstream>

FunctionDec::FunctionDec(string* name,Type rettype): 
	vardecl_(new VariableDec(name,fun_t,true)),
	returnType_(rettype),icode_(0),uniqueDeclarations_(0) { }

FunctionDec::~FunctionDec() {
   delete pars_;
   delete body_;
   delete vardecl_;

   //check necessary because this object might already be
   //destroyed during eg parsing or typechecking, when icode_
   //has not yet been generated
   if(icode_)
     delete icode_;
   if (uniqueDeclarations_)
     delete uniqueDeclarations_;
}

void FunctionDec::setBody(Block* newBody) {
  if (body_ == newBody)
    return;
  delete body_;
  body_ = newBody;
}

ostream& FunctionDec::prettyPrint(ostream& os,int indent) const {
   while (indent--) {
      os << " ";
   }
   os << type2str(returnType_) << " " << *vardecl_->name() << "(";
   Identifiers pars;
   parameters(pars);
   Identifierit i=pars.begin();
   if (i!=pars.end()) {
      (*i)->prettyPrint(os);
      i++;
   }
   for(;i!=pars.end();i++) {
      // *i is an Identifier*
      os << ",";
      (*i)->prettyPrint(os);
   }
   os << ")";
   return os;
}

ostream& FunctionDec::prettyPrintBody(ostream& os,int indent) const {
   prettyPrint(os,indent);
   body_->prettyPrint(os,indent+1);
   return os;
}

string FunctionDec::toString() const {
   stringstream ss;
   ss << "[ function [ " << type2str(returnType_) << "]";
   ss << "[" << *vardecl_->name() << "] [ ";
   Identifiers pars;
   parameters(pars);
   for(Identifierit v=pars.begin();v!=pars.end();v++) {
      // *v is an Identifier*
      ss << "[ " << (*v)->toString() << " ]";
   }
   ss << "]]"; 
   return ss.str();
}

string FunctionDec::toStringBody() const {
   stringstream ss;
   ss << "[" << toString();
   ss << "[ " << body_->toString() << " ]]";
   return ss.str();
}

void FunctionDec::patch(SymbolTable* pars,Block* body) {
   pars_ = pars;
   body_ = body;
}


void FunctionDec::parameters(Identifiers& output) const {
   pars_->declarations(output);
}

//FunctionDecs should by definition exist only once, thus use
//pointer comparison
bool FunctionDec::operator==(Function& other) const {
   return (this == (FunctionDec*) &other);
}

void FunctionDec::setIntermediateCode(Instructions* code) {
  icode_ = code;
}

void FunctionDec::setUniqueDeclarations(Identifiers* ids) {
  uniqueDeclarations_ = ids;
}
