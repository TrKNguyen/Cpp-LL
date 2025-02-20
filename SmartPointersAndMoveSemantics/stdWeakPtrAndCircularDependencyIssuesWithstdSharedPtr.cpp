/*
We saw how std::shared_ptr allowed us to have multiple smart pointers
co-owning the same resource, However, in certain cases, this can become
problematic. Consider the following case, where the shared pointers in
two separate objects each points at the other object 

#include <iostream>
#include <memory> // for std::shared_ptr
#include <string>

class Person
{
	std::string m_name;
	std::shared_ptr<Person> m_partner; // initially created empty

public:

	Person(const std::string &name): m_name(name)
	{
		std::cout << m_name << " created\n";
	}
	~Person()
	{
		std::cout << m_name << " destroyed\n";
	}

	friend bool partnerUp(std::shared_ptr<Person> &p1, std::shared_ptr<Person> &p2)
	{
		if (!p1 || !p2)
			return false;

		p1->m_partner = p2;
		p2->m_partner = p1;

		std::cout << p1->m_name << " is now partnered with " << p2->m_name << '\n';

		return true;
	}
};

int main()
{
	auto lucy { std::make_shared<Person>("Lucy") }; // create a Person named "Lucy"
	auto ricky { std::make_shared<Person>("Ricky") }; // create a Person named "Ricky"

	partnerUp(lucy, ricky); // Make "Lucy" point to "Ricky" and vice-versa

	return 0;
}

In above example, we dynamically allocate two Persons, then we partner
them up, shared pointers are meant to be shared, so it's fine that both
lucky shared pointer and Risk's m_partner shared pointer both point
at Lucy and vice-versa

However, this program doesn’t execute as expected:

Lucy created
Ricky created
Lucy is now partnered with Ricky

And that's it. No deallocation took place Oh no, what's happpend?

After partnerUp() is calledl, there are two shared pointers pointing to
Ricky and Lucy 

At the end of main(), the ricky shared ointer goes out of scope first. 

When that happens, ricly checks if there are nay other shared pointers 
that co-own the Person "Ricky". There are. Because of this, it doesn't 
deallocate Ricky(if it did, then Lucy partner would end up as dangling
pointer). At this point, we now have one shared pointer to Ricky 
and two sahred pointers to Lucy 

Next Lucy shared pointer goes out of the scope , and the same thing happens, 
The shared poiunter lucy checks if there are any other shared pointers 
co-owning the Person "Lucy". There are (Ricky's m_partner), so Lucy
isn't deallocated. At this point, there is one shared pointer to Lucy 
and one shared pointer to Ricky 

The the program ends -- and neither Lucy and Ricky have been deallocated
Lucy end up keeping Ricky from being destroyed, Ricky end up keeping Lucy 
from being destroyed

It turns out that this can happen any time shared pointer form a cicular
reference 

*/

/*
    Circular references 

A Circular referenes is a series of reference where each object references
the next, and the last object references back to the first, causing a referential
loop, The reference do not need to be actual C++ refernce, they can be pointers
unique IDs, or any other means of identifying specific objects 

    A reductive cases 

It turns our, this cyclical reference issue can even happen with a single 
shared pointer

#include <iostream>
#include <memory> // for std::shared_ptr

class Resource
{
public:
	std::shared_ptr<Resource> m_ptr {}; // initially created empty

	Resource() { std::cout << "Resource acquired\n"; }
	~Resource() { std::cout << "Resource destroyed\n"; }
};

int main()
{
	auto ptr1 { std::make_shared<Resource>() };

	ptr1->m_ptr = ptr1; // m_ptr is now sharing the Resource that contains it

	return 0;
}
*/

/*
    So what's is std::weak_ptr for anyway? 

std::weak_ptr was designed to solve the cyclical ownership problem described
above. A std::weak_ptr is an observerm it can observe and access the same
object as a std::shared_ptr but it is not considered an owner 

When a std::shared pointer goes out of scope, it only considers whether 
other std::shared_ptr are co-owning the object. std::weak_ptr does not 
count!

Let's solve our Person-al issue using a std::weak_ptr 

#include <iostream>
#include <memory> // for std::shared_ptr and std::weak_ptr
#include <string>

class Person
{
	std::string m_name;
	std::weak_ptr<Person> m_partner; // note: This is now a std::weak_ptr

public:

	Person(const std::string &name): m_name(name)
	{
		std::cout << m_name << " created\n";
	}
	~Person()
	{
		std::cout << m_name << " destroyed\n";
	}

	friend bool partnerUp(std::shared_ptr<Person> &p1, std::shared_ptr<Person> &p2)
	{
		if (!p1 || !p2)
			return false;

		p1->m_partner = p2;
		p2->m_partner = p1;

		std::cout << p1->m_name << " is now partnered with " << p2->m_name << '\n';

		return true;
	}
};

int main()
{
	auto lucy { std::make_shared<Person>("Lucy") };
	auto ricky { std::make_shared<Person>("Ricky") };

	partnerUp(lucy, ricky);

	return 0;
}

This code behaves properly:

Lucy created
Ricky created
Lucy is now partnered with Ricky
Ricky destroyed
Lucy destroyed

*/

