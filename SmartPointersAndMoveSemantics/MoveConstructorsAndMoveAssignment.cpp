/*
    Move constructors and move assignment 

C++11 defines two new functions in service of move semantics: 
a move constructor
a move assignment operator 

Whereas the goal of the copy constructor and copy assignment is to 
make a copy of one object to another, the goal of the move constructor
and move assignment is to move ownership of the resources from one object 
to another(which is typically much less expensive than making a copy)

Defining a move constructor and move assignment work analogously to 
their copy counterparts. However, whereas the copy flavors of these 
function take a const lvalue reference parameter(which will bind to just
abour anything), the move flavors of these functions use non-const rvalue
reference(which only bind to rvalues)

#include <iostream>

template<typename T>
class Auto_ptr4
{
	T* m_ptr {};
public:
	Auto_ptr4(T* ptr = nullptr)
		: m_ptr { ptr }
	{
	}

	~Auto_ptr4()
	{
		delete m_ptr;
	}

	// Copy constructor
	// Do deep copy of a.m_ptr to m_ptr
	Auto_ptr4(const Auto_ptr4& a)
	{
		m_ptr = new T;
		*m_ptr = *a.m_ptr;
	}

	// Move constructor
	// Transfer ownership of a.m_ptr to m_ptr
	Auto_ptr4(Auto_ptr4&& a) noexcept
		: m_ptr { a.m_ptr }
	{
		a.m_ptr = nullptr; // we'll talk more about this line below
	}

	// Copy assignment
	// Do deep copy of a.m_ptr to m_ptr
	Auto_ptr4& operator=(const Auto_ptr4& a)
	{
		// Self-assignment detection
		if (&a == this)
			return *this;

		// Release any resource we're holding
		delete m_ptr;

		// Copy the resource
		m_ptr = new T;
		*m_ptr = *a.m_ptr;

		return *this;
	}

	// Move assignment
	// Transfer ownership of a.m_ptr to m_ptr
	Auto_ptr4& operator=(Auto_ptr4&& a) noexcept
	{
		// Self-assignment detection
		if (&a == this)
			return *this;

		// Release any resource we're holding
		delete m_ptr;

		// Transfer ownership of a.m_ptr to m_ptr
		m_ptr = a.m_ptr;
		a.m_ptr = nullptr; // we'll talk more about this line below

		return *this;
	}

	T& operator*() const { return *m_ptr; }
	T* operator->() const { return m_ptr; }
	bool isNull() const { return m_ptr == nullptr; }
};

class Resource
{
public:
	Resource() { std::cout << "Resource acquired\n"; }
	~Resource() { std::cout << "Resource destroyed\n"; }
};

Auto_ptr4<Resource> generateResource()
{
	Auto_ptr4<Resource> res{new Resource};
	return res; // this return value will invoke the move constructor
}

int main()
{
	Auto_ptr4<Resource> mainres;
	mainres = generateResource(); // this assignment will invoke the move assignment

	return 0;
}

The move assignment and move constructor are simple. Instead of deep 
copying the source object(a) into the destination object(the implicit object), 
we simply move(steal) the source object's resources. This involves shallow
copying the source pointer into the implicit object, then setting the 
source pointer to null. 

When run, this program prints: 

Resource acquired 
Resource destroyed

That's much better 

*/

/*
    When are the move constructor and move assignment called? 

The move constructor and move assignment are called when those functions
have been defined, and the argument for construction or assignment is an
rvalue. Most typically, this rvalue will be a literal or temporary value

The copy constructor and copy assignment are used otherwise(when the 
argument is an lvalue, or when the argument is an rvalue and move 
constructor or move assignement aren't defined)

*/

/*
    Implicit move constructor and move assignment operator 

The compiler will create an implicit move constructor and move assignment
operator if all of the following are true: 

- There are no user-declared copy constructors or copy assignment operators
- There are no user-declared move constructors or move assignement operators
- There is no user-declared destructor 

These functions do a memberwise move, which behaves as follows: 

- If memeber has a move constructor or move assignment, it will be invoked 
- Otherwise, the member will be copied

Notably, this means that pointers will be copied, not moved!

    Warning

The implicit move constructor and move assignment will copy pointers, not
move them. If you want to move a pointer member, you will need to define 
the move constructor and move assignment yourself.
*/

