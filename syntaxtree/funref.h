#ifndef FUNREF_H
#define FUNREF_H

#include "function.h"
#include "variable.h"
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
class Function;
class SymbolTable;

/**
 * Class representing a reference to a function variable.
 * @see VariableRef
 */
class FunctionRef: public Function {

   public:

      FunctionRef(Function* ref): reference_(ref) { }

      virtual ~FunctionRef() { }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      /**
       * Outputs the body, after the header
       */
      virtual ostream& prettyPrintBody(ostream& os = cout,int indent=0) const;

      /**
       * Also converts the entire body to a string
       */
      virtual string toStringBody() const;

      virtual string* name() const;

      virtual void setName(string* newName);

      virtual void accept(TreeVisitor* v);

      virtual Type returnType() const;


      virtual void parameters(Identifiers& output) const; 

      virtual Block* body() const;

      virtual SymbolTable* paramScope() const;

      virtual void setBody(Block* newBody);

      /**
       * Overload comparison operator: two references are equal of the
       * functions they point to are equal
       */
      virtual bool operator==(Function& other) const;

      void setReference(Function* newRef) {
	reference_ = newRef;
      }

      virtual void setIntermediateCode(Instructions* code);

      virtual Instructions* intermediateCode() const;

      virtual void setUniqueDeclarations(Identifiers* ids);
      
      virtual Identifiers* uniqueDeclarations() const;

   private:

      Function* reference_;

};

#endif
