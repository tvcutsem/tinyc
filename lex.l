%{
/*
 * 	Lexical analyzer for Tiny
 */
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>	
#include	<string>
#include	<sstream>

#include "syntaxtree/expression.h"
#include "syntaxtree/lexp.h"
#include "syntaxtree/statement.h"
#include "syntaxtree/readstmt.h"
#include "syntaxtree/writestmt.h"
#include "syntaxtree/returnstmt.h"
#include "syntaxtree/ifstmt.h"
#include "syntaxtree/whilestmt.h"
#include "syntaxtree/binop.h"
#include "syntaxtree/function.h"
#include "syntaxtree/funcall.h"
#include "syntaxtree/variable.h"
#include "syntaxtree/assign.h"
#include "syntaxtree/number.h"
#include "syntaxtree/qchar.h"
#include "syntaxtree/aggregates.h"

#include "stringpool.h"
#include "exceptions.h"

#include "TinyParser.tab.h" /* token type definitions */
#include "tinylexer.h"

using namespace std;

extern int lineno;


void lex_init() { }

#define	YY_USER_INIT	lex_init();

%}

alpha			[A-Za-z_]
digit			[0-9]
alphanum		[A-Za-z0-9_]

%option yyclass="TinyLexer"
%option noyywrap

%x comment
%x linecomment

%%

[ \t]			break;		/* ignore white space */
"\n" 			++lineno;       /* increment line count */


int			return INT;	/* Keywords come before NAMEs */
void			return VOID;
if			return IF;
else			return ELSE;
return			return RETURN;
char			return CHAR;
length			return LENGTH;
while			return WHILE;
write			return WRITE;
read			return READ;
break			return BREAK;
continue		return CONTINUE;
for			return FOR;

{alpha}{alphanum}*	{
			StringPool* pool = StringPool::instance();
			lexval.name = pool->getString(strdup(yytext));
			return NAME;
			}

{digit}+		{
			lexval.value	= atoi(yytext);
			return NUMBER;
			}

"'"."'"			{
			char* charval = strdup(yytext);
			//yylval.charvalue = charval[1];
			lexval.charvalue = charval[1];
			return QCHAR;
			}

"("			return LPAR;
")"			return RPAR;
"{"			return LBRACE;
"}"			return RBRACE;
"["			return LBRACK;
"]"			return RBRACK;
"=="			return EQUAL;
"!="			return NEQUAL;
"="			return ASSIGN;
";"			return SEMICOLON;
","			return COMMA;
"+"			return PLUS;
"-"			return MINUS;
"*"			return TIMES;
"/"			return DIVIDE;
">"			return GREATER;
"<"			return LESS;
"!"			return NOT;

"/*"			BEGIN(comment);

<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++lineno; /* also advance line number in comments */
<comment>"*"+"/"        BEGIN(INITIAL); /* back to default state */


"//"			BEGIN(linecomment);
<linecomment>\n		{ ++lineno; BEGIN(INITIAL); }
<linecomment>[^\n]*	/* ignore rest of the line */

.			{
                        std::stringstream ss;
			ss << "illegal character on line " << lineno << ": ";
			ss << yytext;
			throw ProcessError(ss.str());
			}

%%

