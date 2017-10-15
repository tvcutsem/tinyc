#ifndef TINY_TO_JAVA_H
#define TINY_TO_JAVA_H

#include "../syntaxtree/identifier.h"
#include "../instructions/register.h"

#include <string>
#include <stdexcept>

/**
 * Maps a given Tiny type to it's Java VM equivalent, represented
 * as a string.
 * If the identifier given is an array, its java type corresponds to the base type.
 * If the identifier is a function, the java type is interpreted as the return type.
 * @throw logic_error if a mapping is asked for an unknown type
 */
std::string type2javaType(Identifier* id) throw (std::logic_error);

/**
 * Does the same, except that this maps a register to the java type.
 * VarRegisters are treated like Variables,
 * ArrayRegisters like Arrays,
 * and FunRegisters like Functions.
 */
std::string type2javaType(Register* reg) throw (std::logic_error);

std::string tinytype2java(Type t);

#endif
