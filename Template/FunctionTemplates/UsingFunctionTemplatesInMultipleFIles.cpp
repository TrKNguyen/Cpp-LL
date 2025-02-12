/*
    Key insight 

Template definitions are exempt from the part of the one-defintion rule
that requires only one definiton per program, so it is not a problem to
have the same template definition #included into multiple sources files 
And functions implicitly instantiated from function templates are 
implicitly inline, so they can be defined in multiple files, so long as 
each definiton is identical
The templates themselves are not inline, as the concept of inline only
applies to variables and functions 

Here's another example of a function template placed in a header file, 
so it can be included into multiple source files:

max.h:

#ifndef MAX_H
#define MAX_H

template <typename T>
T max(T x, T y)
{
    return (x < y) ? y : x;
}

#endif
foo.cpp:

#include "max.h" // import template definition for max<T>(T, T)
#include <iostream>

void foo()
{
	std::cout << max(3, 2) << '\n';
}

main.cpp:

#include "max.h" // import template definition for max<T>(T, T)
#include <iostream>

void foo(); // forward declaration for function foo

int main()
{
    std::cout << max(3, 5) << '\n';
    foo();

    return 0;
}
In the above example, both main.cpp and foo.cpp #include "max.h" 
so the code in both files can make use of the max<T>(T, T) function 
template.

    Best practice 

Templates that are needed in multiple files should be defined in a header
file, and then #included wherever needed. This allows the compiler to
see the full template definition and instantiate the template when needed


*/