/*
    Recall: When passing an argument by value, a copy of the argument 
is made into the function parameter. For fundamental types(which are cheap
cheap to copy), this is fine. But copying is typically expensive for class
types(such as std::string). We can avoid making an expensive copy by utilizing 
passing by(const reference(or by address) instead. 
We encounter a similar situation when returning by value: a copy of the return
value is passed back to the caller. If the return type of the function 
is a class type, this can be expensive 

*/

/*
    Return by reference 

In cases where we're passing a class type back to the caller, we may(or may
not) want to return by reference instead. 

Return by reference returns a reference that is bound to the object being 
returned, which avoids making a copy of the return value. To return by 
reference, we simply define the return value of the function to be a 
reference type.

std::string&  returnByReference(); 
// returns a reference to an existing std::string (cheap)
const std::string& returnByReferenceToConst(); 
// returns a const reference to an existing std::string (cheap)

#include <iostream>
#include <string>

const std::string& getProgramName() // returns a const reference
{
    static const std::string s_programName { "Calculator" }; // has static duration, destroyed at end of program

    return s_programName;
}

int main()
{
    std::cout << "This program is named " << getProgramName();

    return 0;
}

And this program still print This program is named Calculator

Because getProgramName() returns a const reference, when the line 
return s_programName is executed, getProgramName() will return a const
reference to s_programName(thus avoiding making a copy). That const 
reference can then be used by the caller to access the value of s_programName,
which is printed

*/

/*
    Important:
    The object being returned by reference must exist after the function returns

Using return by reference has one major caveat: the programmer must be sure
that the object being referenced outlives the function returning the 
reference. Otherwise, the reference being returned will be left dangling
(reference an object have been destroyed), and use of that reference 
will result in undefined behaviour

Above example having s_programName has static duration => valid 
But if not using static: 

#include <iostream>
#include <string>

const std::string& getProgramName()
{
    const std::string programName { "Calculator" }; // now a non-static local variable, destroyed when function ends

    return programName;
}

int main()
{
    std::cout << "This program is named " << getProgramName(); // undefined behavior

    return 0;
}

The result of this program is undefined!. When getProgramName() returns, 
a reference bound to local variable programName is returned but local 
variable programName is destroyed at the end of the function. That means 
the returned reference is now dangling, and use of programName in the 
main() function results in undefined behavior!!!

Note: Modern compiler will produce a warning or error when try to return
a local variable, but compilers sometimes have trouble detect more complicated
cases

    Warning

Objects returned by reference must live beyond the scope of the function 
returning the reference, or a dangling reference will result. Never return 
a (non-static) local variable or temporary by reference.

*/

/*
Lifetime extension doesn't work across function boundaries
#include <iostream>

const int& returnByConstReference()
{
    return 5; // returns const reference to temporary object
}

int main()
{
    const int& ref { returnByConstReference() };

    std::cout << ref; // undefined behavior

    return 0;
}

In the above program, 5 is rvalue, return type is const lvalue reference
=> it will result in the creation of temporary object holding value 5
and the function will return the reference to this temporary object 
But when go out of the scope => temporary object are also be destroyed
=> leaving the temporary reference in the scope of caller(main) dangling

By the time the temporary reference in the scope of the caller is bound 
to const reference varaible ref(in main()), it is too late to extend the
lifetime of the temporary object - as it has been destroyed when go out 
of the function scope, => ref is a dangling reference, and use of the 
value of ref will result in undefined behaviour!!!

#include <iostream>

const int& returnByConstReference(const int& ref)
{
    return ref;
}

int main()
{
    // case 1: direct binding
    const int& ref1 { 5 }; // extends lifetime
    std::cout << ref1 << '\n'; // okay

    // case 2: indirect binding
    const int& ref2 { returnByConstReference(5) }; // binds to dangling reference
    std::cout << ref2 << '\n'; // undefined behavior

    return 0;
}

In case 2, temporary object is created to hold value 5, which function 
parameter ref binds to. The function just returns this reference back to 
the caller, which then uses the reference to initalize ref2. Because this 
is not a direct binding to the temporary object(as the reference was bounced
through a function), lifetime extension doesn't apply 
This leaves ref2 dangling, and its subsequent use is undefined behaviour

    Warning

Reference lifetime extension does not work across function boundaries.
*/

