/*
We discussed that the compiler can use implicit type conversion
to convert a value from one data type to another. When we 
want to numerically promote a value from one data type to 
a wider data type, using implicit type conversion is fine.


double d = 10 / 4; // does integer division, initializes d 
with value 2.0
Because 10 and 4 are both of type int, integer division is 
performed, and the expression evaluates to int value 2. 
This value then undergoes numeric conversion to double 
value 2.0 before being used to initialize variable d. Most 
likely, this isn’t what was intended.

In the case where you are using literal operands, replacing 
one or both of the integer literals with double literals 
will cause floating point division to happen instead:

double d = 10.0 / 4.0; // does floating point division, 
initializes d with value 2.5
But what if you are using variables instead of literals?
Consider this case:

int x { 10 };
int y { 4 };
double d = x / y; // does integer division, initializes d 
with value 2.0
Because integer division is used here, variable d will end 
up with the value of 2.0. How do we tell the compiler that 
we want to use floating point division instead of integer 
division in this case? Literal suffixes can’t be used with 
variables. We need some way to convert one (or both) of the 
variable operands to a floating point type, so that floating 
point division will be used instead.

Fortunataly, C++ comes with a number of different type casting 
operators(more commonly called casts) that can be used by 
the programmer to have the compiler perform type conversion 
Because casts are explicit requests by the programmer, this 
form of type conversion is often called an explicit type 
conversion (as opposed to implicit type conversion, where
the compiler performs a type conversion automatically)

*/

/*

    Type casting 

C++ supports 5 differnt types of casts: static_cast, dynamic_cast, 
const_cast, reinterepret_cast, and C-style casts. The first 
four are sometimes referred to as named casts. 

| Cast Type          | Description                                                                                                     | Safe?                   |
|--------------------|-----------------------------------------------------------------------------------------------------------------|-------------------------|
| `static_cast`      | Performs compile-time type conversions between related types.                                                   | Yes                     |
| `dynamic_cast`     | Performs runtime type conversions on pointers or references in a polymorphic (inheritance) hierarchy.           | Yes                     |
| `const_cast`       | Adds or removes `const`.                                                                                        | Only for adding `const` |
| `reinterpret_cast` | Reinterprets the bit-level representation of one type as if it were another type.                               | No                      |
| C-style casts      | Performs some combination of `static_cast`, `const_cast`, or `reinterpret_cast`.                                | No                      |

Each cast works the same way. As input, the cast takes an 
expression (that evaluates to a value or an object), and a 
target type. As output, the cast returns the result of 
the conversion 

Because they are the most commonly used casts, we'll cover
casts, and static_cast. 

const_cast and reinterpret_cast should generally be avoided
because they are only useful in rare cases and can be harmful 
if used incorrectly 

    Warning 

Avoid const_cast and reinterpret_cast unless you have a very 
good reason to use them.


*/


/*

    C-style cast

In standard C programming, casting is done via operator()
with the name of the type to convert to placed inside the 
parentheses, and the value to convert to placed immediately 
to the right of the closing parenthesis. In C++, this type 
of cast is called a C-style cast. We may still see these used
in code that has been converted from C. 

#include <iostream>

int main()
{
    int x { 10 };
    int y { 4 };

    std::cout << (double)x / y << '\n'; // C-style cast of x to double

    return 0;
}
In the above program, we use a C-style cast to tell the compiler 
to convert x to a double. Because the left operand of 
operator/ now evaluates to a floating point value, the 
right operand will be converted to a floating point value 
as well, and the division will be done using floating point 
division instead of integer division.

C++ also provides an alternative form of C-style cast known 
as a function-style cast, which resembles a function call:

std::cout << double(x) / y << '\n'; //  // function-style cast of x to double

The function-style cast makes it a bit easier to tell what 
is being converted (as it looks like a standard function argument).

There are a couple of significant reasons that C-style casts 
are generally avoided in modern C++.

First, although a C-style cast appears to be a single cast, 
it can actually perform a variety of different conversions 
depending on how it is used. This can include a static cast, 
a const cast, or a reinterpret cast (the latter two of which 
we mentioned that they should be avoided). A C-style cast 
does not make it clear which cast(s) will actual be performed, 
which not only makes your code that much harder to understand, 
but also opens the door for inadvertent misuse (where you 
think you’re implementing a simple cast and you end up doing 
something dangerous instead). Often this will end up producing 
an error that isn’t discovered until runtime.

Also, because C-style casts are just a type name, parenthesis, 
and variable or value, they are both difficult to identify 
(making your code harder to read) and even more difficult 
to search for.

In contrast, the named casts are easy to spot and search for, 
make it clear what they are doing, are limited in their 
abilities, and will produce a compilation error if you try 
to misuse them.

    Best practice

Avoid using C-style casts.


    For advanced readers

A C-style cast tries to perform the following C++ casts, 
in order:

const_cast
static_cast
static_cast, followed by const_cast
reinterpret_cast
reinterpret_cast, followed by const_cast

There is one thing we can do with a C-style cast that we 
can’t do with C++ casts: C-style casts can convert a derived 
object to a base class that is inaccessible (e.g. because 
it was privately inherited).


*/

