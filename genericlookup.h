#ifndef GENERIC_LOOKUP_H
#define GENERIC_LOOKUP_H

#include "symboltable.h"
#include "syntaxtree/identifier.h"
#include "syntaxtree/aggregates.h"
#include "exceptions.h"

#include <string>
#include <map>

using namespace std;

typedef map<string*,Identifier*>::iterator VariableRecord;

class SymbolTable;

/**
 * GenericLookup is a function which looks up a variable in a given
 * symboltable mapping, it works with both Variables, Arrays and Functions
 */

//a generic pointer to isVariable, isFunction or isArray
typedef bool (Identifier::*query)() const;

template<class Kind, class WrongKindException, class ReferenceToKind>
Kind* symbolTableLookupKind(SymbolTable& table,string* name,query q) {

   VariableRecord end(table.symtab_->end());
   VariableRecord found = table.symtab_->find(name);
   if (found != end) {
      //check to see whether we have got the right kind
      Identifier* i(found->second);
      if (!(i->*q)())
	 throw WrongKindException(*name);
      else {
	 //return a proxy reference to the found variable
	 Kind* knd = dynamic_cast<Kind*>(found->second);
	 return new ReferenceToKind(knd);
      }
   }
   else {
      //continue search in higher scope if available
      if (table.parent_)
	 return symbolTableLookupKind<Kind,WrongKindException,ReferenceToKind>
	    (*table.parent_,name,q);
      //top scope, raise error
      else
	 throw UndefException(*name);
   }
}

#endif
