#include "jvmtranslator.h"

//Abstract Syntax Tree classes
#include "../syntaxtree/identifier.h"
#include "../syntaxtree/variable.h"
#include "../syntaxtree/array.h"
#include "../syntaxtree/function.h"

//Instruction classes
#include "../instructions/instruction.h"
#include "../instructions/addressi.h"
#include "../instructions/arralloci.h"
#include "../instructions/arraccessi.h"
#include "../instructions/arrassigni.h"
#include "../instructions/arrparami.h"
#include "../instructions/binassigni.h"
#include "../instructions/calli.h"
#include "../instructions/gotoi.h"
#include "../instructions/ifi.h"
#include "../instructions/lengthi.h"
#include "../instructions/parami.h"
#include "../instructions/readi.h"
#include "../instructions/returni.h"
#include "../instructions/retvali.h"
#include "../instructions/simpleassigni.h"
#include "../instructions/unassigni.h"
#include "../instructions/writei.h"
#include "../instructions/dimlengthi.h"
#include "../instructions/multidimi.h"
#include "../instructions/multiarrayi.h"

#include "../instructions/varregister.h"
#include "../instructions/funregister.h"
#include "../instructions/arrayregister.h"
#include "../instructions/numregister.h"
#include "../instructions/charregister.h"
#include "../instructions/tempregister.h"

//Operator classes
#include "../operators/divides.h"
#include "../operators/equal.h"
#include "../operators/evaluate.h"
#include "../operators/greater.h"
#include "../operators/less.h"
#include "../operators/minus.h"
#include "../operators/negate.h"
#include "../operators/nequal.h"
#include "../operators/operator.h"
#include "../operators/plus.h"
#include "../operators/times.h"
#include "../operators/uminus.h"
#include "../operators/operators.h"

#include "../optimization/usecount.h"
#include "tiny2java.h"

#include <stdexcept>
#include <iostream>


JVMTranslator::JVMTranslator(string& funName,
                             string& className,
			     ostream& codestream):
      funName_       (funName),
      className_     (className),
      outstream_     (codestream),
      registerSaver_ (new RegisterSaver(codestream_,*this)),
      registerLoader_(new RegisterLoader(codestream_,*this)),
      tos_           (0),
      maxStack_      (0),
      currentStack_  (0) { }

JVMTranslator::~JVMTranslator() {
  delete registerSaver_;
  delete registerLoader_;
}

//map declarations

void JVMTranslator::visitVariable(Variable* var) {
  //type name
  codestream_ << type2javaType(var) << " " << *(var->name()) << endl; 
}

void JVMTranslator::visitArray(Array* arr) {
  //type[] name
  codestream_ << type2javaType(arr) << " " << *(arr->name()) << endl; 
}

void JVMTranslator::visitFunction(Function* fun) {
  //method public static ret_type function(type arg1, ..., type argN) max_stack {
  //  declarations body }
  outstream_ << "method public static " << type2javaType(fun);
  outstream_ << " " << *(fun->name()) << " (";

  stringstream paramInit; //used for parameter initialization code
  
  Identifiers pars;
  fun->parameters(pars);

  Identifierit id = pars.begin();
  int index(0);
  if (id != pars.end()) {
    string javaType = generateDeclaration(*id,paramInit,index++);
    outstream_ << javaType; 
    id++;
  }
  while (id != pars.end()) {
    // *id is an Identifier*
    string javaType = generateDeclaration(*id,paramInit,index++);
    outstream_ << "," << javaType; 
    id++;
  }

  //generate code for the declarations
  Identifiers* decls = fun->uniqueDeclarations();
  for (Identifierit decl = decls->begin(); decl != decls->end(); decl++) {
    // *decl is an Identifier*
    (*decl)->accept(this);
  }

  codestream_ << endl;
  
  //generate code for the function body
  Instructions* code = fun->intermediateCode();
  for (Instructionit instr = code->begin(); instr != code->end(); instr++) {
    // *instr is an Instruction*
    (*instr)->accept(this);
  }

  //generate code for the temporary declarations
  ostringstream tempdeclarations;
  generateTempDeclarations(usedTemps_,tempdeclarations);

  outstream_ << ") max_stack " << maxStack_ << " {" << endl;
  //paste the parameter initialization code
  outstream_ << paramInit.str();
  outstream_ << tempdeclarations.str();
  outstream_ << codestream_.str();

  //if the returntype is void, generate a return instruction
  //also precede this by a label, because it can possibly be jumped to
  if (fun->returnType()==void_t) {
    outstream_ << label2str(code->nextInstructionLabel()) << ": return" << endl;
  }

  outstream_ << "}" << endl;
}
     

