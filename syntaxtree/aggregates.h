#ifndef AGGREGATES_H
#define AGGREGATES_H

#include <list>

#include "variable.h"
#include "statement.h"
#include "expression.h"

using namespace std;

/**
 * These type definitions merely provide a convenient way to create
 * aggregate structures, for example, when a function is called, its
 * parameters are represented as a list of expressions
 */

/**
 * A list of variable declarations
 * Used by for example a function: its formal parameters are Variables
 * Also used by a block which expects any number of variable declarations
 */
typedef list<Variable*> Variables;

/**
 * a list of functions 
 */
typedef list<Function*> Functions;

/**
 * a list of identifiers (variables, functions,...)
 */
typedef list<Identifier*> Identifiers;

/**
 * A list of statements
 * Used by a block which expects any number of statements
 */
typedef list<Statement*> Statements;

/**
 * A list of expressions
 * Used for the actual arguments of a function call
 */
typedef list<Expression*> Expressions;

//iterator shorthands
typedef Expressions::iterator Expressionit;
typedef Variables::iterator Variableit;
typedef Functions::iterator Functionit;
typedef Statements::iterator Statementit;
typedef Identifiers::iterator Identifierit;

#endif
