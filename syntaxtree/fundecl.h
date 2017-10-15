#ifndef FUNDECL_H
#define FUNDECL_H

#include <list>
#include <iostream>
#include <string>

#include "variable.h"
#include "vardecl.h"
#include "statement.h"
#include "aggregates.h"
#include "block.h"
#include "function.h"
#include "../type.h"
#include "../symboltable.h"
#include "../visitors/treevisitor.h"
#include "../instructions/instructions.h"

using namespace std;

class Block;

class SymbolTable;

/**
 * class representing a function declaration.
 * contains all information regarding function scope, name,
 * returntype and arguments
 */

class FunctionDec: public Function {

   public:
      //constructor
      FunctionDec(string* name,Type rettype);

      //desctructor
      virtual ~FunctionDec(); 

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

      /**
       * used to backpatch a function. Necessary because of recursive
       * calls
       */
      void patch(SymbolTable* pars, Block* body);

      virtual string* name() const { return vardecl_->name(); }

      virtual void setName(string* newName) { vardecl_->setName(newName); }

      virtual void accept(TreeVisitor* v) {
	 v->visitFunction(this);
      }

      //accessors
      virtual Type returnType() const { return returnType_; }

      /**
       * @param output the list where all retrieved parameters will be stored
       * @return all formal arguments of this function
       * A formal argument is an Identifier: either a variable
       * denoting a primitive value, or an array
       */
      virtual void parameters(Identifiers& output) const; 

      virtual Block* body() const { return body_; }

      virtual SymbolTable* paramScope() const { return pars_; }

      virtual void setBody(Block* newBody);

      virtual bool operator==(Function& other) const;

      virtual void setIntermediateCode(Instructions* code);

      virtual Instructions* intermediateCode() const {
	return icode_;
      }

      virtual void setUniqueDeclarations(Identifiers* ids);
      
      virtual Identifiers* uniqueDeclarations() const {
	return uniqueDeclarations_;
      }

   private:

      Variable* vardecl_; //the variable declaration of this function
      Type returnType_; //the returntype
      SymbolTable* pars_; //this function's declared parameters
      Block* body_;  //the body of statements to execute

      Instructions* icode_; //generated intermediate code
      Identifiers* uniqueDeclarations_; //uniquely named declared identifiers

};

#endif