/*
    Using std::weak_ptr 

One downside of weak pointer is that weak pointer are not directly usable
(they have no operator->)
To use a weak pointer, we must first convert it into a share pointer. Then
we can use the shared pointer. To convert a weak pointer to shared pointer
we can use the lock() member function 

#include <iostream>
#include <memory> // for std::shared_ptr and std::weak_ptr
#include <string>

class Person
{
	std::string m_name;
	std::weak_ptr<Person> m_partner; // note: This is now a std::weak_ptr

public:

	Person(const std::string &name) : m_name(name)
	{
		std::cout << m_name << " created\n";
	}
	~Person()
	{
		std::cout << m_name << " destroyed\n";
	}

	friend bool partnerUp(std::shared_ptr<Person> &p1, std::shared_ptr<Person> &p2)
	{
		if (!p1 || !p2)
			return false;

		p1->m_partner = p2;
		p2->m_partner = p1;

		std::cout << p1->m_name << " is now partnered with " << p2->m_name << '\n';

		return true;
	}

	std::shared_ptr<Person> getPartner() const { return m_partner.lock(); } // use lock() to convert weak_ptr to shared_ptr
	const std::string& getName() const { return m_name; }
};

int main()
{
	auto lucy { std::make_shared<Person>("Lucy") };
	auto ricky { std::make_shared<Person>("Ricky") };

	partnerUp(lucy, ricky);

	auto partner = ricky->getPartner(); // get shared_ptr to Ricky's partner
	std::cout << ricky->getName() << "'s partner is: " << partner->getName() << '\n';

	return 0;
}

Lucy created
Ricky created
Lucy is now partnered with Ricky
Ricky's partner is: Lucy
Ricky destroyed
Lucy destroyed

We don’t have to worry about circular dependencies with std::shared_ptr variable 
“partner” since it’s just a local variable inside the function. It will eventually 
go out of scope at the end of the function and the reference count will be 
decremented by 1.
*/

/*
    Avoiding danling pointers with weak pointer

Consider the case where a normal dumb pointer is holding the address of 
some object, and then that object is destroyed. Such a pointer is dangling
and dereferencing the pointer holding a non-null address is dangling or not. 
This is a large part of the reason dumb pointer are dangerous 

Because weak pointer wont keep an owned resource alive, it's similarly
possible for a weak pointer to be left with pointing to a resource that has
been deallocated by a shared pointer, however, weak pointer has a neat
trick up its sleeve, because it has access to the reference count for an 
object, it can determine if it is pointing to a valid object or not!
If the reference count is non-zero, the resource is still valid. If the
reference count is zero, then the resource has been destroyed. 

The easiest way to test whether a week pointer is valid is to use the 
expired() memeber function which returns true if the weak pointer is 
pointing to an invalid object, and false otherwise

// h/t to reader Waldo for an early version of this example
#include <iostream>
#include <memory>

class Resource
{
public:
	Resource() { std::cerr << "Resource acquired\n"; }
	~Resource() { std::cerr << "Resource destroyed\n"; }
};

// Returns a std::weak_ptr to an invalid object
std::weak_ptr<Resource> getWeakPtr()
{
	auto ptr{ std::make_shared<Resource>() };
	return std::weak_ptr<Resource>{ ptr };
} // ptr goes out of scope, Resource destroyed

// Returns a dumb pointer to an invalid object
Resource* getDumbPtr()
{
	auto ptr{ std::make_unique<Resource>() };
	return ptr.get();
} // ptr goes out of scope, Resource destroyed

int main()
{
	auto dumb{ getDumbPtr() };
	std::cout << "Our dumb ptr is: " << ((dumb == nullptr) ? "nullptr\n" : "non-null\n");

	auto weak{ getWeakPtr() };
	std::cout << "Our weak ptr is: " << ((weak.expired()) ? "expired\n" : "valid\n");

	return 0;
}

weak pointer can be used when you need multiple smart pointer that can
co-own a resource, the resource will be deallocated when the last shared
pointer goes out of the scope, weak pointer can be used when we want to 
a smart pointer that can see that used a shared resource, but does not 
participate in the ownership of that resource 

*/