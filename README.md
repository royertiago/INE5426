INE5426 - Compiler Construction
===============================

This is the project developed at the INE5426 (Compller Construction)
diciscipline; it is a compiler to a language invented by us.

To see an overview of the language, see sample.txt; for an overview of
the program structure, see system\_overview.txt.


Libraries
---------

-   lexertl, for lexical analysis; and
-   Catch, for unit testing.

Both libraries are included in the Git project as Git submodules.


Building
---------

The makefile assumes that both the lexertl and Catch repositories are
in the root folder, as loaded submodules.

The source code uses several features of the upcoming C++14 standard,
as well as some GNU extensions, so you must have gcc-4.9 to build the program.
At the time of this writing (2014-11-03), the easiest way to get gcc-4.9 is to

    sudo apt-get install gcc-snapshot

The binary referred by the makefile assumes you got gcc-4.9 this way.
