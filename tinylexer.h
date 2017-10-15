#ifndef TINY_LEXER_H
#define TINY_LEXER_H

#include <iostream>
#include <FlexLexer.h>

#include "syntaxtree/aggregates.h"

class Expression;
class LExpression;
class Identifier;
class Function;
class Array;
class Number;
class QChar;
class BinaryOp;
class Statement;
class Assignment;
class Block;

#include "TinyParser.tab.h"

using namespace std;

class TinyLexer: public yyFlexLexer {
  public:

    TinyLexer(istream* yyin = 0, ostream* yyout = 0):
      yyFlexLexer(yyin,yyout),lineno(1) { }

    virtual int yylex();

    int yylex(YYSTYPE& v) {
      int t = yylex();
      v = lexval;
      return t;
    }

    char* lastToken() { return yytext; }

    int lineNo() { return lineno; }

  private:
    YYSTYPE	lexval;
    int 	lineno;

};

#endif
