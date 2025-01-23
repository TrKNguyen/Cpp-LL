/*
Looking at a typcial function dclaration it is not possible to determine
whether a function might throw an exceptoin or not: 

int doSth(); 

The above eaxmple can doSth() throw an exception? It's not clear. But 
the answer is important in some contexts. 
We described how an exception that is thrown our of a destructor during stack 
unwinding will cause the program to halt. If doSomething() can throw an 
exception. then calling it from a destructor is trisky. Although we
could have the destructor handle exceptions thrown by doSoemthing() (so those 
exceptions don't propagate out of the destructor), we have to remember 
to do this, and we have to ensure we cover all the different types 
of exceptions that may be thrown. 

While comments may help enumerate whether a function throws exceptions 
or not documentation can grow stale and there is no compiler enforcement
for comments. 

Exception specifications are a language mechansism that was originally designed
to document what kind of exceptions a fucntion might throw 
as part of function specification. While most of the exception specifications 
have now been depreacated or removed, one useful exception specification was 
added as a replacement, which we'll cover in this lesson 


*/

/*
    The noexcept specifier 

In C++, all functions are clssified as either non-throwing or potentially 
throwing. A non-throwing function is one that promises not to throw exceptions 
that are visible to the caller. A potentially throwing function may throw
exceptions that are visible to the acller. 

To define a function as non-throwing, we can use the noexcept specifier. 
To do so, we use the noexcept keyword in the funcion declaration, placed 
to the right of the function parameter list: 

void doSth() noexcept; 

Note that noexcept doesn't actually prevent the function from 
throwing exceptions or calling other function that are potentially throwing/ 
This is allowed so long as the noexcept function catches and handles 
those exceptions internally, and those exceptions do not exit the noexcept 
function. 

If an unhandled exception would exit a noexcept funciton, std::terminate 
will be called(even if there is an exception handler that would otherwise
handler such an exception somewhere up the stack). And if std::terminate
is called from inside a noexcept function, stack unwinding may or may not 
occur(depending on implementation and optimizations), which means our 
objects may or may not be destructed propoerly prior to termination 

    Key insight 

The promise that a noexcept function makes to not throw exceptions that 
are visible to the caller is a contractual promise, not a promise enforces 
by the compiler. So while calling a noexcept function should be safe, 
any exception handling bugs in the noexcept function that cause the 
contract to be broken will result in termination of the program! This 
shouldn't happen, but neither should bugs 

For this reason, it's best that noexcept functions don't mess wuth exceptions 
at all, or call potentially throwing function that could raise an exception 
A noexcept function can't have an exception handling bug if no exceptions 
can possibly be raised in the first place!

*/

/*

    Illustrating the behavior of noexcept functions and exceptions 

// h/t to reader yellowEmu for the first draft of this program
#include <iostream>

class Doomed
{
public:
    ~Doomed()
    {
        std::cout << "Doomed destructed\n";
    }
};

void thrower()
{
    std::cout << "Throwing exception\n";
    throw 1;
}

void pt()
{
    std::cout << "pt (potentally throwing) called\n";
    //This object will be destroyed during stack unwinding (if it occurs)
    Doomed doomed{};
    thrower();
    std::cout << "This never prints\n";
}

void nt() noexcept
{
    std::cout << "nt (noexcept) called\n";
    //This object will be destroyed during stack unwinding (if it occurs)
    Doomed doomed{};
    thrower();
    std::cout << "this never prints\n";
}

void tester(int c) noexcept
{
    std::cout << "tester (noexcept) case " << c << " called\n";
    try
    {
        (c == 1) ? pt() : nt();
    }
    catch (...)
    {
        std::cout << "tester caught exception\n";
    }
}

int main()
{
    std::cout << std::unitbuf; // flush buffer after each insertion
    std::cout << std::boolalpha; // print boolean as true/false
    tester(1);
    std::cout << "Test successful\n\n";
    tester(2);
    std::cout << "Test successful\n";

    return 0;
}

On the author’s machine, this program printed:

tester (noexcept) case 1 called
pt (potentially throwing) called
Throwing exception
Doomed destructed
tester caught exception
Test successful

tester (noexcept) case 2 called
nt (noexcept) called
throwing exception
terminate called after throwing an instance of 'int'
*/

