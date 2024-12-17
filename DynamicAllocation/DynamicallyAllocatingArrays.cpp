/*

In addition to dynamically allocating single values, we can also dynamically 
allocate arrays of variables. Unlike a fixed array, where the array size 
must be fixed at compile time, dynamically allocating an array allows us 
to choose an array length at runtime (meaning our length does not need 
to be constexpr).

*/

/*

To allocate an array dynamically, we use the array form of new and 
delete (often called new[] and delete[]):

#include <cstddef>
#include <iostream>

int main()
{
    std::cout << "Enter a positive integer: ";
    std::size_t length{};
    std::cin >> length;

    int* array{ new int[length]{} }; // use array new.  Note that length does not need to be constant!

    std::cout << "I just allocated an array of integers of length " << length << '\n';

    array[0] = 5; // set element 0 to value 5

    delete[] array; // use array delete to deallocate array

    // we don't need to set array to nullptr/0 here because it's going out of scope immediately after this anyway

    return 0;
}

Because we are allocating an array, C++ knows that it should use the 
array version of new instead of the scalar version of new. 
Essentially, the new[] operator is called, even though the [] isn’t 
placed next to the new keyword.

The length of dynamically allocated arrays has type std::size_t. If 
you are using a non-constexpr int, you’ll need to static_cast to 
std::size_t since that is considered a narrowing conversion and 
your compiler will warn otherwise.

Note that because this memory is allocated from a different place 
than the memory used for fixed arrays, the size of the array can be 
quite large. You can run the program above and allocate an array of 
length 1,000,000 (or probably even 100,000,000) without issue. Try 
it! Because of this, programs that need to allocate a lot of memory 
in C++ typically do so dynamically.
*/ 

/* 
    Dynamically deleting arrays

When deleting a dynamically allocated array, we have to use the array 
version of delete, which is delete[].

This tells the CPU that it needs to clean up multiple variables instead 
of a single variable. One of the most common mistakes that new programmers 
make when dealing with dynamic memory allocation is to use delete instead 
of delete[] when deleting a dynamically allocated array. Using the 
scalar version of delete on an array will result in undefined behavior, 
such as data corruption, memory leaks, crashes, or other problems.

One often asked question of array delete[] is, “How does array delete 
know how much memory to delete?” The answer is that array new[] keeps 
track of how much memory was allocated to a variable, so that array 
delete[] can delete the proper amount. Unfortunately, this size/length 
isn’t accessible to the programmer.

*/ 

/* 

    Dynamic arrays are almost identical to fixed arrays

C-style array decay: a fixed array holds the memory address of the first 
array element. A fixed array can decay into a pointer that points to 
the first element of the array. In this decayed form, the length of 
the fixed array is not available (and therefore neither is the size 
of the array via sizeof()), but otherwise there is little difference.

A dynamic array starts its life as a pointer that points to the first 
element of the array. Consequently, it has the same limitations in that 
it doesn’t know its length or size. A dynamic array functions 
identically to a decayed fixed array, with the exception that the 
programmer is responsible for deallocating the dynamic array via the 
delete[] keyword.


*/

/*
    Initializing dynamically allocated arrays

If you want to initialize a dynamically allocated array to 0, the syntax 
is quite simple:

int* array{ new int[length]{} };
Prior to C++11, there was no easy way to initialize a dynamic array to 
a non-zero value (initializer lists only worked for fixed arrays). 
This means you had to loop through the array and assign element values 
explicitly.


int* array = new int[5];
array[0] = 9;
array[1] = 7;
array[2] = 5;
array[3] = 3;
array[4] = 1;


Super annoying!
However, starting with C++11, it’s now possible to initialize dynamic 
arrays using initializer lists!


int fixedArray[5] = { 9, 7, 5, 3, 1 }; // initialize a fixed array before C++11
int* array{ new int[5]{ 9, 7, 5, 3, 1 } }; // initialize a dynamic array since C++11
// To prevent writing the type twice, we can use auto. This is often done for types with long names.
auto* array{ new int[5]{ 9, 7, 5, 3, 1 } };


Note that this syntax has no operator= between the array length and 
the initializer list.

For consistency, fixed arrays can also be initialized using uniform 
initialization:


int fixedArray[]{ 9, 7, 5, 3, 1 }; // initialize a fixed array in C++11
char fixedArray[]{ "Hello, world!" }; // initialize a fixed array in C++11
Explicitly stating the size of the array is optional.

*/ 


/* 
    Resizing arrays

Dynamically allocating an array allows you to set the array length at 
the time of allocation. However, C++ does not provide a built-in way to 
resize an array that has already been allocated. It is possible to work 
around this limitation by dynamically allocating a new array, copying 
the elements over, and deleting the old array. However, this is error 
prone, especially when the element type is a class (which have special 
rules governing how they are created).

Consequently, we recommend avoiding doing this. 
Use std::vector instead. :D 


*/