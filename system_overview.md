System Overview
===============

Take a look first at the file sample.txt to have an idea on how the
language is intended to work.

This documents explains how the lexical and syntatical analysis is done,
and what is intended for the semantical analysis (that is still not 
fully implemented).

Other information about the system can be found at concepts.md,
naming\_conventions.txt and mangling.md.



Lexical Analysis
================

This is the easy step. Most work is done by the lexertl library.
A tiny part of syntatical analysis is done here. Since there are only
eleven tokens that can start a declaration (class, category, include,
f, fx, xf, fy, yf, xfx, xfy, yfx) (first two are treated as the same),
if such lexemes words appear in the beginning of the line they are
treated as the correspondign token; in the middle of a sentence, they
are treated as simple identifiers.

The lexical analyser is a model of JavaIterator (see concepts.md).



Syntatical Analysis
===================

We did not use automation here; we wrote a recursive descendend parser
to build the basic syntatical structure.

Since there are user-definable priorities in the language operators,
we cannot build the syntax tree completely - this is done at semantical
level.

The syntatical analyser also models a JavaIterator.



Semantical Analysis
===================

The semantical analysis is still in construction.
    
There are in important distiction between syntactical constructs
(that are defined in file ast.h) and semantical constructs (that are defined 
in other files in the program).

This phase of the compiler performs transformations in the strucutres of ast.h
that were missing from syntatical analysis (since now we have the necessary
knowledge about priorities and variables to reconstruct the syntax tree for
every operator expression), resolve includes and category definitions and
builds the symbol table.

The main byproduct of this phase is a list with all NullayOperators,
UnaryOperators and Binary operators defined in input source code,
with respective dynamic overloads and syntax trees correctly constructed.
