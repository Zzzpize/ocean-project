#include "predator.hpp"
#include "ocean.hpp"
#include "herbivore.hpp" 
#include "utils/logger.hpp"
#include <iostream>
#include <vector>
#include <utility>

PredatorFish::PredatorFish(int initial_energy)
    : energy_(initial_energy), age_(0) {
    if (energy_ > Config::PREDATOR_MAX_ENERGY) {
        energy_ = Config::PREDATOR_MAX_ENERGY;
    }
    Logger::debug("PredatorFish created. Energy: ", energy_);
}

bool PredatorFish::isDead() const {
    if (energy_ <= 0) {
        Logger::info("Predator died from hunger. Age: ", age_, ", Energy: ", energy_);
        return true;
    }
    if (age_ >= Config::PREDATOR_MAX_AGE) {
        Logger::info("Predator died from old age. Age: ", age_, ", Energy: ", energy_);
        return true;
    }
    return false;
}

void PredatorFish::reproduce(Ocean& ocean, int current_r, int current_c) {
    if (energy_ >= Config::PREDATOR_REPRODUCTION_ENERGY_THRESHOLD &&
        Random::getInt(1, 100) <= Config::PREDATOR_REPRODUCTION_CHANCE_PERCENT) {
        
        std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(current_r, current_c);
        if (!emptyNeighbors.empty()) {
            Random::shuffle(emptyNeighbors);
            std::pair<int, int> offspringPos = emptyNeighbors[0];

            auto offspring = std::make_unique<PredatorFish>(Config::PREDATOR_OFFSPRING_INITIAL_ENERGY);
            
            if (ocean.addEntity(std::move(offspring), offspringPos.first, offspringPos.second)) {
                energy_ -= Config::PREDATOR_REPRODUCTION_COST;
                Logger::info("Predator at (", current_r, ",", current_c, ") reproduced to (", 
                           offspringPos.first, ",", offspringPos.second, "). Parent energy now: ", energy_);
            }
        } else {
            Logger::debug("Predator at (", current_r, ",", current_c, ") wanted to reproduce, but no empty space.");
        }
    }
}

void PredatorFish::update(Ocean& ocean, int r, int c) {
    Logger::debug("Predator at (", r, ",", c, ") updating. Age: ", age_ + 1, ", Energy: ", energy_ - Config::PREDATOR_ENERGY_PER_TICK);
    age_++;
    energy_ -= Config::PREDATOR_ENERGY_PER_TICK;

    if (isDead()) {
        return;
    }

    reproduce(ocean, r, c);

    if (isDead()) {
        return;
    }

    if (eat(ocean, r, c)) {
        return;
    }
    
    if (isDead()) {
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
            Logger::info("Predator at (", herbivorePos.first, ",", herbivorePos.second, 
                          ") ate a Herbivore. New energy: ", energy_);
            return true;
        } else if (eatenHerbivore) {
            Logger::warn("Predator at (", current_r, ",", current_c, ") tried to eat non-herbivore at (",
                         herbivorePos.first, ",", herbivorePos.second, ")! Type: ", static_cast<int>(eatenHerbivore->getType()));
            ocean.addEntity(std::move(eatenHerbivore), herbivorePos.first, herbivorePos.second); // Вернуть обратно
        }
    } else {
        Logger::debug("Predator at (", current_r, ",", current_c, ") found no Herbivores to eat.");
    }
    return false;
}

void PredatorFish::move(Ocean& ocean, int current_r, int current_c) {
    std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(current_r, current_c);

    if (!emptyNeighbors.empty()) {
        Random::shuffle(emptyNeighbors);
        std::pair<int, int> targetCell = emptyNeighbors[0];
        
        Logger::debug("Predator at (", current_r, ",", current_c, ") moving to (", targetCell.first, ",", targetCell.second,")");
        ocean.moveEntity(current_r, current_c, targetCell.first, targetCell.second);
    } else {
        Logger::debug("Predator at (", current_r, ",", current_c, ") has nowhere to move.");
    }
}

char PredatorFish::getSymbol() const {
    if (energy_ < Config::PREDATOR_INITIAL_ENERGY / 3) return 'p';
    return 'P';
}

EntityType PredatorFish::getType() const {
    return EntityType::PREDATOR;
}