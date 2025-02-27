#include<bits/stdc++.h>
using namespace std; 

class String {
private: 
    char *str;
    int size = 0;
public: 
    // 0 argument constructor 
    String() {
        str = new char[1];
        size = 0;
        str[0] = '\0';
    }
    // 1 argument constructor 
    String(const char *param) {
       size = strlen(param);
       str = new char[size + 1];
       memcpy(str, param, size); 
       str[size] = '\0';
    }
    // copy constructor 
    String(const String& o) {
        size = strlen(o.str);
        str = new char[size + 1]; 
        memcpy(str, o.str, size);
        str[size] = '\0';
    }
    // copy assignment 
    String& operator=(const String& o) {
        String copy(o.str);
        size = o.size; 
        swap(this->str, copy.str);
        return *this;
    }
    // move constructor 
    String(String&& o) {
        size = strlen(o.str); 
        swap(this->str, o.str);
    }
    // move assignment 
    String& operator=(String&& o) {
        size = strlen(o.str); 
        swap(this->str, o.str);
        return *this;
    }
    // destructor 
    ~String() {
        delete[] str;
    }

    // push_back 
    void push_back(char c) {
        size++;
        char* newBuffer = new char[size + 1]; 
        for (int i = 0; i < size - 1; i++) {
            newBuffer[i] = str[i];
        }
        newBuffer[size - 1] = c; 
        newBuffer[size] = '\0'; 
        delete[] str; 
        str = newBuffer;
    }

    // access string element 
    char& operator[](int pos) {
        if (pos < 0 || pos >= size) {
            throw out_of_range("Index out of bound: " + to_string(pos));
        }
        return str[pos];
    }

    // + opeartor 
    String operator+(const String& o) {
        int newSize = size + o.size; 
        char *newBuffer = new char[newSize + 1]; 
        memcpy(newBuffer, str, size); 
        memcpy(newBuffer + size, o.str, o.size);
        newBuffer[newSize] = '\0';
        String result(newBuffer);
        return result;
    }

    // += operator 
    String& operator+=(const String& o) {
        int newSize = size + o.size; 
        char *newBuffer = new char[newSize + 1]; 
        memcpy(newBuffer, str, size); 
        memcpy(newBuffer + size, o.str, o.size);
        newBuffer[newSize] = '\0';
        delete[] str; 
        str = newBuffer;
        size = newSize;
        return *this;
    }

    // << operator 
    friend ostream& operator<<(ostream& os, const String& s) {
        // Inferior implementation since we're making iostream calculate the
        // size when we already know it:
        // os << s.str; 
        // return os;
        // better way: 
        return os.write(s.str, static_cast<streamsize>(s.size));
    }
}; 

int main() {
    String s("Hello world! ");
    String t("I'm a noob and try to print"); 
    s += t; 
    s[6] = 'W';
    s = s + ". Now I check the + operator";
    cout << s << endl;
    s = s + ". Okay seem good!"; 
    cout << s << endl; 
}