/*
    Don’t return non-const static local variables by reference

#include <iostream>
#include <string>

const int& getNextId()
{
    static int s_x{ 0 }; // note: variable is non-const
    ++s_x; // generate the next id
    return s_x; // and return a reference to it
}

int main()
{
    const int& id1 { getNextId() }; // id1 is a reference
    const int& id2 { getNextId() }; // id2 is a reference

    std::cout << id1 << id2 << '\n';

    return 0;
}

// result : 22 
// what we want: 12

This happens because id1 and id2 are referencing the same object(the 
static variable s_x), so when anything (getNextId()) modifes that value,
all references are now accessing the modified value

#include <iostream>
#include <string>
#include <string_view>

std::string& getName()
{
    static std::string s_name{};
    std::cout << "Enter a name: ";
    std::cin >> s_name;
    return s_name;
}

void printFirstAlphabetical(const std::string& s1, const std::string& s2)
{
    if (s1 < s2)
        std::cout << s1 << " comes before " << s2 << '\n';
    else
        std::cout << s2 << " comes before " << s1 << '\n';
}

int main()
{
    printFirstAlphabetical(getName(), getName());

    return 0;
}

Here’s the result from one run of this program:

Enter a name: Dave
Enter a name: Stan
Stan comes before Stan


    Best practice: 
Avoid returning references to non-const local static variables 


*/

/*
    Assigning/initializing a normal variable with a returned reference makes a copy 
if a function returns a reference, and that reference is used to initialize
or assign to a non-reference variable, the return value will be copied 
(as if it had been returned by value)

#include <iostream>
#include <string>

const int& getNextId()
{
    static int s_x{ 0 };
    ++s_x;
    return s_x;
}

int main()
{
    const int id1 { getNextId() }; // id1 is a normal variable now and receives a copy of the value returned by reference from getNextId()
    const int id2 { getNextId() }; // id2 is a normal variable now and receives a copy of the value returned by reference from getNextId()

    std::cout << id1 << id2 << '\n';

    return 0;
}

function return const lvalue reference but id1, id2 is normal value 
=> the value of the returned reference is copied into the normal 
variable
and it will print 1, 2 

if a program returns a dangling reference, the reference is left dangling
before the copy is made, which will lead to undefined behaviour: 

#include <iostream>
#include <string>

const std::string& getProgramName() // will return a const reference
{
    const std::string programName{ "Calculator" };

    return programName;
}

int main()
{
    std::string name { getProgramName() }; // makes a copy of a dangling reference
    std::cout << "This program is named " << name << '\n'; // undefined behavior

    return 0;
}
*/

/*
    It's okay to return reference parameters by refernce 
#include <iostream>
#include <string>

// Takes two std::string objects, returns the one that comes first alphabetically
const std::string& firstAlphabetical(const std::string& a, const std::string& b)
{
	return (a < b) ? a : b; // We can use operator< on std::string to determine which comes first alphabetically
}

int main()
{
	std::string hello { "Hello" };
	std::string world { "World" };

	std::cout << firstAlphabetical(hello, world) << '\n';

	return 0;
}

This prints: 
    Hello 

*/

/*
    It's okay for an rvalue passed by const reference to be returned 
by const reference

#include <iostream>
#include <string>

std::string getHello()
{
    return "Hello"; // implicit conversion to std::string
}

int main()
{
    const std::string s{ getHello() };

    std::cout << s;

    return 0;
}

*/

/*
    The caller can modify the values through the reference

#include <iostream>

// takes two integers by non-const reference, and returns the greater by reference
int& max(int& x, int& y)
{
    return (x > y) ? x : y;
}

int main()
{
    int a{ 5 };
    int b{ 6 };

    max(a, b) = 7; // sets the greater of a or b to 7

    std::cout << a << b << '\n';

    return 0;
}
This will prints 57
*/

/*
    Return by address 

Return by address works almost identically to return by reference, 
expect a pointer to an object is returned instead of a reference to an
object. Return by address has the same primary caveat as return by
reference - the object being returned by address must outlive the scope 
of the function returning the address, otherwise the caller will receive
a dangling pointer

The major advantage of return by address over return by reference is that
we can have function return nullptr if there is no valid object to return
For example, let's say we have list of students that we want to search. 
If we find the student we are looking for in the list, we can return 
a pointer to the object representing the matching student. If we 
don't find any student matching, we can return nullptr, to indicate a
matching student object was not found 

The major disadvantage of returnby address is that the caller has to 
remember to do a nullptr check before dereferencing the return value,
otherwise a null pointer dereference may occur and undefined behaviour
will result. Because of this danger, return by reference should be 
prefered over return by address unless the ability to return "no object"
is needed

    Best practice

Prefer return by reference over return by address unless the ability 
to return "no object" (using nullptr) is important 



*/