#include <bits/stdc++.h>
using namespace std; 

/// some note: we can replace new by allocator, then call allocate, construct for new and 

template<typename T> 
class Vector { 
private: 
  T* m_value; 
  int m_size; 
  int m_cap; 

public:
  // constructor 
  Vector() {
    this->m_value = new T[1]; 
    this->m_size = 0;
    this->m_cap = 1;
  }
  // constructor with size 
  Vector(int size) {
    this->m_value = new T[size]; 
    this->m_size = size;
    this->m_cap = size;
  }
  // copy constructor 
  Vector(const Vector<T>& o) {
    this->m_size = o.m_size; 
    this->m_cap = o.m_cap;
    this->m_value = new T[this->m_cap];
    for (int i = 0; i < this->m_size; i++) {
      this->m_value[i] = o.m_value[i];
    }
  }
  // copy assignment 
  Vector<T>& operator=(const Vector<T>& o) {
    Vector<T> temp = Vector(o);
    this->swap(temp);
    return *this;
  }
  // move constructor 
  Vector(Vector<T>&& o) {
    this->m_size = o.size; 
    this->m_cap = o.m_cap; 
    swap(this->m_value, o.m_value);
  }
  // move assignment 
  Vector<T>& operator=(Vector<T>&& o) {
    this->m_size = o.size; 
    this->m_cap = o.m_cap; 
    swap(this->m_value, o.m_value);
  }
  // destructor 
  ~Vector<T>() {
    delete[] m_value;
  }
  void swap(Vector& temp) {
    std::swap(this->m_value, temp.m_value);
    std::swap(this->m_size, temp.m_size); 
    std::swap(this->m_cap, temp.m_cap);
  }
  void reserve(int new_cap) {
    if (new_cap <= m_cap) return;
    m_cap = m_cap * 2; 
    T* temp = new T[m_cap];
    // memcpy(temp, this->m_value, this->m_size);
    for (int i = 0; i < this->m_size; i++) {
      temp[i] = this->m_value[i];
    }
    delete[] m_value;
    m_value = temp;
  }
  void push_back(const T& n_elem) {
    if (m_size == m_cap) {
      reserve(2 * m_cap);
    }
    m_value[m_size] = n_elem; 
    m_size++;
  }
  void pop_back() {
    if (m_size == 0) {
      throw out_of_range("Cannot popback empty vector");
    }
    m_size--;
  }
  T& operator[](int index) {
    if (index < 0 || index >= m_size) {
      throw out_of_range("Index out of bound");
    }
    return m_value[index];
  }

  int size() const noexcept {
    return this->m_size;
  }
  int capacity() const noexcept {
    return this->m_cap;
  }
  friend ostream& operator<<(ostream& os, Vector<T>& vt) {
    os << "["; 
    for (int i = 0; i < vt.size(); i++) {
      if (i > 0) os << ", ";
      os << vt[i]; 
    }
    os << "]";
    return os;
  }
  using iterator = T*;
  using const_iterator = const T*; 

  iterator begin() {
    return m_value;
  }
  iterator end() {
    return m_value + m_size;
  }
  const_iterator begin() const{
    return m_value;
  }
  const_iterator end() const{ 
    return m_value + m_size;
  }

}; 

int main() {
  // vector<int> vt1; 
  // cout << vt1.size() <<" " << vt1.capacity() << endl; 
  Vector<int> vt; 
  cout << vt.size() <<" " << vt.capacity() << endl;
  vt.push_back(0); 
  vt.push_back(10); 
  vt.push_back(-100);
  int size = vt.size(); 
  cout << vt << endl;
  Vector<int> vt1(vt); 
  cout << vt1 << endl;
} 