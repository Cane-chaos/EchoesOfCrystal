#include "core/RNG.h"
#include <chrono>

RNG::RNG() 
    : m_seed(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()))
    , m_generator(m_seed)
{
}

RNG::RNG(unsigned int seed)
    : m_seed(seed)
    , m_generator(seed)
{
}

int RNG::rollD6() {
    std::uniform_int_distribution<int> dist(1, 6);
    return dist(m_generator);
}

int RNG::rollRange(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(m_generator);
}

bool RNG::flipCoin() {
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(m_generator) == 1;
}

float RNG::randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_generator);
}

bool RNG::checkPercentage(float percentage) {
    return randomFloat(0.0f, 100.0f) < percentage;
}

void RNG::setSeed(unsigned int seed) {
    m_seed = seed;
    m_generator.seed(seed);
}

unsigned int RNG::getSeed() const {
    return m_seed;
}
