/*
    The copy constructor 

A copy constructor is a constructor that is used to initailize an object
with an existing object of the same type. After the copy constructor executes
the newly created object should be a copy of the obhect passed in as the
initializer.  

*/

/*
    An implicit copy constructor

If we dont explicitly write a separate copy constructor for our classes,
C++ will create a public implicit copy constructor for us. 

#include <iostream>

class Fraction
{
private:
    int m_numerator{ 0 };
    int m_denominator{ 1 };

public:
    // Default constructor
    Fraction(int numerator=0, int denominator=1)
        : m_numerator{numerator}, m_denominator{denominator}
    {
    }

    void print() const
    {
        std::cout << "Fraction(" << m_numerator << ", " << m_denominator << ")\n";
    }
};

int main()
{
    Fraction f { 5, 3 };  // Calls Fraction(int, int) constructor
    Fraction fCopy { f }; // What constructor is used here?

    f.print();
    fCopy.print();

    return 0;
}

In this code, the reason dont have constructor for the Fraction type, but
the code still works. This Fraction fCopy{f} actually invokes the implicit
copy constructor to intialize fCopy with f. 

By default, the implicit copy constructor will do memberwise initialization, 
This means each member will be initialized using the corresponding memeber
of the class passes in as the initializer. In the example above, 
fCopy.m_numerator is initialized using f.m_numerator
fCopy.m_denominator is initialized using f.m_denominator

*/

/*
    Defining your own copy constructor

We can explicitly define our own copy constructor

Example 

Fraction(const Fraction& other) {
    m_numerator = other.m_numerator; 
    m_denomerator = other.m_denomerator; 
    std::cout << "Copy constructor called" << "\n";
}

The copy constructor we defined here is functionally equivalent to the one
we'd get by default, except we're added an output statement to prove the 
copy constructor is actually being called

    Best practice 

Copy constructors should have no side effects beyond copying.
*/

/*
    Prefer the implicit copy constructor 

Unlike the implicit default constructor, which does nothing (and thus is
rarely what we want), the memberwise initialization performed by the
implicit copy constructor is usually exactly what we want. Therefore, in 
most cases, using the implicit copy constructor is perfectly fine.

    Best practice 
Prefer the implicit copy constructor, unless you have a specifc reason to
create your own. 

*/

/*
    The copy constructor's parameter must be a const lvalue reference

It is a requirement that the parameter of a copy constructor be an lvalue 
reference or const lvalue reference. Because the copy constructor should 
not be modifying the parameter, using a const lvalue reference is preferred.
    
    Best practice 
If you write your own copy constructor, the parameter should be a const
lvalue reference 

*/

/*
    Pass by value and the copy constructor 

When an object is passed by value, the argument is copied into the parameter. 
Wehn the argument and parameter are the same class type, the copy is made by implicitly 
invoking the copy constructor 

#include <iostream>

class Fraction
{
private:
    int m_numerator{ 0 };
    int m_denominator{ 1 };

public:
    // Default constructor
    Fraction(int numerator = 0, int denominator = 1)
        : m_numerator{ numerator }, m_denominator{ denominator }
    {
    }

    // Copy constructor
    Fraction(const Fraction& fraction)
        : m_numerator{ fraction.m_numerator }
        , m_denominator{ fraction.m_denominator }
    {
        std::cout << "Copy constructor called\n";
    }

    void print() const
    {
        std::cout << "Fraction(" << m_numerator << ", " << m_denominator << ")\n";
    }
};

void printFraction(Fraction f) // f is pass by value
{
    f.print();
}

int main()
{
    Fraction f{ 5, 3 };

    printFraction(f); // f is copied into the function parameter using copy constructor

    return 0;
}

    (Very interesting findout)

Result: 
Copy constructor called
Fraction(5, 3)
*/

/*
    Return by value and the copy constructor 

We noted that return by value creates a temporary object(holding a copy
of the return value) that is passed back to the caller. When the return type
and the return alue are the same class type, the temporary object is initialized 
by implicitly invoking the copy constructor
#include <iostream>

class Fraction
{
private:
    int m_numerator{ 0 };
    int m_denominator{ 1 };

public:
    // Default constructor
    Fraction(int numerator = 0, int denominator = 1)
        : m_numerator{ numerator }, m_denominator{ denominator }
    {
    }

    // Copy constructor
    Fraction(const Fraction& fraction)
        : m_numerator{ fraction.m_numerator }
        , m_denominator{ fraction.m_denominator }
    {
        std::cout << "Copy constructor called\n";
    }

    void print() const
    {
        std::cout << "Fraction(" << m_numerator << ", " << m_denominator << ")\n";
    }
};

void printFraction(Fraction f) // f is pass by value
{
    f.print();
}

Fraction generateFraction(int n, int d)
{
    Fraction f{ n, d };
    return f;
}

int main()
{
    Fraction f2 { generateFraction(1, 2) }; // Fraction is returned using copy constructor

    printFraction(f2); // f2 is copied into the function parameter using copy constructor

    return 0;
}

When generateFraction returns a Fraction back to main, a temporary Fraction 
object is created and intialized using the copy constructor. Because this 
temporary is used to initialize Fraction f2, this invokes the copy constructor
again to copy the temporary into f2. 

And when f2 is passed to printFraction(), the copy constructor is called 
a third time.

Therefore, it will print 

Copy constructor called
Copy constructor called
Copy constructor called
Fraction(1, 2)
*/


/*
    Copy elisionnnnnnnnn : very imporanttttt 

If you compile and execute the above example, you may find that only two 
calls to the copy constructor occur. This is a compiler optimization known
as copy elisionnnnnnnnnnnnnnn. 
*/

/*
    Using = default to generate a default copy constructor 

If a class has no copy constructor, the compiler will implicitly generate
one for us. if we prefer, we can explicitly request the complier create
a default copy constructor for us using the = default syntax: 

#include <iostream>

class Fraction
{
private:
    int m_numerator{ 0 };
    int m_denominator{ 1 };

public:
    // Default constructor
    Fraction(int numerator=0, int denominator=1)
        : m_numerator{numerator}, m_denominator{denominator}
    {
    }

    // Explicitly request default copy constructor
    Fraction(const Fraction& fraction) = default;

    void print() const
    {
        std::cout << "Fraction(" << m_numerator << ", " << m_denominator << ")\n";
    }
};

int main()
{
    Fraction f { 5, 3 };
    Fraction fCopy { f };

    f.print();
    fCopy.print();

    return 0;
}
*/

/*
    Using =delete to prevent copies

Occasionally we run into cases where we do not want objects of a certain
class to be copyable. We can prevent this by making the copy constructor
function as deleted, using the =delete syntax 

#include <iostream>

class Fraction
{
private:
    int m_numerator{ 0 };
    int m_denominator{ 1 };

public:
    // Default constructor
    Fraction(int numerator=0, int denominator=1)
        : m_numerator{numerator}, m_denominator{denominator}
    {
    }

    // Delete the copy constructor so no copies can be made
    Fraction(const Fraction& fraction) = delete;

    void print() const
    {
        std::cout << "Fraction(" << m_numerator << ", " << m_denominator << ")\n";
    }
};

int main()
{
    Fraction f { 5, 3 };
    Fraction fCopy { f }; // compile error: copy constructor has been deleted

    return 0;
}

When the compiler goes to find a constructor to intialize fCopy with f,
it will see that the copy constructor has been deleted. This will cause
it to emit a compiler error!!!

*/
