#include<bits/stdc++.h> 
// this header just contains most of the commonly used 
// C++ standard libraries
// but it's sepcific to GCC and not part of the standard c++ lib 
// using namespace std;
// this directive will tell complier auto add std:: at 
// the front of every std lib we call  
#include<memory> 
class MyClass {
public:
    MyClass(int x, int y) {
        std::cout << "MyClass constructed\n";
    }
    ~MyClass() {
        std::cout << "MyClass destroyed\n";
    }
    void display(int counter) const {
        std::cout << "Displaying MyClass instance" << counter << "\n";
    }
};
void functionForUniquePointer() {
  // Default constructor: sets the pointer to null
  int arg1, arg2;
  std::unique_ptr<MyClass> anim;

  // `std::make_unique`: convenience function to construct
  // an object and put it in a `std::unique_ptr`
  anim = std::make_unique<MyClass>(arg1, arg2); 

  // This won't work, since you can't copy a `std::unique_ptr`
  // (otherwise multiple of them might point to the same heap object)
  // std::unique_ptr<MyClass> anim_bad = anim;

  // Move constructor: moves the contained pointer,
  // and sets the moved-from pointer to null
  std::unique_ptr<MyClass> anim2 = std::move(anim);

}  // The heap object is automatically freed at this point,
   // in the destructor of `std::unique_ptr<MyClass>`

/* 

definition: 
In C++, unique ptr thin wrapper around a raw pointer so make it movable but uncopyable, and each unique
std::unique_ptr ensures exclusive ownership of an object, meaning only one unique_ptr can manage a particular object at any time.

Conclusion: unique_pointer itself is created on stack 
make_unique<MyClass> (arg1, arg2) allocates an object/instance of MyClass on heap and return unique_pointer 
it will automatically deleted when goes out of the scope 
movable: std::unique_ptr<int> anotherUniquePointer = std::move(uniquePointer);
Problem: manually deleting the object managed by unique_pointer leads to a double-free

Another way to talk 
std::unique_ptr is a smart pointer that ensures exclusive ownership of a dynamically allocated object. 
It is movable but non-copyable, meaning ownership can be transferred to another unique_ptr, but it cannot be duplicated. */

void uniquePointerLearning() {
    // unique pointer: 
    // represents unique ownership of a rescource. 
    std::unique_ptr<int> uniquePointer = std:: make_unique<int>(10);
    //every time we go out of the scope that this pointer was declared, 
    // it will automatically deleted
    // unique_ptr enforces exclusive owner ship 
    // Testting 
    int* rawPointer = uniquePointer.get(); 
    std::cout << "Value via uniquePtr: " << *uniquePointer << " " << uniquePointer.get() << std::endl; 
    std::cout << "Value via rawPtr: " << *rawPointer << " " << rawPointer << std::endl; 
    // uniquePointer.reset(); // segmentation fault when I call reset
    // std::cout << "Value via uniquePtr: " << *uniquePointer << " " << uniquePointer.get() << std::endl;  // segmentation fault
    // std::cout << "Value via rawPtr: " << *rawPointer << " " << rawPointer << std::endl; 
    //  rawPointer; // double free happen because when goes out of current scope unique pointer automatically delete
    // lead to double free error 
    // the memory that was still managed by the unique_pointer 
    std::unique_ptr<int> anotherUniquePointer = std::move(uniquePointer); // movable
    // delete anotherUniquePointer.get();
    // // calling reset() on uniquePointer also lead to double free error happen.
    // // std::cout << "Value via uniquePtr: " << *uniquePointer << " " << uniquePointer.get() << std::endl; 
    // std::cout << "Value via anotherUniquePointer: " << *anotherUniquePointer << " " << anotherUniquePointer.get() << std::endl; 
    // std::cout << "Value via rawPtr: " << *rawPointer << " " << rawPointer << std::endl; 

    // try to change the value of object unique_ptr point to but using raw pointer 
    (*rawPointer) = 20;
    // std::cout << "Value via uniquePtr: " << *uniquePointer << " " << uniquePointer.get() << std::endl; 
    std::cout << "Value via rawPtr: " << *rawPointer << " " << rawPointer << std::endl; 
    // the use case of unique_ptr: it will automatically deleted when goes out of the scope 
    // unique ptr thin wrapper around a raw pointer so make it movable but uncopyable 
    // => it will ensuring that each unique_ptr object is either a unique pointer to some heap memory or Null;
}
/*
Definition:
In C++, a `std::shared_ptr` is a smart pointer that manages shared ownership of a dynamically allocated object. Multiple `shared_ptr` instances can own the same object, and the object is destroyed when the last remaining `shared_ptr` owning it is destroyed or reset.

Conclusion:
- `shared_ptr` instances themselves are typically allocated on the stack, but they manage objects allocated on the heap.
- Use `std::make_shared<T>(args...)` to allocate an object of type `T` on the heap and obtain a `shared_ptr` managing it.
- The managed object is automatically deleted when the last `shared_ptr` owning it goes out of scope or is reset.
- `shared_ptr` is copyable and movable, allowing multiple `shared_ptr` instances to share ownership of the same object.

Another way to say it:
`std::shared_ptr` is a smart pointer that allows multiple pointers to share ownership of a dynamically allocated object. It ensures that the object is destroyed when the last `shared_ptr` referencing it is destroyed or reset.
*/