/*
    The noexcept specifier with a Boolean parameter 

The noexcept specifier has an optional Bookean parameter. noexcept(true)
is equivalent to noexcept, meaning the funcion is non-throwing. noexcept(false)
means the function is potentailly wrong. These parameters are typically 
only used in template function, so that a template function can be 
dynamically created as non-throwing or potentaially throwing based on 
some parameterized value. 

*/

/*
    Which functions are non-throwing and potentially-throwing

Functions that are implicitly non-throwing 
-Destructors 

Functions that are non-throwing by default for implicitly-declared or 
defaulted funcitons: 

-Constructors: defaults, copy, move 
-Assignments: copy, move 
-Comparison operators(as of C++20) 

However, if any of these functions calls (explicitly or implicitly) another
function which is potentially throwing, then the listed function will be 
treated as potentially throwing as well. For example, if a class has 
a data member with a potentailly throwing constructors, then the class's 
constructors will be trated as potentially throwing as well. As another 
example, if a copy assignment operator calls a potentialky throwing assignment 
operator, then the copy assignment will be potentially throwing as well. 

Functions that are potentially throwing 

- Normal functions 
- User-defined constructors 
- User-defined operators 

*/

/*
    The noexcept operator

The noexcept operator can also be used inside expressions. It takes an 
expression as an argument, and returns true or false if the compiler 
thinks it will throw an exception or not. The noexcept operator is 
checked statically at compile-time, and doesn’t actually evaluate the 
input expression.

void foo() {throw -1;}
void boo() {};
void goo() noexcept {};
struct S{};

constexpr bool b1{ noexcept(5 + 3) }; // true; ints are non-throwing
constexpr bool b2{ noexcept(foo()) }; // false; foo() throws an exception
constexpr bool b3{ noexcept(boo()) }; // false; boo() is implicitly noexcept(false)
constexpr bool b4{ noexcept(goo()) }; // true; goo() is explicitly noexcept(true)
constexpr bool b5{ noexcept(S{}) };   // true; a struct's default constructor is noexcept by default

The noexcept operator can be used to conditionally execute code depending 
on whether it is potentially throwing or not. This is required to fulfill 
certain exception safety guarantees, which we’ll talk about in the next 
section.

*/

/*

    Exception safety guarantees 

An exception saety guarantees is a contractual guideline about how function or 
classes will behave in the event an exception occurs . There are fould level 
of exceion safety gurantee: 

- No guarantee -- There are no guarantees about what will happen if an 
exception is thrown (e.g. a class may be left in an unusable state)
- Basic guarantee -- If an exception is thrown, no memory will be leaked 
and the object is still usable, but the program may be left in a modified state.
- Strong guarantee -- If an exception is thrown, no memory will be leaked 
and the program state will not be changed. This means the function must 
either completely succeed or have no side effects if it fails. This is 
easy if the failure happens before anything is modified in the first place, 
but can also be achieved by rolling back any changes so the program 
is returned to the pre-failure state.
- No throw / No fail guarantee -- The function will always succeed (no-fail) 
or fail without throwing an exception that is exposed to the caller 
(no-throw). Exceptions may be thrown internally if not exposed. The 
noexcept specifier maps to this level of exception safety guarantee.

*/

/*

    When to use noexcept 

    Best practice 

Always make move constructor, move assignment ans swap function noexcept
Make copy constructors and copy assignment operator noexcept when we can 
Use noexcept on other functoin to express a no-fail or no-throw guarantee

    Best practice 

If we are uncertain whether a function should have a no-fail/no-throw 
guarantee, err on the side of caution and do not mark it with noexcept. 
Reversing a decision to use noexcept violateds an interface commitment 
to the user about the behavior of the function, and may break existing 
code. Making guarantees stronger by later adding noexcept to a function 
that was not originally no except is considered safe. 

*/

