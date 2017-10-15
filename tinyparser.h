#ifndef TINY_PARSER_H
#define TINY_PARSER_H

#include <iostream>
#include <sstream>

#include "exceptions.h"
#include "tinylexer.h"
#include "TinyParser.tab.h"

using namespace std;

/**
 * The Parser implemented through bisonpp which will parse
 * a given tiny source file.
 */

class TinyParser {

  public:

    TinyParser(const char* base, istream& is): base_(base), lexer_(&is) { }

    int yyparse();

    int lineNo() { return lexer_.lineNo(); }

    void where(ostream& os) {
      os << base_ << ".tiny - line " << lexer_.lineNo();
    }

    const char* base() { return base_; }

  private:

    void yyerror(const char* msg) {
      stringstream ss;
      ss << "Syntax error on line # " << lexer_.lineNo() << ": " << msg << endl;
      ss << "Last token was '" << lexer_.lastToken() << "'" << endl;
      throw ProcessError(ss.str());
    }

    int yylex() { return lexer_.yylex(yylval); }

  private:

    int             yynerrs;
    int             yychar;
    YYSTYPE         yylval;
    const char*     base_;
    TinyLexer       lexer_; /**< my lexical analyzer */

};

#endif
