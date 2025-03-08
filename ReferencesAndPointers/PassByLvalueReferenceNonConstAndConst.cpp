/*
    Recall: Pass by value

#include <iostream>

void printValue(int y)
{
    std::cout << y << '\n';
} // y is destroyed here

int main()
{
    int x { 2 };

    printValue(x); // x is passed by value (copied) into parameter y (inexpensive)

    return 0;
}

In this program, when the function is called, the value of x(2) is copied into
parameter y. Then, at the end of the function, the object y is destroyed!
*/

/*
    Problem: Some objects are expensive to copy 

Most of the types provided by the standard library(e.g std::string) are class
types. Class types are usually expensive to copy. Whenever possible, we want 
to avoid making unnecessary copies of jects that are expensive to copy, 
especially when we will destroy those copies almost immediately

We can do better!!!!!
*/

/*
    Pass by non-const lvalue reference 
One way to avoid making an expensive copy of an argument when calling a function
is to use pass by reference instead of pass by value. When using pass by reference
we declare a function parameter as a reference type(or const reference type) rather
than as a normal type. When the function is called, each reference parameter is bound 
to the appropriate argument. Because the reference acts as an alias for the 
argument, no copy of the argument is made. (but seem like still need to make a copy of address)

Key insight 
Pass by reference allows us to pass arguments to a function without makeing 
copies of those arguments each time the function is called!!!

    Pass by reference allows us to change the value of an argument
Key insight
Passing values by reference to non-const allows us to write functions that modify 
the value of arguments passed in.

   Pass by reference can only accept modifiable lvalue arguments

Because a reference to a non-const value can only bind to a modifiable lvalue 
this means that pass by reference only works with arguments that are modifiable
lvalues. In practical terms, this significantly limits the usefulness of pass
by reference to non-const => we cannot pass conse variables or literals

#include <iostream>

void printValue(int& y) // y only accepts modifiable lvalues
{
    std::cout << y << '\n';
}

int main()
{
    int x { 5 };
    printValue(x); // ok: x is a modifiable lvalue

    const int z { 5 };
    printValue(z); // error: z is a non-modifiable lvalue

    printValue(5); // error: 5 is an rvalue

    return 0;
}

But we always have solutions
*/

/*
    Pass by const lvalue reference

#include <iostream>

void printRef(const int& y) // y is a const reference
{
    std::cout << y << '\n';
}

int main()
{
    int x { 5 };
    printRef(x);   // ok: x is a modifiable lvalue, y binds to x

    const int z { 5 };
    printRef(z);   // ok: z is a non-modifiable lvalue, y binds to z

    printRef(5);   // ok: 5 is rvalue literal, y binds to temporary int object

    return 0;
}

Passing by const references offers the same primary benefit as pass by non-const
reference(avoiding making a copy of the argument), while also guaranteering
that the function can not change the value being referenced

For example, the following action is disallowed: 
void add(const int& val) {
    val+=1; // not allowed: val is const
}

    Best practice 

Favor passing by const reference over passing by non-const reference unless 
you have specific reason to do otherwise (the function needs to change the
value of argument)

*/

/*
    Passing arguments of a different type to a const lvalue reference parameter

Lvalue references to const: we noated that const lvalue reference can bind to 
a value of different type, as long as that value is convertiable to the type 
of the reference. Conversion creates a temporary object(not the original one!!!!)
that the reference parameter can then bind to.

#include <iostream>

void printVal(double d)
{
    std::cout << d << '\n';
}

void printRef(const double& d)
{
    std::cout << d << '\n';
}

int main()
{
    printVal(5); // 5 converted to temporary double, copied to parameter d
    printRef(5); // 5 converted to temporary double, bound to parameter d

    return 0;
}

With pass-by-value, we expect a copy to be made, so if a conversion occurs
first(resulting in an additional copy), it's raely an issue(and the compiler
will likely optimize on of the two copies away). 

However, we often use pass by reference when we dont want a copy to be made. 
if a conversion occurs first, this will typically result in a copy being made, 
which can be suboptimal

*/

