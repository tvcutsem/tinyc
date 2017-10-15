#ifndef VARIABLESET_H
#define VARIABLESET_H

#include "../syntaxtree/variable.h"
#include "../syntaxtree/aggregates.h"

#include <set>
#include <iostream>

using namespace std;

/**
 * A VariableSet is an abstraction for a set of variables used when
 * checking for uninitialized variables.
 */

class VariableSet: public set<Variable*> {

  public:

    /**
     * Returns whether the given variable is in the set
     */
    bool contains(Variable* var);

    /**
     * Removes the variable from the set
     */
    void remove(Variable* var);

    /**
     * Fills the set with all variables from het variablelist
     */
    void add(Variables& vars);

    friend ostream& operator<<(VariableSet& set, ostream& out);

};

/**
 * For debugging purposes
 */
ostream& operator<<(ostream& os, VariableSet& set);

/**
 * @post result = set1 U set2
 */
void setUnion(VariableSet& result, VariableSet& set1, VariableSet& set2);

#endif
