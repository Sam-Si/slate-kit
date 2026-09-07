#pragma once
// Official-style CG map generation (Agade CSB-Runner-Arena / issue #2):
// pick one of 13 templates, ROTATE the ring (do not shuffle), jitter ±30.
#include <algorithm>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

inline const std::vector<std::vector<std::pair<int, int>>>& GetAgadeMapTemplates() {
    static const std::vector<std::vector<std::pair<int, int>>> k = {
        {{12460, 1350}, {10540, 5980}, {3580, 5180}, {13580, 7600}},
        {{3600, 5280}, {13840, 5080}, {10680, 2280}, {8700, 7460}, {7200, 2160}},
        {{4560, 2180}, {7350, 4940}, {3320, 7230}, {14580, 7700}, {10560, 5060}, {13100, 2320}},
        {{5010, 5260}, {11480, 6080}, {9100, 1840}},
        {{14660, 1410}, {3450, 7220}, {9420, 7240}, {5970, 4240}},
        {{3640, 4420}, {8000, 7900}, {13300, 5540}, {9560, 1400}},
        {{4100, 7420}, {13500, 2340}, {12940, 7220}, {5640, 2580}},
        {{14520, 7780}, {6320, 4290}, {7800, 860}, {7660, 5970}, {3140, 7540}, {9520, 4380}},
        {{10040, 5970}, {13920, 1940}, {8020, 3260}, {2670, 7020}},
        {{7500, 6940}, {6000, 5360}, {11300, 2820}},
        {{4060, 4660}, {13040, 1900}, {6560, 7840}, {7480, 1360}, {12700, 7100}},
        {{3020, 5190}, {6280, 7760}, {14100, 7760}, {13880, 1220}, {10240, 4920}, {6100, 2200}},
        {{10323, 3366}, {11203, 5425}, {7259, 6656}, {5425, 2838}},
    };
    return k;
}

inline std::vector<std::pair<int, int>> GenerateAgadeMap(uint32_t seed, int instance) {
    std::mt19937 rng(seed + static_cast<uint32_t>(instance) * 10007u);
    const auto& tmpls = GetAgadeMapTemplates();
    std::uniform_int_distribution<int> pick(0, static_cast<int>(tmpls.size()) - 1);
    auto cps = tmpls[static_cast<size_t>(pick(rng))];
    std::uniform_int_distribution<int> rot(0, static_cast<int>(cps.size()) - 1);
    const int r = rot(rng);
    std::rotate(cps.begin(), cps.begin() + r, cps.end());
    std::uniform_int_distribution<int> jitter(-30, 30);
    for (auto& p : cps) {
        p.first += jitter(rng);
        p.second += jitter(rng);
    }
    return cps;
}
