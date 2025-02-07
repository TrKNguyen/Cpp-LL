/*

    Templates and container classes 

When coding, we will face some cases when need to write some two classes
are almost identical! In fact, the only substantive difference is the 
contained data type. This is another area where templates can be pute to 
good use, to free us from having to create classes that are bound to one
specific data type. 

Creating template classes works pretty much identically to creating template 
functions, so we'll proceed by example. 

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>

template <typename T> // added
class Array
{
private:
    int m_length{};
    T* m_data{}; // changed type to T

public:

    Array(int length)
    {
        assert(length > 0);
        m_data = new T[length]{}; // allocated an array of objects of type T
        m_length = length;
    }

    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;

    ~Array()
    {
        delete[] m_data;
    }

    void erase()
    {
        delete[] m_data;
        // We need to make sure we set m_data to 0 here, otherwise it will
        // be left pointing at deallocated memory!
        m_data = nullptr;
        m_length = 0;
    }

    // templated operator[] function defined below
    T& operator[](int index); // now returns a T&

    int getLength() const { return m_length; }
};

// member functions defined outside the class need their own template declaration
template <typename T>
T& Array<T>::operator[](int index) // now returns a T&
{
    assert(index >= 0 && index < m_length);
    return m_data[index];
}

#endif

As we can see, this version is almost identical to the the normal version, 
except we've added the template declaration, and changed 
the contained data type from int to T. 

Note that we've also defined the operator[] function outside of the class
declaration. This isn't necessary, but new programmers typically stumble
when trying to do this for the first tume dues to the syntax, so an example 
is instructive. Each templated member function defined outside the class
declaration needs its own template declaration. Also, note that the name
of the templated array class is Array<T>, not Array, Array would refer to 
a non-templated version of a class named Array, unless Array is used inside
of the class. For example, the copy constructor and copy-assignment 
operator used Array rather than Array<T>. When the class name is used without 
template arguments inside of the class, the arguments are the same as the 
ones of the current instantiation. 

Here's a short example using the above templated array class

#include <iostream>
#include "Array.h"

int main()
{
	const int length { 12 };
	Array<int> intArray { length };
	Array<double> doubleArray { length };

	for (int count{ 0 }; count < length; ++count)
	{
		intArray[count] = count;
		doubleArray[count] = count + 0.5;
	}

	for (int count{ length - 1 }; count >= 0; --count)
		std::cout << intArray[count] << '\t' << doubleArray[count] << '\n';

	return 0;
}

This example prints the following:

11     11.5
10     10.5
9       9.5
8       8.5
7       7.5
6       6.5
5       5.5
4       4.5
3       3.5
2       2.5
1       1.5
0       0.5

Templates classes are instantiated in the same way template functions are
the compiler stencils out a copy upon demand, with the template parameter
replaced by the actual data type the user needs, and then compiles the 
copy. If we dont ever use a template class, the compiler won't even compile
it. 

Template classes are ideal for implementing container classes, because it 
is hightly desrirable to have containters work across a wide ariety of 
data types, and templates allow we to do so without duplicating code. 
Although the syntax is ugly, and the error message can be cryptic, 
templates classes are truly one of C++'s best and most useful features
*/

