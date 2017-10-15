#include "globaljvmtranslator.h"
#include "jvmtranslator.h"
#include "tiny2java.h"
#include "../stringpool.h"
#include "../syntaxtree/variable.h"
#include "../syntaxtree/function.h"
#include "../syntaxtree/array.h"
#include "../syntaxtree/block.h"
#include "../instructions/calli.h"
#include "../instructions/funregister.h"
#include "../syntaxtree/fundecl.h"

string* MAIN_NAME(StringPool::instance()->getString("main"));
string* TINY_NAME(StringPool::instance()->getString("tiny"));
string*  ARG_NAME(StringPool::instance()->getString("args"));

JVMGenerator::JVMGenerator(string& className,
                           ostream& codestream,
    			   Instructions* globalInstr,
			   SymbolTable* globalScope):
	className_         (className),
	codestream_        (codestream),
	globalInstructions_(globalInstr),
	globalScope_       (globalScope) { }

void JVMGenerator::generateCode(Identifiers& ids) {
  codestream_ << "public class " << className_ << " {" << endl;
  for (Identifierit id = ids.begin(); id != ids.end() ; id++) {
     (*id)->accept(this);
  }
  generateMain();
  codestream_ << functionstream_.str();
  codestream_ << "}" << endl;
}

void JVMGenerator::visitVariable(Variable* var) {
  codestream_ << "field static " << type2javaType(var) << " "
              << *(var->name()) << endl << endl;;
}

void JVMGenerator::visitFunction(Function* fun) {
  JVMTranslator functionTranslator(*fun->name(),className_,functionstream_);
  fun->accept(&functionTranslator);
  functionstream_ << endl;
}

void JVMGenerator::visitArray(Array* array) {
  codestream_ << "field static " << type2javaType(array) << " "
              << *(array->name()) << endl << endl;
}

void JVMGenerator::generateMain() {
  //add a call to Tiny()
  Function* tiny = globalScope_->lookupFunction(TINY_NAME);
  (*globalInstructions_) << (new CallI(new FunRegister(tiny),0));

  //create the main function
  Function* main = globalScope_->declareFunction(MAIN_NAME,void_t);
  SymbolTable* args(new SymbolTable(globalScope_));
  args->declareArray(ARG_NAME,string_t,new Expressions);
  globalScope_->patchRecursion(MAIN_NAME,args,
      			       new Block(new SymbolTable(args),new Statements));
  main->setIntermediateCode(globalInstructions_);
  Identifiers* uniqueGlobals = new Identifiers;
  main->setUniqueDeclarations(uniqueGlobals);

  //map it to JVM
  JVMTranslator mainTranslator(*MAIN_NAME,className_,codestream_);
  main->accept(&mainTranslator);
}
