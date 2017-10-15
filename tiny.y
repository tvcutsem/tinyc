%{
/**
 * @file tiny.y	
 * Parser for the Tiny language.
 */

#include <stdio.h>	
#include <stdlib.h>	
#include <string>	
#include <iostream>	

//Abstract Syntax Tree files
#include "syntaxtree/expression.h"
#include "syntaxtree/function.h"
#include "syntaxtree/array.h"
#include "syntaxtree/arrayaccess.h"
#include "syntaxtree/lexp.h"
#include "syntaxtree/statement.h"
#include "syntaxtree/readstmt.h"
#include "syntaxtree/writestmt.h"
#include "syntaxtree/returnstmt.h"
#include "syntaxtree/ifstmt.h"
#include "syntaxtree/whilestmt.h"
#include "syntaxtree/binop.h"
#include "syntaxtree/unop.h"
#include "syntaxtree/funcall.h"
#include "syntaxtree/variable.h"
#include "syntaxtree/assign.h"
#include "syntaxtree/number.h"
#include "syntaxtree/qchar.h"
#include "syntaxtree/lengthexp.h"
#include "syntaxtree/aggregates.h"
#include "syntaxtree/break.h"
#include "syntaxtree/continue.h"

#include "operators/operators.h"
#include "type.h"
#include "symboltable.h"

#include "tinyparser.h"

using namespace std;

SymbolTable* 	currentScope;	/* the current symbol table (and thus scope) */

%}

%union	{
	string*		name;
	char		charvalue;
	int		value;
	Expression*	exp;
	Expressions*	exps;
	LExpression*	lexp;
	Identifier*	var;
	Function*	fun;
	Array*		array;
	Variables*	vars;
	Number*		number;
	QChar*		qchar;
	BinaryOp*	binop;
	Statement*	stmt;
	Statements*	stmts;
	Assignment*	assign;
	Type		type;
	Block*		block;
	}

%token	NAME	/* the name of a variable, function, ... */
%token	NUMBER 	/* a sequence of digits */
%token	QCHAR	/* a character between single quotes */

//keywords
%token	IF ELSE RETURN WRITE READ LENGTH WHILE CHAR INT VOID BREAK CONTINUE FOR
//statements
%token	LPAR RPAR LBRACE RBRACE LBRACK RBRACK ASSIGN SEMICOLON COMMA
//operators
%token	PLUS MINUS TIMES DIVIDE EQUAL GREATER LESS NOT NEQUAL

%type	<name>	    NAME	//string*
%type	<value>	    NUMBER	//int
%type	<charvalue> QCHAR	//char

%type	<exps>	pars 		//Expressions
%type	<block>	block 		//Block
%type	<type>  type		//Type
%type	<stmts>	statements more_statements //Statements
%type	<stmt>	statement 	//Statement
%type	<lexp>	lexp 		//LExpression
%type	<exp>	exp		//Expression
%type	<var> 	var 		//Identifier
%type	<fun> 	fun 		//Function
%type	<array> array 		//Array
%type	<exps>	indices

/* associativity and precedence */

%nonassoc	LOW  /* dummy to force shift instead of reduce on ELSE */
%nonassoc	ELSE 

%right		ASSIGN

%nonassoc	EQUAL	NEQUAL
%nonassoc	GREATER LESS

%left		PLUS	MINUS
%left		TIMES	DIVIDE
%left		UMINUS	/* dummy for context precedence  */
%nonassoc	NOT
%left		LBRACK

%%

program		: declarations
		;

declarations	: declaration declarations { }
		| /* empty */ { } 
		;

declaration	: fun_declaration 
		| var_declaration
		;

fun_declaration	: type NAME
 		  { currentScope->declareFunction($2,$1);
		    currentScope = new SymbolTable(currentScope); }
		  LPAR formal_pars RPAR
		  block
		  { 
		    SymbolTable* funscope = currentScope;
		    currentScope = currentScope->parent();
		    currentScope->patchRecursion($2,funscope,$7); 
	 	  }
		;

formal_pars	: more_pars
		| /* empty */ { }
		;

more_pars	: formal_par COMMA formal_pars { }
		| formal_par { }
		;

formal_par	: type NAME
		  { currentScope->declareVariable($2,$1); }
		| type LBRACK indices NAME
		  {
		    currentScope->declareArray($4,$1,$3);
		  }
		;

block		: LBRACE { currentScope = new SymbolTable(currentScope); }
		  var_declarations statements RBRACE
		  {
		    $$ = new Block(currentScope,$4);
		    //close the current scope
		    currentScope = currentScope->parent();
		  }
		;

var_declarations: var_declaration var_declarations
		| /* empty */
		;

var_declaration	: type NAME SEMICOLON
		  {
		    currentScope->declareVariable($2,$1);
		  }
		| type LBRACK indices NAME SEMICOLON
		  {
		    currentScope->declareArray($4,$1,$3);
		  }
		;

