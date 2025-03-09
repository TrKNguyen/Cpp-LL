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

struct Example {
    char testChar;
    int b;
    long long c;
};
struct Example1 {
    int b;
    long long c;
    char testChar;
};
struct Example2 { // size = 24 
    int b;
    long long c;
    char testChar; 
};

struct Example3 { // size = 24;
    int b;
    long long c;
    char testChar;
    char testChar1;
    char testChar2;
    int testInt;
};
struct Example4 { // size = 8 
    char testChar;
    short testShort; 
    int testInt;
};
// 3 things to remember when talking about Memory Padding
// size 
// alignment 
// padding 
// starting address should always mutiple of min(alignment value, 4 bytes) 
// long long start from 8 bytes 
// int/char start from 4 bytes
// alignment do: 
// 1. make sure the starting address of each data type is a multiple of its size
// 2. make sure the size of each data type is a multiple of its size

// Advantage: Enhance the performance, if we do SIMD operation on 
// the array of float, we adding two large arrays of float. Each
// float is 4 byes, the processor if optimized for 16-byte memory 
// accesses for SIMD operations
// Without the proper alignment if the starting address of the array 
// isn't aligned to 16-byte boundary, accessing elements of the array 
// could cause the CPU to perform additional memory loads or stores, 
// potentially resulting in multiple cycles per element fetch, slowing 
// down the program 
// Misalignment: Each load and store operation might take 2 or more cycles

// Here's a deeper look at the steps involved when dealing with misaligned data:

// Accessing Misaligned Data: When data is misaligned, say a 128-bit
// chunk that begins at an address which is not a multiple of 16 
// (e.g., at address 0x123), the CPU needs to load the data in pieces.

// Scenario 1: The first 4 bytes (out of the 16 bytes of the 128-bit
// chunk) might be in one cache line, while the next 4 bytes could 
// be in a different cache line. So, the CPU will need to fetch the
// first cache line, then the second cache line, to access all 16 bytes.
// Scenario 2: Even if only part of the chunk crosses a cache line,
// the CPU may have to perform a two-step fetch from two different
// memory locations, which causes extra memory fetch cycles

// Multiple Cycles for a Single Access: On modern CPUs, aligned
//  accesses can load all data at once (in a single 128-bit fetch), 
// while misaligned accesses may require multiple fetches. This
// could involve:
// Fetching the first portion of the data from the first cache line.
// Fetching the second portion from the next cache line or from a
// different memory page.


// Extra Cycles for Buffering:  CPUs may also need to buffer or 
// reorganize the data internally to assemble it properly. This
//  buffering or handling of misaligned data takes extra cycles,
// especially when the data spans multiple cache lines.

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