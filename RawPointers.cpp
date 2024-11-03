#include<bits/stdc++.h> 
using namespace std; 

int main() {
    int y = 0;
    int* x = new int(20);
    cout << y << " " << *x <<endl;
    vector<int> vt = *(new vector<int>(20, 100));
    for (int i = 0; i < 5; i++) {
        cout << vt[i] << " " << i <<endl;
    }
    int* firstElement = (new int[25]);
    for (int t = 0; t < 5; t++) {
        if (t < 3) *(firstElement + t) = t + 10;
        cout << firstElement + t <<" "<<*(firstElement + t) << endl;
    }
    delete firstElement;
    cout <<"After delete" << endl; 
    for (int t = 0; t < 5; t++) {
        cout << firstElement + t <<" "<<*(firstElement + t) << endl;
    }
    // raw pointer is a normal pointer, using this one you must manually handle allocation and deallocation
    // a memory leak when a program allocates memory but does not release it after the memory no longer needed. 
    // => lead to unused memory that cannot be reclaimed, potentially causing the program to use more memory than necessary and in severe case, exhausting available memory 
    
}