/*
    Missing pass by value and pass by reference

#include <string>

void foo(int a, int& b, const std::string& c)
{
}

int main()
{
    int x { 5 };
    const std::string s { "Hello, world!" };

    foo(5, x, s);

    return 0;
}

In this example, first argument is passed by value, second is paased by reference
third is passed by const reference
*/

/*
    When to use pass by value vs pass by reference

Best practice
As a rule of thumb, pass fundamental types by value and class types by const
references. 

If you aren't sure what to do, pass by const reference :D, as you're less like;y 
to encounter unexpected behaviour
    

*/


/*
    Very important!!!!!!!!!!
    The cost of pass by value vs pass by reference 

Not all class types need to be passed by reference(such as std::string_view)
which is normally passed by value). 

Question: why we dont just pass everything by reference???

Let dig deep into the cost of pass by value vs pass by reference, and try 
to find our best practice as to when we should use each!!!!!


First, we need to consider the cost of initializing the function parameter. With
pass by value, initialization means makeing a copy. The cost of copying an object 
is generally proportional to two things: 
- The size of the object. Objects that use more memory take more time to copy
- Any additional set up costs. Some class types do additional setup when they 
are instantiated(e.g such as opening a file or database :D, or allocating a 
certain amount of dynamic memory to hold an object of a variable size). These
setup costs muse be paid each time an object is copied

On the other hand, binding a reference to an object is always fast(about the 
same speed as copying a fundamental type). Second, we need to consider the cost 
of using the function parameter. When setting up a function call, the compiler
may be able to optimize by placing a reference or a copy of a passed-by-value 
argument(if it is small in size) into CPU register!!! (which so so fast(fastest?)
to access) rather than in RAM(which is much slower to access) :DDD

Each time a value parameter is used, the running program can directly access
the storage location(CPU register or RAM) of the copied argument. However, when
a reference parameter is used, there is usually an extra step. The running 
program must first directly access the storage location(CPU register or RAM) 
allocated to the reference, in order to determin which object is being referenced
Only then can it access the storage location of referenced object(in RAM)

Therefore, each use of value parameter is a single CPU register or RAM access, 
whereas each use of a reference parameter is a single CPU register or RAM access
plus a second RAM access :DDDDDDD

Third, the compiler can sometimes optimize code that uses pass by value 
more effectively than code that uses pass by reference. In particular, 
optimizers have to convervative when there is any chance of aliasing
(when two or more pointers or references can access the same object)
Because pass by value results in the copy of argument values, there
is no chance for aliasing to occur, allowing optimizers to be more 
aggressive. 

We can now answer these question of why we don't pass everything by 
reference: 
-   For objects that are cheap to copy, the cost of copying is similar
to the cost of binding, but accessing the objects is faster and the compiler
is likely to be able to optimize better
-   For objects that are expensive to copy, the cost of the copy dominates
other performance considerations

The last question then is, how do we define "Cheap to copy"? There is 
no absoluate answer here, as this varis by compiler, use case and 
architecture. However, we can formulate a good rule of thumb: An object
is cheap to copy if it uses 2 or fewer "words" of memory (where a "word"
is approximated by the size of memory address) and it has no set up costs

So interesting :DDDDDD

#include <iostream>

// Function-like macro that evaluates to true if the type (or object) is equal to or smaller than
// the size of two memory addresses
#define isSmall(T) (sizeof(T) <= 2 * sizeof(void*))

struct S
{
    double a;
    double b;
    double c;
};

int main()
{
    std::cout << std::boolalpha; // print true or false rather than 1 or 0
    std::cout << isSmall(int) << '\n'; // true

    double d {};
    std::cout << isSmall(d) << '\n'; // true
    std::cout << isSmall(S) << '\n'; // false

    return 0;
}

For function parameters, prefer std::string_view over const std::string&
in most cases 
One question that comes up often in modern C++: when writing a function
that has a string parameter, should the type of the parameter be const
std::string& or std::string_view?

In most cases, std::string_view is the better choice, as it can handle
a wider range of argument types efficiently. A std::string_view parameter
also allows the caller to pass in a substring without having to copy that
substring into its own string first.

void doSomething(const std::string&);
void doSomething(std::string_view);   // prefer this in most cases

There are a few cases where using a const std::string& parameter may 
be more appropriate:

If you’re using C++14 or older, std::string_view isn’t available.
If your function needs to call some other function that takes a C-style string
or std::string parameter, then const std::string& may be a better choice, as 
std::string_view is not guaranteed to be null-terminated (something that C-style
string functions expect) and does not efficiently convert back to a std::string.

Best practice

Prefer passing strings using std::string_view (by value) instead of 
const std::string&, unless your function calls other functions that require 
C-style strings or std::string parameters.

Why std::string_view parameters are more efficient than const std::string& 
<Advanced>

In C++, a string argument will typically be a std::string, a std::string_view, or 
a C-style string/string literal.

As reminders:

If the type of an argument does not match the type of the corresponding parameter, the 
compiler will try to implicitly convert the argument to match the type of the parameter.
Converting a value creates a temporary object of the converted type.
Creating (or copying) a std::string_view is inexpensive, as std::string_view does
not make a copy of the string it is viewing.
Creating (or copying) a std::string can be expensive, as each std::string object
makes a copy of the string. Here’s a table showing what happens when we try to pass each type:
____________________________________________________________________________________________________________________
|Argument Type	                    | std::string_view parameter	| const std::string& parameter                 |
|std::string	                    | Inexpensive conversion	    | Inexpensive reference binding                |
|std::string_view	                | Inexpensive copy	            | Expensive explicit conversion to std::string |
|C-style string / literal	        | Inexpensive conversion	    | Expensive conversion                         |
|------------------------------------------------------------------------------------------------------------------|
With a std::string_view value parameter:

If we pass in a std::string argument, the compiler will convert the std::string to a std::string_view, which is inexpensive, so this is fine.
If we pass in a std::string_view argument, the compiler will copy the argument into the parameter, which is inexpensive, so this is fine.
If we pass in a C-style string or string literal, the compiler will convert these to a std::string_view, which is inexpensive, so this is fine.
As you can see, std::string_view handles all three cases inexpensively.

With a const std::string& reference parameter:

If we pass in a std::string argument, the parameter will reference bind to the argument, which is inexpensive, so this is fine.
If we pass in a std::string_view argument, the compiler will refuse to do an implicit conversion, and produce a compilation error. We can use static_cast to do an explicit conversion (to std::string), but this conversion is expensive (since std::string will make a copy of the string being viewed). Once the conversion is done, the parameter will reference bind to the result, which is inexpensive. But we’ve made an expensive copy to do the conversion, so this isn’t great.
If we pass in a C-style string or string literal, the compiler will implicitly convert this to a std::string, which is expensive. So this isn’t great either.
Thus, a const std::string& parameter only handles std::string arguments inexpensively.

The same, in code form:

#include <iostream>
#include <string>
#include <string_view>

void printSV(std::string_view sv)
{
    std::cout << sv << '\n';
}

void printS(const std::string& s)
{
    std::cout << s << '\n';
}

int main()
{
    std::string s{ "Hello, world" };
    std::string_view sv { s };

    // Pass to `std::string_view` parameter
    printSV(s);              // ok: inexpensive conversion from std::string to std::string_view
    printSV(sv);             // ok: inexpensive copy of std::string_view
    printSV("Hello, world"); // ok: inexpensive conversion of C-style string literal to std::string_view

    // pass to `const std::string&` parameter
    printS(s);              // ok: inexpensive bind to std::string argument
    printS(sv);             // compile error: cannot implicit convert std::string_view to std::string
    printS(static_cast<std::string>(sv)); // bad: expensive creation of std::string temporary
    printS("Hello, world"); // bad: expensive creation of std::string temporary

    return 0;
}

Additionally, we need to consider the cost of accessing the parameter inside the function. Because a 
std::string_view parameter is a normal object, the string being viewed can be accessed directly. 
Accessing a std::string& parameter requires an additional step to get to the referenced object 
before the string can be accessed.

Finally, if we want to pass in a substring of an existing string (of any type), it is 
comparatively cheap to create a std::string_view substring, which can then be cheaply 
passed to a std::string_view parameter. In comparison, passing a substring to a const 
std::string& is more expensive, as the substring must at some point be copied into the 
std::string that the reference parameter binds to.
*/

