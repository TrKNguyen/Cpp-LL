/*
    The properties of an expression 
To determine how expressions should evaluate and where they can be used
all expressions in C++ have two properties: a type and a value category

*/ 

/* 
    Topic 1: The type of an expression

The type of an expression is the type of value, object or function that 
results from the evaluated expression

The compiler can use the type of an expression to determine whether an 
expression is valid in the context

#include <iostream>

void print(int x)
{
    std::cout << x << '\n';
}

int main()
{
    print("foo"); // error: print() was expecting an int argument, we tried to pass in a string literal

    return 0;
}

The type of expression we passing(string) doesn't match with int and no 
conversion can be found => compile error 

Important: the type of any expression should be determinable at compile 
time to facilitate type checking and type deduction(auto)

*/

/*
    Topic 2: The value category of an expression (very important)

int main()
{
    int x{};

    x = 5; // valid: we can assign 5 to x
    5 = x; // error: can not assign value of x to literal value 5

    return 0;
}

only x = 5 is valid while 5 = x doesn't 
Question: How the compiler determine which expressions can legally appear on 
either side of an assignment statement? 

The answer lies in the second property of expressions: the value category
Definition: the value category of an expression indicates whether an expression 
resolve to (produce) a value, a function or an object of some kind 

Prior to C++11, there were only 2 possible value categories: lvalue and rvalue

In C++ 11, three additional value caregories (gvalue, prvalue, and xvalue) were
added to support a new feature move semantics 
*/



/*
    Lvalue and rvalue expressions 

A lvalue is an expression have address or an identifiable object or function.
The term "identity" is used by C++ standard, an entity that has an identity can be 
differentiated from other similar entities(they use address of them to identify and 
compare with other entities' address)

Good point: Entities/Object with identities can be accessed via an identifier, 
reference, pointer and have life time longer than a single expression or statement

int main()
{
    int x { 5 };
    int y { x }; // x is an lvalue expression

    return 0;
}

Lvalue is also divided into 2 subtypes: a modifiable lvalue(whose value
can be modified) and a non-modifiable lvalue(whose value can't be modified)

int main()
{
    int x{};
    const double d{};

    int y { x }; // x is a modifiable lvalue expression
    const double e { d }; // d is a non-modifiable lvalue expression

    return 0;
}

A rvalue is an expression that is not lvalue. Rvalue expressions evaluate 
to(produce) a (real) value

Commonly rvalues include literals, the return value of an functions and 
operators that return by value, the biggest difference is that rvalues 
aren't identifiable(dont have address for them) and thy have to be used 
immediately, it only exist within the scope of the expression in which 
they are used (it will be removed from memory right after that)

int return5()
{
    return 5;
}

int main()
{
    int x{ 5 }; // 5 is an rvalue expression
    const double d{ 1.2 }; // 1.2 is an rvalue expression

    int y { x }; // x is a modifiable lvalue expression
    const double e { d }; // d is a non-modifiable lvalue expression
    int z { return5() }; // return5() is an rvalue expression (since the result is returned by value)

    int w { x + 1 }; // x + 1 is an rvalue expression
    int q { static_cast<int>(d) }; // the result of static casting d to an int is an rvalue expression

    return 0;
}

    Key insight

Lvalue expressions evaluate to an identifiable object.
Rvalue expressions evaluate to a value.
*/


/*
    Value categories and operators 

Unless otherwise specified, operators expect their operands to be rvalues
binary operator+ expects its operands to be rvalues

#include <iostream>

int main()
{
    std::cout << 1 + 2; // 1 and 2 are rvalues, operator+ returns an rvalue
    // The literals 1 and 2 are both rvalue expressions. operator+ will 
    // happily  use these to return the rvalue expression 3.
    return 0;
}


    Comeback to previous case: why x = 5 is value but 5 = x is not
Imporant: 
an assignment operation requires its left operand to be a modfiable
lvalue expressions!!!!!
=> 5 = x is invalid because 5 is rvalue expression

int main()
{
    int x{};

    // Assignment requires the left operand to be a modifiable lvalue expression and the right operand to be an rvalue expression
    x = 5; // valid: x is a modifiable lvalue expression and 5 is an rvalue expression
    5 = x; // error: 5 is an rvalue expression and x is a modifiable lvalue expression

    return 0;
}

*/

/*
    Lvalue-to-rvalue conversion 
Because assignment operations expect the right oprand to be an rvalue
expression, there will be some confusing in here: 

int main()
{
    int x{ 1 };
    int y{ 2 };

    x = y; // y is not an rvalue, but this is legal

    return 0;
}

In cases where an rvalue is expected but an lvalue is provided, the 
lvalue will undergo an lvalue-to-rvalue conversion to that it can be 
used in such contexts. This basically mean that the lvalue is evaluated
to produce its value, which is an rvalue. 

In above example, y undergos an lvalue-and=rvalue conversions, which 
make y to produce the rvalue(2), which is then assigned to x.

    Key insight

An lvalue will implicitly convert to an rvalue. This means an lvalue 
can be used anywhere an rvalue is expected. An rvalue, on the other hand,
will not implicitly convert to an lvalue.

Another example: 
int main()
{
    int x { 2 };

    x = x + 1;

    return 0;
}

In this assignment, the variable x is being used in two different contexts. 
On the left side of the assignment operator(where an lvalue expression is 
required), x is an lvalue expression that evaluates to variable x
On the right side of the assignment operator, x undergoes an lvalue-to-rvalue
conversion and is then evaluated so that its(2) can be used as the left
operand of operator+. operator+ returns the rvalue expression 3, which is 
then used as the right operand for the assignment
*/

/*
    How to differntiate lvalues and rvalues 
Tip

A rule of thumb to identify lvalue and rvalue expressions:

Lvalue expressions are those that evaluate to functions or identifiable 
objects (including variables) that persist beyond the end of the expression.
Rvalue expressions are those that evaluate to values, including literals
and temporary objects that do not persist beyond the end of the expression.

Advanced: In C++11, rvalues are broken into two subtypes: prvalues and
xvalues, so the rvalues here are the sum of both of those categories.

*/ 
#include <iostream>
#include <string>

// T& is an lvalue reference, so this overload will be preferred for lvalues
template <typename T>
constexpr bool is_lvalue(T&)
{
    return true;
}

// T&& is an rvalue reference, so this overload will be preferred for rvalues
template <typename T>
constexpr bool is_lvalue(T&&)
{
    return false;
}

// A helper macro (#expr prints whatever is passed in for expr as text)
#define PRINTVCAT(expr) { std::cout << #expr << " is an " << (is_lvalue(expr) ? "lvalue\n" : "rvalue\n"); }

int getint() { return 5; }

int main()
{
    PRINTVCAT(5);        // rvalue
    PRINTVCAT(getint()); // rvalue
    int x { 5 };
    PRINTVCAT(x);        // lvalue
    PRINTVCAT(std::string {"Hello"}); // rvalue
    PRINTVCAT("Hello");  // lvalue
    PRINTVCAT(++x);      // lvalue
    PRINTVCAT(x++);      // rvalue
}

/*
This method relies on two overloaded function: one with lvalue reference
parameter and one with an rvalue reference parameter. The lvalue reference
version will be preferred for lvalue arguments, and the rvalue reference
version will be preferred for rvalue arguments. Thus we can determine 
whether the argument is an lvalue or rvalue based on which function gets 
selected

Very interesting find out: We can see that whether operator++ results in 
an lvalue or an rvalue depends on whether it is used as prefix operator
(which returns an lvalue) or a postfix oeprator(which return an rvalue) 

*/



