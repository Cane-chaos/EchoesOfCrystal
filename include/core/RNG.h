#pragma once
#include <random>

class RNG {
public:
    RNG();
    explicit RNG(unsigned int seed);
    
    // Dice rolling
    int rollD6();
    int rollRange(int min, int max);
    
    // Coin flipping
    bool flipCoin();
    
    // Float generation
    float randomFloat(float min = 0.0f, float max = 1.0f);
    
    // Percentage checks
    bool checkPercentage(float percentage);
    
    // Seed management
    void setSeed(unsigned int seed);
    unsigned int getSeed() const;
    
    // Utility
    template<typename Container>
    auto& randomElement(Container& container);
    
private:
    unsigned int m_seed;        // Đặt seed TRƯỚC
    std::mt19937 m_generator;
};

template<typename Container>
auto& RNG::randomElement(Container& container) {
    std::uniform_int_distribution<size_t> dist(0, container.size() - 1);
    auto it = container.begin();
    std::advance(it, dist(m_generator));
    return *it;
}
