#include "ictranslator.h"

//Abstract Syntax Tree classes
#include "../syntaxtree/aggregates.h"
#include "../syntaxtree/block.h"
#include "../syntaxtree/ifstmt.h"
#include "../syntaxtree/whilestmt.h"
#include "../syntaxtree/assign.h"
#include "../syntaxtree/returnstmt.h"
#include "../syntaxtree/readstmt.h"
#include "../syntaxtree/writestmt.h"
#include "../syntaxtree/unop.h"
#include "../syntaxtree/binop.h"
#include "../syntaxtree/lengthexp.h"
#include "../syntaxtree/funcall.h"
#include "../syntaxtree/arrayaccess.h"
#include "../syntaxtree/number.h"
#include "../syntaxtree/qchar.h"

//Instruction classes
#include "../instructions/arralloci.h"
#include "../instructions/arraccessi.h"
#include "../instructions/arrassigni.h"
#include "../instructions/binassigni.h"
#include "../instructions/calli.h"
#include "../instructions/gotoi.h"
#include "../instructions/ifi.h"
#include "../instructions/lengthi.h"
#include "../instructions/parami.h"
#include "../instructions/arrparami.h"
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

#include "../instructions/reglist.h"
#include "../operators/operators.h"
#include "../symboltable.h"
#include "labelset.h"
#include "icoptranslator.h"

#include <stdexcept>
#include <iostream>
#include <list>

static const string BLOCK_NAME("block_");

ICTranslator::ICTranslator(Instructions* codestream, SymbolTable* globalScope):
  codestream_    (*codestream),
  scope_         (globalScope),
  myOpTranslator_(new ICOpTranslator(*this)),
  currentBexpTranslator_(0) { }

ICTranslator::~ICTranslator() { delete myOpTranslator_; }

void ICTranslator::visitVariable(Variable* var) {
  place_ = new VarRegister(var);
}


void ICTranslator::visitFunction(Function* fun) {
  Instructions* instr = new Instructions;

  //first, we need to process the formal parameters
  //the only thing needed there is to calculate offsets
  //for multi-dimensional arrays
  Identifiers formals;
  fun->parameters(formals);
  for (Identifierit formal = formals.begin(); formal != formals.end(); formal++) {
    // *formal is an Identfier*
    if ((*formal)->isArray()) {
      Array* formalarray(dynamic_cast<Array*>(*formal));
      if (formalarray->dimension()>1)
	generateMultidimOffsets(*(fun->body()->scope()), *instr, *formalarray);
    }
  }
  
  //translate code for the function's body
  ICTranslator functionTranslator(instr,fun->body()->scope());

  //set the new translator's fields
  functionTranslator.functionName_ = (*fun->name());
  functionTranslator.nestingLevel_ = 0;
  Identifiers* uniqueDeclarations = new Identifiers;
  functionTranslator.declarations_ = uniqueDeclarations;
  fun->body()->accept(&functionTranslator);

  //save the generated code in the function
  fun->setIntermediateCode(functionTranslator.getCodeStream());
  //and set it's declarations
  fun->setUniqueDeclarations(uniqueDeclarations);
}