//code inherited from CodeVisitor
//map each instruction to a JVM assembler statement

void JVMTranslator::visitArrayAllocInstruction(ArrayAllocI* aloc) {
  //load length
  //don't load anything if the right variable is already on TOS
  if (!onTop(aloc->getLength()))
    load(aloc->getLength());
  //newarray array.base_type
  codestream_ << "newarray " << tinytype2java(aloc->getTarget()->baseType()) << endl;
  //store array
  save(aloc->getTarget());

  //for multidimensional arrays, create the dimension array holding the
  //lengths of each dimension (will be filled up by MULTIDIM instructions)
  NumRegister* nrOfDimensions(aloc->getDimension());
  if (nrOfDimensions->value()>1) {
    codestream_ << "int[] " << getArrayDimensions(aloc->getTarget()) << endl;
    load(nrOfDimensions);
    codestream_ << "newarray int" << endl;
    codestream_ << "astore " << getArrayDimensions(aloc->getTarget()) << endl;
    decStack();
  }
}

void JVMTranslator::visitArrayAccInstruction(ArrayAccessI* aac) {
  //save TOS
  //Register* tos(getTos());

  //don't load the index if it was already on TOS
  if (onTop(aac->getIndex())) {
    tos_ = 0; //'virtually' load the index by setting tos to 0
    load(aac->getArrayRegister()); // | index => | index array
    codestream_ << "swap" << endl; // | index array => | array index
  }
  else {
    load(aac->getArrayRegister()); // | tos => | tos array
    load(aac->getIndex());	   // | tos array => | tos array index
  }

  codestream_ << "iaload" << endl;
  decStack(1);
  //save into var
  save(aac->getTarget());
}

void JVMTranslator::visitArrayAssInstruction(ArrayAssignmentI* aas) {
  if (onTop(aas->getIndex())) {
    tos_ = 0; //release tos_
    load(aas->getArrayRegister());
    //the index is already on top of stack ie the stack is:
    //| index array
    //and should become:
    //| array index value
    codestream_ << "swap" << endl; 	//| index array | => | array index |
    load(aas->getValue()); 		//| array index | => | array index value |
  } else if (onTop(aas->getValue())) {
    tos_ = 0; //release tos_
    load(aas->getArrayRegister());
    //this time, the value is already on TOS, ie the stack is:
    //| value array
    //and should become:
    //| array index value
    codestream_ << "swap" << endl;	//| value array | => | array value |
    load(aas->getIndex());		//| array value | => | array value index |
    codestream_ << "swap" << endl;	//| array value index | => | array index value |
  } else {
    //stack is | , so just push all three variables onto the stack
    //load array
    load(aas->getArrayRegister());
    //load index
    load(aas->getIndex());
    //load var
    load(aas->getValue());
  }
  codestream_ << "iastore" << endl;
  decStack(3);
}

void JVMTranslator::visitBinaryAssInstruction(BinAssignmentI* bas) {
  //if the binary assignment is of a form like x = x + C then generate
  //more efficient code
  if (checkForIncrement(bas))
    //returns true if optimization succeeded
    return;
  VarRegister* arg1 = bas->getFirstArg();
  VarRegister* arg2 = bas->getSecondArg();
  //only load arg2 if arg1 is already on TOS 
  if (onTop(arg1)) {
    tos_ = 0; //release tos_
    load(arg2);
  //only load arg1 if arg2 is already on TOS
  } else if (onTop(arg2)) {
    tos_ = 0; //release tos_
    load(arg1);
    if (!(bas->getOperator()->isCommutative()))
      //a swap is necessary to safeguard that the
      //operator gets its args in the right order
      codestream_ << "swap" << endl;
  } else {
    //load arg1
    load(arg1);
    //load arg2
    load(arg2);
  }

  //visit the operator
  bas->getOperator()->accept(this);
  codestream_ << endl;
  decStack();
  //store the result
  save(bas->getTarget());
}

void JVMTranslator::visitCallInstruction(CallI* call) {
  codestream_ << "invokestatic ";
  //build the signature
  FunRegister* fun = call->getFunction();
  codestream_ << type2javaType(fun) << " ";
  codestream_ << (*fun->name()) << " (";
  
  Identifiers* formals = fun->formals();
  Identifierit id = formals->begin();
  if (id!=formals->end()) {
    codestream_ << type2javaType(*id);
    id++;
  }
  
  while (id!=formals->end()) {
    // *t is a Type
    codestream_ << "," << type2javaType(*id);
    id++;
  }
  codestream_ << ")" << endl;
  decStack(call->getNrOfArgs()-1);
}

