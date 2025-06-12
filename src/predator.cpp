#include "predator.hpp"
#include <iostream>

PredatorFish::PredatorFish() {}

void PredatorFish::update(int r, int c) {}

char PredatorFish::getSymbol() const {
    return 'P';
}

EntityType PredatorFish::getType() const {
    return EntityType::PREDATOR;
}
