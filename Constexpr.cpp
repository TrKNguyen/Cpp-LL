// constexpr declare that a variable and function can be evaluated at compile time 
// it suggest the compiler that it can calculate it in compile time

// advantage: compiler optimization => move from run time to compile time => reduce running time 
// disadvantage: the values or expressions used in constexpr functions must be known at compile time 
// when test with time-consuming task: find out :

/*
runForLongTime(1e9) have: 

Error 1: 
.\Constexpr.cpp:16:5: error: 'constexpr' loop iteration count exceeds limit of 262144 (use '-fconstexpr-loop-limit=' to increase the limit)
   16 |     for (int i = 0; i < n; i++) {
      |    
mean that by default, loop literation for constexpr have limit only 262144 iterations


Error 2: 
still have error : 
.\Constexpr.cpp:23:21: error: 'constexpr' evaluation operation count exceeds limit of 33554432 (use '-fconstexpr-ops-limit=' to increase the limit)
   23 |     for (int i = 0; i < n; i++) {
      |                     ^
even when have this flag : g++ -fconstexpr-loop-limit=1000000000 -o .\Constexpr.o .\Constexpr.cpp


Error 3: 
Follow the instruction and run this command g++ -fconstexpr-ops-limit=1000000000 -o Constexpr.o Constexpr.cpp
but have this error 1 again 

Constexpr.cpp:32:5: error: 'constexpr' loop iteration count exceeds limit of 262144 (use '-fconstexpr-loop-limit=' to increase the limit)

The strict limit is 33554432

when let n = 1e6 then everything fine 
*/
#include<bits/stdc++.h>
using namespace std; 

constexpr int add(int a, int b) {
    return a + b;
}

constexpr long long runForLongTime(int n) {
    long long ans = 0;
    for (int i = 0; i < n; i++) {
        ans += i;
    }
    return ans;
}

int main() {
    constexpr int result = add(3, 4); 
    cout << result << endl;

    // try the effect of constexpr for every time-comsuming task
    constexpr int n = 1e6;
    constexpr long long ans = runForLongTime(n);
    cout << ans << endl;
}

/*

Best practice

Any constant variable whose initializer is a constant expression should 
be declared as constexpr.

Any constant variable whose initializer is not a constant expression 
(making it a runtime constant) should be declared as const

+------------------------+--------------------------------------------------------------------------------------+
| Term                   | Definition                                                                          |
+------------------------+--------------------------------------------------------------------------------------+
| Compile-time constant  | A value or non-modifiable object whose value must be known at compile time          |
|                        | (e.g., literals and constexpr variables).                                           |
+------------------------+--------------------------------------------------------------------------------------+
| Constexpr              | Keyword that declares objects as compile-time constants (and functions that can be  |
|                        | evaluated at compile-time). Informally, shorthand for “constant expression.”        |
+------------------------+--------------------------------------------------------------------------------------+
| Constant expression    | An expression that contains only compile-time constants and operators/functions      |
|                        | that support compile-time evaluation.                                               |
+------------------------+--------------------------------------------------------------------------------------+
| Runtime expression     | An expression that is not a constant expression.                                    |
+------------------------+--------------------------------------------------------------------------------------+
| Runtime constant       | A value or non-modifiable object that is not a compile-time constant.               |
+------------------------+--------------------------------------------------------------------------------------+

*/