void ICTranslator::visitArray(Array* array) {
  //calculate length for array allocation and calculate
  //list of temporaries which hold the indices
  Expressions& indices = array->lengths();

  //exception: one-dimensional arrays, no need to calculate length etc.
  if (indices.size() == 1) {
    //allocate the array directly after generating code for the sole index
    (*indices.begin())->accept(this);
    //the 'value' of the index is now stored in place_
    NumRegister* oneDim = new NumRegister(1);
    codestream_ << (new ArrayAllocI(new ArrayRegister(array),place_,oneDim));
    return;
  }

  VarRegisterList indexregs;
  Variables* indexVars = new Variables;
  for (Expressionit idx=indices.begin(); idx != indices.end() ; idx++) {
    // *idx is an Expression*
    (*idx)->accept(this);
    //the 'value' of the index is now stored in place_
    //store this value in a new temporary for calculating the offsets
    //when accessing multidimensional arrays
    Variable* tempidx = scope_->declareTempVariable((*idx)->type());
    //generate assignment to copy value in temporary register
    //codestream_ << (new SimpleAssignmentI(new TempRegister(tempidx),place_));
    //indexregs.push_back(new TempRegister(tempidx));
    codestream_ << (new SimpleAssignmentI(new TempRegister(tempidx),place_));
    indexregs.push_back(new TempRegister(tempidx));
    //create a list of the temp variables creating place_
    indexVars->push_back(tempidx);
  }
  
  //the length of a multidimensional array A[i,...,k] is i*...*k,
  //thus, we have to calculate this length to generate an ARRAY instruction
  Variable* temp = scope_->declareTempVariable(int_t);
  Number one(1);
  NumRegister* onereg = new NumRegister(&one); //neutral element of x (times)
  codestream_ << (new SimpleAssignmentI(new TempRegister(temp), onereg));
  for (VarRegisterit r=indexregs.begin();r!=indexregs.end(); r++) {
    // *r is a VarRegister* holding the index of dimension i
    codestream_ << (new BinAssignmentI(&operatorTimes, new TempRegister(temp),
	  			       new TempRegister(temp), (*r)));
  }

  //finally, allocate the array
  codestream_ << (new ArrayAllocI(new ArrayRegister(array),
	                          new TempRegister(temp),
				  new NumRegister(indices.size())));

  //for multidimensional arrays such as this one, also generate
  //a MULTIDIM instruction per dimension
  int nth(0);
  for (Variableit dim = indexVars->begin(); dim != indexVars->end(); dim++) {
    codestream_ << (new MultiDimI(new ArrayRegister(array),
	  			  new NumRegister(nth++),
	                          new TempRegister(*dim)));
  }

  //and finally a MULTIARRAY a instruction
  codestream_ << (new MultiArrayI(new ArrayRegister(array)));

  //store the indices in the Array, then they can be accessed by ArrayAccess
  //don't store the first, it will never be used for offset
  indexVars->pop_front();
  array->setIndices(indexVars);
}


void ICTranslator::visitBlock(Block* b) {
  //first, visit all declarations (arrays need to be processed)
  Identifiers ids;
  b->scope()->declarations(ids);
  for (Identifierit id = ids.begin(); id != ids.end() ; id++) {
    place_ = 0;
    if (b->scope()->isTemporary(*id))
      continue;
    //*id is an Identifier*
    //set the name of this declaration to a qualified name
    (*id)->setName(qualifyName((*id)->name()));
    //generate code for it
    (*id)->accept(this);
    //delete the 'value' of a variable to avoid memory leaks
    if ((*id)->isVariable())
      delete place_;
    //add it to the function declarations
    declarations_->push_back(*id);
  }
  nestingLevel_++;
  for (Statementit stmt = b->body()->begin();stmt!=b->body()->end();stmt++) {
    //*stmt is a Statement*
    (*stmt)->accept(this);
  }
  nestingLevel_--;
}


void ICTranslator::visitIfStatement(IfStatement* ifstmt) {
  BexpTranslator bexpvisitor(this);

  //generate code for the boolean expression
  ifstmt->condition()->accept(&bexpvisitor);

  //the true exit for bexp should be rerouted to here
  Label trueExit = codestream_.nextInstructionLabel();
  ifstmt->truebranch()->accept(this);

  //generate an escape for stmt1 to jump over stmt2 when done
  GotoI* gotoEnd = new GotoI(0);
  codestream_ << gotoEnd;

  //the false exit of bexp should be rerouted to here
  Label falseExit = codestream_.nextInstructionLabel();
  ifstmt->falsebranch()->accept(this);

  //the escape goto should be rerouted to here
  Label endExit = codestream_.nextInstructionLabel();
  gotoEnd->setDestination(endExit);

  //backpatch the boolean expression
  backpatch(bexpvisitor.trueSet(),codestream_,trueExit);
  backpatch(bexpvisitor.falseSet(),codestream_,falseExit);
}


