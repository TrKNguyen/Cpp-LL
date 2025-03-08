/*
In c++, a reference is an alias for an existing object. Once a reference
has been defined, any operation on the reference is applied to the object
being referenced. This means we can use a refence to read or modify the 
object being referenced 

(references actually is a address refer to the object)
Although references might seem silly, useless, or redundant at first
but it is super important :D 
References are used everywhere in C++ 
*/

/*
    Lvalue references types 

An lvalue reference acts as an alias for an existing lvalue(such as 
a variable) 
Just like the type of an object determines what kind of can hold, 
the type of a reference determines what type of object it can reference

Lvalue reference types can be identifed by use of a single ampersand(&)
in the type specifier: 
// regular types
int        // a normal int type (not an reference)
int&       // an lvalue reference to an int object
double&    // an lvalue reference to a double object
const int& // an lvalue reference to a const int object

For example, int& is the type of an lvalue refence to int object, and const int& 
is the type of an lvalue reference to a const int object 

A type that specifies a reference(e.g int&) is called a reference type
The type that can be referenced(e.g int) is called the referenced type(differenct)

    There are two kinds of lvalue references:

An lvalue reference to a non-const is commonly just called an “lvalue reference”, 
but may also be referred to as an lvalue reference to non-const or a non-const 
lvalue reference (since it isn’t defined using the const keyword).
An lvalue reference to a const is commonly called either an lvalue reference 
to const or a const lvalue reference.
*/

/*
    Lvalue reference variables

One of things we can do with an lvalue refence type is create lvalue 
reference variable. An lvalue reference variable is a variable that acst
as a reference to an lvalue(usually another variable)

#include <iostream>

int main()
{
    int x { 5 };    // x is a normal integer variable
    int& ref { x }; // ref is an lvalue reference variable that can now be used as an alias for variable x

    std::cout << x << '\n';  // print the value of x (5)
    std::cout << ref << '\n'; // print the value of x via ref (5)

    return 0;
}

In the above example the type int& defines ref as an lvalue reference 
to an int, which we then intialize with lvalue expression x. Thereafter,
ref and x can be used sysnoymously. This program thus prints
5
5

From the comiler's perspective, it doesn't matter whether the ampersand
is "attached" to the type name(int& ref) or the variable's name(int &ref),
and which you choose is a matter of style. Modern C++ programmers tend to 
prefer attaching the ampersand to the type, as it makes clearer that 
the refence is part of the type information not the identifier 

    Best practice
When defining a reference, place the ampersand next to the type(not 
the reference variable's name)

*/

/*
Reference initialization

int main()
{
    int& invalidRef;   // error: references must be initialized

    int x { 5 };
    int& ref { x }; // okay: reference to int is bound to int variable

    return 0;
}

When a reference is initalized with an object, we say it is bound to that 
object(or function). The process by which such a reference is bound is 
called reference binding. The object being referenced is sometimes
called referent

Non-const lvalue references can only be bound to a modfiable lvalue

int main()
{
    int x { 5 };
    int& ref { x };         // okay: non-const lvalue reference bound to a modifiable lvalue

    const int y { 5 };
    int& invalidRef { y };  // invalid: non-const lvalue reference can't bind to a non-modifiable lvalue
    int& invalidRef2 { 0 }; // invalid: non-const lvalue reference can't bind to an rvalue

    return 0;
}

    Key insight
if non-const lvalue references could be bound to non-modifiable (const)
lvalues or rvalues, then you would be able to change those values 
through the reference, which would be a violation of their const-ness

Lvalue references to void are disalloed
Even though the tyoe of the reference(int&) doesn't exactly match the type 
of the object being bound(int), no conversion is applied here, the 
difference in types is handled as part of the reference initilization 
process

*/

/*
    A reference will(usually) only bind to an object matching its 
referenced type 

Key insight 
Since the result of a conversion is an rvalue, and a non-const lvalue
reference can't bind to an rvalue, trying to bind a non-const reference 
to an object that does not match its referenced type will result in 
a compilation error

int main()
{
    int x { 5 };
    int& ref { x };            // okay: referenced type (int) matches type of initializer

    double d { 6.0 };
    int& invalidRef { d };     // invalid: conversion of double to int is narrowing conversion, disallowed by list initialization
    double& invalidRef2 { x }; // invalid: non-const lvalue reference can't bind to rvalue (result of converting x to double)

    return 0;
}

*/

/*
References can't be reseated(cahnged to refer to another object)
#include <iostream>

int main()
{
    int x { 5 };
    int y { 6 };

    int& ref { x }; // ref is now an alias for x

    ref = y; // assigns 6 (the value of y) to x (the object being referenced by ref)
    // The above line does NOT change ref into a reference to variable y!

    std::cout << x << '\n'; // user is expecting this to print 5

    return 0;
}

But it prints: 6 
When a reference is evaluated in an expression, it resolves to object
it's referencing. So ref = y doesn't change ref to now reference y. 
Ratherm because ref is an alias for x, the expression evaluates as if it 
was written x = y
and since y evaluates to value 6, x is assigned the value 6 
*/


