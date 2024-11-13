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
    MyClass() {

    }
    MyClass(int x, int y) {
        
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

int main() {
  

}