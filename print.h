#include <iostream>

#include "syntaxtree/aggregates.h"

/**
 * Print out the parse-tree of all functions.
 */
extern void printParseTree(Identifiers& ids, ostream& out);


/**
 * Print out the generated three address code of all given functions.
 * @param out everything is written to out
 */
extern void printIntermediateCode(Identifiers& ids, ostream& out);


/**
 * Silly function to print out a number of '-' signs.
 */
extern void printBars(int howMany, ostream& out);
