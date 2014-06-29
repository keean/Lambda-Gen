//=============================================================================
// Random lambda term generator

#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>

using namespace std;

//-----------------------------------------------------------------------------

class xor_shift_plus_128 {
    uint64_t s0;
    uint64_t s1;

public:
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
};

template <typename Random>
double rand_01(Random& r) {
    return static_cast<double>(r()) / numeric_limits<uint64_t>::max();
}

template <typename Random>
class lambdagen {
    static double constexpr p_var = 0.3703026;
    static double constexpr p_abs = 0.25939476;

    Random& prng;
    string abs1;
    string abs2;

    void unrank(int const i) {
        double const n = rand_01(prng);

        if (i > 10000 || (i > 0 && n < p_var)) {
            cout << "t" << prng() % i;
        } else if (n < p_var + p_abs) {
            cout << "(" << abs1 << i << abs2;
            unrank(i + 1);
            cout << ")";
        } else {
            cout << "(";
            unrank(i);
            cout << " ";
            unrank(i);
            cout << ")";
        }
    }

public:
    template <typename Str1, typename Str2>
    lambdagen(Random& r, Str1&& a1, Str2&& a2)
    : prng(r)
    , abs1(forward<Str1>(a1))
    , abs2(forward<Str2>(a2))
    {}

    void operator() () {
        unrank(0);
    }
};

template <typename Random, typename Str1, typename Str2>
lambdagen<Random> make_lambdagen(Random& r, Str1&& a1, Str2&& a2) {
    return lambdagen<Random>(r, forward<Str1>(a1), forward<Str2>(a2));
}

int main() {
    xor_shift_plus_128 xsp128;
    //auto gen = make_lambdagen(xsp128, "\\t", " . ");
    auto gen = make_lambdagen(xsp128, "fun t", " -> ");

    cout << "let t = ";
    gen();
    cout << ";;\n";
}
