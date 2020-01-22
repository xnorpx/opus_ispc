#include <gtest/gtest.h>
#include <ispc/sum.h>

#include <algorithm>
#include <numeric>
#include <random>

TEST(ISPC, Sum) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_real<float> dist{1, 52};

    auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

    std::vector<float> v(1024);
    std::generate(v.begin(), v.end(), gen);
    EXPECT_NEAR(std::accumulate(v.begin(), v.end(), 0.f),
                ispc::sum(v.data(), static_cast<int>(v.size())), 0.1);
}
