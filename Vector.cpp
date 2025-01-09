#include <bits/stdc++.h>
using namespace std; 

/// some note: we can replace new by allocator, then call allocate, construct for new and 
/// Very important: Value categories are given to expressions and not types!
template<typename T> 
class Vector { 
private: 
  T* m_value; 
  int m_size; 
  int m_cap; 

public:
  // constructor 
  Vector() {
    m_value = new T[1]; 
    m_size = 0;
    m_cap = 1;
  }
  // constructor with size 
  Vector(int size) {
    cout <<"size constructor" <<" check" << endl; 
    m_value = new T[size]; 
    m_size = size;
    m_cap = size;
  }
  // constructor with std::initializer_list
  // Vector(std::initializer_list<T> init_list) {
  //   cout <<"initialize list" << " check" << endl; 
  //   m_size = init_list.size(); 
  //   m_cap = init_list.size();
  //   m_value = new T[m_size]; 
  //   for (int i = 0; i < m_size; i++) {
  //     m_value[i] = init_list.begin()[i];
  //   }
  // }
  // copy constructor 
  Vector(const Vector<T>& o) {
    cout <<"copy constructor" <<" check" << endl;
    this->m_size = o.m_size; 
    this->m_cap = o.m_cap;
    this->m_value = new T[this->m_cap];
    for (int i = 0; i < this->m_size; i++) {
      this->m_value[i] = o.m_value[i];
    }
  }
  // copy assignment 
  Vector<T>& operator=(const Vector<T>& o) {
    cout <<"copy assignment" << " check" << endl;
    Vector<T> temp = Vector(o);
    this->swap(temp);
    return *this;
  }
  // move constructor 
  Vector(Vector<T>&& o) {
    cout <<"move constructor" << " check" << endl;
    std::swap(this->m_size, o.m_size); 
    std::swap(this->m_cap, o.m_cap); 
    std::swap(this->m_value, o.m_value);
  }
  // move assignment 
  Vector<T>& operator=(Vector<T>&& o) {
    cout <<"move assignment" << " check" << endl;
    auto moved = Vector<T>(std::move(o));
    this->swap(moved); 
    return *this;
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
    for (int i = 0; i < this->m_size; i++) {
      temp[i] = this->m_value[i];
    }
    delete[] m_value;
    m_value = temp;
  }
  template<typename... Args> 
  void emplace_back(Args... args) {
    if (m_size == m_cap) {
      reserve(2 * m_cap);
    }
    m_value[m_size] = T(std::forward<Args>(args)...); 
    m_size++;
  }
  void push_back(T n_elem) {
    if (m_size == m_cap) {
      reserve(2 * m_cap);
    }
    m_value[m_size] = std::move(n_elem); 
    m_size++;
  }
  void pop_back() {
    if (m_size == 0) {
      throw out_of_range("Cannot popback the empty vector");
    }
    m_size--;
  }
  T& operator[](int index) {
    if (index < 0 || index >= m_size) {
      throw out_of_range("Index out of bound");
    }
    return m_value[index];
  }
  const T& operator[](int index) const {
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
  friend void swap(vector<T>& a, vector<T>& b) {
    // std::swap(a.m_value, b.m_value); 
    // std::swap(a.m_size, b.m_size); 
    // std::swap(a.m_cap, b.m_cap);
    a.swap(b);
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
  // Vector<int> vt; 
  // cout << vt.size() <<" " << vt.capacity() << endl;
  // vt.push_back(0); 
  // vt.push_back(10); 
  // vt.push_back(-100);
  // int size = vt.size(); 
  // cout << vt << endl;
  // Vector<int> vt1(vt); 
  // cout << vt1 << endl;
  // Vector<int> a{1, 2, 3, 4};
  Vector<int> b(3);
  Vector<int> vt(std::move(b));

} 