void ICTranslator::visitWhileStatement(WhileStatement* whilestmt) {
  BexpTranslator bexpvisitor(this);

  //set the current while bexptranslator, keep reference to previous one
  BexpTranslator* previousTranslator(currentBexpTranslator_);
  currentBexpTranslator_ = &bexpvisitor;

  //the goto at the end (or continues in the body) should jump to here
  Label previousWhileBeginLabel(currentWhileBeginLabel_);
  currentWhileBeginLabel_ = codestream_.nextInstructionLabel();
  
  //generate code for the boolean expression
  whilestmt->condition()->accept(&bexpvisitor);

  //the true exit for bexp should be rerouted to here
  Label trueExit = codestream_.nextInstructionLabel();

  //generate code for the body
  whilestmt->body()->accept(this);

  //generate a goto instruction that restarts the loop 
  codestream_ << (new GotoI(currentWhileBeginLabel_));

  //the false exit of bexp should be rerouted to here
  Label falseExit = codestream_.nextInstructionLabel();

  //backpatch the boolean expression
  backpatch(bexpvisitor.trueSet(),codestream_,trueExit);
  backpatch(bexpvisitor.falseSet(),codestream_,falseExit);

  //reset the old bexptranslator and label
  currentBexpTranslator_  = previousTranslator;
  currentWhileBeginLabel_ = previousWhileBeginLabel;
}


void ICTranslator::visitAssignment(Assignment* assgn) {
  assgn->expression()->accept(this);
  //place_ now contains the 'value' of the expression
  VarRegister* value = place_;
  //if the target is an array, we have to generate an array access instruction
  //otherwise, it's an ordinary assignment
  if (assgn->target()->isArrayAccess()) {
    //generate code for indices
    ArrayAccess* arrayaccess = dynamic_cast<ArrayAccess*>(assgn->target());
    Expressions& indices = arrayaccess->indices();
    Array* array = arrayaccess->array();
    VarRegister* index = foldIndex(array,indices);

    //finally, generate array[index] = value
    codestream_ << (new ArrayAssignmentI(new ArrayRegister(array),
				    	 index,
				   	 value));
  } else {
    //downcast to var is safe: Function* and Array* ruled out by the typechecker
    Variable* assignTarget = lexp2variable(assgn->target());
    codestream_ << (new SimpleAssignmentI(new VarRegister(assignTarget), value));
  }
}


void ICTranslator::visitReturnStatement(ReturnStatement* ret) {
  Expression* exp = ret->expression();
  exp->accept(this);
  //the result of exp is now placed in place_
  codestream_ << (new ReturnI(place_));
}


void ICTranslator::visitFuncall(Funcall* funcall) {
  Expressions* actuals = funcall->arguments();

  //store all calculated arguments in a list
  RegisterList paramlist;
  for (Expressionit e = actuals->begin(); e != actuals->end(); e++) {
    //*e is an Expression*
    //the 'value' of the actual argument is now stored in place_
    if ((*e)->type() == array_t) {
      //if the actual is an array, pass it by reference
      Array* array = dynamic_cast<Array*>((*e));
      paramlist.push_back(new ArrayRegister(array));
    } else {
      (*e)->accept(this);
       paramlist.push_back(place_);
    }
  }
  //now generate a number of param instructions
  for (RegisterList::iterator p=paramlist.begin();p!=paramlist.end();p++) {
    //*p is a Register*
    //if we're dealing with a variable, generate a PARAM x instruction,
    //otherwise, generate an ARRAYPARAM a instruction.
    VarRegister* var = dynamic_cast<VarRegister*>(*p);
    if (var)
      codestream_ << (new ParamI(var));
    ArrayRegister* arr = dynamic_cast<ArrayRegister*>(*p);
    if (arr)
      codestream_ << (new ArrayParamI(arr));
  }
  //finally, generate a call instruction
  codestream_ << (new CallI(new FunRegister(funcall->callee()),actuals->size())); 

  //generate an instruction to retrieve the result,
  //but only if this function call is used as an expression
  if (funcall->isExpression()) {
    Variable* resulttemp = scope_->declareTempVariable(funcall->callee()->returnType());
    codestream_ << (new RetValI(new TempRegister(resulttemp)));
    place_ = new TempRegister(resulttemp); 
  }
}


