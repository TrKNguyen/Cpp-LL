#include <iostream>
#include <vector>

class Object {
public:
    Object() = default;
    Object(const Object &o) {
        std::cout << "Copy constructor called" << std::endl;
    }
    Object(Object &&o) {
        std::cout << "Move constructor called" << std::endl;
    }
};

std::vector<Object> v;

template<typename T>
void func(T &&x) {
    v.emplace_back(std::forward<T>(x));
}

Object get() {
    Object o;
    return o;
}

int main() {
    Object o;
    func(get());
}
