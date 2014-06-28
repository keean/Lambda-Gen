//=============================================================================
// Random lambda term generator

#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>

using namespace std;

//-----------------------------------------------------------------------------

struct xor_shift_plus_128 {
    xor_shift_plus_128(xor_shift_plus_128 const&) = delete;
    xor_shift_plus_128& operator= (xor_shift_plus_128 const&) = delete;
    xor_shift_plus_128() {
        uint64_t random_seed[2];
        std::ifstream random_fs;

        random_fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        random_fs.open("/dev/urandom", std::ifstream::binary);
        random_fs.read(reinterpret_cast<char*>(random_seed), sizeof(random_seed));
        random_fs.close();

        s0 = random_seed[0];
        s1 = random_seed[1];
    }
    explicit xor_shift_plus_128(uint64_t const seed[2])
        : s0(seed[0])
        , s1(seed[1])
        {}
    uint64_t operator() () {
        uint64_t l1 = s0;
        uint64_t const l0 = s1;
        s0 = l0;
        l1 ^= l1 << 23;
        return (s1 = (l1 ^ l0 ^ (l1 >> 17) ^ (l0 >> 26))) + l0;
    }
private:
    uint64_t s0;
    uint64_t s1;
};

template <typename Random>
double rand_01(Random&& r) {
    return static_cast<double>(r()) / numeric_limits<uint64_t>::max();
}

template <typename Random>
void unrank(Random&& r, int const i) {
    static double constexpr p_var = 0.3703026;
    static double constexpr p_abs = 0.25939476;
    double const n = rand_01(r);

    if (i > 10000 || (i > 0 && n < p_var)) {
        cout << "t" << r() % i;
    } else if (n < p_var + p_abs) {
        cout << "(\\t" << i << " . ";
        unrank(r, i + 1);
        cout << ")";
    } else {
        cout << "(";
        unrank(r, i);
        cout << " ";
        unrank(r, i);
        cout << ")";
    }
}

int main() {
    xor_shift_plus_128 xsp128;

    unrank(xsp128, 0);
    cout << "\n";
}
