#include <algorithm>
#include <cmath>
#include <iostream>
#include <ranges>
#include <vector>
#include <random>
#include <iterator>
#include <unordered_set>
#include <bit>


#include <fxd/fxd.hpp>


using std::cout;
using std::clog;
using std::endl;

using F = fxd::fixed<24, 8>;
//using F = double;



struct Pt {
    F x, y;

    constexpr bool operator ==(const Pt&) const noexcept = default;
};


namespace std {

    template<>
    struct hash<Pt> {
        size_t
        operator ()(const Pt& p)
            const noexcept
        {
            using H = hash<F>;
            return rotl(H{}(p.y), 3) ^ H{}(p.x);
        }
    };

};


F global_min;


void
check(const Pt& a, const Pt& b)
{
#if 0
    auto dx = a.x - b.x;
    auto dy = a.y - b.y;
    auto d2 = dx * dx + dy * dy;
#else
    using exc = fxd::safe::except;
    auto dx = exc::minus(a.x, b.x);
    auto dy = exc::minus(a.y, b.y);
    auto dx2 = exc::multiplies(dx, dx);
    auto dy2 = exc::multiplies(dy, dy);
    auto d2 = exc::plus(dx2, dy2);
#endif
    global_min = std::min(global_min, d2);
}


template<std::ranges::random_access_range R>
void
closest_pair(R&& points)
{
    using std::ranges::subrange;

    const auto n = size(points);
    if (n < 2)
        return;
    const auto n2 = n / 2;

    auto mid = next(begin(points), n2);

    auto center = mid->x;

    closest_pair(subrange{begin(points), mid});
    closest_pair(subrange{mid, end(points)});

    std::inplace_merge(begin(points),
                       begin(points) + n2,
                       end(points),
                       [](const Pt& a, const Pt& b) { return a.y < b.y; });

    Pt p1, p2, p3, p4;
    p1 = p2 = p3 = p4 = *begin(points);
    for (const auto& p : points | std::views::drop(1)) {
#if 0
        F d = p.x - center;
        d = d * d;
#else
        F d = fxd::safe::except::minus(p.x, center);
        d = fxd::safe::except::multiplies(d, d);
#endif
        if (d < global_min) {
            check(p, p1);
            check(p, p2);
            check(p, p3);
            check(p, p4);
            p1 = p2;
            p2 = p3;
            p3 = p4;
            p4 = p;
        }
    }
}



F
solve_rec(const std::vector<Pt>& points)
{
    clog << "solving in n log n" << endl;
    global_min = std::numeric_limits<F>::max();

    auto mpoints = points;

    std::ranges::sort(mpoints, [](const Pt& a, const Pt& b) { return a.x < b.x; });
    closest_pair(mpoints);
    clog << "done" << endl;
    return global_min;
}


F
solve_slow(const std::vector<Pt>& points)
{
    clog << "solving in n^2" << endl;
    global_min = std::numeric_limits<F>::max();

    for (size_t i = 0; i + 1 < points.size(); ++i)
        for (size_t j = i + 1; j < points.size(); ++j)
            check(points[i], points[j]);
    clog << "done" << endl;
    return global_min;
}


int main()
{
    std::random_device dev;
    std::mt19937 eng{dev()};
    fxd::uniform_real_distribution<F> dist{-100, 100};


    std::vector<Pt> points;
    std::unordered_set<Pt> filter;

    for (int i = 0; i < 100; ++i) {
        Pt p{dist(eng), dist(eng)};
        if (filter.contains(p))
            continue;
        filter.insert(p);
        points.push_back(p);
    }
    filter.clear();

    cout << "generated " << points.size() << " points" << endl;

    auto a = solve_rec(points);
    auto b = solve_slow(points);

    cout << "recursive solution: " << a << endl;
    cout << "slow solution:      " << b << endl;
    if (a != b) {
        cout << "inconsistency found!" << endl;
        return -1;
    }


}
