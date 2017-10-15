#include "evaluate.h"

#include <stdexcept>

Constant* eval(Constant* c1, Constant* c2,operation o) {
  int op1;
  int op2;
  //indicates whether constant is a QChar
  bool op1char = false;
  bool op2char = false;

  if (c1->isNumber()) {
    Number* n = dynamic_cast<Number*>(c1);
    op1 = n->value();
  } else if (c1->isQChar()) {
    QChar* c = dynamic_cast<QChar*>(c1);
    op1 = c->value();
    op1char = true;
  } else {
    throw logic_error("logic error in eval: constant is neither number nor char");
  }

  if (c2->isNumber()) {
    Number* n = dynamic_cast<Number*>(c2);
    op2 = n->value();
  } else if (c2->isQChar()) {
    QChar* c = dynamic_cast<QChar*>(c2);
    op2 = c->value();
    op2char = true;
  } else {
    throw logic_error("logic error in eval: constant is neither number nor char");
  }

  if (op1char && op2char)
    return new QChar((*o)(op1,op2));
  else
    return new Number((*o)(op1,op2));
}