/*
    Reference scope and duration

Reference variables follow the same scoping and duration rules that 
normal variables do: 
#include <iostream>

int main()
{
    int x { 5 }; // normal integer
    int& ref { x }; // reference to variable value

     return 0;
} // x and ref die here
*/

/*
    References and referents have independent lifetimes

The lifetime of a reference and the life time of its referent are 
independent. That means, both of following are true: 

- The reference can be destroyed before the referenced object
- The referened object can be destroyed before the reference

#include <iostream>

int main()
{
    int x { 5 };

    {
        int& ref { x };   // ref is a reference to x
        std::cout << ref << '\n'; // prints value of ref (5)
    } // ref is destroyed here -- x is unaware of this

    std::cout << x << '\n'; // prints value of x (5)

    return 0;
} // x destroyed here


*/

/*
    Dangling references

When an object being referenced is destroyed before a reference to it,
the reference is left referencing an object that no longer exist
=> Dangling reference, accessing this dangling reference leads to 
undefined behaviours

*/

/*
    Lvalue references to const 
int main()
{
    const int x { 5 }; // x is a non-modifiable (const) lvalue
    int& ref { x }; // error: ref can not bind to non-modifiable lvalue

    return 0;
}

int main()
{
    const int x { 5 };    // x is a non-modifiable lvalue
    const int& ref { x }; // okay: ref is a an lvalue reference to a const value

    return 0;
}
Lvalue references to const can bind to non-modifiable lvalues
*/

/*

    Initializing an lvalue reference to const with a modifiable lvalue
Lvalue references to const can bind to modifiable lvalues. In this case
the object being referenced is treated as const when accessed through 
the reference(even though the underlying object is non-const)

#include <iostream>

int main()
{
    int x { 5 };          // x is a modifiable lvalue
    const int& ref { x }; // okay: we can bind a const reference to a modifiable lvalue

    std::cout << ref << '\n'; // okay: we can access the object through our const reference
    ref = 7;                  // error: we can not modify an object through a const reference

    x = 6;                // okay: x is a modifiable lvalue, we can still modify it through the original identifier
    // Important: we still can modify x using itself, cannot modify it
    // using ref 
    return 0;
}

    Best pracice
Favor lvalue references to const over lvalue references to non-const 
unless you need to modify the object being referenced 

    Initializing an lvalue reference to const with an rvalue
Very surprisingly, lvalues references to const can also bind to rvalues :)))))
#include <iostream>

int main()
{
    const int& ref { 5 }; // okay: 5 is an rvalue

    std::cout << ref << '\n'; // prints 5

    return 0;
}

When this happens, a temporary object is created and initialized with 
the rvalue, and the reference to const is bound to that temporary object 

    Initializing an lvalue reference to const with a value of a different type
Lvalue references to const can even bind to values of a different type,
so long as those values can be implicitly converted to the reference type

#include <iostream>

int main()
{
    // case 1
    const double& r1 { 5 };  // temporary double initialized with value 5, r1 binds to temporary

    std::cout << r1 << '\n'; // prints 5

    // case 2
    char c { 'a' };
    const int& r2 { c };     // temporary int initialized with value 'a', r2 binds to temporary

    std::cout << r2 << '\n'; // prints 97 (since r2 is a reference to int)

    return 0;
}

    Key insight 

if you try to bind a const lvalue reference to a value of different type 
the compiler will create a temporary object of the same type as the reference
initialize it using the value, and then bind the reference to the temporary 


    Warning

We normally assume that a reference is identical to the object it is bound to 
-- but this assumption is broken when a reference is bound to a temporary copy
of the object or a temporary resulting from the conversion of the object instead.
Any modifications subsequently made to the original object will not be seen 
by the reference (as it is referencing a different object), and vice-versa.

Here’s a silly example showing this:
#include <iostream>

int main()
{
    short bombs { 1 };         // I can has bomb! (note: type is short)

    const int& you { bombs };  // You can has bomb too (note: type is int&)
    --bombs;                   // Bomb all gone

    if (you)                   // You still has?
    {
        std::cout << "Bombs away!  Goodbye, cruel world.\n"; // Para bailar la bomba
    }

    return 0;
}
In the above example, bombs is a short and you is a const int&. Because
you can only bind to an int object, when you is initialized with bombs,
the compiler will implicitly convert bombs to an int, which results in the 
creation of a temporary int object (with value 1). you ends up bound to this 
temporary object rather than bombs.

When bombs is decremented, you is not affected because it is referencing a 
different object. So although we expect if (you) to evaluate to false, it 
actually evaluates to true.

If you would stop blowing up the world, that would be great.

    Key insight 
Lvalue references can only bind to modifiable lvalues 
Lvalue references to const can bind to modifiable lvalues, non-modifiable, 
and rvalues. This makes them a much more flexible type of references.
*/