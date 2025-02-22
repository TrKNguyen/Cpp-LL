/*
#include <string_view>

class Base
{
protected:
    int m_value {};

public:
    Base(int value)
        : m_value{ value }
    {
    }

    std::string_view getName() const { return "Base"; }
    int getValue() const { return m_value; }
};

class Derived: public Base
{
public:
    Derived(int value)
        : Base{ value }
    {
    }

    std::string_view getName() const { return "Derived"; }
    int getValueDoubled() const { return m_value * 2; }
};

When we create a Derived object =, it contains a Base part(which is 
constructed first), and a Derived part(which is constructed seocnd)

Remeber that inheritance implies an is a relationship between two classes, 
Since a Derived is a Base, it is appropriate that Derived contain a Base
part 

*/

/*
    Pointers, references, and derived classes

It should be fairly intuitive that we can se Derived pointers and
references to Derived objectes 

#include <iostream>

int main()
{
    Derived derived{ 5 };
    std::cout << "derived is a " << derived.getName() << " and has value " << derived.getValue() << '\n';

    Derived& rDerived{ derived };
    std::cout << "rDerived is a " << rDerived.getName() << " and has value " << rDerived.getValue() << '\n';

    Derived* pDerived{ &derived };
    std::cout << "pDerived is a " << pDerived->getName() << " and has value " << pDerived->getValue() << '\n';

    return 0;
}

This produces the following output:

derived is a Derived and has value 5
rDerived is a Derived and has value 5
pDerived is a Derived and has value 5

However, since Derived has a Base part, a more interesting question is 
whether C++ will let us set a Base pointer or reference to a Derived 
object, and the answer is yes. 

#include <iostream>

int main()
{
    Derived derived{ 5 };

    // These are both legal!
    Base& rBase{ derived }; // rBase is an lvalue reference (not an rvalue reference)
    Base* pBase{ &derived };

    std::cout << "derived is a " << derived.getName() << " and has value " << derived.getValue() << '\n';
    std::cout << "rBase is a " << rBase.getName() << " and has value " << rBase.getValue() << '\n';
    std::cout << "pBase is a " << pBase->getName() << " and has value " << pBase->getValue() << '\n';

    return 0;
}

This produces the result:

derived is a Derived and has value 5
rBase is a Base and has value 5
pBase is a Base and has value 5

It quite different from what we were expecting at first 

It turn out that because rBase and pBase are a Base reference and pointer
they can only see members of Base(or any classes that Base inherited)
So even though Derived::getName() shadows() Base::getName() for Derived
object, Base pointers/reference can not see Derived::getName()
Consequently, they call Base::getName(), which is why rBase and pBase
report that they are a Base rather than a Derived

Note that this also means it is not possible to call Derived::getValueDouble()
using rBase or pBase. They are unable to see anything in Derived 

Here's another slightly more complex example that we'll build on 

#include <iostream>
#include <string_view>
#include <string>

class Animal
{
protected:
    std::string m_name;

    // We're making this constructor protected because
    // we don't want people creating Animal objects directly,
    // but we still want derived classes to be able to use it.
    Animal(std::string_view name)
        : m_name{ name }
    {
    }

    // To prevent slicing (covered later)
    Animal(const Animal&) = delete;
    Animal& operator=(const Animal&) = delete;

public:
    std::string_view getName() const { return m_name; }
    std::string_view speak() const { return "???"; }
};

class Cat: public Animal
{
public:
    Cat(std::string_view name)
        : Animal{ name }
    {
    }

    std::string_view speak() const { return "Meow"; }
};

class Dog: public Animal
{
public:
    Dog(std::string_view name)
        : Animal{ name }
    {
    }

    std::string_view speak() const { return "Woof"; }
};

int main()
{
    const Cat cat{ "Fred" };
    std::cout << "cat is named " << cat.getName() << ", and it says " << cat.speak() << '\n';

    const Dog dog{ "Garbo" };
    std::cout << "dog is named " << dog.getName() << ", and it says " << dog.speak() << '\n';

    const Animal* pAnimal{ &cat };
    std::cout << "pAnimal is named " << pAnimal->getName() << ", and it says " << pAnimal->speak() << '\n';

    pAnimal = &dog;
    std::cout << "pAnimal is named " << pAnimal->getName() << ", and it says " << pAnimal->speak() << '\n';

    return 0;
}
This produces the result:

cat is named Fred, and it says Meow
dog is named Garbo, and it says Woof
pAnimal is named Fred, and it says ???
pAnimal is named Garbo, and it says ???

We see the same issue here. Because pAnimal is an Animal pointer, it can 
only see the Animal portion of the class. Consequently, pAnimal->speak()
calls Animal::speak() rather than the Dog::speak() or Cat::speak() function


*/

/*
    Use for pointers and references to base classes 

Now we might be saying, "The above examples seem kind of silly, why 
would we set a pointer or reference to the base class of a derived object
when we can just use the derived class?" It turns out that there are
quite a few good reasons 

First, let's say we wanted to write a function that printed an animal'
name and sound, without using a pointer to a base class, we'd have to 
write it using overloaded function, like this : 

void report(const Cat& cat)
{
    std::cout << cat.getName() << " says " << cat.speak() << '\n';
}

void report(const Dog& dog)
{
    std::cout << dog.getName() << " says " << dog.speak() << '\n';
}

Not too difficult, but consider what would happen if we had 30 different 
animal types instead of 2, we'd have to write 30 almost identical functions
Plus, if we ever added a new type of animal, you'd have to write a new 
function for that one too. This is a huge waste of time considering the 
only real difference is the type of the parameter 

However, because Cat and Dog are derieved from Animal, Cat and Dog have 
an Animal part. Therefore, it makes sence that we should be able to 
do something like this: 

void report(const Animal& rAnimal)
{
    std::cout << rAnimal.getName() << " says " << rAnimal.speak() << '\n';
}

This would let us pass in any class derived from Animal, even ones that
we created after we wrote the function! Instead of one function per derived
class, we get one function that works with all classes derived from 
Animal!

Although both of these technique could help us save a lot of time and
energy, they have the same problem. The pointer or reference to the base 
class calls the base version of the function rather than the derived version
If only there was some way to make those base pointers call the derived
version of a function instead of the base version... 

Seem like we need something to support this? Is that virtual function? :DD 

*/
