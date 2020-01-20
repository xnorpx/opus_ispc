#include <benchmark/benchmark.h>
#include <sum.h>

#include <numeric>
#include <random>
#include <vector>

using namespace std;

// constexpr size_t kN = 1048576;
constexpr size_t kN = 384;

static void BM_SUM_ACCUMULATE(benchmark::State& state) {
    mt19937 mersenne_engine{1337};
    uniform_real<float> dist{1, 52};

    auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

    std::vector<float> v(kN);
    std::generate(v.begin(), v.end(), gen);

    float sum = 0.f;
    for (auto _ : state) {
        sum = std::accumulate(v.begin(), v.end(), 0.f);
        benchmark::DoNotOptimize(sum);
    }
}

static void BM_SUM(benchmark::State& state) {
    mt19937 mersenne_engine{1337};
    uniform_real<float> dist{1, 52};

    auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

    std::vector<float> v(kN);
    std::generate(v.begin(), v.end(), gen);

    float sum = 0.f;
    for (auto _ : state) {
        for (const auto& val : v) {
            sum += val;
        }
        benchmark::DoNotOptimize(sum);
    }
}

static void BM_SUM_ISPC_AVX2(benchmark::State& state) {
    mt19937 mersenne_engine{1337};
    uniform_real<float> dist{1, 52};

    auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

    std::vector<float> v(kN);
    std::generate(v.begin(), v.end(), gen);

    float sum = 0.f;
    for (auto _ : state) {
        sum = ispc::sum(v.data(), static_cast<int>(v.size()));
        benchmark::DoNotOptimize(sum);
    }
}

BENCHMARK(BM_SUM_ACCUMULATE);
BENCHMARK(BM_SUM);
BENCHMARK(BM_SUM_ISPC_AVX2);

BENCHMARK_MAIN();