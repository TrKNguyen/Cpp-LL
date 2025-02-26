/*
    Rvalue references

C++11 adds a new type of reference called an r-value reference. 
An rvalue reference is a reference that is designed to be intialized 
with  rvalue(only). 
While an l-value reference is created using a single ampersand, 
an r-value reference is created using a double ampersand: 

int x{ 5 };
int& lref{ x }; // l-value reference initialized with l-value x
int&& rref{ 5 }; // r-value reference initialized with r-value 5

Rvalue references cannot be intialized with lvalues

| R-value reference       | Can be initialized with | Can modify |
|-------------------------|-------------------------|------------|
| Modifiable l-values     | No                      | No         |
| Non-modifiable l-values | No                      | No         |
| R-values                | Yes                     | Yes        |

| R-value reference to const | Can be initialized with | Can modify |
|----------------------------|-------------------------|------------|
| Modifiable l-values        | No                      | No         |
| Non-modifiable l-values    | No                      | No         |
| R-values                   | Yes                     | No         |

*/


/*
R-value references have two properties that are useful. First, r-value 
references extend the lifespan of the object they are intialized with 
to the lifespan of the r-value refernce (lvalue references to const 
objects can do this too). Second, non-const allow you to modfy the r-value. 

#include <iostream>

class Fraction
{
private:
	int m_numerator { 0 };
	int m_denominator { 1 };

public:
	Fraction(int numerator = 0, int denominator = 1) :
		m_numerator{ numerator }, m_denominator{ denominator }
	{
	}

	friend std::ostream& operator<<(std::ostream& out, const Fraction& f1)
	{
		out << f1.m_numerator << '/' << f1.m_denominator;
		return out;
	}
};

int main()
{
	auto&& rref{ Fraction{ 3, 5 } }; // r-value reference to temporary Fraction

	// f1 of operator<< binds to the temporary, no copies are created.
	std::cout << rref << '\n';

	return 0;
} // rref (and the temporary Fraction) goes out of scope here

This program prints: 

3/5 

As an anonymous object, Fraction(3, 5) would normally go out of scope 
at the end of the expression in which it is defined. However, since we're
initializing an r-value reference with it, its duration is extended until
the end of the block. We can then use that rvalue reference to print the 
Fraction's value. 

#include <iostream>

int main()
{
    int&& rref{ 5 }; // because we're initializing an r-value reference with a literal, a temporary with value 5 is created here
    rref = 10;
    std::cout << rref << '\n';

    return 0;
}

While it may seem weird to initialize an r-value reference with literal value
and then be able to change that value, when initializing an r-value 
reference with a literal, a temporary object is constructed from the literal 
so that the refernce is referencing a temporary object, not a literal value

R-value references are not very often used in either of the manners illustrated 
above.

*/

/*
    Rvalue references as function parameters 

Rvalue references are more often used as function parameters. This is 
most useful for function overloads when we want to have different
behaviour for lvalue and rvalue arguments 

#include <iostream>

void fun(const int& lref) // l-value arguments will select this function
{
	std::cout << "l-value reference to const: " << lref << '\n';
}

void fun(int&& rref) // r-value arguments will select this function
{
	std::cout << "r-value reference: " << rref << '\n';
}

int main()
{
	int x{ 5 };
	fun(x); // l-value argument calls l-value version of function
	fun(5); // r-value argument calls r-value version of function

	return 0;
}

This program will print: 

l-value reference to const: 5 
r-value reference: 5 

As we can see, when passed lvalue => it choose version with lvalue references
And then we when passed rvalue => it choose version with rvalue reference 

Why would we wanna do that? Needless to say, It's an important part of 
move semantics 
*/

/*
    Rvalue refernces variables are lvalues 

int&& ref{ 5 };
fun(ref);

Which version of fun would you expect the above to call: 

fun(const int &) or fun(int &&) ??

The answer is very surprised. This calls fun(const int&)

Although variable ref have type int&&, when used in an expression
it is an lvalue(as are all named variables). 
    
    Key insight:
    The type of an object and its value category are independent. 

We already know that literal "5" is an rvalue of type "int", and 
"int x" is an lvalue of type "int". 
Similarly, "int&& ref" is an lvalue of type "int&&"!!!!!

So not only does fun(ref) call fun(const int&), it does not even 
match fun(int &&), as rvalue reference can't bind to lvalues

*/

/*
    Returning an r-value reference 

You should almost never return an r-value references, for the same reason
you should almost never return an l-value reference. In most cases,
you'll end up returning a hanging references when the referenced object 
goes out of scope at the end of the function 


*/