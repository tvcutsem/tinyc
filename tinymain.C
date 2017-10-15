#include "process.h"
#include "visitors/typechecker.h"
#include "visitors/livenesschecker.h"
#include "visitors/constantfolder.h"
#include "visitors/ictranslator.h"
#include "visitors/globaljvmtranslator.h"
#include "stringpool.h"
#include "symboltable.h"
#include "tinyparser.h"
#include "instructions/instructions.h"
#include "parseargs.h"

#include <string>
#include <sstream>
#include <iostream>	
#include <stdexcept>

using namespace std;

extern SymbolTable* currentScope;

/**
 * The main function of the Tiny Compiler
 */
int main(int argc,char *argv[]) {
  Arguments args;
  if (!parseArguments(args,argc,argv)) {
    cerr << USAGE << endl;
    return 1;
  }

  Process compiler(args,std::cerr);
  TinyParser parser(args.baseName,std::cin);
  Node::setParser(parser);

  TypeCheckVisitor typechecker;
  LivenessChecker  livenesschecker;
  ConstantFolder   constantfolder;

  Instructions*    globalCodestream(new Instructions);
  ICTranslator     ictranslator(globalCodestream,compiler.globalScope());

  string baseName(args.baseName);
  JVMGenerator jvmtranslator(baseName, std::cout,
                             globalCodestream, compiler.globalScope());

  try {
    compiler << parser
             << typechecker
             << livenesschecker
             << constantfolder 
             << ictranslator
             << jvmtranslator;
  }
  //thrown when the above process is interrupted
  catch(ProcessError& e) {
    cerr << e.what() << endl;
    return 1;
  }
  catch(Exception& e) {
    parser.where(std::cerr);
    cerr << ": " << e.what() << endl;
    return 1;
  } catch (std::logic_error& e) {
    cerr << "Logic Error: " << e.what() << endl;
    return 1;
  }
  return 0;
}
