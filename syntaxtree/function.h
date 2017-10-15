#ifndef FUNCTION_H
#define FUNCTION_H

#include "variable.h"
#include "identifier.h"
#include "statement.h"
#include "aggregates.h"
#include "block.h"
#include "../type.h"
#include "../symboltable.h"
#include "../visitors/treevisitor.h"
#include "../instructions/instructions.h"

#include <list>
#include <iostream>
#include <string>

using namespace std;

class Block;
class SymbolTable;

/**
 * Common abstract superclass for 'real' function objects (FunctionDecls)
 * and their Proxies (FunctionRefs)
 */
class Function: public Identifier {

   public:

      virtual ~Function() { }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const = 0;

      virtual string toString() const = 0;

      /**
       * Outputs the body, after the header
       */
      virtual ostream& prettyPrintBody(ostream& os = cout,int indent=0) const = 0;

      /**
       * Also converts the entire body to a string
       */
      virtual string toStringBody() const = 0;

      virtual Type type() const { return fun_t; }

      virtual Type returnType() const = 0;

      virtual bool isVariable() const { return false; }

      virtual bool isFunction() const { return true; }

      virtual bool isArray() const { return false; }

      /**
       * returns all formal arguments via the reference parameter
       */
      virtual void parameters(Identifiers& output) const = 0; 

      virtual Block* body() const = 0;

      virtual SymbolTable* paramScope() const = 0;

      virtual void setBody(Block* newBody) = 0;

      virtual void accept(TreeVisitor* v) = 0;

      /**
       * defines equality for functions
       */
      virtual bool operator==(Function& other) const = 0;

      /**
       * Sets the generated Three Address Code of this function.
       */
      virtual void setIntermediateCode(Instructions* code) = 0;

      virtual Instructions* intermediateCode() const  = 0;

      /**
       * Sets the uniquely named identifiers, which were all
       * declared in blocks inside this function.
       * The Identifiers are shared with the symboltable and should
       * thus not be deleted by the function!
       */
      virtual void setUniqueDeclarations(Identifiers* ids) = 0;
      
      virtual Identifiers* uniqueDeclarations() const = 0;

};

#endif
