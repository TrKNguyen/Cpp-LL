#include <iostream>
using namespace std;
// padding => just for memory alignment 
struct MyStruct1 {
    char a;        // 1 byte
    double d;      // 8 bytes
    int b;         // 4 bytes
};
// why is 24 bytes: 

// 64-architecture: 
// divide memory into many chunk of 8 bytes
// each chunk size = 8 bytes(cause address now = 8 bytes)
// char a: 1 bytes: padding 7 bytes 
// double d: padding 0 bytes(good) 
// int b: padding 4 bytes 

struct MyStruct2 {
    char a1;       // 1 byte 
    char a2;       // 1 byte
    int c;         // 4 byte
    int h;
    double d;      // 8 bytes
    int b;         // 4 bytes
};

struct MyStruct3 {
    int c;
};

class ClassWithVirtualTable { 
    char a; 
    virtual void func() {
        return;
    }
};

struct TestWith4bytes {
    int a;   // 4 bytes
    float b; // 4 bytes
    char c;  // 1 byte
};
// 12 bytes 
// align with the largest data type inside the struct ? 

struct TestWith4bytesHave8bytes {
    int a;   // 4 bytes
    long long b; // 8 bytes
    char c;  // 1 byte
};
// 24 bytes 
// => data alignment: align with the largest data type inside the struct 

int main() {
    cout << "Size of MyStruct1: " << sizeof(MyStruct1) << " bytes\n";
    cout << "Size of MyStruct2: " << sizeof(MyStruct2) << " bytes\n";
    cout << "Size of MyStruct3: " << sizeof(MyStruct3) << " bytes\n";
    cout << "Size of ClassWithVirtualTable: " << sizeof(ClassWithVirtualTable) << " bytes\n";
    cout << "Size of TestWith4bytes: " << sizeof(TestWith4bytes) << " bytes\n";
    cout << "Size of TestWith4bytesHave8bytes: " << sizeof(TestWith4bytesHave8bytes) << " bytes\n";
    TestWith4bytesHave8bytes test; 
    test.a = 1e9; 
    test.b = (long)1e18; 
    test.c = 'a'; 
    cout << sizeof(test) << " " << test.a << " "<< test.b <<" " << test.c << endl; 
}