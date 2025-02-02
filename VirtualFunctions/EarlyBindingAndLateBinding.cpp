/*
    When a C++ program is executed, it executes sequentially, beginning 
at the top of main(). When a function call is encounterd, the point of 
execution jumps to the beginning of the function being called. How does
the CPU know how to do this? 

When a program is compiled, the compiler converts each statement in 
our C++ program into one or more lines of machine language or assembly 
language. Each line of machine language is given its own unique sequential
address. This is no different for functions -- when a function is encountered
, it is converted into machine language and given the next available address. 
Thus, each function ends up with a unique address. 
*/

/*
    Binding and dispatching 

Our programs contain many names(identifiers, keywords, ...). Each name 
has a set of associcated properties: for example if the name represents 
a variable, that variable has a type, a value, a memory address, etc... 

For example, when we say int x, we're telling the compiler to associate
the name x with the type int. Later if we say x = 5, the compiler can
use this association to type check the assignment to ensure it is valid

In general programming, binding is the process of associating names with 
such properties. Function binding(or method binding) is the process that 
determines what function definition is associated with a fucntion call. 
The process of actually invoking a bound function is called dispaching 

In C++, the term binding is used more casually( and dispatching is usually
considered part of binding)

    Nomenclature

Binding is an overloaded term. In other contexts, binding may refer to: 
- The binding of a reference to an object 
- std::bind 
- Language binding 

*/

/*
    Early binding 

Most of the function calls the compiler encounters will be direct funcion
calls. A direct function call is a statement that direcly calls a function. 

#include <iostream>

struct Foo
{
    void printValue(int value)
    {
        std::cout << value;
    }
};

void printValue(int value)
{
    std::cout << value;
}

int main()
{
    printValue(5);   // direct function call to printValue(int)

    Foo f{};
    f.printValue(5); // direct function call to Foo::printValue(int)
    return 0;
}

In C++, when a direct call is made to a non-member function or a
non-virtual member function, the compiler can determine which function 
definition should be matched to the call. This is sometimes called 
early binding(or static binding), as it can be performed at compile time
The compiler can then generate machine language instructions that tell 
CPU to jump directly to the address of the function 

If we look at the assembly code generated for the call to printValue(5) (using clang x86-64), we see something like this:

        mov     edi, 5           ; copy argument 5 into edi register in preparation for function call
        call    printValue(int)  ; directly call printValue(int)

We can clearly see that this is a direct function call to printValue(int)

Calls to overloaded functions and function templates can also be resolved
at compile-time: 

#include <iostream>

template <typename T>
void printValue(T value)
{
    std::cout << value << '\n';
}

void printValue(double value)
{
    std::cout << value << '\n';
}

void printValue(int value)
{
    std::cout << value << '\n';
}

int main()
{
    printValue(5);   // direct function call to printValue(int)
    printValue<>(5); // direct function call to printValue<int>(int)

    return 0;
}

Let's take a look at a simple calculator program that uses early binding: 

#include <iostream>

int add(int x, int y)
{
    return x + y;
}

int subtract(int x, int y)
{
    return x - y;
}

int multiply(int x, int y)
{
    return x * y;
}

int main()
{
    int x{};
    std::cout << "Enter a number: ";
    std::cin >> x;

    int y{};
    std::cout << "Enter another number: ";
    std::cin >> y;

    int op{};
    std::cout << "Enter an operation (0=add, 1=subtract, 2=multiply): ";
    std::cin >> op;

    int result {};
    switch (op)
    {
        // call the target function directly using early binding
        case 0: result = add(x, y); break;
        case 1: result = subtract(x, y); break;
        case 2: result = multiply(x, y); break;
        default:
            std::cout << "Invalid operator\n";
            return 1;
    }

    std::cout << "The answer is: " << result << '\n';

    return 0;
}

Because add(), substract(), and multiply() are all direct function call 
to non-member function, the compiler will match these function calls 
to their respectice function definitions at compile-time

Note that because of the switch statement, which function is actually 
called is not determined until runtime. However, that is a path of execution
issue, not a binding issue. 

*/




/*
    Late binding 

In some cases, a function call can't be resolved until runtime. In C++, 
this is sometimes known as late binding(or in case of virtual function
resolution, dynamic dispatch)

In general programming terminology, the term "late binding" usually means 
that the function being called can't be determined based on static type 
information alone, but must be resolved using dynamic tyoe information 

In C++, the term tends to be used more loosely to mean any function call 
where the actual function being called is not known by the compiler or 
linker at the point where the function call is actually being made. 

In C++, one way to get late binding is to use function pointers. To 
review function pointers briefly, a function pointer is a type of pointer
that points to a function instead of a variable. The function that a function
pointer points to can be called by using the function call operator () 
on the pointer 

#include <iostream>

void printValue(int value)
{
    std::cout << value << '\n';
}

int main()
{
    auto fcn { printValue }; // create a function pointer and make it point to function printValue
    fcn(5);                  // invoke printValue indirectly through the function pointer

    return 0;
}

Calling a function call via a function pointer is also known as an 
indirect function call. At the point where fcn(5) is actually called. 
The compiler does not know at compile-time what function is being called
Instead, at runtime, an indirect function call is made to whatever function
exists at the address held by the function pointer 

If we look at the assembly code generated for the call to fcn(5) (using clang x86-64), we see something like this:

        lea     rax, [rip + printValue(int)] ; determine address of printValue and place into rax register
        mov     qword ptr [rbp - 8], rax     ; move value in rax register into memory associated with variable fcn

        mov     edi, 5                       ; copy argument 5 into edi register in preparation for function call
        call    qword ptr [rbp - 8]          ; invoke the function at the address held by variable fcn
You can clearly see that this is a indirect function call to printValue(int) via its address.


The following calculator program is functionally identically to the
calculator example above, except it uses a function pointer instead 
of a direct function call 

#include <iostream>

int add(int x, int y)
{
    return x + y;
}

int subtract(int x, int y)
{
    return x - y;
}

int multiply(int x, int y)
{
    return x * y;
}

int main()
{
    int x{};
    std::cout << "Enter a number: ";
    std::cin >> x;

    int y{};
    std::cout << "Enter another number: ";
    std::cin >> y;

    int op{};
    std::cout << "Enter an operation (0=add, 1=subtract, 2=multiply): ";
    std::cin >> op;

    using FcnPtr = int (*)(int, int); // alias ugly function pointer type
    FcnPtr fcn { nullptr }; // create a function pointer object, set to nullptr initially

    // Set fcn to point to the function the user chose
    switch (op)
    {
        case 0: fcn = add; break;
        case 1: fcn = subtract; break;
        case 2: fcn = multiply; break;
        default:
            std::cout << "Invalid operator\n";
            return 1;
    }

    // Call the function that fcn is pointing to with x and y as parameters
    std::cout << "The answer is: " << fcn(x, y) << '\n';

    return 0;
}

In this example, instead of calling the add(), subtract(), or multiply() 
function directly, we’ve instead set fcn to point at the function we wish 
to call. Then we call the function through the pointer.

The compiler is unable to use early binding to resolve the function call 
fcn(x, y) because it can not tell which function fcn will be pointing to 
at compile time!

*/

/*

Late binding is slightly less efficient since it involves an extra level 
of indirection. With early binding, the CPU can jump directly to the
function's address. With late binding, the program has to read the address
held in the pointer and then jump to that address. This involves one
extra step, making it slightly slower. However, the advange of late binding
is that it is more flexible than early binding, because decisions about 
what function to call do not need to be made until runtime. 

The powerful usecase of late binding is for implementing virtual functions 


*/