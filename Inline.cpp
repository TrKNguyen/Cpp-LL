/// definition: fast, because inline keyword will suggest compiler to replace the function call by function's actual code => reduce the overhead of make a stack frame for calling function  
// actual work: modify the assembly instructions, add function code into the function call line
// advantage: use for non-recursive funcition => reduce the overhead of calling function and stuff, reduce memory usage, time for small function 
// disadvantage: cannot use for recursive function => make infinity loop, also make the code larger 

#include<bits/stdc++.h>
using namespace std; 

inline int sum(int x, int y) {
    return x + y; 
}

int main() {
    cout << sum(1, 2) << endl; 
}