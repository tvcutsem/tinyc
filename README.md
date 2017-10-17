# The Tiny Compiler

The Tiny Compiler is a ["by the book"](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools) compiler which compiles a small C-like language (called Tiny) to Java Virtual Machine assembler.

In 2003, back when I was a CS student at university I followed a Compilers course. To this day, I still consider it one of the most fascinating topics in computer science.

This repo contains my project assignment for the course. The objective: compile a C-like language to JVM bytecode. I share it here in the hope others may find it useful. Keep in mind this was a student project. Learn from it what you can (in case you are curious, I got the maximum score for the course :wink:)

The compiler architecture is built using an object-oriented architecture with extensive use of the visitor design pattern to process the datastructures
representing source code and IR code. It follows a traditional compiler pipeline (lexing, parsing, name resolution and typechecking, IR code generation and optimization, target code generation and optimization).

The compiler is written in C++ and was tested and compiled using gcc v3.1 (remember, this project was written in 2003).

## Features

  * Static typechecking
  * Liveness analysis (detect unused variables, use before initialize)
  * Control flow analysis (detect dead code, missing return statements)
  * Constant folding
  * IR based on three address code (static single assignment form)
  * IR optimization pass based on DAG representation (reduces temporary registers, common subexpression elimination)
  * Code generation to JVM bytecode, register machine to stack machine translation
  * Peephole optimization

Detailed descriptions of optimizations and the generated code are explained in [this document](tinyc.pdf).

## Building and running

A simple `make` command will build the tiny compiler.

To compile a file using the tiny compiler, e.g. `factorial.tiny`:

```
./tinyc.sh factorial < factorial.tiny
```

This generates a Java `.class` file.