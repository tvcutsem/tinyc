#include <iostream>

#include "process.h"
#include "exceptions.h"
#include "stringpool.h"
#include "type.h"
#include "instructions/bbmapping.h"
#include "syntaxtree/function.h"
#include "visitors/livenessexc.h"
#include "optimization/usecount.h"
#include "print.h"

extern SymbolTable* currentScope; 

string* MAIN_FUNCTION_NAME(StringPool::instance()->getString("tiny"));
const char* ILLEGAL_MAIN_TYPESIG("Illegal type signature for function tiny");
const char* MAIN_FUNCTION_NOT_FOUND("Could not find the tiny main function");


Process::Process(Arguments& args, ostream& errstream):
	args_(args), errstream_(errstream),
	globalScope_(new SymbolTable(0)) { }


Process::~Process() {
  delete globalScope_;
  StringPool::destroy();
}


//PARSING
Process& Process::operator<<(TinyParser& parser) {
  currentScope = globalScope_;
  try {
    parser.yyparse();
  } catch (Exception& e) {
    parser.where(errstream_);
    errstream_ << ": " << e.what() << endl;
    throw ProcessError("Errors reported during parsing");
  }

  if (args_.printParseTree) {
    Identifiers ids;
    globalScope_->declarations(ids);
    printParseTree(ids,errstream_);
  }
  
  return *this;
}


//TYPECHECKING
Process& Process::operator<<(TypeCheckVisitor& v) {

  //make sure there is a void tiny() function
  checkTiny();

  Identifiers ids;
  globalScope_->declarations(ids);
  for (Identifierit i=ids.begin();i!=ids.end();i++) {
    // *i is an Identifier*
    (*i)->accept(&v);
  }

  //errors reported during typechecking?
  if (v.numerrors()>0) {
    TypeCheckErrors& errs=v.errors();
    for (TypeCheckErrit err=errs.begin(); err!=errs.end(); err++) {
      // *err is a TypeCheckError*
      (*err)->report(errstream_);
      errstream_ << endl;
    }
    throw ProcessError("Errors reported during typechecking");
  }
  return *this;
}


//CODE ANALYSIS
Process& Process::operator<<(LivenessChecker& lc) {
  Identifiers ids;
  globalScope_->declarations(ids);
  ControlErrors errlist;
  for (Identifierit i=ids.begin();i!=ids.end();i++) {
    // *i is an Identifier*
    lc.check(*i,errlist);

    //print out all the errors if there are any
    if (errlist.size()) {
      errstream_ << "In function " << *(*i)->name() << ": " << endl;
      for (ControlErrorit err = errlist.begin(); err != errlist.end(); err++) {
	// *err is a ControlError
	// order the controlerror to report itself using the error stream
	err->report(errstream_);
      }
      errlist.clear();
      throw ProcessError("Errors reported during code analysis");
    }
  }
  return *this;
}


//CONSTANT FOLDING
Process& Process::operator<<(ConstantFolder& cf) {
  Identifiers ids;
  globalScope_->declarations(ids);
  try {
    for (Identifierit i=ids.begin();i!=ids.end();i++) {
      // *i is an Identifier*
      (*i)->accept(&cf);
    }
  } catch (Exception& e) {
    errstream_ << e.what() << endl;;
    throw ProcessError("Errors reported during syntax tree folding");
  }
  return *this;
}


//INTERMEDIATE CODE GENERATION
Process& Process::operator<<(ICTranslator& translator) {
  Identifiers ids;
  globalScope_->declarations(ids);
  try {
    for (Identifierit i=ids.begin();i!=ids.end();i++) {
      // *i is an Identifier*
      (*i)->accept(&translator);
    }

    if (args_.print3AddrCode) {
      errstream_ << "generated intermediate code: " << endl;
      printIntermediateCode(ids,errstream_);
    }

    //optimize the code
    for (Identifierit i=ids.begin();i!=ids.end();i++) {
      if ((*i)->isFunction()) {
	Function* f = dynamic_cast<Function*>(*i);
        Instructions* optimized = new Instructions;
        optimize(*f->intermediateCode(),*optimized,*(f->name()),args_.printDAG);
        //unoptimized code can be deleted
        delete f->intermediateCode();
        //save the optimized code
        f->setIntermediateCode(optimized);
      }
    }

    if (args_.printOptCode) {
      errstream_ << "optimized intermediate code: " << endl;
      printIntermediateCode(ids,errstream_);
    }

  } catch (Exception& e) {
    errstream_ << e.what() << endl;;
    throw ProcessError("Errors reported during intermediate code generation");
  }
  return *this;
}


//CODE GENERATION
Process& Process::operator<<(CodeGenerator& codegenerator) {
  Identifiers ids;
  globalScope_->declarations(ids);
  try {
    codegenerator.generateCode(ids);
  } catch (Exception& e) {
    errstream_ << e.what() << endl;;
    ProcessError("Errors reported during code generation");
  }
  return *this;
}


void Process::checkTiny() {
  try {
    Function* tiny = globalScope_->lookupFunction(MAIN_FUNCTION_NAME);
    Identifiers formals;
    tiny->parameters(formals);
    if ((tiny->returnType()!=void_t)||formals.size()>0)
      throw ProcessError(ILLEGAL_MAIN_TYPESIG);
  } catch (UndefException& err) {
    //no such function defined
    throw ProcessError(MAIN_FUNCTION_NOT_FOUND);
  } catch (FunctionExpected& err) {
    //defined, but not a function
    throw ProcessError(MAIN_FUNCTION_NOT_FOUND);
  }
}
