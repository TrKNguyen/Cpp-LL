/*
A class can always access its own (non-inherited) members.
The public accesses the members of a class based on the access specifiers 
of the class it is accessing.
A derived class accesses inherited members based on the access specifier 
inherited from the parent class. This varies depending on the access 
specifier and type of inheritance used.
*/

/*
    Public inheritance 

Public inheritance is by far the most commonly used type of inheritance 
In fact, very rarely will we see or use the other type of inheritance 
so our primary goal should be focus on understanding this section, 

Public inheriatance is also the easiest to understand, when we inherit
a base class publicly, inherited public members stay public, and inherited
protected members stay protected, inherited private, members, which were
inaccessible because they were private in the base class, stay inaccessible 

+------------------------------+-----------------------------------+
|Access specifier in base class| Access specifier when inherited publicly |
+------------------------------+-----------------------------------+
| Public                       | Public                            |
| Protected                    | Protected                         |
| Private                      | Inaccessible                      |
+------------------------------+-----------------------------------+

    Best practice 

Use public inheriatance unless we have a specific reason to do otherwise

*/

/*
    Protected inheritance 

Protected inheritance is the least common method of inheritance. It is 
almost never used, except in very particular cases, with protected 
inheritance, the public and protected memebers become protected and 
private members stay inaccessible 

Because this form of inheriatance is so rare, we'll skip the example and 
just summarize with a table: 

+------------------------------+-------------------------------------+
|Access specifier in base class| Access specifier when inherited protectedly |
+------------------------------+-------------------------------------+
| Public                       | Protected                           |
| Protected                    | Protected                           |
| Private                      | Inaccessible                        |
+------------------------------+-------------------------------------+

*/

/*
    Private inheritance 

With private inheritance, all memebers from the base class are inherited 
as private. This mean private members are inaccessible, and protected and 
public memebers become private

Note that this does not affect the way that the derived class accesses
members inherited from its parent! It only affects the code trying to access 
those members through the derived class

class Base
{
public:
    int m_public {};
protected:
    int m_protected {};
private:
    int m_private {};
};

class Pri: private Base // note: private inheritance
{
    // Private inheritance means:
    // Public inherited members become private (so m_public is treated as private)
    // Protected inherited members become private (so m_protected is treated as private)
    // Private inherited members stay inaccessible (so m_private is inaccessible)
public:
    Pri()
    {
        m_public = 1; // okay: m_public is now private in Pri
        m_protected = 2; // okay: m_protected is now private in Pri
        m_private = 3; // not okay: derived classes can't access private members in the base class
    }
};

int main()
{
    // Outside access uses the access specifiers of the class being accessed.
    // In this case, the access specifiers of base.
    Base base;
    base.m_public = 1; // okay: m_public is public in Base
    base.m_protected = 2; // not okay: m_protected is protected in Base
    base.m_private = 3; // not okay: m_private is private in Base

    Pri pri;
    pri.m_public = 1; // not okay: m_public is now private in Pri
    pri.m_protected = 2; // not okay: m_protected is now private in Pri
    pri.m_private = 3; // not okay: m_private is inaccessible in Pri

    return 0;
}

+------------------------------+----------------------------------+
|Access specifier in base class| Access specifier when inherited privately |
+------------------------------+----------------------------------+
| Public                       | Private                          |
| Protected                    | Private                          |
| Private                      | Inaccessible                     |
+------------------------------+----------------------------------+


*/

/*
+----------------------------------------+------------------------------------------------+------------------------------------------------+------------------------------------------------+
| Access specifier in base class         | Access specifier when inherited publicly       | Access specifier when inherited privately      | Access specifier when inherited protectedly     |
+----------------------------------------+------------------------------------------------+------------------------------------------------+------------------------------------------------+
| Public                                 | Public                                         | Private                                        | Protected                                      |
| Protected                              | Protected                                      | Private                                        | Protected                                      |
| Private                                | Inaccessible                                   | Inaccessible                                   | Inaccessible                                   |
+----------------------------------------+------------------------------------------------+------------------------------------------------+------------------------------------------------+


*/

/*
    Best practice

Avoid multiple unless alternatives lead to more complexity 

*/