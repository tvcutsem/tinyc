#ifndef EVALUATE_H
#define EVALUATE_H

#include "../syntaxtree/constant.h"
#include "../syntaxtree/number.h"
#include "../syntaxtree/qchar.h"

using namespace std;

typedef int (*operation)(int op1, int op2);

/**
 * Default evaluation for operations, which means the operation
 * can both be applied to ints and chars, and the result will
 * always be an int (or char if both operands ar chars)
 */

Constant* eval(Constant* c1, Constant* c2,operation o);

#endif
