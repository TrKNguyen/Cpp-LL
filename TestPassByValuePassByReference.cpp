#include<iostream> 
using namespace std; 

Vector<T>(const Vector<T>& o) {  // copy constructor 

}

Vector<T>& operator=(const Vector<T>& o) { // copy assignment

}

Vector<T>(Vector<T>&& o) { // move constructor 
    std::swap(this->arr, o->arr); 
    std::swap(this->size, o->size); 
    std::swap(this->cap, o->cap);
}

Vector<T>& opeartor=(Vector<T>&& o) {
    auto movable = Vector<T>(std::move(o)); 
    this->swap(movable);
    return *this;
}

void dosth(string a) { // pass by value 
    cout <<"Pass by value" << " " << a << endl;
}


void dosth(string& a) { // pass by lvalue reference 
    cout <<"Pass by lvalue reference" << " " << a << endl; 
}


void dosth(const string& a) { // pass by const lvalue reference 
    cout <<"Pass by const lvalue reference" << " " << a << endl; 
}

 
void dosth(string&& a) { // pass by rvalue reference 
    cout <<"Pass by rvalue reference: " <<" " << a << endl; 
}


int main() {
    string s = "test";
    dosth(s); // lvalue  
    /*
    more than one instance of overloaded function "dosth" matches the argument list:C/C++(308)
    TestPassByValuePassByReference.cpp(27, 5): function "dosth(std::string a)" (declared at line 5)
    TestPassByValuePassByReference.cpp(27, 5): function "dosth(std::string &a)" (declared at line 10)
    TestPassByValuePassByReference.cpp(27, 5): argument types are: (std::string)
    */
    string& b = s; 
    dosth(b);
    /*
    more than one instance of overloaded function "dosth" matches the argument list:C/C++(308)
    TestPassByValuePassByReference.cpp(35, 5): function "dosth(std::string a)" (declared at line 5)
    TestPassByValuePassByReference.cpp(35, 5): function "dosth(std::string &a)" (declared at line 10)
    TestPassByValuePassByReference.cpp(35, 5): argument types are: (std::string)
    */
    dosth("abc");
    /*
    more than one instance of overloaded function "dosth" matches the argument list:C/C++(308)
    TestPassByValuePassByReference.cpp(42, 5): function "dosth(std::string a)" (declared at line 5)
    TestPassByValuePassByReference.cpp(42, 5): function "dosth(std::string &&a)" (declared at line 20)
    TestPassByValuePassByReference.cpp(42, 5): argument types are: (const char [4])
    */
    // dosth()
}