void ICTranslator::visitReadStatement(ReadStatement* readstmt) {
  LExpression* lexp = readstmt->target();
  if (lexp->isArrayAccess()) {
    //READ A[I] -> generate READ t1; A[I] = t1
    
    ArrayAccess* aac(lexp2array(lexp));
    Expressions& indices = aac->indices();
    Array* array = aac->array();
    Variable* temp = scope_->declareTempVariable(array->baseType());
    VarRegister* index = foldIndex(array,indices);

    codestream_ << (new ReadI(new TempRegister(temp)));

    codestream_ << (new ArrayAssignmentI(
	                new ArrayRegister(array),
			index,
			new TempRegister(temp)));
  } else {
    //READ VAR -> generate READ VAR
    codestream_ << (new ReadI(new VarRegister(lexp2variable(lexp))));
  }
}


void ICTranslator::visitWriteStatement(WriteStatement* writestmt) {
  writestmt->expression()->accept(this);
  //the 'value' of the expression is now stored in place_ 
  codestream_ << (new WriteI(place_));
}


void ICTranslator::visitUnaryOp(UnaryOp* unop) {
  Expression* exp = unop->expression();
  Operator& op = unop->unaryOp();

  //descend down the tree
  exp->accept(this);
  
  //let my operator translator translate the unary op
  myOpTranslator_->setResultType(op.mapsTo(exp->type(),void_t));
  myOpTranslator_->setFirstArg(place_);
  myOpTranslator_->setSecondArg(0);
  op.accept(myOpTranslator_);
  place_ = myOpTranslator_->result();
}


void ICTranslator::visitBinaryOp(BinaryOp* binop) {
  Expression* first = binop->firstArg();
  Expression* second = binop->secondArg();
  Operator& op = binop->binaryOp();

  //descend down the tree
  first->accept(this);
  VarRegister* value1 = place_;
  second->accept(this);
  VarRegister* value2 = place_;

  myOpTranslator_->setResultType(op.mapsTo(first->type(),second->type()));
  myOpTranslator_->setFirstArg(value1);
  myOpTranslator_->setSecondArg(value2);
  op.accept(myOpTranslator_);
  place_ = myOpTranslator_->result();
}


void ICTranslator::visitNumber(Number* num) {
  place_ = new NumRegister(num);
}


void ICTranslator::visitQChar(QChar* chr) {
  place_ = new QCharRegister(chr);
}


void ICTranslator::visitLengthExpression(LengthExpression* len) {
  //downcast should be guaranteed by the typechecker
  Array* array = dynamic_cast<Array*>(len->lexpression());
  if (!array)
    throw logic_error("Array expected for LExpression on LengthExpression");
  Variable* temp = scope_->declareTempVariable(len->type());
  codestream_ << (new LengthI(new TempRegister(temp),new ArrayRegister(array)));
  place_ = new TempRegister(temp);
}


void ICTranslator::visitArrayAccess(ArrayAccess* arrayaccess) {
  Expressions& indices = arrayaccess->indices();
  Array* array = arrayaccess->array();
  VarRegister* index = foldIndex(array,indices);
  Variable* resulttemp = scope_->declareTempVariable(array->baseType());
  
  //generate resulttemp = array[index]
  codestream_ << (new ArrayAccessI(new TempRegister(resulttemp),
				   new ArrayRegister(array),
				   index));

  //the entire value of this expression will be stored in resulttemp
  place_ = new TempRegister(resulttemp);
}

void ICTranslator::visitBreakStatement(BreakStatement* breakstmt) {
  //if we're in the body of a while...
  if (currentBexpTranslator_) {
    //generate jump to false exit
    GotoI* jump(new GotoI(0));
    Label jumplbl = codestream_.nextInstructionLabel();
    codestream_ << jump;
    //backpatch later with the false exit
    currentBexpTranslator_->falseSet().insert(jumplbl);
  } else {
    //else, throw an exception, illegal statement
    throw Exception("Break statement used outside the body of a while or for loop");
  }
}


