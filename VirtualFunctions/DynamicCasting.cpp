/*
    Dynamic casting 

The need for dynamic_cast: when deadling with polymorphism, we'll often
encounter cases where we have a pointer to a base class, but you want to
access some information that exists only in a derived class. Consider
the following program: 

#include <iostream>
#include <string>
#include <string_view>

class Base
{
protected:
	int m_value{};

public:
	Base(int value)
		: m_value{value}
	{
	}

	virtual ~Base() = default;
};

class Derived : public Base
{
protected:
	std::string m_name{};

public:
	Derived(int value, std::string_view name)
		: Base{value}, m_name{name}
	{
	}

	const std::string& getName() const { return m_name; }
};

Base* getObject(bool returnDerived)
{
	if (returnDerived)
		return new Derived{1, "Apple"};
	else
		return new Base{2};
}

int main()
{
	Base* b{ getObject(true) };

	// how do we print the Derived object's name here, having only a Base pointer?

	delete b;

	return 0;
}

In this program, function getObject() always returns a Base pointer, but 
that pointer may be pointing to either a Base or a Derived object. In the 
case where the Base pointer is actually pointing to a Derived object, how 
would we call Derived::getName()?

One way would be to add a virtual function to Base called getName() (so 
we could call it with a Base pointer/reference, and have it dynamically 
resolve to Derived::getName()). But what would this function return if 
you called it with a Base pointer/reference that was actually pointing 
to a Base object? There isn’t really any value that makes sense. 
Furthermore, we would be polluting our Base class with things that really 
should only be the concern of the Derived class.

We know that C++ will implicitly let you convert a Derived pointer into 
a Base pointer (in fact, getObject() does just that). This process is 
sometimes called upcasting. However, what if there was a way to convert 
a Base pointer back into a Derived pointer? Then we could call 
Derived::getName() directly using that pointer, and not have to worry 
about virtual function resolution at all.

*/

/*

    dynamic_cast 

C++ provides a casting operator named dynamic_cast that can be used for 
just this purpose. Although dynamic cases have a few different capabilities
by far the most coomon use for dynamic casting is for converting base-class
pointers into derived-class pointers. This process is called downcasting 

Using dynamic_cast works just like static_cast. Here's our example main()
from above, using a dynamic_cast to convert our Base pointer back 
into a Derived pointer: 

int main()
{
	Base* b{ getObject(true) };

	Derived* d{ dynamic_cast<Derived*>(b) }; // use dynamic cast to convert Base pointer into Derived pointer

	std::cout << "The name of the Derived is: " << d->getName() << '\n';

	delete b;

	return 0;
}

This prints: 

The name of the Derived is: Apple 

*/

/*

    dynamic_cast failure 

The above example works because b is actually pointing to a Derived 
object, so converting b into a Derived pointer is successful 

However, we've made quite a dangerous assumption: that bi poiting to a 
Derived object. What if b wasn't pointing to a Derived object? This is
easily tested by changing the argument to getObject() from true to false
In that case, getObject() will return a Base pointer to a Base object
When we try to dynamic_cast that to a Derived, it will fail, because the 
conversion can't be made. 

if a dynamic_cast fails, the result of the coversion will be a null pointer

Because we haven't checked for a null pointer result, we access d->getName() 
which will try to dereference a null pointer, leading to undefined 
behaviour (probably a crash). 

In order to make this program safe, we need to ensure the result of 
dynamic_cast actually succeeded: 

int main()
{
	Base* b{ getObject(true) };

	Derived* d{ dynamic_cast<Derived*>(b) }; // use dynamic cast to convert Base pointer into Derived pointer

	if (d) // make sure d is non-null
		std::cout << "The name of the Derived is: " << d->getName() << '\n';

	delete b;

	return 0;
}

Note that because dynamic_cast does some consistency checking at runtime
(to ensure the conversion can be made), use of dynamic_cast does incur 
a performance penalty 

Also note that there are several cases where downcasting using dynamic_cast
will not work: 

1. With proteced or private inheritance 
2. For classes that do not declare or inherit any virtual functions(
and thus don't have a virtua; table). 
3. In certain cases involving virtual base classes

    Rule
    
Always ensure your dynamic case sacutally succeeded by checking for 
a null pointer result. 

*/

