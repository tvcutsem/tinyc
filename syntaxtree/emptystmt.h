#ifndef EMPTY_STATEMENT_H
#define EMPTY_STATEMENT_H

#include "statement.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * Fake statement to allow for eg. an empty else branche
 * by using this class as a 'placeholder', no explicit tests
 * for empty branches need to be placed
 */

class EmptyStatement: public Statement {

   public:
      //constructor
      EmptyStatement() { }

      //desctructor
      virtual ~EmptyStatement() { }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const {
	 return os;
      }

      virtual string toString() const { return ""; }

      virtual void accept(TreeVisitor* v) {
	 v->visitEmptyStatement(this);
      }

};

#endif
