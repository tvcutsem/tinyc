#include "variableset.h"

#include <algorithm>
#include <iterator>

bool VariableSet::contains(Variable* var) {
  iterator found(find(var->reference()));
  return (found != end());
}

void VariableSet::remove(Variable* var) {
  erase(var->reference());
}

void VariableSet::add(Variables& vars) {
  for (Variables::iterator var = vars.begin(); var != vars.end(); var++)
    // *var is a Variable*
    insert((*var)->reference());
}

ostream& operator<<(ostream& os, VariableSet& set) {
  os << "contents of variableset: " << endl;
  std::copy(set.begin(),set.end(), ostream_iterator<Variable*>(os,"\n"));
  return os;
}

void setUnion(VariableSet& result, VariableSet& set1, VariableSet& set2) {
  //don't you just love stl? :)
  insert_iterator<VariableSet> ii(result,result.begin());
  std::set_union(set1.begin(),set1.end(),set2.begin(),set2.end(),ii);
}
