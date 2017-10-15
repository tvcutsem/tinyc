#ifndef USECOUNT_H
#define USECOUNT_H

#include "../instructions/register.h"

#include <map>
#include <iostream>
#include <string>

using namespace std;

typedef map<string, int> RegCount;
typedef RegCount::iterator RegCountit;

/**
 * A UseCounter counts the number of times a register is used within the code.
 * This is used to perform an optimization during JVM code generation.
 *
 * If we know a register is a temporary, and is used no more than 2 times
 * within a function body (that is: it should be written to once and read from
 * once), then in some cases, we can leave that temporary on the runtime stack.
 *
 * Example:
 *  x = w+y+z;
 * is translated to (optimized):
 *  t0 = w  + y
 *  x  = t0 + z
 *
 * Now, it is perfectly possible to generate the following code:
 *   load w
 *   load y
 *   iadd
 *   load z
 *   store x
 *
 * Note that we did not add the statements
 *  store t0
 *  load t0
 * because they are wasteful
 * Such statements appear very frequently in code that makes heavy use of
 * expressions (because temps are always used to store a partly computation
 * of an expression).
 *
 * The reason we need to count the number of times a variable is used is that,
 * because of the common subexpression elimination, t0 can for example be used
 * in another part of the body. If we then don't add the store t0; load t0 statements,
 * t0 will not be initialized, or will contain a wrong value.
 * The trick is to only ommit the redundant save/load when the temporary is used
 * not more than 2 times.
 *
 * This class is a singleton.
 */

class UseCount {

  public:

    static UseCount& instance();

    /**
     * Increment the usage count of register r
     */
    void incCount(string& funname, Register* r);

    /**
     * Returns the number of times register r was counted
     */
    int count(string& funname, Register* r);

    friend ostream& operator<<(ostream& os, UseCount& uc);

  private:

    /**
     * Qualifies the register name with the function name
     */
    string qualify(string& funname, Register* r);

    static UseCount instance_;

    RegCount useCount_;

};

/**
 * Prints each register counted and the number of times it is used.
 * Used for debugging.
 */
ostream& operator<<(ostream& os, UseCount& uc);

#endif
