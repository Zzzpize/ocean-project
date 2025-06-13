#include "herbivore.hpp"
#include "ocean.hpp"
#include "algae.hpp" 
#include <iostream>
#include <vector>
#include <utility>

HerbivoreFish::HerbivoreFish() 
    : energy_(Config::HERBIVORE_INITIAL_ENERGY), age_(0) {
}

bool HerbivoreFish::isDead() const {
    return energy_ <= 0 || age_ >= Config::HERBIVORE_MAX_AGE;
}

void HerbivoreFish::update(Ocean& ocean, int r, int c) {
    age_++;
    energy_ -= Config::HERBIVORE_ENERGY_PER_TICK;



    if (isDead()) {
        return; 
    }

    if (eat(ocean, r, c)) {
        return;
    }

    move(ocean, r, c);
}


bool HerbivoreFish::eat(Ocean& ocean, int current_r, int current_c) {
    std::vector<std::pair<int, int>> algaeNeighbors = ocean.getAdjacentCellsOfType(current_r, current_c, EntityType::ALGAE);

    if (!algaeNeighbors.empty()) {
        Random::shuffle(algaeNeighbors); 
        std::pair<int, int> algaePos = algaeNeighbors[0];
        std::unique_ptr<Entity> eatenAlgae = ocean.removeEntity(algaePos.first, algaePos.second);
        if (eatenAlgae) {
            energy_ += Config::HERBIVORE_ENERGY_FROM_ALGAE;
            if (energy_ > Config::HERBIVORE_MAX_ENERGY) {
                energy_ = Config::HERBIVORE_MAX_ENERGY;
            }
            ocean.moveEntity(current_r, current_c, algaePos.first, algaePos.second);
            return true; 
        }
    }
    return false; 
}

void HerbivoreFish::move(Ocean& ocean, int current_r, int current_c) {
    std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(current_r, current_c);

    if (!emptyNeighbors.empty()) {
        Random::shuffle(emptyNeighbors);
        std::pair<int, int> targetCell = emptyNeighbors[0];
        
        ocean.moveEntity(current_r, current_c, targetCell.first, targetCell.second);
    } else {
    }
}


char HerbivoreFish::getSymbol() const {
    if (energy_ < Config::HERBIVORE_INITIAL_ENERGY / 2) return 'h'; 
    return 'H';
}

EntityType HerbivoreFish::getType() const {
    return EntityType::HERBIVORE;
}