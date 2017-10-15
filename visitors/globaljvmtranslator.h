#ifndef GLOBAL_JVM_TRANSLATOR_H
#define GLOBAL_JVM_TRANSLATOR_H

#include "treevisitor.h"
#include "codegenerator.h"
#include "../instructions/instructions.h"
#include "../syntaxtree/aggregates.h"
#include "../symboltable.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

//forward declarations

class Variable;
class Function;
class Array;

/**
 * The Global JVM Generator is responsible for translating global Tiny language
 * constructs, like global variable, array and functiondeclarations to JVM assembler
 * statements.
 */

class JVMGenerator: public TreeVisitor, public CodeGenerator {

  public:

    /**
     * @param className name of the Java class to generate
     * @param codestream stream to which the java assembler is written
     * @param globalInstr three-address code generated for global code
     * @param globalScope the global symboltable containing global declarations
     */
    JVMGenerator(string& className, ostream& codestream,
		 Instructions* globalInstr, SymbolTable* globalScope);

    /**
     * Traverses all elements and maps them to JVM assembler
     */
    virtual void generateCode(Identifiers& ids);

    /**
     * type name;
     *  ->
     * field static javaType name
     */
    virtual void visitVariable(Variable* var);

    /**
     * returnType name(type1 arg1, ... typeN argN) { ... }
     *  ->
     * handled by a JVMTranslator
     * @see JVMTranslator
     */
    virtual void visitFunction(Function* fun);

    /**
     * type[...] name;
     *  ->
     * field static javaType[] name;
     *
     * Code necessary for the allocation of the array will be generated in
     * the main function.
     */
    virtual void visitArray(Array* array);

  private:

    /**
     * creates a function so that a JVMTranslator can map the function to JVM
     * assembler. The body of the function consists of the global three address
     * code + a call to tiny()
     */
    void generateMain();

    string&       className_;
    ostream&      codestream_;
    Instructions* globalInstructions_;
    SymbolTable*  globalScope_;

    /**
     * Stream for functions only, used because all field declarations should
     * come first, thus, this stream is appended to the real outputstream.
     */
    ostringstream functionstream_;

};

#endif
