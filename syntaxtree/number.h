#ifndef NUMBER_H
#define NUMBER_H

#include <iostream>
#include <string>

#include "constant.h"
#include "../visitors/treevisitor.h"
#include "../type.h"

using namespace std;

class Number: public Constant {

   public:

      Number(int value): value_(value) { }

      virtual ~Number() { }

      operator int() { return getValue(); }

      int getValue() const { return value_; }

      void setValue(int newVal) { value_ = newVal; }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitNumber(this);
      }

      virtual Type type() const { return int_t; }

      virtual bool isFalse() const { return (value_ == 0); }

      virtual bool isNumber() const { return true; }

      virtual bool isQChar() const { return false; }

      //accessors
      int value() const { return value_; }


   private:
      int value_;

};

#endif
