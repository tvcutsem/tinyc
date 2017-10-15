#ifndef CONTINUE_H
#define CONTINUE_H

#include "statement.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * A Continue Statement can be used to immediately start the next iteration
 * in a while statement.
 */

class ContinueStatement: public Statement {

   public:

      /**
       * prettyPrints a continue statement to an ostream, with a given indentation,
       * which denotes the number of whitespace to precede the statement
       */
      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      /**
       * @return a string value representing the continue statement
       */
      virtual string toString() const;

      /**
       * accept a TreeVisitor and do a double dispatch according to the
       * statement type
       */
      virtual void accept(TreeVisitor* v);

};

#endif
