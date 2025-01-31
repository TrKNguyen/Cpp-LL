/*
    Virtual destructors 

Although C++ provides a default destructors for your classes if we do 
not provide one ourself, it is sometimes the case that we will want to 
provide our own destructor(particularly if the class needs to deallocate
memory), we should always make our destructors virtual if we're dealing 
with inheritance. Consider the following example: 

#include <iostream>
class Base
{
public:
    ~Base() // note: not virtual
    {
        std::cout << "Calling ~Base()\n";
    }
};

class Derived: public Base
{
private:
    int* m_array {};

public:
    Derived(int length)
      : m_array{ new int[length] }
    {
    }

    ~Derived() // note: not virtual (your compiler may warn you about this)
    {
        std::cout << "Calling ~Derived()\n";
        delete[] m_array;
    }
};

int main()
{
    Derived* derived { new Derived(5) };
    Base* base { derived };

    delete base;

    return 0;
}

It only prints: 

Calling ~Base()

However, we really want the delete function to call Derived's destructor
otherwise m_array will not be deleted. We do this by making Base's 
destructor virtual: 

#include <iostream>
class Base
{
public:
    virtual ~Base() // note: virtual
    {
        std::cout << "Calling ~Base()\n";
    }
};

class Derived: public Base
{
private:
    int* m_array {};

public:
    Derived(int length)
      : m_array{ new int[length] }
    {
    }

    virtual ~Derived() // note: virtual
    {
        std::cout << "Calling ~Derived()\n";
        delete[] m_array;
    }
};

int main()
{
    Derived* derived { new Derived(5) };
    Base* base { derived };

    delete base;

    return 0;
}

Now it prints 

Calling ~Derived()
Calling ~Base() 

    Rule 

Whenever we are dealing with inheritance, we should make any explicit 
destructors virtual. 

As a normal virtual member functions, if a base class function is virtual,
all derived overrides will be considered virtual regradless of whether they 
are specifiers as such. It is not necessary to create an empty derived
class destructor
Just to mark it as virtual

Note that if we want our base class to have a vritual destructor that is 
otherwise empty, we can define our destructors this way: 
virtual ~Base() = default; // generate a virtual default destructor
*/

/*
    Virtual assignment

It is possible to make the assignment operator virtual. But it really 
opens up a bag full of worms and gets into some advanced topics.
Consequently, we are going to recommend to just leave assignments 
non-virtual for now, in the interest of simplicity 

*/

/*
    Ignoring virtualization 

Very rarely we may want to ignore the virtualization of a function. 

#include <string_view>
class Base
{
public:
    virtual ~Base() = default;
    virtual std::string_view getName() const { return "Base"; }
};

class Derived: public Base
{
public:
    virtual std::string_view getName() const { return "Derived"; }
};

There may be cases where we want a Base pointer to Derived object to 
call Base::getName() instead of Derived::getNmae(). To do so, simply use 
the scope resolution operator

#include <iostream>
int main()
{
    Derived derived {};
    const Base& base { derived };

    // Calls Base::getName() instead of the virtualized Derived::getName()
    std::cout << base.Base::getName() << '\n';

    return 0;
}
*/

/*
    Should we make all destructors virtual 

This is a common question asked by new programmers. As noted in the top
example, if the base class destructor isn't marked as virtual, then the
program is at risk of leaking memory if a programmer later deletes a base 
class pointer that it pointing to a derived object. One way to avoid this 
is to mark all your destructors as virtual. But should we? 

It's easy to say yes, so that way we can later use any class as a base
class -- but there's a performance penalty for ding so (a virtual pointer
added to every instance of our class), so we have to balance that cost, 
as well as our intent. 

We'd suggest the following: if a class isn't explicitly designed to be 
a base class, then it's generally better to have no virtual members and 
no virtual destructor. The class can still be used via composition. If 
a class is designed to be used as a base class and/or has any virtual 
function, then it should always have a virtual destructor 

if the decision is made to have a class not be inheritable, then the next
question is whether it's possible to enforce this. 

Conventional wisdom has suggest avoiding the non-virtual destructor memory 
leak situation as follows: "A base class destructor should be either public
and virtual, or protected and non-virtual". A base classs with a protected
destructor can't be deleted using a base class pointer, which prevents deleting
a derived class object through a base class pointer. 

Unfortunately, this also prevents any use of the base class destructor by
the public. That means: 

- We shouldn't dynamically allocate base class objects by we have no 
conventional way to delete them(there are non-conventional workarounds,
but yuck)
- We can't even satically allocate base class objects because the 
destructor isn't accessible when they go out of scope. 

In other word, using this, to make the derived class safe, we have to 
make the base class practically unusable by itself

Now that the final specifier has been introduced into the language, our
recommendations are as follow: 

- If you intend your class to be inherited from, make sure your destructor
is virtual and public 
- If you do not intend your class to be inherited from, mark your class
as final. This will prevent other classes from inheriting from it in 
the first place, without imposing any other use restrictions on the 
class it selff. 

*/