/*
    The key insight behind move semantics 

You now have enough context to understand the key insight behind move 
semantics

If we construct an object or do an assignment where the argument is an 
l-value, the only thing we can reasonably do is copy the lvalue. 

We cant assume it's safe to alter the lvalue, because it may be used 
again later in the program. If we have an expression "a = b" (where
b is an lvalue), we wouldn't reasonably expect b to be changed in 
any way. 

However, if we construct an object or do an assignment where the argument
is an rvalue, then we know that rvalue is just a temporary object of soem
kind. Instead of copying it(which can be expensive), we can simply transfer
its resources(which is cheap) to the object we're constructing or assigning 
This is safe to do because temporary will be destroyed at the end of 
the expression anyway, so we know it will never be used again!!!!

C++11, through rvalue reference, give us the ability to provide different
behaviours when the argument is an rvalue vs an lvalue, enabling us to 
make smarter and more efficient decisions about how our objects should 
behave. 

    Key insight 

Move semantics is an optimization opportunity

*/

/*
    Move functions should always leave both objects in a valid state

In the above example, both the move constructor and move assignment functions
set a.m_ptr to nullptr(still a valid state). This may seem extraneous 
After all, if a is a temporary rvalue, why bother doing "cleanup" if 
parameter a is going to be destroyed anyway? 

The answer is simple when a goes our of the scope, the destructor for 
a will STILL be called, and "a.m_ptr" will be deleted. If at that point, 
this pointer "a.m_ptr" is still pointing to the same object as "m_ptr"
then "m_ptr" will be left as a dangling pointer. When the object containing
m_ptr eventually gets used(or destroyed), we'll get undefined behaviour

Therefore, when implementing move semantics, it's very important to 
ensure the moved-from object is left in a valid state as well, so that 
it will destruct properly(without creating undefined behaviors)

*/

/*
    Automatic l-values returned by value may be moved instead of copied 

In the generateResource() function of the Auto_ptr4 example above, when
variable res is returned by value, it is moved instead of copied, even 
though res is an lvalue!!!

The C++ specification has a special rule that says automatic objects 
returned from a function by value can be moved even if they are lvalues. 
This makes sense, since res was going to be destroyed at the end of the 
function anyway(pretty same as rvalue right :D). Then we might as well
steal its resources instead of making an expensive and unnecessary copy

Although the compiler can move lvalue return values, in some cases it 
may be able to do even better by simply eliding the copy altogether
(which avoids the need to make a copy or do a move at all). In such 
a case neither the copy constructor nor move constructor would be called

*/

/*
    Disabling copying 

In the Auto_ptr4 class above, we left in the copy constructor and
assignment operator for comparision purposes. But in move-enabled classes
it is sometimes desirable to delete the copy constructor and copy assignment
to ensure copies aren't made. In the case of our Auto_ptr class, we dont
want to copy our templated object T - both because it's expensive and 
whatever class T is may not even support copying!

#include <iostream>

template<typename T>
class Auto_ptr5
{
	T* m_ptr {};
public:
	Auto_ptr5(T* ptr = nullptr)
		: m_ptr { ptr }
	{
	}

	~Auto_ptr5()
	{
		delete m_ptr;
	}

	// Copy constructor -- no copying allowed!
	Auto_ptr5(const Auto_ptr5& a) = delete;

	// Move constructor
	// Transfer ownership of a.m_ptr to m_ptr
	Auto_ptr5(Auto_ptr5&& a) noexcept
		: m_ptr { a.m_ptr }
	{
		a.m_ptr = nullptr;
	}

	// Copy assignment -- no copying allowed!
	Auto_ptr5& operator=(const Auto_ptr5& a) = delete;

	// Move assignment
	// Transfer ownership of a.m_ptr to m_ptr
	Auto_ptr5& operator=(Auto_ptr5&& a) noexcept
	{
		// Self-assignment detection
		if (&a == this)
			return *this;

		// Release any resource we're holding
		delete m_ptr;

		// Transfer ownership of a.m_ptr to m_ptr
		m_ptr = a.m_ptr;
		a.m_ptr = nullptr;

		return *this;
	}

	T& operator*() const { return *m_ptr; }
	T* operator->() const { return m_ptr; }
	bool isNull() const { return m_ptr == nullptr; }
};

We are trying to test its performance, with copy we do in  0.00825559 seconds
With the move, on the same machine, we do in 0.0056 seconds 

=> 32.1% faster 

*/

