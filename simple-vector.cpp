// simple-vector.cpp
// NOTE: SimpleVector1 and SimpleVector2 don't handle all the edge cases,
// so use SimpleVector for reference.

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ostream>
#include <utility>

template <typename ElemTy>
struct SimpleVector1 {

private:
  ElemTy* m_buffer;
  size_t m_size;

public:
  SimpleVector1() : m_buffer(nullptr), m_size(0) {}

  SimpleVector1(const SimpleVector1& other) : m_size(other.m_size) {
    m_buffer = new ElemTy[m_size];
    for (size_t i = 0; i < m_size; i++) {
      m_buffer[i] = other.m_buffer[i];
    }
  }

  SimpleVector1(std::initializer_list<ElemTy> xs) : m_size(xs.size()) {
    m_buffer = new ElemTy[m_size];
    for (size_t i = 0; i < xs.size(); i++) {
      m_buffer[i] = std::data(xs)[i];
    }
  }

  ~SimpleVector1() {
    delete[] m_buffer;
  }

  // note: broken
  SimpleVector1& operator=(const SimpleVector1& other) {
    delete[] m_buffer;

    m_buffer = new ElemTy[other.m_size];
    m_size = other.m_size;
    for (size_t i = 0; i < m_size; i++) {
      m_buffer[i] = other.m_buffer[i];
    }
    return *this;
  }

  void push_back(const ElemTy& element) {
    ElemTy* new_buffer = new ElemTy[m_size + 1];
    for (size_t i = 0; i < m_size; i++) {
      new_buffer[i] = m_buffer[i];
    }
    new_buffer[m_size] = element;
    m_size += 1;

    delete[] m_buffer;
    m_buffer = new_buffer;
  }

  ElemTy pop_back() {
    assert(m_size > 0);
    ElemTy* new_buffer = new ElemTy[m_size - 1];
    for (size_t i = 0; i < m_size - 1; i++) {
      new_buffer[i] = m_buffer[i];
    }

    ElemTy last_elem = m_buffer[m_size - 1];
    m_size -= 1;

    delete[] m_buffer;
    m_buffer = new_buffer;

    return last_elem;
  }

  size_t size() const {
    return m_size;
  }

  ElemTy& operator[](size_t idx) {
    return m_buffer[idx];
  }

  const ElemTy& operator[](size_t idx) const {
    return m_buffer[idx];
  }

  void swap(SimpleVector1& other) {
    std::swap(m_buffer, other.m_buffer);
    std::swap(m_size, other.m_size);
  }

  friend void swap(SimpleVector1& a, SimpleVector1& b) {
    a.swap(b);
  }

  friend std::ostream& operator<<(std::ostream& os, SimpleVector1& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); i++) {
      if (i > 0) {
        os << ", ";
      }
      os << vec[i];
    }
    os << "]";
    return os;
  }

  // the stuff below lets us conform to the iterator API, which will
  // be covered a little later in this lecture.
  using value_type = ElemTy;
  using reference = ElemTy&;
  using const_reference = const ElemTy&;
  using iterator = ElemTy*;
  using const_iterator = const ElemTy*;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  iterator begin() {
    return m_buffer;
  }

  iterator end() {
    return m_buffer + m_size;
  }

  const_iterator begin() const {
    return m_buffer;
  }
  const_iterator end() const {
    return m_buffer + m_size;
  }

  const_iterator cbegin() const {
    return m_buffer;
  }

  const_iterator cend() const {
    return m_buffer + m_size;
  }
};

// --------------------------------------------------------
template <typename ElemTy>
struct SimpleVector2 {
private:
  ElemTy* m_buffer;
  size_t m_size;

public:
  SimpleVector2() : m_buffer(nullptr), m_size(0) {}
  SimpleVector2(const SimpleVector2& other) : m_size(other.m_size) {
    m_buffer = new ElemTy[m_size];
    for (size_t i = 0; i < m_size; i++) {
      m_buffer[i] = other.m_buffer[i];
    }
  }
  SimpleVector2(std::initializer_list<ElemTy> init_list)
      : m_size(init_list.size()) {
    m_buffer = new ElemTy[m_size];
    for (size_t i = 0; i < init_list.size(); i++) {
      m_buffer[i] = std::data(init_list)[i];
    }
  }
  ~SimpleVector2() {
    delete[] m_buffer;
  }

  // note: (still) broken
  SimpleVector2& operator=(const SimpleVector2& other) {
    if (this == &other) {
      return *this;
    }

    delete[] m_buffer;
    m_buffer = new ElemTy[other.m_size];
    m_size = other.m_size;
    for (size_t i = 0; i < m_size; i++) {
      m_buffer[i] = other.m_buffer[i];
    }
    return *this;
  }