void JVMTranslator::visitGotoInstruction(GotoI* gotoi) {
  //goto label
  codestream_ << "goto " << label2str(gotoi->getDestination()) << endl;
}

void JVMTranslator::visitIfInstruction(IfI* ifi) {
  VarRegister* arg1 = ifi->getFirstArg();
  VarRegister* arg2 = ifi->getSecondArg();

  //only load arg2 if arg1 is already on TOS 
  if (onTop(arg1)) {
    tos_ = 0; //release tos_
    load(arg2);
  //only load arg1 if arg2 is already on TOS
  } else if (onTop(arg2)) {
    tos_ = 0; //release tos_
    load(arg1);
    //a swap is necessary to safeguard that the
    //comparator gets its args in the right order
    codestream_ << "swap" << endl;
  } else {
    //load arg1
    load(arg1);
    //load arg2
    load(arg2);
  }

  //translate(op)
  ifi->getOp()->accept(this);
  //append label
  codestream_ << " " << label2str(ifi->getDestination()) << endl;
  decStack(2);
}

void JVMTranslator::visitLengthInstruction(LengthI* length) {
  load(length->getSource());
  codestream_ << "arraylength" << endl;
  //save var
  save(length->getTarget());
}

void JVMTranslator::visitParamInstruction(ParamI* par) {
  if (onTop(par->getValue()))
    tos_ = 0; //release tos_
  else
    load(par->getValue());
}

void JVMTranslator::visitArrayParamInstruction(ArrayParamI* par) {
  load(par->getValue());
}

void JVMTranslator::visitReadInstruction(ReadI* read) {
  //getstatic java.io.InputStream java.lang.System.in
  codestream_ << "getstatic java.io.InputStream java.lang.System.in" << endl;
  incStack();
  //invokevirtual int java.io.InputStream.read()
  codestream_ << "invokevirtual int java.io.InputStream.read()" << endl;

  //if we're reading into a char, we need to perform a cast first
  if (read->getSource()->type() == char_t) {
    codestream_ << "i2c" << endl;
  } else if (read->getSource()->type() == int_t) {
    //we need to extract the numerical value out of the char
    //in Java syntax:
    //char c = (char) System.in.read();
    //int x = Character.getNumericValue(c);
    codestream_ << "i2c" << endl;
    codestream_ << "invokestatic int java.lang.Character.getNumericValue(char)" << endl;
  }

  //store var
  save(read->getSource());
}

void JVMTranslator::visitReturnInstruction(ReturnI* ret) {
  //load var if it is not on TOS
  if (onTop(ret->getValue()))
    tos_ = 0; //release tos
  else
    load(ret->getValue());
  //return
  codestream_ << "ireturn" << endl;
}

void JVMTranslator::visitReturnValInstruction(RetValI* rval) {
  //the return value is on top of stack, just save this into the variable
  save(rval->getTarget());
}

void JVMTranslator::visitSimpleAssInstruction(SimpleAssignmentI* sim) {
  //load source if it is not on TOS
  if (onTop(sim->getSource()))
    tos_ = 0;
  else
    load(sim->getSource());
  //save target
  save(sim->getTarget());
}

void JVMTranslator::visitUnaryAssInstruction(UnAssignmentI* uas) {
  //load source if it is not on TOS
  if (onTop(uas->getArg()))
    tos_ = 0;
  else
    load(uas->getArg());
  //translate(op)
  uas->getOperator()->accept(this);
  codestream_ << endl;
  //save target
  save(uas->getTarget());
}

void JVMTranslator::visitWriteInstruction(WriteI* write) {
  //getstatic java.io.PrintStream java.lang.System.out
  codestream_ << "getstatic java.io.PrintStream java.lang.System.out" << endl;
  incStack();
  //load var if it is not already on TOS
  if (onTop(write->getSource())) {
    //the var is on top of the out object so we have to generate a swap
    codestream_ << "swap" << endl;   // | source System.out | => | System.out source |
    tos_ = 0;
  }
  else
    load(write->getSource());
  //invokevirtual void java.io.PrintStream.println(type)
  codestream_ << "invokevirtual void java.io.PrintStream.print(";
  codestream_ << type2javaType(write->getSource()) << ")" << endl;
  decStack(2);
}

