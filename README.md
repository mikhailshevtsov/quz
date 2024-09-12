The quz library provides additional meta-information about standard C++ structures.
With its help, working with C++ structures becomes much easier and more convenient.

The main features provided by the library are
Accessing structure fields by index or name, using standard square bracket syntax.
Obtaining metainformation about structure fields, which includes types, string representations of types and names.
Converting C++ structures to std::tuple objects and vice versa.
Iteration over structure fields using quz::for_each loop.
Ability to define any number of structures in one translation unit.

All this is achieved by embedding a small number of macros in the structure definition.