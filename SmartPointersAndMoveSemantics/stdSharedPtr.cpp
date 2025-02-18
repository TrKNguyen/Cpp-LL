/*
    std::shared_ptr

Unlike std::unique pointer, which is designed to singly own and manage
a resource, std::shared_ptr is meant to solve the case where you need
multiple smart pointers co-owning a resource 

This means that it is fine to have multiple std::shared_ptr pointing 
to the same resource. Internally, std::shared_ptr keeps track of how 
many std::shared_ptr are sharing the resource. As long as at least one 
std::shared_ptr is pointing to the resource, the resource will not be 
deallocated, even if individual std::shared_ptr are destroyed. As soon as
the last std::shared_ptr managing the resource goes out of the scope 
(or is reassigned to point at something else), the resource will be 
deallocated 

Like unique_ptr, shared_ptr lives in the <memory> header 

#include <iostream>
#include <memory> // for std::shared_ptr

class Resource
{
public:
	Resource() { std::cout << "Resource acquired\n"; }
	~Resource() { std::cout << "Resource destroyed\n"; }
};

int main()
{
	// allocate a Resource object and have it owned by std::shared_ptr
	Resource* res { new Resource };
	std::shared_ptr<Resource> ptr1{ res };
	{
		std::shared_ptr<Resource> ptr2 { ptr1 }; // make another std::shared_ptr pointing to the same thing

		std::cout << "Killing one shared pointer\n";
	} // ptr2 goes out of scope here, but nothing happens

	std::cout << "Killing another shared pointer\n";

	return 0;
} // ptr1 goes out of scope here, and the allocated Resource is destroyed


This prints:

Resource acquired
Killing one shared pointer
Killing another shared pointer
Resource destroyed

Note that we created a second shared pointer from the first share pointer
This is important. Consider the following similar program: 

#include <iostream>
#include <memory> // for std::shared_ptr

class Resource
{
public:
	Resource() { std::cout << "Resource acquired\n"; }
	~Resource() { std::cout << "Resource destroyed\n"; }
};

int main()
{
	Resource* res { new Resource };
	std::shared_ptr<Resource> ptr1 { res };
	{
		std::shared_ptr<Resource> ptr2 { res }; // create ptr2 directly from res (instead of ptr1)

		std::cout << "Killing one shared pointer\n";
	} // ptr2 goes out of scope here, and the allocated Resource is destroyed

	std::cout << "Killing another shared pointer\n";

	return 0;
} // ptr1 goes out of scope here, and the allocated Resource is destroyed again

This program prints: 

Resource acquired
Killing one shared pointer
Resource destroyed
Killing another shared pointer
Resource destroyed

and then crashes

The difference here is that we created two shared pointer independently 
from each other. As a consequence, even though they're both pointing to 
the same Resource, they aren't aware of each other. When ptr2 go out of the 
scope, it thinks it's the only owner then delete the resource 

Ptr1 also think the same thing, and tries to delete the Resource again 

Then bad things happens 

Fortunately, this is easily avoied, if we need more than one shared
pointer to a given resource, copy an existing shared pointer 

    Best practice 

Always make a copy of an existing std::shared_ptr if we need more than one
std::shared_ptr pointing to the same resource 

*/

/*
    std::make_shared 

Much like std::make_unique() can be used to create unique pointer 
std::make_shared() can be used to make a shared pointer and it's available
in C++11

Here's our example, using make_shared()

#include <iostream>
#include <memory> // for std::shared_ptr

class Resource
{
public:
	Resource() { std::cout << "Resource acquired\n"; }
	~Resource() { std::cout << "Resource destroyed\n"; }
};

int main()
{
	// allocate a Resource object and have it owned by std::shared_ptr
	auto ptr1 { std::make_shared<Resource>() };
	{
		auto ptr2 { ptr1 }; // create ptr2 using copy of ptr1

		std::cout << "Killing one shared pointer\n";
	} // ptr2 goes out of scope here, but nothing happens

	std::cout << "Killing another shared pointer\n";

	return 0;
} // ptr1 goes out of scope here, and the allocated Resource is destroyed

The reasons for using std::make_shared() are the same as std::make_unique() -- 
std::make_shared() is simpler and safer (there’s no way to create two 
independent std::shared_ptr pointing to the same resource but unaware of 
each other using this method). However, std::make_shared() is also more 
performant than not using it. The reasons for this lie in the way that 
std::shared_ptr keeps track of how many pointers are pointing at a given resource.
*/

/*
    Digging into share_ptr 

Unlike std::unique_ptr, which uses a single pointer internally, shared pointer
used two pointer internally. One pointer points at the resource being managed
The other points at a "control block", which is dynamically allocated object
that tracks of bunch of stuff, including how many shared pointer are pointing 
to the resource. 

When share pointer is created via std::shared_ptr constructor, the memory
for manage the managed object(which is usually passed in) and control block 
(which the constructor creates) are allocated separately. However, when 
using std::make_shared(), this can be optimized into single memory allocation, 
which leads to better performance. 

This also explains why independently creating two std::shared_ptr pointed
to the same resource gets us into trouble 
Each shared_ptr will have one pointer pointing at the resource. However, 
each std::shared_ptr will independently allocate its own control block
which will deallocate the resource, not realizing there are other shared
ponter also trying to manage that resource

However, when a std::shared_ptr is cloned using copy assignment, the data
in the control block can be appropriately updated to indicate that there 
are now additional std::share_ptr co-managing the resource 

*/

/*
    Shared pointers can be created from unique pointers 

A std::unique_ptr can be converted into a std::shared_ptr via special
std::shared_ptr constructor that accepts a std::unique_ptr rvalue 
The contents of the std::unique_ptr will be moved to the std::shared_ptr

However, std::shared_ptr can not be safely converted to a std::unique_ptr
This means that if we're creating a function that is going to return a smart
pointer, we're better off returning a std::unique_ptr and assigning it to 
a shared_ptr if and when that's appropriate 
*/

/*
    The perils of std::shared_ptr 

std::shared_ptr has some of the same challenges as std::unique_ptr -- if
the std::shared_ptr is not properly disposed of (either because it was
dynamically allocated and never deleted, or it was part of an object 
that was dynamically allocated and never deleted) then the resource it 
is managing won’t be deallocated either. With std::unique_ptr, you only 
have to worry about one smart pointer being properly disposed of. With 
std::shared_ptr, you have to worry about them all. If any of the 
std::shared_ptr managing a resource are not properly destroyed, the 
resource will not be deallocated properly.
*/

/*
    std::shared_ptr and arrays 

In C++17 and earlier, std::shared_ptr does not have proper support for 
managing arrays, and should not be used to manage a C-style array. As 
of C++20, std::shared_ptr does have support for arrays 

*/