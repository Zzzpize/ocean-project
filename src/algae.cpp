#include "algae.hpp"
#include "ocean.hpp"
#include "utils/random.hpp" 
#include <iostream> 
#include <vector>
#include <utility> 

Algae::Algae() {}

void Algae::update(Ocean& ocean, int r, int c) {
    constexpr int REPRODUCTION_CHANCE = 10;
    if (Random::getInt(1, 100) <= REPRODUCTION_CHANCE) {
        std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(r, c);
        
        if (!emptyNeighbors.empty()) {
            int choice = Random::getInt(0, static_cast<int>(emptyNeighbors.size() - 1)); 
            std::pair<int, int> targetCell = emptyNeighbors[choice];
            
            ocean.addEntity(std::make_unique<Algae>(), targetCell.first, targetCell.second);
        }
    }
}

char Algae::getSymbol() const {
    return 'A';
}

EntityType Algae::getType() const {
    return EntityType::ALGAE;
}
