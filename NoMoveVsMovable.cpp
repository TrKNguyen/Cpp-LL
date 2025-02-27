#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono; // Fix: Bring std::chrono into the scope with alias
// How to create a non-movable type:
// The implicitly declared move constructor is deleted if the class contains non-move-constructible types, or if the copy constructor is explicitly declared (whether defaulted or not)
struct NoMove {
    NoMove() {
        xs.resize(100);
    }
    NoMove(const NoMove&) = default;
    NoMove& operator=(const NoMove&) = default;
    std::vector<int> xs;
};

struct Movable {
    Movable() {
        xs.resize(100);
    }
    std::vector<int> xs;
};

int main() {
    using ms = std::chrono::milliseconds; // Fix: Define 'ms' as milliseconds

    auto start = system_clock::now(); // Fix: Use std::chrono::system_clock::now()
    std::vector<NoMove> cant_move_me{};
    for (size_t i = 0; i < 50000; i++) {
        cant_move_me.push_back(NoMove{});
    }
    std::cout
        << "elapsed (not-movable) = "
        << duration_cast<ms>(system_clock::now() - start).count()
        << " ms\n";

    start = system_clock::now();
    std::vector<Movable> can_move_me{};

    for (size_t i = 0; i < 50000; i++) {
        can_move_me.push_back(Movable{});
    }
    std::cout
        << "elapsed (movable) = "
        << duration_cast<ms>(system_clock::now() - start).count()
        << " ms\n";
}
/* 
elapsed (not-movable) = 35 ms
elapsed (movable) = 16 ms
*/