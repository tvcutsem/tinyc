#include "print.h"
#include "syntaxtree/identifier.h"
#include "syntaxtree/function.h"

#define BARWIDTH 20

void printParseTree(Identifiers& ids, ostream& out) {
  for (Identifierit i=ids.begin();i!=ids.end();i++) {
    // *i is an Identifier*
    if ((*i)->isFunction()) {
      Function* f(dynamic_cast<Function*>(*i));
      out << f->toStringBody() << endl;
    } else
      out << (*i)->toString() << endl;
    out << endl;
  }
}


void printIntermediateCode(Identifiers& ids, ostream& out) {
  //print all generated function code
  for (Identifierit i=ids.begin(); i!=ids.end(); i++) {
    if ((*i)->isFunction()) {
      Function* f = dynamic_cast<Function*>(*i);
      string& funName(*(f->name()));
      int widthsize((BARWIDTH - funName.length())/2);
      printBars(widthsize,out);
      out << funName;
      printBars(widthsize,out);

      out << endl << (* f->intermediateCode());

      printBars(BARWIDTH,out);
      out << endl;
    }
  }
}


void printBars(int howMany, ostream& out) {
  if (howMany <= 0)
    return;
  while (howMany--)
    out << "-";
}
