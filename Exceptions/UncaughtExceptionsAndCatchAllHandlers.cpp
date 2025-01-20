/*
    Uncaught exceptions 

When a function throws an eexception that it does not handle itself, 
it is making the assumption that a function somewhere down the call stack
will handle the exception. 

Here’s our square root program again, minus the try block in main():

#include <iostream>
#include <cmath> // for sqrt() function

// A modular square root function
double mySqrt(double x)
{
    // If the user entered a negative number, this is an error condition
    if (x < 0.0)
        throw "Can not take sqrt of negative number"; // throw exception of type const char*

    return std::sqrt(x);
}

int main()
{
    std::cout << "Enter a number: ";
    double x;
    std::cin >> x;

    // Look ma, no exception handler!
    std::cout << "The sqrt of " << x << " is " << mySqrt(x) << '\n';

    return 0;
}
Now, let’s say the user enters -4, and mySqrt(-4) raises an exception. 
Function mySqrt() doesn’t handle the exception, so the program looks to 
see if some function down the call stack will handle the exception. main() 
does not have a handler for this exception either, so no handler can be found.

When no exception handler for a function can be found, std::terminate() 
is called, and the application is terminated. In such cases, the call 
stack may or may not be unwound! If the stack is not unwound, local 
variables will not be destroyed, and any cleanup expected upon destruction 
of said variables will not happen!

    Warning 

The call stack may or may not be unwound if an exception is unhandled
If the stack is not unwound, local variables will not be destroyed, 
which may cause problems if those variables have non-trivial destructors 

    As an asise.. 
Although it might seem strange to not unwind the stack in such a case, 
there is a good reason for not doing so. An unhandled exception is generally 
sth we want to avoid at all costs. If the stack were unwound, then all of the 
debug information about the state of the stack that led up to the throwing 
of the unhandled exception would be lost! By not unwinding, we preserve
that information, makeing it easiser to determine how an unhandled exception
was thrown, and fix it. 

When an exception is unhandled, the opearating system will generally 
notify we that an unhandled exception error has occured. How it does 
this depends on the opearting system, but possibilites include printing
an error message, popping up an error dialog, or simply crashing
Some OSes are less graceful than others. Generally this is sth we want 
to avoid. 

*/

/*

    Cacth-all handlers 

And now we find ourselves in a conundrum: 

- Functions can potentially throw exception of any data type, meaning 
there is an infinte number of possible exception tty[es to catch 
- if an eception is not caught, our program will terminate immedidately 
(and the stack may not be unwound), so our program may not even clean up
after itself properly 
- Adding explicti catch handlers for every possible type is tedious, 
espcially for the one s that are expected to be reached only in 
the exceptional cases!

Fortunately, C++ also provides us with a mechanism to cactch all types of excepions. This is know as a catch-all handler. A catch0all handler works just like a normal catch back, except that instead of using s pecific type to catch, it uses the ellipses operator(...) as the 
type to catch for this rason, the catch all handler is also somethimgs called ellipsis catch handler 

Recall, ellipeses were previously used to pass arguments of any type of a function. In this context, they represent exceptions of any data type

#include <iostream>

int main()
{
	try
	{
		throw 5; // throw an int exception
	}
	catch (double x)
	{
		std::cout << "We caught an exception of type double: " << x << '\n';
	}
	catch (...) // catch-all handler
	{
		std::cout << "We caught an exception of an undetermined type\n";
	}
}

Because ther is no specific excetion handler for type int, the catch-all handler catches this exception . This example, produces the following result 

We caught an exception of an undetermined type
The catch-all handler must be placed last in the catch block chain. This is to ensure that exceptions can be caught by exception handlers tailored to specific data types if those handlers exist.

Often, the catch-all handler block is left empty:

catch(...) {} // ignore any unanticipated exceptions

This will catch any unanticipated exceptions, ensuring that stack unwinding occurs up to this point and preventing the program from terminating, but does no specific error handling.
*/


/*
    Using the catch-all handler to wrap main()    

One use for the catch-all handler is to wrap the contents of main():

#include <iostream>

struct GameSession
{
    // Game session data here
};

void runGame(GameSession&)
{
    throw 1;
}

void saveGame(GameSession&)
{
    // Save user's game here
}

int main()
{
    GameSession session{};

    try
    {
        runGame(session);
    }
    catch(...)
    {
        std::cerr << "Abnormal termination\n";
    }

    saveGame(session); // save the user's game (even if catch-all handler was hit)

    return 0;
}
In this case, if runGame() or any of the functions it calls throws an 
exception that is not handled, it will be caught by this catch-all handler. 
The stack will be unwound in an orderly manner (ensuring destruction of 
local variables). This will also prevent the program from terminating 
immediately, giving us a chance to print an error of our choosing and save 
the user’s state before exiting.

*/

/*

    Debussing unhandled exceptions 

Unhandleed exceptions are an indation that sth unexcepected has ha[[end, and we probalby want to 
diagnorewhy an unahndled exception was thrown in the first place. Many debuggers will 
break on unhandled exceptions allowing us to view the stack at the point where the unhndled exception was thrown

However, if we have a catch-all handler, then all exceotions are handled and we lose useful diagnostic information 

Therefore, in debug builds, it can be useful to disable the catch-all 
handler , we can do this via conditional compilation directives 

#include <iostream>

struct GameSession
{
    // Game session data here
};

void runGame(GameSession&)
{
    throw 1;
}

void saveGame(GameSession&)
{
    // Save user's game here
}

class DummyException // a dummy class that can't be instantiated
{
    DummyException() = delete;
};

int main()
{
    GameSession session {};

    try
    {
        runGame(session);
    }
#ifndef NDEBUG // if we're in release node
    catch(...) // compile in the catch-all handler
    {
        std::cerr << "Abnormal termination\n";
    }
#else // in debug mode, compile in a catch that will never be hit (for syntactic reasons)
    catch(DummyException)
    {
    }
#endif

    saveGame(session); // save the user's game (even if catch-all handler was hit)

    return 0;
}

Syntacticallym a try block requires at least one asscoaited catch 
block. So if the catch-all handler is conditionally compiled out, we 
either need to consitionally compile out the try block, or we need to conditionally compile 
in another catch block, it's cleaner to do the latter 

For this purpose we create class DummyException which can't be instantiated because it has deleted 
default constructor and no other constructors. When NDEBUG is defined, we compile in a catch handler to catch an eception 
of type DummnyException. Since we can't create a Dummny exception, this catch 
handler will never catch anything Threfore any exceptions that reach this point will not be handled 

*/