void JVMTranslator::visitDimLengthI(DimLengthI* dim) {
  load(dim->getArray());
  load(dim->getDimension());
  codestream_ << "invokestatic int TinyRuntime.dimension(java.lang.Object,int)" << endl;
  decStack(1);
  save(dim->getTarget());
}

void JVMTranslator::visitMultiDimInstruction(MultiDimI* multi) {
  incStack();
  codestream_ << "aload " << (getArrayDimensions(multi->getArray())) << endl;
  load(multi->getNthDimension());
  load(multi->getDimLength());
  codestream_ << "iastore" << endl;
  decStack(3);
}


void JVMTranslator::visitMultiArrayInstruction(MultiArrayI* marr) {
  load(marr->getArray());
  incStack();
  codestream_ << "aload " << (getArrayDimensions(marr->getArray())) << endl;
  codestream_ << "invokestatic void TinyRuntime.declareArray(java.lang.Object, int[])"
              << endl;
  decStack(2);
}


void JVMTranslator::visitLeader(Instruction* i) {
  //generate label:
  codestream_ << label2str(i->getLabel()) << ":" << endl;
}


void JVMTranslator::RegisterSaver::visitArrayRegister(ArrayRegister* areg) {
  if (areg->isGlobal()) {
    codestream_ << "putstatic " << type2javaType(areg) << " " << outer.className_
      		<< "." << *(areg->name()) << endl;
  } else {
    codestream_ << "astore " << *(areg->name()) << endl;
  }
  outer.tos_ = 0;
}


void JVMTranslator::RegisterSaver::visitVarRegister(VarRegister* varreg) {
  if (varreg->isTemporary() &&
      (UseCount::instance().count(outer.funName_,varreg)<=2)) {
    //don't generate code for the temporary, just mark it as 'top of stack'
    //see UseCount for more details
    outer.tos_ = varreg;
    //variable not saved, so stack does not diminish
    outer.incStack();
    return;
  }
  if (varreg->isTemporary())
    //this temporary variable is used, so we need to declare it
    outer.usedTemps_.insert(varreg);
  if (varreg->isGlobal()) {
    codestream_ << "putstatic " << type2javaType(varreg) << " " << outer.className_
      		<< "." << *(varreg->name()) << endl;
  } else {
    codestream_ << "istore " << *(varreg->name()) << endl;
  }
  //release the top of stack
  outer.tos_ = 0;
}

void JVMTranslator::RegisterLoader::visitArrayRegister(ArrayRegister* areg) {
  if (areg->isGlobal()) {
    codestream_ << "getstatic " << type2javaType(areg) << " " << outer.className_
      		<< "." << *(areg->name()) << endl; 
  }
  else 
    codestream_ << "aload " << *(areg->name()) << endl;
}

void JVMTranslator::RegisterLoader::visitVarRegister(VarRegister* varreg) {
  if (varreg->isGlobal()) {
    codestream_ << "getstatic " << type2javaType(varreg) << " " << outer.className_
      		<< "." << *(varreg->name()) << endl;
  } else {
    codestream_ << "iload " << *(varreg->name()) << endl;
  }
}

void JVMTranslator::RegisterLoader::visitNumRegister(NumRegister* numreg) {
  int val = numreg->value();

  if (val == -1)  			//efficient loading of -1
    codestream_ << "iconst_m1" << endl;
  else if ((0 <= val) && (val <= 5)) 	//efficient loading of [0,5]
    codestream_ << "iconst_" << val << endl;
  else if ((-128 <= val)&&(val <= 127))	//efficient loading of [-128,127]
    codestream_ << "bipush " << val << endl;
  else
    codestream_ << "ldc " << val << endl;
}

void JVMTranslator::RegisterLoader::visitQCharRegister(QCharRegister* creg) {
  codestream_ << "ldc " << (int) (creg->value()) << endl;
}


//methods inherited from OperatorVisitor
//these generate code for the operators themselves

void JVMTranslator::visitDividesOp(DividesOp* op) {
  codestream_ << "idiv";
}

void JVMTranslator::visitEqualOp(EqualOp* op) {
  codestream_ << "if_icmpeq";
}

void JVMTranslator::visitGreaterOp(GreaterOp* op) {
  codestream_ << "if_icmpgt";
}

void JVMTranslator::visitLessOp(LessOp* op) {
  codestream_ << "if_icmplt";
}

void JVMTranslator::visitMinusOp(MinusOp* op) {
  codestream_ << "isub";
}

