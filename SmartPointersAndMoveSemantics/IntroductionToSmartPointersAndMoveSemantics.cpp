/*
    Smart Pointer classes to the rescue

One of the best things about classes is that they contain destructors
that automatically get executed when an object of the lcass goes out 
of the scope. So if we allocate(or acquire) memory in the constructor.
we can deallocate it in our destructor, and be guranteed that the memory
will be deallocated when the class object is destroyed(regardless of 
whether it goes out of scope, gets explicitly deleted, etc..)

This is the heart of the RAII programming paradigm. 

Then we can use a class to help us manage and clean up our pointers

Consider a class whose sole job was to hold and "own" a pointer passed
to it, and then deallocate that pointer when the class object went out
of the scope. As long as objects of that class were only created as 
local variables, we could guarantee that the class would properly go
out of scope(regardless of when or how our functions terminate) and the
owned pointer would get destroyed. 

Here's a first draft of the idea: 

#include <iostream>

template <typename T>
class Auto_ptr1
{
	T* m_ptr {};
public:
	// Pass in a pointer to "own" via the constructor
	Auto_ptr1(T* ptr=nullptr)
		:m_ptr(ptr)
	{
	}

	// The destructor will make sure it gets deallocated
	~Auto_ptr1()
	{
		delete m_ptr;
	}

	// Overload dereference and operator-> so we can use Auto_ptr1 like m_ptr.
	T& operator*() const { return *m_ptr; }
	T* operator->() const { return m_ptr; }
};

// A sample class to prove the above works
class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destroyed\n"; }
};

int main()
{
	Auto_ptr1<Resource> res(new Resource()); // Note the allocation of memory here

        // ... but no explicit delete needed

	// Also note that we use <Resource>, not <Resource*>
        // This is because we've defined m_ptr to have type T* (not T)

	return 0;
} // res goes out of scope here, and destroys the allocated Resource for us

The program prints: 

Resource acquired 
Resource destroyed 

As long as Auto_ptr1 is defined as a local variable, the Resource will 
be guaranteed to be destroyed at the end of the block it is declared in,
regardless of how the function terminates 

Such a class is called a smart pointer 
*/

/*
    A Smart Pointer is a composition class that is designed to manage
dynamically allocated memory and ensure that memory gets deleted when 
the smart pointer object goes out of the scope.

Here is how the smart pointer can solve our challenge: 

#include <iostream>

template <typename T>
class Auto_ptr1
{
	T* m_ptr {};
public:
	// Pass in a pointer to "own" via the constructor
	Auto_ptr1(T* ptr=nullptr)
		:m_ptr(ptr)
	{
	}

	// The destructor will make sure it gets deallocated
	~Auto_ptr1()
	{
		delete m_ptr;
	}

	// Overload dereference and operator-> so we can use Auto_ptr1 like m_ptr.
	T& operator*() const { return *m_ptr; }
	T* operator->() const { return m_ptr; }
};

// A sample class to prove the above works
class Resource
{
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource destroyed\n"; }
    void sayHi() { std::cout << "Hi!\n"; }
};

void someFunction()
{
    Auto_ptr1<Resource> ptr(new Resource()); // ptr now owns the Resource

    int x;
    std::cout << "Enter an integer: ";
    std::cin >> x;

    if (x == 0)
        return; // the function returns early

    // do stuff with ptr here
    ptr->sayHi();
}

int main()
{
    someFunction();

    return 0;
}

if user enters a non-zero integer, the program prints: 

Resource acquired
Hi!
Resource destroyed

if user enters zero, the above program will terminate early, prints: 

Resource acquired
Resource destroyed

Regardless of how the function be terminated, the resource will be 
destroyed eventually 

*/