type		: INT { $$ = int_t; }
		| CHAR { $$ = char_t; }
		| VOID { $$ = void_t; }
		;

statements	: more_statements
		| /* empty */ { $$ = new Statements; }
		;

more_statements : statement SEMICOLON more_statements
                  {
		    $3->push_front($1);
		    $$ = $3;
		  }
		| statement
		  {
		    //the list of statements is parsed back to front,
		    //ie the last element (this) is reduced first
		    Statements* s = new Statements;
		    s->push_front($1);
		    $$ = s;
		  }
		;

statement	: IF LPAR exp RPAR statement %prec LOW
		  { $$ = new IfStatement($3,$5); }
		| IF LPAR exp RPAR statement ELSE statement
		  { $$ = new IfStatement($3,$5,$7); }
		| WHILE LPAR exp RPAR statement
		  { $$ = new WhileStatement($3,$5); } 
		| lexp ASSIGN exp
		  { $$ = new Assignment($1,$3); }
		| RETURN exp
		  { $$ = new ReturnStatement($2); }
		| fun LPAR RPAR
		  { $$ = new Funcall($1,new Expressions,false); }
		| fun LPAR pars RPAR
		  { $$ = new Funcall($1,$3,false); }
		| block
		  { $$ = $1; }
		| WRITE exp
		  { $$ = new WriteStatement($2); }
		| READ lexp
		  { $$ = new ReadStatement($2); }
		| BREAK
		  { $$ = new BreakStatement; }
		| CONTINUE
		  { $$ = new ContinueStatement; }
		| FOR LPAR statement SEMICOLON exp SEMICOLON statement RPAR statement
		  { /* translate                to       init
		       for (init, cond, inc)             while(cond)
		         body                              body
		                                           inc
		    */
		      Statements* outerstmts = new Statements;
		      outerstmts->push_back($3); //init code

		      Statements* whilestmts = new Statements;
		      whilestmts->push_back($9); //while body
		      whilestmts->push_back($7); //increment code
		      Block* whilebody = new Block(new SymbolTable(currentScope),
		                                   whilestmts);

		      WhileStatement* whilestmt = new WhileStatement($5,whilebody);
		      outerstmts->push_back(whilestmt);
		      $$ = new Block(new SymbolTable(currentScope), outerstmts);
		  }
		;

pars		: exp
                {
		  //the list is built in reverse, this is the last
		  //element, but reduced first, so create the list now
		  Expressions* exps = new Expressions;
		  exps->push_front($1);
		  $$ = exps;
		}
		| exp COMMA pars { $3->push_front($1); $$ = $3; }
		;

indices		: exp RBRACK
		{
		  Expressions* expr = new Expressions;
		  expr->push_front($1);
		  $$ = expr;
		}
		| exp RBRACK LBRACK indices
		{
		  $4->push_front($1);
		  $$ = $4;
		}
		;

lexp		: var { $$ = $1; }
		| array LBRACK indices { $$ = new ArrayAccess($1,$3); }
		;

exp		: lexp { $$ = $1; }
		| exp MINUS   exp { $$ = new BinaryOp($1,$3,operatorMinus); }
		| exp PLUS    exp { $$ = new BinaryOp($1,$3,operatorPlus); } 
		| exp TIMES   exp { $$ = new BinaryOp($1,$3,operatorTimes); } 
		| exp DIVIDE  exp { $$ = new BinaryOp($1,$3,operatorDivides); } 
		| exp EQUAL   exp { $$ = new BinaryOp($1,$3,operatorEqual); } 
		| exp NEQUAL  exp { $$ = new BinaryOp($1,$3,operatorNEqual); } 
		| exp GREATER exp { $$ = new BinaryOp($1,$3,operatorGT); } 
		| exp LESS    exp { $$ = new BinaryOp($1,$3,operatorLT); } 
		| MINUS exp %prec UMINUS { $$ = new UnaryOp($2,operatorUMinus); }
		| NOT exp { $$ = new UnaryOp($2,operatorNegate); }
		| LPAR exp RPAR { $$ = $2; }
		| fun LPAR RPAR      { $$ = new Funcall($1,new Expressions,true); }
		| fun LPAR pars RPAR { $$ = new Funcall($1,$3,true); }
		| NUMBER { $$ = new Number($1); }
		| QCHAR  { $$ = new QChar($1); }
		| LENGTH lexp { $$ = new LengthExpression($2); }
		;

var		: NAME
		  { 
	 	    $$ = currentScope->lookupIdentifier($1);
		  }
		;

fun		: NAME
		  {
		    $$ = currentScope->lookupFunction($1);
		  }
		;

array		: NAME
		  {
		    $$ = currentScope->lookupArray($1);
		  }
		;

%%