void JVMTranslator::visitNEqualOp(NEqualOp* op) {
  codestream_ << "if_icmpne";
}

void JVMTranslator::visitPlusOp(PlusOp* op) {
  codestream_ << "iadd";
}

void JVMTranslator::visitTimesOp(TimesOp* op) {
  codestream_ << "imul";
}

void JVMTranslator::visitUMinusOp(UMinusOp* op) {
  codestream_ << "ineg";
}

string JVMTranslator::generateDeclaration(Identifier* id, stringstream& codestream,
    					  int index) {
  string javaType(type2javaType(id));
  string loadOpcode;
  string saveOpcode;
  if (id->isArray()) {
    loadOpcode = "aload";
    saveOpcode = "astore";
  } else {
    loadOpcode = "iload";
    saveOpcode = "istore";
  }
  codestream << javaType   << " " << *(id->name()) << endl;
  codestream << loadOpcode << " " << index << endl;
  incStack();
  codestream << saveOpcode << " " << *(id->name()) << endl; 
  decStack();
  return javaType;
}

void JVMTranslator::save(Register* r) {
  r->accept(registerSaver_);
  //decrement stack because variable was saved into register
  decStack();
}

void JVMTranslator::load(Register* r) {
  //first save the current tos_ into a register
  if (tos_) {
    codestream_ << "istore " << *(tos_->name()) << endl;
    //this temporary variable is used, so we need to declare it
    usedTemps_.insert(tos_);
  }
  //next, generate specific loading code
  r->accept(registerLoader_);

  //increment the stack count (a variable was loaded)
  incStack();
  //release the top of stack
  tos_ = 0;
}

void JVMTranslator::incStack(int cnt) {
  currentStack_ += cnt;
  if (currentStack_ > maxStack_)
    maxStack_ = currentStack_;
}

void JVMTranslator::decStack(int cnt) {
  currentStack_ -= cnt;
}

bool JVMTranslator::onTop(Register* reg) {
  if (tos_)
    return (*tos_ == *reg);
  else
    return false;
}

//generate code for all temporary declarations
void JVMTranslator::generateTempDeclarations(RegisterSet& usedTemps,
    					     ostringstream& declarationstream) {
  typedef RegisterSet::iterator Tempit;
  for (Tempit temp = usedTemps.begin(); temp != usedTemps.end(); temp++) {
    declarationstream << type2javaType(*temp) << " " << *((*temp)->name()) << endl;
  }
}

bool JVMTranslator::checkForIncrement(BinAssignmentI* bas) {
  //target should not be a temporary or a global
  if (bas->getTarget()->isTemporary()||
      bas->getTarget()->isGlobal())
    return false;
  
  //x = y + z?
  if (bas->getOperator() == &operatorPlus) {
    VarRegister* target(bas->getTarget());
    //x = x + z?
    if (*target == *bas->getFirstArg()) {
      VarRegister* second(bas->getSecondArg());
      //x = x + C?
      if (second->isConstant()) {
	return generateIncCode(target,second,false); //x = x + C
      } else 
	return false; //x = x + z
    //x = y + x?
    } else if (*target == *bas->getSecondArg()) {
      VarRegister* first(bas->getFirstArg());
      //x = C + x?
      if (first->isConstant()) {
	return generateIncCode(target,first,false); //x = C + x
      } else 
	return false; //x = y + x
    } else
      return false; //x = y + z
  } else

  //x = y - z?
  if (bas->getOperator() == &operatorMinus) {
    VarRegister* target(bas->getTarget());
    //x = x - z?
    if (*target == *bas->getFirstArg()) {
      VarRegister* second(bas->getSecondArg());
      //x = x - C?
      if (second->isConstant()) {
	return generateIncCode(target,second,true); //x = x - C
      } else 
	return false; //x = x - z
    }
  }
  return false; //x = y op z
}

bool JVMTranslator::generateIncCode(VarRegister* target,
                                    VarRegister* value, bool neg) {
  NumRegister* number = dynamic_cast<NumRegister*>(value);
  int val;
  if (number)
    val = number->value();
  QCharRegister* character = dynamic_cast<QCharRegister*>(value);
  if (character) 
    val = (int) character->value();
  if ((-128 <= val) && (val <= 127)) {
    codestream_ << "iinc " << *(target->name()) << " " << ((neg) ? -val : val) << endl;
    return true;
  }
  else
    return false;
}

string JVMTranslator::getArrayDimensions(ArrayRegister* array) {
  return string("_"+ *(array->name()) + "_dimensions_");
}
