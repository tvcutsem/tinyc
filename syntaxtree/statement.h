#ifndef STATEMENT_H
#define STATEMENT_H

#include "node.h"
#include "../visitors/treevisitor.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * Abstract superclass for all statements, specifies the interface
 * Used as a 'placeholder'
 */
class Statement: public virtual Node {

   public:

      Statement() { }

      virtual ~Statement() { }

};

#endif
