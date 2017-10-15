#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include "../syntaxtree/aggregates.h"

/**
 * Abstract interface to which every code generator for tiny should adhere
 */

class CodeGenerator {

  public:

    /**
     * Generate code for all given identifiers (variables, arrays and functions)
     * The passed variables and arrays are globals.
     */
    virtual void generateCode(Identifiers& ids) = 0;

    /**
     * Virtual destructor.
     */
    virtual ~CodeGenerator() { }

};

#endif
