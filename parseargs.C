#include "parseargs.h"

Arguments::Arguments():
  baseName(0),
  printParseTree(false),
  print3AddrCode(false),
  printOptCode(false),
  printDAG(false) { }

bool parseArguments(Arguments& args, int argc, char** argv) {
  if (argc < 2)
    return false;
  for (int count = 1; count < argc; count++) {
    char* arg = argv[count];
    if (arg[0] == '-') {
      if (!checkFlags(args, ++arg))
         return false;
    } else
      args.baseName = arg;
  }
  //return whether basename is filled in
  return (args.baseName != 0);
}

bool checkFlags(Arguments& args, char* flags) {
  int i(0);
  while (flags[i]) {
    switch (flags[i]) {
      case 'p':
	args.printParseTree = true;
	break;
      case 'c':
	args.print3AddrCode = true;
	break;
      case 'o':
	args.printOptCode = true;
	break;
      case 'd':
	args.printDAG = true;
	break;
      default:
	return false;
    }
    i++;
  }
  return true;
}

const char* USAGE("usage: tiny [options] baseName\nOptions include:\n"
    "  -p  prints parse tree of the source file\n"
    "  -c  prints the intermediate code\n"
    "  -o  prints optimized intermediate code\n"
    "  -d  prints the optimization DAG\n");