/*
    Downcasting with static_cast 

It turns out that downcasting can also be done with static_cast. The main
difference is that static_cast does no runtim type checking to ensure that 
what we're doing makes sense. This makes using static_cast faster, but more
dangerous. If we cast a Base* to a Derived*, it will succeed even if the 
Base pointer isn't pointing to a Derived object. This will result in 
undefined behaviour when we try to access the resulting Derived pointer
(that is actually pointing to a Base object). 

If we're absolutely sure that the pointer we're downcasting will succeed, 
then using static_cast is acceptable. One way to ensure that we know 
what type of object we're pointing to is to use a virtual function 

#include <iostream>
#include <string>
#include <string_view>

// Class identifier
enum class ClassID
{
	base,
	derived
	// Others can be added here later
};

class Base
{
protected:
	int m_value{};

public:
	Base(int value)
		: m_value{value}
	{
	}

	virtual ~Base() = default;
	virtual ClassID getClassID() const { return ClassID::base; }
};

class Derived : public Base
{
protected:
	std::string m_name{};

public:
	Derived(int value, std::string_view name)
		: Base{value}, m_name{name}
	{
	}

	const std::string& getName() const { return m_name; }
	ClassID getClassID() const override { return ClassID::derived; }

};

Base* getObject(bool bReturnDerived)
{
	if (bReturnDerived)
		return new Derived{1, "Apple"};
	else
		return new Base{2};
}

int main()
{
	Base* b{ getObject(true) };

	if (b->getClassID() == ClassID::derived)
	{
		// We already proved b is pointing to a Derived object, so this should always succeed
		Derived* d{ static_cast<Derived*>(b) };
		std::cout << "The name of the Derived is: " << d->getName() << '\n';
	}

	delete b;

	return 0;
}

But if we're going to go through all of the trouble to implement this 
(and pay the cost of calling a virtual function and processing the result), 
we might as well just use dynamic_cast.

Also consider what would happen if our object were actually some class
that is derived from Derived (let’s call it D2). The above check 
b->getClassID() == ClassID::derived will fail because getClassId() would 
return ClassID::D2, which is not equal to ClassID::derived. 
Dynamic casting D2 to Derived would succeed though, since a D2 is a Derived!

*/

/*

    dynamic_cast and references 

Although all of the above examples show dynamic casting of pointer(which
is more common), dynamic_cast can also be used with references. This works 
analogously to know dynamic_cast works with pointers 

#include <iostream>
#include <string>
#include <string_view>

class Base
{
protected:
	int m_value;

public:
	Base(int value)
		: m_value{value}
	{
	}

	virtual ~Base() = default;
};

class Derived : public Base
{
protected:
	std::string m_name;

public:
	Derived(int value, std::string_view name)
		: Base{value}, m_name{name}
	{
	}

	const std::string& getName() const { return m_name; }
};

int main()
{
	Derived apple{1, "Apple"}; // create an apple
	Base& b{ apple }; // set base reference to object
	Derived& d{ dynamic_cast<Derived&>(b) }; // dynamic cast using a reference instead of a pointer

	std::cout << "The name of the Derived is: " << d.getName() << '\n'; // we can access Derived::getName through d

	return 0;
}

Because C++ does not have a "null reference" dynamic_cast can't return 
a null reference upon failure. Instead if the dynamic_cast of a reference
fails, an exception of type std::bad_cast is thrown. 

*/

/*

    dynamic_cast  VS  static_cast 

New programmers are sometimes confused about when to use static_cast vs
dynamic_cast. The answer is quite simple: use static_cast unless you're
downcasting, in which case dynamic_cast is usually a better choice. However, 
we should also consider avoiding casting altogether and just use virtual
functions. 


*/

/*

    Downcasting vs virtual functions 

There are some developers who believe dynamic_cast is evil and indicative
of a bad class design. Instead, these programmers say we should use virtual
functions. 

In general, using a virtual function should be preferred over downcasting. 
However, there are times when downcasting is the better choice: 

- When we can not modify the base class to add a virtual function(e.g
because base class is part of the standard library)
- When we need access to something that is derived-class specific (e.g
an access function that only exists in the derived class)
- When adding a virtual function to out base class doesn't make sense
(e.g there is no appropriate value for the base class to return), Using 
a pure virtual function may be an option here if we don't need to 
instantiate the base class. 

*/

/*

    A warning about dynamic_cast and RTTI

Run-time type information(RTTI) is a feature of C++ that exposes information
about an object's data types at runtime. This capability is leveraged 
by dynamic_cast. Because RTTI has a pretty significant space performance
cost, some compilers allow us to turn RTTI off as an optimization. 
Needless to say, if we do this, dynamic_cast won't function correctly
*/