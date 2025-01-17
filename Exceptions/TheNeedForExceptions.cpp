/*
    When return code fails 

When writing reusable code, error handling is a necessity. One of the most 
common ways to handle potential erros is via return codes. For example: 

#include <string_view>

int findFirstChar(std::string_view string, char ch)
{
    // Step through each character in string
    for (std::size_t index{ 0 }; index < string.length(); ++index)
        // If the character matches ch, return its index
        if (string[index] == ch)
            return index;

    // If no match was found, return -1
    return -1;
}

This function returns the index of the first character matching ch within
string. If the character can not be found, the functions returns -1 as 
an indicator that the character wasn't found. The primary virtue of this 
approach is that it is extremly simple. However, using return codes has 
a number of drawbacks which can quickly become apparent when used in 
non-trivial cases: 

    First, return values can be cryptic -- if a functions returns -1, is 
it trying to indicate an erro, or is that actually a valid return value? 
It's often hard to tell without digging into the guts of the function
of consulting documentations. 

Second, functions can only return one value, so what happens when we need
to return both a function result and a possible error code? 

Consider the following function 

double divide(int x, int y)
{
    return static_cast<double>(x)/y;
}

This function is in desperate need of some error handling, because it will 
crash if the user passes in 0 for parameter y. However, it also needs 
to return the result of x/y. How can it do both? The most common answer is 
that either the result or the error handling will have to be passes back 
as a reference parameter, which makes for ugly code that is less convenient 
to use. For examle: 

#include <iostream>

double divide(int x, int y, bool& outSuccess)
{
    if (y == 0)
    {
        outSuccess = false;
        return 0.0;
    }

    outSuccess = true;
    return static_cast<double>(x)/y;
}

int main()
{
    bool success {}; // we must now pass in a bool value to see if the call was successful
    double result { divide(5, 3, success) };

    if (!success) // and check it before we use the result
        std::cerr << "An error occurred" << std::endl;
    else
        std::cout << "The answer is " << result << '\n';
}

Third, in sequences of code where many things can go wrong, error codes have to 
be checked coonstantly. Consider the following snippet of code that 
involves parsing a text file for values that are supposed to be there: 

std::ifstream setupIni { "setup.ini" }; // open setup.ini for reading
// If the file couldn't be opened (e.g. because it was missing) return some error enum
if (!setupIni)
    return ERROR_OPENING_FILE;

// Now read a bunch of values from a file
if (!readIntegerFromFile(setupIni, m_firstParameter)) // try to read an integer from the file
    return ERROR_READING_VALUE; // Return enum value indicating value couldn't be read

if (!readDoubleFromFile(setupIni, m_secondParameter)) // try to read a double from the file
    return ERROR_READING_VALUE;

if (!readFloatFromFile(setupIni, m_thirdParameter)) // try to read a float from the file
    return ERROR_READING_VALUE;

It haven't covered file access yet, so don'w worry if we dont't understand 
how the aove works - just note the fact that every class requires an 
error-check and return back to the caller. Now imageine if there were
20 parameters of differing types - we're essentially checking for an error 
and returning ERROR_READING_VALUE 20 times! All of this error checking 
and returning values makes determining what the function is trying to do 
much harder to dicern.alignas

Fourth, return codes do not mix with constructor very well. what happens if 
we're creating an object and sth inside the constructors goes wrong? 
Constructors have no return type to pass back a status indicator, 
and passing one back via a reference paramter is messay and must be 
explicitly checked. Furthermore, even if we do this, the object will 
still be created and then has to be dealt with or disposed of. 

Finally, when an error code is returned to the caller, the caller 
may not always be equipped to handle the error. If the caller doesn't 
want to handle the error. it either has to ignore it(in which case it
will be lost forever), or return the error up the stack to the function
that called it). this can be messay and lead to many of the same issues
noted above

To summarize the primary issue with return codes is that the rror handling 
code ends up intricately linked to the normal control flow of the code. 
THis in turn ends up constraining both how the code is laid out, and how 
erros can be reasonably handled

*/

/*

    Exceptions 

Exception handling provides a mechanism to decouple handling of errors 
or other exceptional ciccumstances from the typical control flow of our 
code. This allows more freedom to handle erros when and how ever is most 
useful for a given situation, alleviating most of the messiness that 
return codes cause. 

*/