#include<bits/stdc++.h>
using namespace std; 

class Base {
public:
    virtual void show() {  // Virtual enables dynamic binding
        cout << "Base class function\n";
    }
};

class Derived : public Base {
public:
    void show() override {
        cout << "Derived class function\n";
    }
};

// class BaseWithoutVirtual {
// public:
//     void show() {  // Virtual enables dynamic binding
//         cout << "Base class function\n";
//     }
// };

// class DerivedWithoutVirtual : public BaseWithoutVirtual {
// public:
//     void show() override {
//         cout << "Derived class function\n";
//     }
// };

int main() {
    Base* ptr;
    Derived obj;
    ptr = &obj;

    ptr->show();  // Calls Derived::show() at runtime (Dynamic Binding)
    // print derived 
    /// test part 1, use normal type instead of pointer, can? 
    Base bs;
    Derived der; 
    bs = der; 
    bs.show(); 
    // print base instead ?? 
    /*
    Object slicing happens when a derived class object is assigned to a base class object by value.
    Since the base class does not know about derived members, the extra derived data/functions are sliced off.
    */
    /// test part 2, not using virtual, still use override 
    /* 
    BaseWithoutVirtual bsv; 
    DerivedWithoutVirtual derv; 
    bsv = derv; 
    bsv.show(); // => error "show" func cannot over right, cause dont have virtual keyword of base class !!!
    */



}
