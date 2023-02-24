#ifndef MAKE_RANDOM_VEC_HPP
#define MAKE_RANDOM_VEC_HPP

#include <random>
#include <vector>


template<typename T>
std::vector<T>
make_random_vec(std::size_t size,
                T min,
                T max)
{
    std::random_device dev;
    std::seed_seq seq{dev(), dev(), dev(), dev()};
    std::mt19937 engine{seq};
    std::uniform_real_distribution<T> dist{min, max};

    std::vector<T> result(size);

    for (auto& x : result)
        x = dist(engine);

    return result;
}


#endif
