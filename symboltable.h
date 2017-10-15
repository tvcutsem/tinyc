#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "syntaxtree/function.h"
#include "syntaxtree/variable.h"
#include "syntaxtree/array.h"
#include "syntaxtree/identifier.h"
#include "syntaxtree/aggregates.h"
#include "syntaxtree/block.h"
#include "syntaxtree/varref.h"
#include "syntaxtree/arrayref.h"
#include "stringpool.h"
#include "exceptions.h"
#include "type.h"
#include "genericlookup.h"

#include <string>
#include <map>
#include <iostream>

using namespace std;

typedef map<string*,Identifier*> Symtab;
typedef map<string*,Identifier*>::iterator VariableRecord;

class FunctionRef;
class Block;

/**
 * SymbolTable defines a mapping between variable names and their
 * values.
 *
 * Values are instances of the Identifier class. These could be
 * Variables, Functions or Arrays. The SymbolTable also represents a
 * scope: functions have bodies as one of their attributes, the
 * body of a function will contain it's own SymbolTable so that
 * scopes can be nested. When doing translation or semantic
 * checking, one should then always use the most closely nested scope.
 */
class SymbolTable {

   public:

     /**
      * Construct a new symboltable using the given one as a parent
      * to delegate the search to in case a lookup fails.
      */
     SymbolTable(SymbolTable* parent);

     ~SymbolTable();

     SymbolTable* parent() const { return parent_; }

      /**
       * Looks up any identifier, this can either be a variable,
       * a function or an array.
       * @param name must be properly shared in the stringpool
       */
      Identifier* lookupIdentifier(string* name);

      /**
       * Looks up a given variable.
       * Use this function whenever a variable lookup needs to be performed
       * by the parser, ie when a variable name is encountered in the source
       * text.
       * @param name must be a properly shared name (in the stringpool)
       * @throw VariableExpected when the name does not represent a true variable
       * @throw UndefException if the name was not defined
       */
      Variable* lookupVariable(string* name);

      /**
       * looks up a function.
       * If the variable found is not a function, an exception will be
       * raised.
       * @param name the name of the function to look up
       * @throw FunctionExpected when the name does not represent a function 
       * @throw UndefException if the name was not defined
       */
      Function* lookupFunction(string* name);

      /**
       * looks up an array reference.
       * If the variable found is not an array, an exception will be
       * raised.
       * @param name the name of the array to look up
       * @throw ArrayExpected when the name does not represent an array 
       * @throw UndefException if the name was not defined
       */
      Array* lookupArray(string* name);

      /**
       * Declares a variable, ie adds it to the symbol table.
       * Use this function whenever a variable is declared in the
       * source code.
       * @param name the (shared) name of this variable
       * @param type this variabe's type
       * @see Type
       */
      Variable* declareVariable(string* name,Type type);

      /**
       * patchRecursion is called when the formals and body of a
       * function are known.
       *
       * They must be backpatched because the function must already be
       * declared in advance to allow recursive references in the function body
       */
      void patchRecursion(string* name,SymbolTable* formals,Block* body);

      /**
       * Declares a function, from now on to be used by the program.
       * @param name the shared name of the function to declare
       * @param returnType the returntype of the function
       */
      Function* declareFunction(string* name,Type returnType);

      /**
       * Declares an array 
       * @param name the shared name of the array to declare
       * @param base the base type of the array
       * @param lengths a list of expressions representing the length of
       * each dimension.
       */
      Array* declareArray(string* name,Type base,Expressions* lengths);

      /**
       * Declares a 'temporary' variable. Mainly used in the
       * three address (intermediate) code. This functions ensures
       * the variable will be unique in this scope.
       */
      Variable* declareTempVariable(Type type);

      /**
       * Utility function to check the contents of a scope
       */
      ostream& printDeclarations(ostream& os = cout);

      /**
       * Returns all declared identifiers stored in the list given 
       */
      void declarations(Identifiers& ids) const;

      /**
       * Is the given identifier a temporary declared in this scope?
       */
      bool isTemporary(Identifier* id) const;

      /**
       * Inserts all variables declared in this scope into the given
       * variable list.
       * Useful for retrieving the formal arguments of a function
       * (which are represented by a symboltable)
       */
      Variables& retrieveVariables(Variables& vars) const; 

      /**
       * Inserts all functions declared in this scope into the given
       * list.
       */
      Functions& retrieveFunctions(Functions& funs) const; 

      /**
       * Adds all declared temporary variables in this scope to the
       * given identifier list.
       */
      void getTemporaries(Identifiers& ids) const;

      //friend declaration necessary because this function requires
      //access to my internals

      friend Variable*
	     symbolTableLookupKind<Variable,VariableExpected,VariableRef>
	     	(SymbolTable&,string*,query);
      friend Function*
	     symbolTableLookupKind<Function,FunctionExpected,FunctionRef>
	     	(SymbolTable&,string*,query);
      friend Array*
	     symbolTableLookupKind<Array,ArrayExpected,ArrayRef>
	     	(SymbolTable&,string*,query);

   private:

      //no need to copy or assign entire SymbolTables
      SymbolTable& operator=(SymbolTable& s);
      SymbolTable(SymbolTable& s);

      /**
       * Generates a new temporary name
       */
      string makeTempName();

   private:

      SymbolTable* parent_;
      Symtab* symtab_;
      int tempCount_;
      Variables temporaries_;
   
};


#endif