/*

    Splitting up template classes 

A template is not a class or a function, it is a stencil used to create 
classes or functions. As much, it does not work in quite the same was as normal
functions or class. In most cases, this isn't much of a issue. However, 
there is one area that commonly causes problems for developers 

With non-template classes, the common procedure is to put the class defintion 
in a header file, and the member function defintions in a similarly named 
code file. In this way, the member function defintions are compiled as 
a separate project file. However, with templates, this does not work. 
Consider the following: 

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>

template <typename T> // added
class Array
{
private:
    int m_length{};
    T* m_data{}; // changed type to T

public:

    Array(int length)
    {
        assert(length > 0);
        m_data = new T[length]{}; // allocated an array of objects of type T
        m_length = length;
    }

    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;

    ~Array()
    {
        delete[] m_data;
    }

    void erase()
    {
        delete[] m_data;
        // We need to make sure we set m_data to 0 here, otherwise it will
        // be left pointing at deallocated memory!
        m_data = nullptr;
        m_length = 0;
    }

    // templated operator[] function defined below
    T& operator[](int index); // now returns a T&

    int getLength() const { return m_length; }
};

// Definition of Array<T>::operator[] moved into Array.cpp below

#endif

Array.cpp:

#include "Array.h"

// member functions defined outside the class need their own template declaration
template <typename T>
T& Array<T>::operator[](int index) // now returns a T&
{
    assert(index >= 0 && index < m_length);
    return m_data[index];
}

#include <iostream>
#include "Array.h"

int main()
{
	const int length { 12 };
	Array<int> intArray { length };
	Array<double> doubleArray { length };

	for (int count{ 0 }; count < length; ++count)
	{
		intArray[count] = count;
		doubleArray[count] = count + 0.5;
	}

	for (int count{ length - 1 }; count >= 0; --count)
		std::cout << intArray[count] << '\t' << doubleArray[count] << '\n';

	return 0;
}

The above program will compilem but cause a linker error L 

undefined reference to 'Array<int>:: operator[] (int)

Just like with function templates, the compiler will only instantiate 
a class template if the class template is used 
both the full class template defintion(not just a declaration) and the 
specific templated needed 

Also remember that C++ compiles files individually. When main.cpp is compiled, 
the contents of the Array.h header(including the template class defintion)
are copied into main.cpp, When the compiler sees that we need two template 
instance, Array<int> and Array<double>, it will instantiate these, and 
compile them as part of the main.cpp translation unit. Because the operator 
member function has a declartion, the compiler will accept a call to it, 
assuming it will be defined elsewhere. 

When array.cpp is compiled separately, the contents of the Array.h header
are copied into Array.cpp, but the compiler won't find any code in Array.cpp 
that requires the Array class template or Array<int>::operator[] function
template to be instantiated - so it won't instantiate anything. 

Thus, when the program is linked, we'll get a linker error, because main.cpp 
made a call to Array<int>::operator[] but that template function was never
instantiated! 

There are quite a few ways to work around this!

The easiest way is to simply put all of our template class code in the 
header file (int this case, put the contents of Array.cpp into Array.h, 
below the class). In this way, when we #include the header. all of the 
template code will be in one place. The upside of this solution is that
it is simple. The downside here is that if the template class is used 
in many files, we will endup with mnay local instances of the template class
, which can increase the compile and link times. This is our prefered 
solution unless the compile or link times start to become a problem. 

If we feel that putting the Array.cpp code into the Array,h header makes 
the header too long/messym an alternative is to move the contents of Array.cpp 
to a new file named Array.inl(.inl stands for inline)m and then include 
Array.inl at the bottom of the Array.h header(inside the header guard)
That yields the smae results as putting all the code in the header, but
helps keep things a little more organized

    Tup

If we use the ,inl method and then get a compiler error about duplicates
definitions, our compiler is most likely compiling the.inl file as part of 
the project as if it were a code file. This results in the contents of 
the .inl getting compiled twice:: once when our compiler compiles the .inl, 
and once when the .cpp file that includes the.inl gets compiled. If the 
.inl file contains any non-inline functions (or variables), then we will 
run afoul of the one definition rule. If this happens, we;ll need to exlcude
the .inl file from being compiled as part of the build 

Excluding the .inl from the build can usually be done by right clicking 
on the .inl file in the project view, and then choosing properties. The 
setting will be somewhere in there. In VS, set "Exclude from build" to "Yes"alignas
In codeblock, uncheck compile file, and link file 

Another alternative is to use a three-file approach. The template class
defintion goes in the header. The template class member functions goes 
in the code file. Then we add a thid file, which contains all of the instantiated
classes we need: 

templates.cpp:

// Ensure the full Array template definition can be seen
#include "Array.h"
#include "Array.cpp" // we're breaking best practices here, but only in this one place

// #include other .h and .cpp template definitions you need here

template class Array<int>; // Explicitly instantiate template Array<int>
template class Array<double>; // Explicitly instantiate template Array<double>

// instantiate other templates here

The "template class" command causes the compiler to explicitly instantiate
the template class. In the above case, the compiler will stencil out 
definitions for Array<int> and Array<double> inside of templates.cpp 
Other code files that want to use these types can include Array.h 
and the linker will link in these explicit type definitions from template.cpp 

This method may be more efficient(depending on how our compiler and linker
handle templates and duplicate definitions) but requires maintaining the 
templates.cpp files for each program. 


*/