  void push_back(const ElemTy& element) {
    ElemTy* new_buffer = new ElemTy[m_size + 1];
    for (size_t i = 0; i < m_size; i++) {
      new_buffer[i] = m_buffer[i];
    }
    new_buffer[m_size] = element;
    m_size += 1;

    delete[] m_buffer;
    m_buffer = new_buffer;
  }
  ElemTy pop_back() {
    assert(m_size > 0);
    ElemTy* new_buffer = new ElemTy[m_size - 1];
    for (size_t i = 0; i < m_size - 1; i++) {
      new_buffer[i] = m_buffer[i];
    }

    ElemTy last_elem = m_buffer[m_size - 1];
    m_size -= 1;

    delete[] m_buffer;
    m_buffer = new_buffer;

    return last_elem;
  }
  size_t size() const {
    return m_size;
  }
  ElemTy& operator[](size_t idx) {
    return m_buffer[idx];
  }
  const ElemTy& operator[](size_t idx) const {
    return m_buffer[idx];
  }
  void swap(SimpleVector2& other) {
    std::swap(m_buffer, other.m_buffer);
    std::swap(m_size, other.m_size);
  }
  friend void swap(SimpleVector2& a, SimpleVector2& b) {
    a.swap(b);
  }
  friend std::ostream& operator<<(std::ostream& os, SimpleVector2& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); i++) {
      if (i > 0) {
        os << ", ";
      }
      os << vec[i];
    }
    os << "]";
    return os;
  }
  using value_type = ElemTy;
  using reference = ElemTy&;
  using const_reference = const ElemTy&;
  using iterator = ElemTy*;
  using const_iterator = const ElemTy*;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  iterator begin() {
    return m_buffer;
  }
  iterator end() {
    return m_buffer + m_size;
  }
  const_iterator begin() const {
    return m_buffer;
  }
  const_iterator end() const {
    return m_buffer + m_size;
  }
  const_iterator cbegin() const {
    return m_buffer;
  }
  const_iterator cend() const {
    return m_buffer + m_size;
  }
};

// --------------------------------------------------------
template <typename ElemTy>
struct SimpleVector {
private:
  ElemTy* m_buffer;
  size_t m_size;

public:
  SimpleVector() : m_buffer(nullptr), m_size(0) {}
  SimpleVector(const SimpleVector& other) : m_size(other.m_size) {
    m_buffer = new ElemTy[m_size];
    for (size_t i = 0; i < m_size; i++) {
      m_buffer[i] = other.m_buffer[i];
    }
  }
  SimpleVector(std::initializer_list<ElemTy> init_list)
      : m_size(init_list.size()) {
    m_buffer = new ElemTy[m_size];
    for (size_t i = 0; i < init_list.size(); i++) {
      m_buffer[i] = std::data(init_list)[i];
    }
  }
  ~SimpleVector() {
    delete[] m_buffer;
  }

  // note: correct (finally)
  SimpleVector& operator=(const SimpleVector& other) {
    SimpleVector copy{other};
    this->swap(copy);

    return *this;
  }

#if 0
  SimpleVector& operator=(SimpleVector copy) {
    this->swap(copy);
    return *this;
  }
#endif

  void push_back(const ElemTy& element) {
    ElemTy* new_buffer = new ElemTy[m_size + 1];
    for (size_t i = 0; i < m_size; i++) {
      new_buffer[i] = m_buffer[i];
    }
    new_buffer[m_size] = element;
    m_size += 1;

    delete[] m_buffer;
    m_buffer = new_buffer;
  }
  ElemTy pop_back() {
    assert(m_size > 0);
    ElemTy* new_buffer = new ElemTy[m_size - 1];
    for (size_t i = 0; i < m_size - 1; i++) {
      new_buffer[i] = m_buffer[i];
    }

    ElemTy last_elem = m_buffer[m_size - 1];
    m_size -= 1;

    delete[] m_buffer;
    m_buffer = new_buffer;

    return last_elem;
  }
  size_t size() const {
    return m_size;
  }
  ElemTy& operator[](size_t idx) {
    return m_buffer[idx];
  }
  const ElemTy& operator[](size_t idx) const {
    return m_buffer[idx];
  }
  void swap(SimpleVector& other) {
    std::swap(m_buffer, other.m_buffer);
    std::swap(m_size, other.m_size);
  }
  friend void swap(SimpleVector& a, SimpleVector& b) {
    a.swap(b);
  }
  friend std::ostream& operator<<(std::ostream& os, SimpleVector& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); i++) {
      if (i > 0) {
        os << ", ";
      }
      os << vec[i];
    }
    os << "]";
    return os;
  }
  using value_type = ElemTy;
  using reference = ElemTy&;
  using const_reference = const ElemTy&;
  using iterator = ElemTy*;
  using const_iterator = const ElemTy*;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  iterator begin() {
    return m_buffer;
  }
  iterator end() {
    return m_buffer + m_size;
  }
  const_iterator begin() const {
    return m_buffer;
  }
  const_iterator end() const {
    return m_buffer + m_size;
  }
  const_iterator cbegin() const {
    return m_buffer;
  }
  const_iterator cend() const {
    return m_buffer + m_size;
  }
};

int main() {
  SimpleVector1<int> sv1{1, 2, 3, 4};
  sv1 = sv1;
  std::cout << "sv1[0] = " << sv1[0] << "\n";
  std::cout << "sv1[1] = " << sv1[1] << "\n";
  std::cout << "sv1[2] = " << sv1[2] << "\n";
  std::cout << "sv1[3] = " << sv1[3] << "\n";

  SimpleVector2<int> sv2{1, 2, 3, 4};
  // sv2 = sv2;
  // std::cout << "sv2[0] = " << sv2[0] << "\n";

  // struct Nest {
  //   SimpleVector2<Nest> nests;
  // };

  // Nest nested{{Nest{{Nest{}, Nest{}}}}};

  // nested = nested.nests[0];
}