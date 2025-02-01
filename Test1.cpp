#include <vector>
#include <utility> // for std::move
#include <iostream>

int foo(std::vector<int>&& vt) {
    std::cout << "Called with rvalue reference" << std::endl;
    return vt.size(); // Example operation
}

int main() {
    std::vector<int> vt = {1, 2, 3}; // `vt` is an lvalue
    foo(std::move(vt)); // Passes vt as an rvalue
    return 0;
}
