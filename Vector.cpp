// vectors.cpp

#include <iostream>
#include <vector>

// just skip this thing, it's not that important
template <typename Container>
struct Printer {
  char Open;
  char Close;
  char Separator;
  const Container& container;

  friend std::ostream& operator<<(std::ostream& os, const Printer& p) {
    os << p.Open;
    for (auto it = p.container.begin(); it != p.container.end(); ++it) {
      if (it != p.container.begin()) {
        os << p.Separator << ' ';
      }
      os << *it;
    }
    os << p.Close;
    return os;
  }
};

template <typename Container>
Printer<Container> print(const Container& cont,
                         char open = '[',
                         char close = ']',
                         char sep = ',') {
  return Printer<Container>{open, close, sep, cont};
}

// start reading from here:
int main() {
  std::vector<int> xs{1, 2, 3};
  std::cout << "xs = " << print(xs) << "\n";

  std::cout << "xs.size() = "  //
            << xs.size() << "\n";
  std::cout << "x[0]      = "  //
            << xs[0] << "\n";
  std::cout << "x[1]      = "  //
            << xs[1] << "\n";

  xs.push_back(10);
  xs.push_back(20);

  std::cout << "xs.size() = "  //
            << xs.size() << "\n";
  xs.pop_back();
  std::cout << "xs = " << print(xs) << "\n";

  std::vector<int> ys{};
  ys = xs;
  std::cout << "ys = " << print(ys) << "\n";

#if 0
  std::cout << xs[99] << "\n";

  std::cout << xs.at(99) << "\n";
#endif

  {
    std::vector<int> default_constructed;
    std::vector<int> initialiser_list{1, 2, 3, 4};
    std::vector<int> copy_constructed{initialiser_list};

    std::cout << "default   = " << print(default_constructed) << "\n";
    std::cout << "init_list = " << print(initialiser_list) << "\n";
    std::cout << "copy_ctor = " << print(copy_constructed) << "\n";
  }

  {
    std::vector<int> a{1, 2, 3};
    std::vector<int> b{4, 5, 6};

    std::cout << "a = "    //
              << print(a)  //
              << "\n";
    std::cout << "b = "    //
              << print(b)  //
              << "\n";

    a.swap(b);
    std::cout << "a = "    //
              << print(a)  //
              << "\n";
    std::cout << "b = "    //
              << print(b)  //
              << "\n";

    b.swap(a);
    std::cout << "a = "    //
              << print(a)  //
              << "\n";
    std::cout << "b = "    //
              << print(b)  //
              << "\n";
  }
}