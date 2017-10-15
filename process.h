#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>

#include "visitors/typechecker.h"
#include "visitors/livenesschecker.h"
#include "visitors/constantfolder.h"
#include "visitors/ictranslator.h"
#include "visitors/codegenerator.h"
#include "symboltable.h"
#include "tinyparser.h"
#include "parseargs.h"

using namespace std;

/**
 * Process acts as the main pipeline of the compiler.
 *
 * The advantage of using the operator<< methods is that they receive
 * their visitor arguments through a parameter. This means the creator
 * of a process is free to pass any subclass of the visitor, thereby
 * changing the behaviour. For example, one could subclass a code optimizer
 * with a better (but slower) version, which can be done entirely transparently
 * from outside the process class.
 */

class Process {

  public:

    Process(Arguments& args, ostream& errstream = std::cerr);
    virtual ~Process();

    //the pipeline
    Process& operator<<(TinyParser& parser);
    Process& operator<<(TypeCheckVisitor& v);
    Process& operator<<(LivenessChecker& lc);
    Process& operator<<(ConstantFolder& cf);
    Process& operator<<(ICTranslator& translator);
    Process& operator<<(CodeGenerator& codegenerator);

    SymbolTable* globalScope() const { return globalScope_; }

  protected:

    /**
     * Check whether the function 'void tiny()' is well-defined
     */
    void checkTiny();

  private:

    Arguments& args_;           /**< settings passed in through command line */
    ostream& errstream_;	/**< default errorstream */
    SymbolTable* globalScope_;	/**< global symboltable */

};

#endif
