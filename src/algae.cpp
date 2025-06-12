#include "algae.hpp"
#include <iostream> 

Algae::Algae() {}

void Algae::update(int r, int c) {}

char Algae::getSymbol() const {
    return 'A';
}

EntityType Algae::getType() const {
    return EntityType::ALGAE;
}