/*
    A critical flaw 

However, above implementation has a critical flaw lurking behind some 
auto-generated code. 

#include <iostream>

// Same as above
template <typename T>
class Auto_ptr1
{
	T* m_ptr {};
public:
	Auto_ptr1(T* ptr=nullptr)
		:m_ptr(ptr)
	{
	}

	~Auto_ptr1()
	{
		delete m_ptr;
	}

	T& operator*() const { return *m_ptr; }
	T* operator->() const { return m_ptr; }
};

class Resource
{
public:
	Resource() { std::cout << "Resource acquired\n"; }
	~Resource() { std::cout << "Resource destroyed\n"; }
};

int main()
{
	Auto_ptr1<Resource> res1(new Resource());
	Auto_ptr1<Resource> res2(res1); // Alternatively, don't initialize res2 and then assign res2 = res1;

	return 0;
} // res1 and res2 go out of scope here

The program prints: 

Resource acquired
Resource destroyed
Resource destroyed

Very likely the program will crash at this point(double delete error), 

What should we do? 
*/

/*
    Move semantics 

Whatif, instead of having our copy constructor and assignment operator 
copy the pointer("copy semantics"), we instead transfer/move ownership
of the pointer from the source to the destination object? (This is the
core idea) behind MOVE SEMANTICS

MOVE SEMANTICS mean the class will transfer ownership of the object 
rather than making a copy. Let's update our Auto_ptr1 class to show 
how this can be done

#include <iostream>

template <typename T>
class Auto_ptr2
{
	T* m_ptr {};
public:
	Auto_ptr2(T* ptr=nullptr)
		:m_ptr(ptr)
	{
	}

	~Auto_ptr2()
	{
		delete m_ptr;
	}

	// A copy constructor that implements move semantics
	Auto_ptr2(Auto_ptr2& a) // note: not const
	{
		// We don't need to delete m_ptr here.  This constructor is only called when we're creating a new object, and m_ptr can't be set prior to this.
		m_ptr = a.m_ptr; // transfer our dumb pointer from the source to our local object
		a.m_ptr = nullptr; // make sure the source no longer owns the pointer
	}

	// An assignment operator that implements move semantics
	Auto_ptr2& operator=(Auto_ptr2& a) // note: not const
	{
		if (&a == this)
			return *this;

		delete m_ptr; // make sure we deallocate any pointer the destination is already holding first
		m_ptr = a.m_ptr; // then transfer our dumb pointer from the source to the local object
		a.m_ptr = nullptr; // make sure the source no longer owns the pointer
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

int main()
{
	Auto_ptr2<Resource> res1(new Resource());
	Auto_ptr2<Resource> res2; // Start as nullptr

	std::cout << "res1 is " << (res1.isNull() ? "null\n" : "not null\n");
	std::cout << "res2 is " << (res2.isNull() ? "null\n" : "not null\n");

	res2 = res1; // res2 assumes ownership, res1 is set to null

	std::cout << "Ownership transferred\n";

	std::cout << "res1 is " << (res1.isNull() ? "null\n" : "not null\n");
	std::cout << "res2 is " << (res2.isNull() ? "null\n" : "not null\n");

	return 0;
}

The program will print: 

Resource acquired
res1 is not null
res2 is null
Ownership transferred
res1 is null
res2 is not null
Resource destroyed

The operator= gave ownership of m_ptr from res1 to res2! Consequently, 
we don't end up with duplicate copies of the pointer, and everything 
gets tidily cleaned up

Delete a nullpir is okay, as it does nothing
*/

/*
    Moving forward 

The ccore problem with the design of std::auto_ptr os that prior to
C++11, the C++ language simply had no mechanism to differentiate 
"copy semantics" from "move semantics". Overriding the copy semantics
to implement move semantics lead to weird edge cases and inadvertent
bugs. For example, you can write res1 = res2 without knowing whether
res2 will be changed or not!

=> Because of this, in C++11, the concept of "move" was formally defined
and "move semantics" were added to the language to properly differentiate
copying from moving. Now that we're set the stage for why move semantics
can be useful. 

*/