/*

    static_cast should be used to cast most values 

By far the most used cast in C++ is the static cat operator, 
which is accessed via static_cast keyword. static_cast is used
when we want to explicitly convert a value of one type into 
a value of another type. We're previously seen static_cast 
used to convert a char into an int so that std::cout prints
it as an integer instead of a character: 

#include <iostream>

int main()
{
    char c { 'a' };
    std::cout << static_cast<int>(c) << '\n'; // prints 97 rather than a

    return 0;
}

To perform a static cast, we start with the static_cast
keywords, and then place the type to convert to inside 
angled brackets. The inside parenthese, we place the 
expression whose value will be converted. Now how much 
the syntax looks like a function a call to a function 
named static_cast<type() with the expression whose value 
will be converted provided as an argument. Static casting 
a value to another type of value returns a temporary object 
that has been direct-initialized with the converted value 

Here's how we'd use static_cast to solve the problem we 
introduced in the introducntion of this lesson: 

#include <iostream>

int main()
{
    int x { 10 };
    int y { 4 };

    // static cast x to a double so we get floating point division
    std::cout << static_cast<double>(x) / y << '\n'; // prints 2.5

    return 0;
}
static_cast<double>(x) returns a temporary double object 
containing the converted value 10.0. This temporary is 
then used as the left-operand of the floating point division.

There are two important properties of static_cast: 

First, static_cast provides compile-time type checking, if 
we try to convert a value to a type and the compiler doesn't 
know how to perform that conversion, we will get a compilation error 

// a C-style string literal can't be converted to an int, so the following is an invalid conversion
int x { static_cast<int>("Hello") }; // invalid: will produce compilation error
Second, static_cast is (intentionally) less powerful than a 
C-style cast, as it will prevent certain kinds of dangerous 
conversions (such as those that require reinterpretation 
or discarding const).

    Best practice

Favor static_cast when we need to convert a value from one 
type to another type. 

    For advanced readers

Since static_cast direct intializationm any explicit
constructors of the target class type will be considered
when intilizing the temporary object to be returned. 

*/

/*

    Using static_cast to make narrowing conversions explicit 

Compilers will often issue warnings when a potentailly unsafe
(narrowing) implicit type conversions is performed. For example,
consider the following snippet: 


int i { 48 };
char ch = i; // implicit narrowing conversion
Casting an int (2 or 4 bytes) to a char (1 byte) is potentially unsafe (as the compiler can’t tell whether the integer value will overflow the range of the char or not), and so the compiler will typically print a warning. If we used list initialization, the compiler would yield an error.

To get around this, we can use a static cast to explicitly convert our integer to a char:

int i { 48 };

// explicit conversion from int to char, so that a char is assigned to variable ch
char ch { static_cast<char>(i) };
When we do this, we’re explicitly telling the compiler that this conversion is intended, and we accept responsibility for the consequences (e.g. overflowing the range of a char if that happens). Since the output of this static cast is of type char, the initialization of variable ch doesn’t generate any type mismatches, and hence no warnings or errors.

Here’s another example where the compiler will typically complain that converting a double to an int may result in loss of data:

int i { 100 };
i = i / 2.5;
To tell the compiler that we explicitly mean to do this:

int i { 100 };
i = static_cast<int>(i / 2.5);
*/

/*

    Casting vs intialiaztion a temporay object. 

Let's say we have some variable x that we need to convert to an 
int. There are two conventiona; ways we can do this: 
- static_cast<int>(x), which returns a temporary int object 
direct-initalized with x
- int{x}, which creates temporary int object direct-list-initaiized 
with x. 

We should avoid int x(), which is a C-syntax cast. This will 
return a temporary int direct-initlized with the value of 
x (like we'd expect from the syntax), but it also has the other
downsides mentions in the C-style cast section(like throwing 
the possibility of performing a dangerous conversion)

There are (at least three) notable differences between teh static_cast 
and the directlist initialized, which disallows narrowing 
conversion This is great when initializing a variable, 
because we rarely intend to lose data in such cases. But when 
using a cast, it is presumed we know what we're doing, and if 
we want to do a cast that might lose some data, we whould be 
able to do that. Narrowing conversion restriction can be 
impediment in this case. 

Let's show an example of this, including how it can lead to 
platform-specific issues: 

#include <iostream>

int main()
{
    int x { 10 };
    int y { 4 };

    // We want to do floating point division, so one of the operands needs to be a floating point type
    std::cout << double{x} / y << '\n'; // okay if int is 32-bit, narrowing if x is 64-bit
}

In this example, we have decided to convert x to a double 
so we can do floating-point division rather than integer 
division. On a 32-bit architecture, this will work fine 
(because a double can represent all the values that can 
be stored in a 32-bit int, so it isn’t a narrowing 
conversion). But on a 64-bit architecture, this is not 
the case, so converting a 64-bit int to a double is a 
narrowing conversion. And since list initialization
disallows narrowing conversions, this won’t compile 
on architectures where int is 64-bits.

- static_case makes it clearer that we are intending to 
perform a conversion. Although the static_cast is move 
verbose than the directlist-initialized alternative, in this 
case, tha's a good thing, as it makes the conversion eaiser 
to psoit and serach for. That ultimately makes our code safer 
and easier to understand. 
- direct-list initialization of a temporary only allows 
single-word type names are allowed(the C++ standard calls)
these names simple type specifiers. So while int {x} is a 
valid conversion syntax. unsigned int {x} is not 

We can see this for ourself in the following example, which 
produces a compile error: 

#include <iostream>

int main()
{
    unsigned char c { 'a' };
    std::cout << unsigned int { c } << '\n';

    return 0;
}
There are simple ways to work around this, the easiest of 
which is to use a single-word type alias:

#include <iostream>

int main()
{
    unsigned char c { 'a' };
    using uint = unsigned int;
    std::cout << uint { c } << '\n';

    return 0;
}
But why go to the trouble when you can just static_cast?

For all these reasons, we generally prefer static_cast over 
direct-list-initialization of a temporary.

    Best practice

Prefer static_cast over initializing a temporary object when 
a conversion is desired


*/