#include<bits/stdc++.h> 
using namespace std; 

int main() {
    int y = 0;
    int* x = new int(20);
    cout << y << " " << *x <<endl;
    vector<int> vt = *(new vector<int>(20, 100));
    for (int i = 0; i < 20; i++) {
        cout << vt[i] << " " << i <<endl;
    }
}