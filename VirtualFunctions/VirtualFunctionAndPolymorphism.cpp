/*
    Virtual functions 

A virtual function is a special type of member function that when called
resolves to the most-derived version of the function for the actual type
of the object being referenced or pointer to 

A derived function is considered a match if it has the same signature
(name, parameter type, and whether it is const) and return type as the
the base version of the function. Such function are called overrides

To make a function virtual, we simply add the "virtual" keyword before
the function declaration 

#include <iostream>
#include <string_view>

class Base
{
public:
    virtual std::string_view getName() const { return "Base"; } // note addition of virtual keyword
};

class Derived: public Base
{
public:
    virtual std::string_view getName() const { return "Derived"; }
};

int main()
{
    Derived derived {};
    Base& rBase{ derived };
    std::cout << "rBase is a " << rBase.getName() << '\n';

    return 0;
}

It prints!!!

rBase is a Derived

Because rBase is a reference to the Base portion of a Derived object, 
when rBase.getName() is evaluated, it would normally resolve to 
Base::getName(), However, Base::getName() is virtual, which tells the 
program to go look and see if there are any more-derived versions of
versions of the function available for a Derived object. In this case, 
it will resolve to Derived::getName()

Another example:  

#include <iostream>
#include <string_view>

class A
{
public:
    virtual std::string_view getName() const { return "A"; }
};

class B: public A
{
public:
    virtual std::string_view getName() const { return "B"; }
};

class C: public B
{
public:
    virtual std::string_view getName() const { return "C"; }
};

class D: public C
{
public:
    virtual std::string_view getName() const { return "D"; }
};

int main()
{
    C c {};
    A& rBase{ c };
    std::cout << "rBase is a " << rBase.getName() << '\n';

    return 0;
}

It will prints 

rBase is a C

Let's look at how this works. First, we instantiate a C class object,
rBase is an A reference, which we set to reference the A portion of the 
C object. Finally, we call rBase.getName(), this function evaluates
to A::getName(), however, A::getName() is virtual, so the compiler 
will call the most-derived match between A and C, in this case, that is 
C::getName(), note that it will not call D::getName(), because our original
object was a C, not a D, so only functions between A and C are considerd


    Warning 

Note that virtual function resolution only works when a virtual member
function is called through a pointer or reference to class type object, this
works because the compiler can differentiate the type of pointer or 
reference from the type of object being pointed to or referenced. We can
see this in example above. 

Calling a virtual member function directly on an object(not through a
pointer or reference) will always invoke the member function belonging 
to the same type of that object. 

C c{};
std::cout << c.getName(); // will always call C::getName

A a { c }; // copies the A portion of c into a (don't do this)
std::cout << a.getName(); // will always call A::getName

    Key insight 

Virtual function resolution only works when a member function is called
through a pointer or refernce to a class type object 
*/

/*
    Tip

Some modern compilers may give an error about having virtual functions 
and an accessible non-virtual destructor. If this is the case, add a 
virtual destructor to the base class. In the above program, add this to 
the definition of Base:

virtual ~Base() = default;
*/

/*
    Polymorphism 

In programming, polymorphism refers to the ability of an entity to have 
mutiple forms(the term "polymorphism" literally means "many forms"). For
example, consider the following two function declarations:

int add(int, int);
double add(double, double);

The identifier add has two form:: add(int, int) and add(double, int)

Compile-time polymorphism refers to forms of polymorphism that are resolved
by the compiler. These include function overload resolution, as well as 
template resolution. 

Runtime polymorphism refers to forms of polymorphism that are resolved
at runtime. This includes virtual function resolution. 

*/

/*
    Rule 

If a function is virtual, all matching overrides in derived classes are
implicitly virtual 

Note: this doesn't work the other way around -- a virtual override in a 
derived class does not implicitly make the base class function virtual 

*/

/*
    Return types of virtual functions 

Under normal circumstances, the return type of a virtual function and its 
override must match. Consider the following example: 

class Base
{
public:
    virtual int getValue() const { return 5; }
};

class Derived: public Base
{
public:
    virtual double getValue() const { return 6.78; }
};


In this case, Derived::getValue() is not considered a matching override 
for Base::getValue() and compilation will fail. 

*/

/*
    Do not call virtual function from constructors or destructors 

Remember that when a Derived class is created, the Base portion is 
constructed first. If you were to call a virtual function from the Base
constructor, and Derived portion of the class hadn't even been created 
yet, it would be unable to call the Derived version of the function because
there's no Derived object for the Derived function to work on. In C++, 
it will call the Base version instead

A similar issue exists for destructors, if we call a virtual function in 
a Base class destructor, it will always resolve to the Base class version
of the function, because the Derived portion of the class will already have
been destroyed 

    Best practice 

Never call virtual function from constructors or destructors 
*/

/*
    The downside of virtual functions 

Since most of the time we'll want our functions to be virtual, why not jsut 
make all functions virtual? The answer is because it's inefficient -
resolving a virtual function call takes longer than resolving a regular
one. 

Furthermore, to make virtual functions works, the compiler has to allocate
an extra pointer for each object of a class that has virtual functions. This
adds a lot of overhead to objects that otherwise have a small size.

*/