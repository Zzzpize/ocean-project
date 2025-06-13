#include "algae.hpp"
#include "ocean.hpp"
#include "utils/random.hpp"
#include "utils/logger.hpp" 
#include <vector>
#include <utility> 

Algae::Algae() {}

void Algae::update(Ocean& ocean, int r, int c) {
    constexpr int REPRODUCTION_CHANCE_PERCENT = 10; 
    if (Random::getInt(1, 100) <= REPRODUCTION_CHANCE_PERCENT) {
        std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(r, c);
        
        if (!emptyNeighbors.empty()) {
            Random::shuffle(emptyNeighbors);
            std::pair<int, int> targetCell = emptyNeighbors[0];
            
            if (ocean.addEntity(std::make_unique<Algae>(), targetCell.first, targetCell.second)) {
                Logger::debug("Algae at (", r, ",", c, ") reproduced to (", targetCell.first, ",", targetCell.second, ")");
            }
        }
    }
}

char Algae::getSymbol() const {
    return 'A';
}

EntityType Algae::getType() const {
    return EntityType::ALGAE;
}