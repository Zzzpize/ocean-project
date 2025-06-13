#include "predator.hpp"
#include "ocean.hpp"
#include "herbivore.hpp" 
#include "utils/logger.hpp"
#include <vector>
#include <utility>

PredatorFish::PredatorFish(int initial_energy)
    : energy_(initial_energy), age_(0) {
    if (energy_ > Config::PREDATOR_MAX_ENERGY) {
        energy_ = Config::PREDATOR_MAX_ENERGY;
    }
}

bool PredatorFish::isDead() const {
    return energy_ <= 0 || age_ >= Config::PREDATOR_MAX_AGE;
}

bool PredatorFish::tryToEat(Ocean& ocean, int current_r, int current_c) {
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
            Logger::info("P at (", herbivorePos.first, ",", herbivorePos.second, 
                          ") ate Herbivore. E:", energy_);
            return true;
        } else if (eatenHerbivore) { 
            Logger::warn("P @(",current_r,",",current_c,") tried to eat non-herbivore at (", herbivorePos.first, ",", herbivorePos.second, ")");
            ocean.addEntity(std::move(eatenHerbivore), herbivorePos.first, herbivorePos.second);
        }
    }
    return false;
}

bool PredatorFish::tryToReproduce(Ocean& ocean, int current_r, int current_c) {
    if (energy_ >= Config::PREDATOR_REPRODUCTION_ENERGY_THRESHOLD &&
        Random::getInt(1, 100) <= Config::PREDATOR_REPRODUCTION_CHANCE_PERCENT) {
        
        std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(current_r, current_c);
        if (!emptyNeighbors.empty()) {
            Random::shuffle(emptyNeighbors);
            std::pair<int, int> offspringPos = emptyNeighbors[0];

            auto offspring = std::make_unique<PredatorFish>(Config::PREDATOR_OFFSPRING_INITIAL_ENERGY);
            
            if (ocean.addEntity(std::move(offspring), offspringPos.first, offspringPos.second)) {
                energy_ -= Config::PREDATOR_REPRODUCTION_COST;
                Logger::info("P at (", current_r, ",", current_c, ") reproduced to (", 
                           offspringPos.first, ",", offspringPos.second, "). Parent E:", energy_);
                return true;
            }
        }
    }
    return false;
}

void PredatorFish::huntOrExplore(Ocean& ocean, int current_r, int current_c) {
    bool actively_hunting = (energy_ < Config::PREDATOR_CRITICAL_ENERGY_THRESHOLD * 1.5);

    if (actively_hunting) {
        std::pair<int, int> direction = ocean.getDirectionToNearestTarget(current_r, current_c, EntityType::HERBIVORE, Config::PREDATOR_SIGHT_RADIUS);

        if (direction.first != 0 || direction.second != 0) {
            int next_r = current_r + direction.first;
            int next_c = current_c + direction.second;

            if (ocean.isValidCoordinate(next_r, next_c)) {
                Entity* entity_at_target_step = ocean.getEntity(next_r, next_c);
                if (!entity_at_target_step || entity_at_target_step->getType() == EntityType::HERBIVORE) {
                    Logger::debug("P @(", current_r, ",", current_c, ") moving towards prey to (", next_r, ",", next_c, ")");
                    ocean.moveEntity(current_r, current_c, next_r, next_c);
                    return; 
                }
            }
        }
    }

    Logger::debug("P @(", current_r, ",", current_c, ") exploring randomly.");
    std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(current_r, current_c);
    if (!emptyNeighbors.empty()) {
        Random::shuffle(emptyNeighbors); 
        std::pair<int, int> targetCell = emptyNeighbors[0];
        Logger::debug("P @(", current_r, ",", current_c, ") random move to (", targetCell.first, ",", targetCell.second,")");
        ocean.moveEntity(current_r, current_c, targetCell.first, targetCell.second);
    } else {
        Logger::debug("P @(", current_r, ",", current_c, ") has nowhere to move (exploring).");
    }
}

void PredatorFish::update(Ocean& ocean, int r, int c) {
    Logger::debug("P updating @(", r, ",", c, "). E:", energy_, ", Age:", age_);
    age_++;
    energy_ -= Config::PREDATOR_ENERGY_PER_TICK;

    if (isDead()) {
        Logger::debug("P @(", r, ",", c, ") is dead at start of update.");
        return; 
    }

    if (tryToEat(ocean, r, c)) { 
        Logger::debug("P @(", r, ",", c, ") ATE. Update finished.");
        return; 
    }

    if (energy_ > Config::PREDATOR_CRITICAL_ENERGY_THRESHOLD * 1.2) { 
        if (tryToReproduce(ocean, r, c)) { 
            Logger::debug("P @(", r, ",", c, ") REPRODUCED. Update might continue.");
             if (isDead()) { 
                Logger::debug("P @(", r, ",", c, ") died after reproducing.");
                return;
            }
        }
    }

    Logger::debug("P @(", r, ",", c, ") will now perform huntOrExplore.");
    huntOrExplore(ocean, r, c); 
    Logger::debug("P @(", r, ",", c, ") finished huntOrExplore. Update finished.");
}

char PredatorFish::getSymbol() const {
    if (isDead()) return 'x';
    if (energy_ < Config::PREDATOR_CRITICAL_ENERGY_THRESHOLD / 2) return 'p';
    return 'P';
}

EntityType PredatorFish::getType() const {
    return EntityType::PREDATOR;
}