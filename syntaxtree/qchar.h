#ifndef QCHAR_H
#define QCHAR_H

#include <iostream>
#include <string>

#include "constant.h"
#include "../visitors/treevisitor.h"
#include "../type.h"

using namespace std;

class QChar: public Constant {

   public:

      //constructor
      QChar(char value): value_(value) { }

      //desctructor
      virtual ~QChar() { }

      operator char() { return getValue(); }

      inline char getValue() const { return value_; }

      inline void setValue(char newVal) { value_ = newVal; }

      virtual ostream& prettyPrint(ostream& os = cout,int indent=0) const;

      virtual string toString() const;

      virtual void accept(TreeVisitor* v) {
	 v->visitQChar(this);
      }

      virtual Type type() const { return char_t; }

      virtual bool isFalse() const { return (value_ == 0); }

      virtual bool isNumber() const { return false; }

      virtual bool isQChar() const { return true; }

      //accessors
      char value() const { return value_; }

   private:
      char value_;

};

#endif