void ICTranslator::visitContinueStatement(ContinueStatement* ctustmt) {
  //if we're in the body of a while...
  if (currentBexpTranslator_)
    //generate jump to beginning of the while
    codestream_ << (new GotoI(currentWhileBeginLabel_));
  else
    //else, throw an exception, illegal statement
    throw Exception("Continue statement used outside the body of a while or for loop");
}


Variable* ICTranslator::lexp2variable(LExpression* lexp) {
  Variable* var = dynamic_cast<Variable*>(lexp);
  if (!var)
    throw std::logic_error("Variable expected for Lexp of Lexpression!");
  return var;
}


ArrayAccess* ICTranslator::lexp2array(LExpression* lexp) {
  ArrayAccess* arr = dynamic_cast<ArrayAccess*>(lexp);
  if (!arr)
    throw std::logic_error("Array Access expected for Lexpression!");
  return arr;
}

string* ICTranslator::qualifyName(string* name) const {
  string qualifiedName(functionName_+"_");
  int level(nestingLevel_);
  while (level--) {
    qualifiedName += BLOCK_NAME;
  }
  qualifiedName += (*name);
  return StringPool::instance()->getString(qualifiedName);
}


VarRegister* ICTranslator::foldIndex(Array* array, Expressions& indices) {
  //calculate the folded index for a multi-dimensional array

  //special case: dimension = 1, no folding necessary
  if (indices.size() == 1) {
    Expressionit idx(indices.begin());
    //generate code for idx, its value will be left in place_
    (*idx)->accept(this);
    return place_;
  }

  Variables* dimensions = array->getIndices();
  Variable* runningval = scope_->declareTempVariable(int_t);

  //initialize the running value to 0 (neutral for +)
  Number zero(0);
  NumRegister* init = new NumRegister(&zero);
  codestream_ << (new SimpleAssignmentI(new TempRegister(runningval),init));

  //generate code for each index expression
  Expressionit idx(indices.begin());
  //dimensions are shifted one place to the left, first one does not count
  Variableit dim(dimensions->begin());

  //if there's more than one dimension, we have to calculate the 'folded' index 
  if (dim != dimensions->end()) {
    Variable* helpval = scope_->declareTempVariable(int_t);
    while(dim != dimensions->end()) {
      //generate code for idx, its value will be placed in place_
      (*idx)->accept(this);
      //generate helpval = idxexpression * arraydimension
      codestream_ << (new BinAssignmentI(&operatorTimes,new TempRegister(helpval),
	    place_, new TempRegister(*dim)));
      //generate runningval = runningval + helpval
      codestream_ << (new BinAssignmentI(&operatorPlus,new TempRegister(runningval),
	    new TempRegister(runningval), new TempRegister(helpval)));
      dim++;
      idx++;
    }
  }

  //if we get here, we still have to process the last index expression (because
  //dimension was shifted by one) (sizes ought to be right after typechecking)
  (*idx)->accept(this);

  //generate runningval = runningval + lastidxexpression
  codestream_ << (new BinAssignmentI(&operatorPlus,new TempRegister(runningval),
				     new TempRegister(runningval), place_));

  //return the register containing the folded value
  return new TempRegister(runningval);
}

void ICTranslator::generateMultidimOffsets(SymbolTable& scope,
                                           Instructions& instrstream, Array& array) {
  Variables* indexVars = new Variables;
  //dim=1 because first dimension (0) does not count
  for (int dim=1; dim < array.dimension(); dim++) {
    Variable* tempidx = scope.declareTempVariable(int_t);
    instrstream << (new DimLengthI(new TempRegister(tempidx),
	  			   new ArrayRegister(&array),new NumRegister(dim)));
    indexVars->push_back(tempidx);
  }
  array.setIndices(indexVars);
}
