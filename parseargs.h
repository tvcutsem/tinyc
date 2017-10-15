#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

/**
 * @file parseargs.h
 * contains utilities to parse the command-line
 */

/**
 * Small structure to store command-line input
 */
struct Arguments {

  Arguments();

  /** name of the file to compile */
  const char* baseName;

  /** user wants to see the parse tree? */
  bool printParseTree;

  /** user wants to see the intermediate code? */
  bool print3AddrCode;

  /** user wants to see the optimized code? */
  bool printOptCode;

  /** user wants to see the optimization DAG? */
  bool printDAG;

};

/**
 * parses the command-line input
 * @param args the arguments object to which the results should be written
 * @return whether the arguments were valid
 */
extern bool parseArguments(Arguments& args, int argc, char** argv);

/**
 * Look for flags in the input string.
 * @param args store the results in the given Arguments struct
 * @return false if an unknown flag was encountered
 */
extern bool checkFlags(Arguments& args, char* flags);

/**
 * Usage string.
 */
extern const char* USAGE;

#endif
