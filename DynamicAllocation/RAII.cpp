/*
    R A I I 

RAII (Resource acquisition is initialization) is a programming technique 
whereby resource use is tied to the life time of object with automatic 
duratio(e.g non-dynamically allocated objects). In C++, RAII is implemented
via classes with constructors and destructors. A resource(such as memory, 
a file or database handle, etc...) is typically acquired in the object's 
constructor(though it can be acquired after the object is created if that
makes sense). That resource can then be used while the object is alive . 
The resource is released in the destructor, when the object is destroyed 
The primary advantage of RAII is that it helps prevent resource leaks 
(e.g memory not beling deallocated) as all resource-holding objects are 
cleaned up automatically 

The IntArray class at the top of this lesson is an example of a class
that implements RAII -- allocation in the constructor, deallocation 
in the destructor, std::string and std::vector are examples of classes
in the standard library that follows RAII -- dynamic memory is acquired
on intialization, and cleaned up automatically on destruction.

*/