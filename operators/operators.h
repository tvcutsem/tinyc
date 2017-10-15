#ifndef OPERATORS_H
#define OPERATORS_H

#include "operator.h"
#include "plus.h"
#include "minus.h"
#include "times.h"
#include "divides.h"

#include "equal.h"
#include "nequal.h"
#include "less.h"
#include "greater.h"

#include "uminus.h"
#include "negate.h"

using namespace std;

/**
 * @file
 * This file declares the operator objects for use in the syntax tree.
 */

extern PlusOp operatorPlus;
extern MinusOp operatorMinus;
extern TimesOp operatorTimes;
extern DividesOp operatorDivides;

extern EqualOp operatorEqual;
extern NEqualOp operatorNEqual;
extern LessOp operatorLT;
extern GreaterOp operatorGT;

extern UMinusOp operatorUMinus;
extern NegateOp operatorNegate;

#endif
