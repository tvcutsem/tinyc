#ifndef ARRAY_ACCESS_H
#define ARRAY_ACCESS_H

#include "lexp.h"
#include "aggregates.h"
#include "../type.h"
#include "../visitors/treevisitor.h"
#include "array.h"

#include <iostream>
#include <string>

using namespace std;

/**
 * An Array Access represents something syntactically written as
 * array[index], where array denotes an Array variable and index is
 * an arbitrary expression which should result in an integer
 *
 * Because multidimensional arrays are also supported, syntax like
 * array[idx1,idx2] is also permitted, so we can have multiple indices
 */

class ArrayAccess: public LExpression {

   public:
      //constructor
      ArrayAccess(Array* array,Expressions* indices);

      //desctructor
      virtual ~ArrayAccess();

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual bool isArrayAccess() const { return true; }

      virtual void accept(TreeVisitor* v);

      /**
       * The type of an Array Access is the base type of its array, formally:
       * array:t ^ idx:int => array[idx]:t
       */
      virtual Type type() const;

      virtual Array* array() const { return array_; }

      virtual Expressions& indices() const { return *indices_; }

   private:
      Array* array_;
      Expressions* indices_;

};

#endif
