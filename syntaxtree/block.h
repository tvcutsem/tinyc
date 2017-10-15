#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <string>

#include "statement.h"
#include "aggregates.h"
#include "../symboltable.h"
#include "../visitors/treevisitor.h"

class Function;
class SymbolTable;

using namespace std;

/**
 * A block is actually a container for multiple statements, but, it also
 * defines a scope, which is a reference to the symboltable containing
 * all declarations made in this block
 */
class Block: public Statement {

   public:

      Block(SymbolTable* scope,Statements* statements);

      virtual ~Block();

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitBlock(this);
      }

      //accessors
      SymbolTable* scope() const { return scope_; }
      Statements*  body()  const { return statements_; }

   private:

	SymbolTable* scope_;
	Statements*  statements_;

};

#endif
