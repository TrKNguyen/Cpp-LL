#include <iostream>

class A
{
public:
    int *p;
};

int main()
{
    std::cout << sizeof(A);
}