void sharedPointerExample() {
    // Creating a shared_ptr
    std::shared_ptr<MyClass> sp1 = std::make_shared<MyClass>(2, 3);

    // Creating another shared_ptr pointing to the same object
    std::shared_ptr<MyClass> sp2 = sp1;

    std::cout << "Use count: " << sp1.use_count() << '\n'; // Outputs: 2

    // Accessing the object
    sp1->display(1);

    // Resetting one shared_ptr
    sp1.reset();
    std::cout << "Use count after reset: " << sp2.use_count() << '\n'; // Outputs: 1

    // sp2 still owns the object
    sp2->display(2);
} // MyClass instance is destroyed here when sp2 goes out of scope

/*
Definition:
In C++, a `std::weak_ptr` is a smart pointer that holds a non-owning ("weak") reference to an object managed by a `std::shared_ptr`. It must be converted to a `shared_ptr` using the `lock()` method to access the referenced object. If the object has already been destroyed, `lock()` returns an empty `shared_ptr`.

Conclusion:
- `weak_ptr` instances themselves are typically allocated on the stack and do not affect the reference count of the managed object.
- They are used to break circular references between `shared_ptr` instances, preventing memory leaks.
- Before accessing the managed object, always convert the `weak_ptr` to a `shared_ptr` using `lock()` and check if the resulting `shared_ptr` is not null.

Another way to say it:
`std::weak_ptr` is a smart pointer that provides a way to reference an object managed by `std::shared_ptr` without affecting its lifetime. It is useful for preventing circular references that can lead to memory leaks.
*/

void weakPointerExample() {
    std::shared_ptr<MyClass> sp = std::make_shared<MyClass>(2, 3);
    std::weak_ptr<MyClass> wp = sp;

    std::cout << "Use count: " << sp.use_count() << '\n'; // Outputs: 1

    if (std::shared_ptr<MyClass> sp2 = wp.lock()) {
        // Accessing the object
        sp2->display(2);
    } else {
        std::cout << "Object no longer exists\n";
    }

    // Resetting the shared_ptr
    sp.reset();

    if (std::shared_ptr<MyClass> sp2 = wp.lock()) {
        sp2->display(2);
    } else {
        std::cout << "Object no longer exists\n"; // This will be printed
    }
}

/* a circular reference occurs when two or more objects hold shared_ptrs to each other, 
creating a reference loop. This loop prevents the reference count from reaching zero, 
leading to memory leaks because the objects cannot be destroyed automatically. To prevent 
this, one of the references should be a std::weak_ptr */ 

int main() {
    weakPointerExample(); 
    // sharedPointerExample();

}