/*
    Deleting the move constructor and move assignment 

You can delte move constructor and move assignment using the "= delete"
syntax in the exact same way you can delete the copy constructor and copy
assignment 

#include <iostream>
#include <string>
#include <string_view>

class Name
{
private:
    std::string m_name {};

public:
    Name(std::string_view name) : m_name{ name }
    {
    }

    Name(const Name& name) = delete;
    Name& operator=(const Name& name) = delete;
    Name(Name&& name) = delete;
    Name& operator=(Name&& name) = delete;

    const std::string& get() const { return m_name; }
};

int main()
{
    Name n1{ "Alex" };
    n1 = Name{ "Joe" }; // error: move assignment deleted

    std::cout << n1.get() << '\n';

    return 0;
}

If you delete the copy constructor, the compiler will not generate an 
implicit move constructor(making your objects neither copyable nor movable)
Therefore, when deleting the copy constructor, it is useful to be explicit
about what behaviour you want from your move constructors. Either explicitly
delete them(making it clear this is the desired behavior), or default them
(making the class move-only)

    Key insight 

The rule of five says that if the copy constructor, copy assignment, 
move constructor, move assignment, or destructor are defined or deleted
then each of those functions should be defined or deleted 

*/

/*

While deleting only the move constructor and move assignment may seem 
like a good idea if you want a copyable but not movable object, this 
has the unfortunate consequence of making the class not returnable by
value in cases where mandatory copy elision does not apply. This happens
because a deleted move constructor is still a declared function, and thus
is eligible for overload resolution. And retun by value will favor a 
deleted move constructor over a non-deleted copy constructor. This 
is illustrated by the following program: 

#include <iostream>
#include <string>
#include <string_view>

class Name
{
private:
    std::string m_name {};

public:
    Name(std::string_view name) : m_name{ name }
    {
    }

    Name(const Name& name) = default;
    Name& operator=(const Name& name) = default;

    Name(Name&& name) = delete;
    Name& operator=(Name&& name) = delete;

    const std::string& get() const { return m_name; }
};

Name getJoe()
{
    Name joe{ "Joe" };
    return joe; // error: Move constructor was deleted
}

int main()
{
    Name n{ getJoe() };

    std::cout << n.get() << '\n';

    return 0;
}
*/

/*
    Issues with move semantics and "std::swap"

Copy and swap also works for move semantics, meaning we can implement 
our move constructor and move assignment by swapping resources with
the object that will be destroyed. 

This has two benefits: 
- The persistent object now controls the resources that were previously 
under ownership of the dying object(which was our primary goal)
- The dying object now controls the resources that were previously under
ownership of the persistent object. When the dying object actually dies, 
it can do any kind of clean up required on those resources. 

When we think about swapping, the first thing that comes to mind is usually 
"std::swap()". However, implementing the move constructor and move assignment
using std::swap() is problematic. Because std::swap() calls both the move
constructor and move assignment on move-capable objects. This will result 
in an infinite recursion issue. 

Here is an example: 

#include <iostream>
#include <string>
#include <string_view>

class Name
{
private:
    std::string m_name {}; // std::string is move capable

public:
    Name(std::string_view name) : m_name{ name }
    {
    }

    Name(const Name& name) = delete;
    Name& operator=(const Name& name) = delete;

    Name(Name&& name) noexcept
    {
        std::cout << "Move ctor\n";

        std::swap(*this, name); // bad!
    }

    Name& operator=(Name&& name) noexcept
    {
        std::cout << "Move assign\n";

        std::swap(*this, name); // bad!

        return *this;
    }

    const std::string& get() const { return m_name; }
};

int main()
{
    Name n1{ "Alex" };
    n1 = Name{"Joe"}; // invokes move assignment

    std::cout << n1.get() << '\n';

    return 0;
}

This prints:

Move assign
Move ctor
Move ctor
Move ctor
Move ctor
And so on… until the stack overflows.

We can implement the move constructor and move assignment using our own
swap function, as long as our swap member function does not call the move
assignment or move assignment. Here's an example of how that can be done:alignas

#include <iostream>
#include <string>
#include <string_view>

class Name
{
private:
    std::string m_name {};

public:
    Name(std::string_view name) : m_name{ name }
    {
    }

    Name(const Name& name) = delete;
    Name& operator=(const Name& name) = delete;

    // Create our own swap friend function to swap the members of Name
    friend void swap(Name& a, Name& b) noexcept
    {
        // We avoid recursive calls by invoking std::swap on the std::string member,
        // not on Name
        std::swap(a.m_name, b.m_name);
    }

    Name(Name&& name) noexcept
    {
        std::cout << "Move ctor\n";

        swap(*this, name); // Now calling our swap, not std::swap
    }

    Name& operator=(Name&& name) noexcept
    {
        std::cout << "Move assign\n";

        swap(*this, name); // Now calling our swap, not std::swap

        return *this;
    }

    const std::string& get() const { return m_name; }
};

int main()
{
    Name n1{ "Alex" };
    n1 = Name{"Joe"}; // invokes move assignment

    std::cout << n1.get() << '\n';

    return 0;
}

This works as expected, and prints:

Move assign
Joe
*/