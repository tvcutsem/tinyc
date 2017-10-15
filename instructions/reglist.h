#ifndef REG_LIST_H
#define REG_LIST_H

#include "register.h"
#include "varregister.h"

#include <list>

/**
 * A Registerlist is a list containing several (variable) registers.
 * This is useful for eg storing the indices of an array.
 */

typedef list<Register*> RegisterList;

typedef list<VarRegister*> VarRegisterList;

typedef RegisterList::iterator Registerit;

typedef VarRegisterList::iterator VarRegisterit;

#endif
