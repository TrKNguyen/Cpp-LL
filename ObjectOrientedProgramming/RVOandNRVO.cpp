/*
    R V O (RETURN VALUE OPTIMIZATION)

RVO is a compiler optimization that eliminates the need for copy or move
construction when returning an object from a function. The compiler constructs
the return value directly in the location where it will be used, avoiding 
unnecessary temporary object creation. 

#include <iostream>

class A {
public:
    A() { std::cout << "Constructor\n"; }
    ~A() { std::cout << "Destructor\n"; }
};

A createA() {
    return A(); // Compiler optimizes this to construct A directly in the caller's scope
}

int main() {
    A obj = createA(); // No extra copy/move constructor calls
}

*/

/*

    N R V O (NAMED RETURN VALUE OPTIMIZATION)

NRVO  is an optmization simlilar to RVO but applies when return a named 
local object 

#include <iostream>

class A {
public:
    A() { std::cout << "Constructor\n"; }
    ~A() { std::cout << "Destructor\n"; }
};

A createA() {
    A obj; // Named local variable
    return obj; // NRVO: Compiler optimizes to construct obj directly in caller
}

int main() {
    A obj = createA(); // No extra copy/move constructor calls with NRVO
}
#include <iostream>

class A {
public:
    A() { std::cout << "Constructor\n"; }
    ~A() { std::cout << "Destructor\n"; }
};

A createA() {
    A obj; // Named local variable
    return obj; // NRVO: Compiler optimizes to construct obj directly in caller
}

int main() {
    A obj = createA(); // No extra copy/move constructor calls with NRVO
}

*/

/*

| Feature                     | RVO (Return Value Optimization)       | NRVO (Named Return Value Optimization)              |
|-----------------------------|---------------------------------------|-----------------------------------------------------|
| Return Type                 | Temporary object                      | Named local variable                                |
| Compiler Requirement (C++17)| Mandatory                             | Not mandatory (compiler-dependent)                  |
| Guarantee                   | Stronger                              | Weaker (depends on compiler)                        |
| Common Usage                | Returning an unnamed temporary object | Returning a named local object                      |
| Performance Impact          | Eliminates copy/move operations       | Avoids copy/move if compiler applies NRVO           |
| Move Semantics Interaction  | Not needed if applied                 | Move constructor may be used if NRVO is not applied |
*/

/*
    RVO vs Move semantics

Before C++ 11, RVO/NRVO was crucial to avoid unnecessary copies. With
C++11 move semantics, the compiler can use move consturctors of RVO/NRVO
isn't applied.

#include <iostream>
#include <utility> // for std::move

class A {
public:
    A() { std::cout << "Constructor\n"; }
    A(const A&) { std::cout << "Copy Constructor\n"; }
    A(A&&) noexcept { std::cout << "Move Constructor\n"; }
};

A createA() {
    A obj;
    return std::move(obj); // Forces move instead of NRVO
}

int main() {
    A obj = createA(); // Move constructor will be used
}


*/