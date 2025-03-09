/*
    The class invariant problem

The biggest difficulty with structs is that they do not provide the effective
way to ducment and enfore class invariants. 
Recall: invariants is a condition that must be true while some component
is executing

In the context of class types(which include structs, classes, and unions)
a class invariant is a condition that must be true throughout the life 
time of an object in order for the object to remain in a valid state. An
object that has a violated class invariant is said to be in an invalid
state, and unexpected or undefined behaviour may result from further
use of that object. 

    Key insight
Using an object whose class invariant has been violated may result in
unexpected or undefined behavior. 

With this struct 
struct Pair
{
    int first {};
    int second {};
};
The first and second members can be independently set to any value, so
Pair struct has no invariant. 
Now consider the following almost-identical struct: 
struct Fraction
{
    int numerator { 0 };
    int denominator { 1 };
};
In the sake of math, denominator of 0 is mathematically undefined
=> we want to ensure the denominator memeber of a Fraction object 
is never set to 0. If it is, then that Fraction object is in an 
in valid state, and undefined behaviour may result from further
use of that object 

    Key insight

Relying on the user of an object to maintain class invariants is likely
to result in problems. 

Ideally, we'd love to bulletproof our class types so that an object either
can't be put into an invalid state, or can signal immediately if it is
(rather than letting undefined behavior occur at some random point in the 
future)

Structs (as aggregates) just don't have the mechanics required to solve this 
problem in an elegant way 

*/

/*
    Introduction to classes

When developing C++, Bjarne Stroustrup wanted to introduce capabilities
that would alloiw developers to create program-defined type that could be 
used more intuitively. He was also interested in finding elegant solutions
to some of the frequent pitfalls and maintenance challenges that plague large
complex programs(such as the previously mentioned class invariant issue)

Bjarne was convinced that it was possible to develop a program-defined
type that was general and powerful enough to be used for almost anything. 
Class was born

Just like structs, a class is a program-defined compound type that can have
many member variables with different types 

    Key insight

From a technical standpoint, structs and classes are almost identical
-- therefore, any example that is implemented using a struct could be
implemented using a class, or vice-versa. However, from a practical 
standpoint, we use structs and classes differently.

    Defining a class        
Because a class is a program-defined data type, it must be defined before
it can be used. Classes are defined similarly to structs, except we use the
class keyword instead of struct. For example, here is a definition for 
a simple employee class: 
class Employee
{
    int m_id {};
    int m_age {};
    double m_wage {};
};

Question: Why the member variables of a class are often prefixed with an "m"? 
Answer: Will know later

*/

/*
    Most of the C++ standard library is classes
Classes are really heart and sould of C++ they are so foundational 
that C++ was originally with the name "C with classes" :DDD 
Once we are familiar with classes, much of our time in C++ will be
spent writing, testing and using them

*/

/*
    Best practice

Member functions can be used with both structs and classes.
However, structs should avoid defining constructor member functions,
as doing so makes them a non-aggregate.

    Best practice

If your class type has no data members, prefer using a namespace.


*/

/*
Key insight

A const member function may not: modify the implicit object, 
call non-const member functions.
A const member function may: modify objects that aren’t the implicit
object, call const member functions, call non-member functions.

Best practice

A member function that does not (and will not ever) modify the state 
of the object should be made const, so that it can be called on both
const and non-const objects.

Key insight

One of the best reasons to prefix private data members with “m_” is to 
avoid having data members and getters with the same name (something C++ 
doesn’t support, although other languages like Java do).
*/