//
// Created by andyroiiid on 11/24/2022.
//

#pragma once

#include <cstdint>
#include <iterator>

class Random {
public:
    explicit Random(uint64_t seed = 42) : m_state(seed) {}

    // [0..n)
    int Int(int n) {
        return static_cast<int>(Next() % n);
    }

    // [a..b)
    int Int(int a, int b) {
        return a + Int(b - a);
    }

    // [a..b]
    int IntInclusive(int a, int b) {
        return Int(a, b + 1);
    }

    // [0..1)
    double Double() {
        return AsDouble(Next());
    }

    // [a..b)
    double Double(double a, double b) {
        return a + (b - a) * Double();
    }

    // [0..1)
    float Float() {
        return static_cast<float>(Double());
    }

    // [a..b)
    float Float(float a, float b) {
        return static_cast<float>(Double(a, b));
    }

    template<class RandomIt>
    void Shuffle(RandomIt first, RandomIt last) {
        using Diff = typename std::iterator_traits<RandomIt>::difference_type;
        for (Diff i = last - first - 1; i > 0; i--) {
            std::swap(first[i], first[Int(i + 1)]);
        }
    }

    template<class Element, std::size_t Size>
    void Shuffle(Element (&a)[Size]) {
        Shuffle(std::begin(a), std::end(a));
    }

    template<class Container>
    void Shuffle(Container &container) {
        Shuffle(container.begin(), container.end());
    }

private:
    // XorShift64
    uint64_t Next() {
        uint64_t &x = m_state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        return x;
    }

    static double AsDouble(uint64_t i) {
        constexpr uint64_t MASK1 = 0x3FF0000000000000ULL;
        constexpr uint64_t MASK2 = 0x3FFFFFFFFFFFFFFFULL;
        i = (i | MASK1) & MASK2;
        return *reinterpret_cast<double *>(&i) - 1;
    }

    uint64_t m_state;
};
