#include "symboltable.h"

#include "syntaxtree/vardecl.h"
#include "syntaxtree/fundecl.h"
#include "syntaxtree/arraydecl.h"
#include "syntaxtree/funref.h"
#include "syntaxtree/undefinedvar.h"
#include "syntaxtree/undefinedfun.h"
#include "syntaxtree/undefinedarr.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>


SymbolTable::SymbolTable(SymbolTable* parent):
  parent_(parent),symtab_(new Symtab()),tempCount_(0) { }

SymbolTable::~SymbolTable() {
   //delete all variables from the symbol table
   for(VariableRecord var=symtab_->begin();var!=symtab_->end();var++) {
      // *var is a pair <string*,Identifier*>
      delete var->second;
   }
   delete symtab_;
}


Identifier* SymbolTable::lookupIdentifier(string* name) {
   VariableRecord end(symtab_->end());
   VariableRecord found = symtab_->find(name);
   if (found != end) {
      //check to see whether we have got a variable
      if ((found->second)->isVariable()) {
      	//return a proxy reference to the found variable
	Variable* var = dynamic_cast<Variable*>(found->second);
      	return new VariableRef(var);
      }
      else if ((found->second)->isFunction()) {
	Function* var = dynamic_cast<Function*>(found->second);
      	return new FunctionRef(var);
      }
      else if ((found->second)->isArray()) {
	Array* var = dynamic_cast<Array*>(found->second);
      	return new ArrayRef(var);
      }
      else
	 throw logic_error("unknown kind retrieved in lookupIdentifier");
   }
   else {
      //continue search in higher scope if available
      if (parent_)
	 return parent_->lookupIdentifier(name);
      //top scope, raise error
      else
         throw UndefException(*name);
   }
}

Variable* SymbolTable::lookupVariable(string* name) {
   return symbolTableLookupKind<Variable,VariableExpected,VariableRef>
      (*this,name,&Identifier::isVariable);
}

Function* SymbolTable::lookupFunction(string* name) { 
   return symbolTableLookupKind<Function,FunctionExpected,FunctionRef>
      (*this,name,&Identifier::isFunction);
}

Array* SymbolTable::lookupArray(string* name) { 
   return symbolTableLookupKind<Array,ArrayExpected,ArrayRef>
      (*this,name,&Identifier::isArray);
}

Variable* SymbolTable::declareVariable(string* name,Type type) {
   if (type == void_t)
     throw IllegalTypeException(*name);
   Variable* newVar;
   VariableRecord end(symtab_->end());
   VariableRecord found = symtab_->find(name);
   //if not already declared, add the declaration
   if (found == end) {
      newVar = new VariableDec(name,type,(parent_==0));
      (*symtab_)[name] = newVar;
      return newVar;
   }
   else 
      throw AlreadyDefException(*name);
}

void SymbolTable::patchRecursion(string* name,SymbolTable* formals,Block* body) {
   VariableRecord end(symtab_->end());
   VariableRecord found = symtab_->find(name);
   //if this function isn't found, raise a logic error, shouldn't happen 
   if (found == end) 
      throw logic_error("patchRecursion failed, function not declared");
   else {
      FunctionDec* fun = dynamic_cast<FunctionDec*>(found->second);
      fun->patch(formals,body);
   }
}


Function* SymbolTable::declareFunction(string* name,Type returnType) {
   Function* newFun;
   VariableRecord end(symtab_->end());
   VariableRecord found = symtab_->find(name);
   if (found == end) {
      newFun = new FunctionDec(name,returnType);
      (*symtab_)[name] = newFun;
      return newFun;
   }
   else 
      throw AlreadyDefException(*name);
}


Array* SymbolTable::declareArray(string* name,Type base,Expressions* lengths) {
   if (base == void_t)
     throw IllegalTypeException(*name);
   Array* newArray;
   VariableRecord end(symtab_->end());
   VariableRecord found = symtab_->find(name);
   if (found == end) {
      newArray = new ArrayDec(name,base,lengths,(parent_==0));
      (*symtab_)[name] = newArray;
      return newArray;
   }
   else 
      throw AlreadyDefException(*name);
}


Variable* SymbolTable::declareTempVariable(Type type) {
  string* tempname = StringPool::instance()->getString(makeTempName());
  VariableRecord end(symtab_->end());
  VariableRecord found = symtab_->find(tempname);
  //if already declared, generate a new name 
  while (found != end) {
    tempname = StringPool::instance()->getString(makeTempName());
    found = symtab_->find(tempname);
  }
  VariableDec* newVar = new VariableDec(tempname,type,false);
  (*symtab_)[tempname] = newVar;
  temporaries_.push_back(newVar);
  return newVar;
}


ostream& SymbolTable::printDeclarations(ostream& os) {
   VariableRecord end(symtab_->end());
   os << "variables declared in current scope: " << endl;
   for (VariableRecord var=symtab_->begin();var!=end;var++) {
      //*var is a pair<string*,Identifier*>
      if ((var->second)->isFunction()) {
	//for functions, also print the body
	Function* fun = dynamic_cast<Function*>(var->second);
	os << fun->toStringBody() << endl;
      } else {
      os << var->second->toString() << endl;
      }
   }
   return os;
}

void SymbolTable::declarations(Identifiers& ids) const {
   for (VariableRecord v=symtab_->begin(); v!=symtab_->end(); v++)
      // *v is a pair<string*,Identifier*>
      ids.push_back(v->second);
}

bool SymbolTable::isTemporary(Identifier* id) const {
  for (Variables::const_iterator temp = temporaries_.begin();
                                 temp != temporaries_.end(); temp++) {
    // *temp is a Variable*
    if (id == *temp)
      return true;
  }
  return false;
}


Variables& SymbolTable::retrieveVariables(Variables& vars) const {
   for (VariableRecord v=symtab_->begin();v!=symtab_->end();v++){
      // *v is a pair<string*,Identifier*>
      if ((v->second)->isVariable()) {
	 Variable* var = dynamic_cast<Variable*>(v->second);
         vars.push_back(var);
      }
   }
   return vars;
}

Functions& SymbolTable::retrieveFunctions(Functions& funs) const {
   for (VariableRecord v=symtab_->begin();v!=symtab_->end();v++){
      // *v is a pair<string*,Identifier*>
      if ((v->second)->isFunction()) {
	 Function* fun = dynamic_cast<Function*>(v->second);
         funs.push_back(fun);
      }
   }
   return funs;
}

void SymbolTable::getTemporaries(Identifiers& ids) const {
   typedef std::back_insert_iterator<Identifiers> inserter;
   std::copy(temporaries_.begin(),temporaries_.end(),inserter(ids));
}

string SymbolTable::makeTempName() {
  stringstream ss;
  ss << "t" << tempCount_++;
  return ss.str();
}
