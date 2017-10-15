# The Tiny Compiler

The Tiny Compiler is a ["by the book"](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools) compiler which compiles a small C-like language (called Tiny) to Java Virtual Machine assembler.

In 2003, back when I was a Master student at university I followed a Compilers course. To this day, I still consider it one of the most fascinating topics in computer science.

This repo is my project assignment for the course. The objective: compile a C-like language to JVM bytecode.

The compiler features parsing and scanning using Bison and Flex parser generators. The source is type-checked, an Abstract Syntax Tree is used for checking for uninitialized variables etc. This tree is then mapped to so-called 'three address code'. This code is then higly optimized using a DAG (Directed Acyclic Graph) representation. Afterwards, the code is transformed to JVM Assembler (to be processed subsequently by tools like javaa which produce .class files for the JVM).

Detailed descriptions of optimizations and the generated code are explained in the docs.

The compiler is written in C++ (tested and compiled using gcc v3.1, remember, this project was written in 2003).

A simple `make` will build the compiler.