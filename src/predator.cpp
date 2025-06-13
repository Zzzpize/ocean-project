#include "predator.hpp"
#include "ocean.hpp"
#include <iostream>

PredatorFish::PredatorFish() {}

void PredatorFish::update(Ocean& ocean, int r, int c) {}

char PredatorFish::getSymbol() const {
    return 'P';
}

EntityType PredatorFish::getType() const {
    return EntityType::PREDATOR;
}
