/*
    Introduction to C++ templates

Welcome to the world of C++ templates

In C++, the templaet system was designed to simplitfy the process of 
creating functions (or classes) that are able to work with different 
data types

Instead of manually creating a bunch of mostly-identical functions or 
classes (one for each set of different types), we instead create a 
single template, just like a normal definition, a template definition 
describes what a function or class lookes like. Unlike a normal definition 
(where all types must be specified), in a teamplate we can use one or
more placeholder types. A placeholder type represents some type that 
that is not known at the time the template is defined, but that will be 
provided later(when the template is used). 

Once a template is defined, the complier can use the template to 
generate as many overloaded fucntions(or classes) as needed, each 
using different actual types!

The end result is the same - we end up with a bunch of mostly identical
functions or classes(one for each set of different types), But we only 
have to create and maintain a single template, and the compiler does
all the hard work to create the rest for us. 

    Key insight 

The compiler can use a single template to generate a family of realted 
functions or classes, each using a different set of actual types 

Because the actual types aren't determined until the template is used 
in a program (not when the template is written), the author of the 
template doesn't have to try to anticipate all of the acutal types that 
might be used. This means template code can be used with types that didn't 
even exist when the template was written! We'll see how this comes in 
handy later, when we start exploring c++ standard library, which is 
absolutely full of template code!

    Key insight 

Templates can work with types that didn't even xist when the template 
was written. This helps make template code both flexible and furture
proof!

*/

/*

    Function templates 

A function template is a function-like definition that is used to 
generate one or more overloaded functions, each with a different set of 
acutal types. This is what will allow us to create functions that can work 
with many different types. The initial function template that is used
to generate other functions is called the primary template, and the 
functions generated from the primary template are called instantiated
functions 

When we create a primary function template, we use placeholder types
(technically called type template parameters, informally called template
types) for any parameter type, return types, or types used in the function
body what we want to be specified later, by the user of the template

    For advanced readers

C++ supports 3 different kind of template parameters: 
- Type template parmeters (where the template parameter represents a 
type)
- Non-type template parameters (where the template parameter represents 
a constexpr value).
- Template template parameters (where the template parameter represents 
a template)

Type template parameters are by far the most common, so we'll focus on 
those first. We'll also discuss non-type template parameters, which are
seeing increased usage in modern C++

Function templates are something that is best taught by example, so 
let's convert our normal max(int, int) function from the example above
into a function template. It's surprisingly easy, and we'll explain what's 
happening along the way. 

*/

/*
    Creating a max() function template

Here's the int version of max() again: 

int max(int x, int y)
{
    return (x < y) ? y : x;
}

Note that we use type int three times in this function: once for 
parameter x, once for parameter y, and once for the return type of the
function 

To create a function template for amx(), we're going to do two things. First
wr're going to replace any actual types that we want to be specified 
with type template parameters. In this case, because we have only one type
that needs replacing (int), we only need one type template parameter
(which we'll call T): 

Here's our new function thatuses a single template type, where all 
occurrences of actual type int have been replaced with type template 
parameter T: 

T max(T x, T y) { // won't compile because we haven't defined T 
    return (x < y) ? y : x; 
}

This is a good start -- however, it won't compile because the compiler
doesn't kmow what T is! And this is still a normal function, not a function
template

Second, we're going to tell the compiler that this is a template, and 
that T is a type template parameter that is a placeholder for any type.
Both of these are done using a template parameter declaration, which 
defines any template parameters that will be subsequently used. The 
scope of a template parameter declaration is strictly limited to the function
template(or class template) that follows. Therefore, each function template
or class template needs its own template parameter declaration. 

template <typename T> // this is the template parameter declaration defining T as a type template parameter
T max(T a, T b) {
    return (a < b) ? a : b; 
}

As an aside…

There is no difference between the typename and class keywords in this 
context. You will often see people use the class keyword since it was 
introduced into the language earlier. However, we prefer the newer typename
keyword, because it makes it clearer that the type template parameter 
can be replaced by any type (such as a fundamental type), not just 
class types.

*/

/*

    Naming template parameters 

Much like we often use a single letter for variable names used in trivial 
situations (e.g x), it's conventional to use a single capital letter 
(starting with T) when the template parameter us used in a trivial 
or obvious way. For example, in our max function template: 

template <typename T>
T max(T x, T y)
{
    return (x < y) ? y : x;
}

We don’t need to give T a complex name, because it’s obviously just 
a placeholder type for the values being compared, and T can be any 
type that can be compared (such as int, double, or char, but not nullptr).

Our function templates will generally use this naming convention.

If a type template parameter has a non-obvious usage or specific 
requirements that must be met, there are two common conventions for 
such names:

- Starting with a capital letter (e.g. Allocator). The standard library 
uses this naming convention.

- Prefixed with a T, then starting with a capital letter (e.g. TAllocator). 
This makes it easier to see that the type is a type template parameter.

Which you choose is a matter of personal preference.

*/

/*
    For advanced readers

As an example, the standard library has an overload of std::max() that is 
declared like this:

template< class T, class Compare >
const T& max( const T& a, const T& b, Compare comp ); 

Because a and b are of type T, we know that we don’t care what type a 
and b are -- they can be any type. Because comp has type Compare, we 
know that comp must be a type that meets the requirements for a Compare 
(whatever that is).

When a function template is instantiated, the compiler replaces the 
template parameters with the template arguments and then compiles the 
resulting instantiated function. Whether the function compiles depends 
on how the object(s) of each type are used within the function. 
Therefore, the requirements for a given template parameter are essentially 
implicitly defined.

Because it can be hard to infer requirements from how objects of the 
type are used, this is one of those areas where it is useful to consult 
technical documentation, which should explicitly state the requirements. 
For example, if we want to know what the requirements for a Compare are, 
we can look up the documentation for std::max 
(e.g. see https://en.cppreference.com/w/cpp/algorithm/max) and it should 
be listed there.  

    Best practice 

Use a single capital letter starting with T(e.g T, U, V, ..) to name 
type template parameters that are used in trivial or obvious ways and 
represent "any reasonable type"alignas

If the type template parameter has a non-obvious usage or specific 
requirements that must be met, then a more descriptive name is warranted
(e.g Allocator or TAllocator)


*/


