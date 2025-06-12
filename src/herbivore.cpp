#include "herbivore.hpp"
#include <iostream>

HerbivoreFish::HerbivoreFish() {}

void HerbivoreFish::update(int r, int c) {}

char HerbivoreFish::getSymbol() const {
    return 'H';
}

EntityType HerbivoreFish::getType() const {
    return EntityType::HERBIVORE;
}
