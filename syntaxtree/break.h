#ifndef BREAK_H
#define BREAK_H

#include "statement.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * A Break Statement can be used to break away from the encompassing
 * while statement.
 */
class BreakStatement: public Statement {

   public:

      /**
       * prettyPrints a break statement to an ostream, with a given indentation,
       * which denotes the number of whitespace to precede the statement
       */
      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      /**
       * @return a string value representing the break statement
       */
      virtual string toString() const;

      /**
       * accept a TreeVisitor and do a double dispatch according to the
       * statement type
       */
      virtual void accept(TreeVisitor* v);

};

#endif
