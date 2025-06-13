#include "predator.hpp"
#include "ocean.hpp"
#include "herbivore.hpp"
#include <iostream>
#include <vector>
#include <utility>

PredatorFish::PredatorFish()
    : energy_(Config::PREDATOR_INITIAL_ENERGY), age_(0) {
}

bool PredatorFish::isDead() const {
    if (energy_ <= 0) {
        return true;
    }
    if (age_ >= Config::PREDATOR_MAX_AGE) {
        return true;
    }
    return false;
}

void PredatorFish::update(Ocean& ocean, int r, int c) {
    age_++;
    energy_ -= Config::PREDATOR_ENERGY_PER_TICK;


    if (isDead()) {
        return;
    }

    if (eat(ocean, r, c)) {
        return;
    }

    move(ocean, r, c);
}

bool PredatorFish::eat(Ocean& ocean, int current_r, int current_c) {
    std::vector<std::pair<int, int>> herbivoreNeighbors = ocean.getAdjacentCellsOfType(current_r, current_c, EntityType::HERBIVORE);

    if (!herbivoreNeighbors.empty()) {
        Random::shuffle(herbivoreNeighbors);
        std::pair<int, int> herbivorePos = herbivoreNeighbors[0];

        std::unique_ptr<Entity> eatenHerbivore = ocean.removeEntity(herbivorePos.first, herbivorePos.second);
        if (eatenHerbivore && eatenHerbivore->getType() == EntityType::HERBIVORE) {
            energy_ += Config::PREDATOR_ENERGY_FROM_HERBIVORE;
            if (energy_ > Config::PREDATOR_MAX_ENERGY) {
                energy_ = Config::PREDATOR_MAX_ENERGY;
            }

            ocean.moveEntity(current_r, current_c, herbivorePos.first, herbivorePos.second);
            return true;
        } else if (eatenHerbivore) {
        }
    }
    return false;
}

void PredatorFish::move(Ocean& ocean, int current_r, int current_c) {
    std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(current_r, current_c);

    if (!emptyNeighbors.empty()) {
        Random::shuffle(emptyNeighbors);
        std::pair<int, int> targetCell = emptyNeighbors[0];
        
        ocean.moveEntity(current_r, current_c, targetCell.first, targetCell.second);
    } else {
    }
}

char PredatorFish::getSymbol() const {
    if (energy_ < Config::PREDATOR_INITIAL_ENERGY / 2) return 'p';
    return 'P';
}

EntityType PredatorFish::getType() const {
    return EntityType::PREDATOR;
}