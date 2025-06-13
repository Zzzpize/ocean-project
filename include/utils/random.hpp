#pragma once
#include <random>
#include <chrono> 
#include <vector> 
#include <algorithm> 

class Random {
public:
    static std::mt19937& getGenerator() {
        static std::mt19937 generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
        return generator;
    }

    static int getInt(int min, int max) {
        if (min > max) {
            std::swap(min, max);
        }
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(getGenerator());
    }

    static double getDouble(double min, double max) {
        if (min > max) {
            std::swap(min, max);
        }
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(getGenerator());
    }
    
    template<typename T>
    static void shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), getGenerator());
    }
};