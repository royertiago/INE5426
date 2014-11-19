Name Mangling
=============

In the C++ programming language, name mangling is a way to add type information
of the parameters of a function, enabling overload in precompiled code.
In our language, although much of "overload" is resolved at runtime, the
language permits overload by parameter number and position - that is, the same
name is allowed to refer to a binary operator, a prefix operator and a category
definition, simultaneously.

Besides whitespaces, the only forbidden characters in identifiers are braces
and comma. A category named meuIdentificadorFofinho will be mangled as
"meuIdentificadorFofinho{}", with an '{}' at the end. If it was an operator,
a comma and the operator category is appended, ignoring grouping. For instance,
if the above identifier refers to a prefix FY operator, the operator name is
mangled to "meuIdentificadorFofinho,FX".

It is important to note that this mangling is used only internally by the
compiler. No information about these names is added to the binary produced.