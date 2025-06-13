#include "herbivore.hpp"
#include "ocean.hpp"
#include "algae.hpp" 
#include "utils/logger.hpp"
#include <vector>
#include <utility> 

HerbivoreFish::HerbivoreFish(int initial_energy) 
    : energy_(initial_energy), age_(0) {
    if (energy_ > Config::HERBIVORE_MAX_ENERGY) { 
        energy_ = Config::HERBIVORE_MAX_ENERGY;
    }
}

bool HerbivoreFish::isDead() const {
    return energy_ <= 0 || age_ >= Config::HERBIVORE_MAX_AGE;
}

bool HerbivoreFish::tryToEat(Ocean& ocean, int current_r, int current_c) {
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

            Logger::debug("H @(",current_r,",",current_c,") moving to eat at (",algaePos.first,",",algaePos.second,")");
            ocean.moveEntity(current_r, current_c, algaePos.first, algaePos.second);
            Logger::info("H at (", algaePos.first, ",", algaePos.second, ") ate Algae. E:", energy_);
            return true;
        }
    }
    return false;
}

bool HerbivoreFish::tryToReproduce(Ocean& ocean, int current_r, int current_c) {
    if (energy_ >= Config::HERBIVORE_REPRODUCTION_ENERGY_THRESHOLD &&
        Random::getInt(1, 100) <= Config::HERBIVORE_REPRODUCTION_CHANCE_PERCENT) {
        
        std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(current_r, current_c);
        if (!emptyNeighbors.empty()) {
            Random::shuffle(emptyNeighbors);
            std::pair<int, int> offspringPos = emptyNeighbors[0];

            auto offspring = std::make_unique<HerbivoreFish>(Config::HERBIVORE_OFFSPRING_INITIAL_ENERGY);
            
            if (ocean.addEntity(std::move(offspring), offspringPos.first, offspringPos.second)) {
                energy_ -= Config::HERBIVORE_REPRODUCTION_COST; 
                Logger::info("H at (", current_r, ",", current_c, ") reproduced to (", 
                           offspringPos.first, ",", offspringPos.second, "). Parent E:", energy_);
                return true;
            }
        }
    }
    return false;
}

void HerbivoreFish::intelligentMove(Ocean& ocean, int current_r, int current_c) {
    if (energy_ < Config::HERBIVORE_CRITICAL_ENERGY_THRESHOLD * 1.5) {
        std::pair<int, int> direction = ocean.getDirectionToNearestTarget(current_r, current_c, EntityType::ALGAE, Config::HERBIVORE_SIGHT_RADIUS);

        if (direction.first != 0 || direction.second != 0) { 
            int next_r = current_r + direction.first;
            int next_c = current_c + direction.second;
            if (ocean.isValidCoordinate(next_r, next_c)) {
                Entity* entity_at_target_step = ocean.getEntity(next_r, next_c);
                if (!entity_at_target_step) {
                    Logger::debug("H @(", current_r, ",", current_c, ") moving towards food to (", next_r, ",", next_c, ")");
                    ocean.moveEntity(current_r, current_c, next_r, next_c);
                    return;
                } else if (entity_at_target_step->getType() == EntityType::ALGAE) {
                     Logger::debug("H @(", current_r, ",", current_c, ") sees food at (", next_r, ",", next_c, ") but cell not empty for step. Will try random.");
                }
            }
        }
    }

    Logger::debug("H @(", current_r, ",", current_c, ") moving randomly.");
    std::vector<std::pair<int, int>> emptyNeighbors = ocean.getEmptyAdjacentCells(current_r, current_c);
    if (!emptyNeighbors.empty()) {
        Random::shuffle(emptyNeighbors); 
        std::pair<int, int> targetCell = emptyNeighbors[0];
        Logger::debug("H @(", current_r, ",", current_c, ") random move to (", targetCell.first, ",", targetCell.second, ")");
        ocean.moveEntity(current_r, current_c, targetCell.first, targetCell.second);
    } else {
        Logger::debug("H @(", current_r, ",", current_c, ") has nowhere to move (randomly).");
    }
}


void HerbivoreFish::update(Ocean& ocean, int r, int c) {
    Logger::debug("H updating @(", r, ",", c, "). E:", energy_, ", Age:", age_);
    age_++;
    energy_ -= Config::HERBIVORE_ENERGY_PER_TICK;

    if (isDead()) {
        Logger::debug("H @(", r, ",", c, ") is dead at start of update.");
        return; 
    }

    if (tryToEat(ocean, r, c)) { 
        Logger::debug("H @(", r, ",", c, ") ATE. Update finished.");
        return; 
    }

    if (energy_ > Config::HERBIVORE_CRITICAL_ENERGY_THRESHOLD * 1.1) {
        if (tryToReproduce(ocean, r, c)) {
            Logger::debug("H @(", r, ",", c, ") REPRODUCED. Update might continue for move.");
            if (isDead()) { 
                Logger::debug("H @(", r, ",", c, ") died after reproducing.");
                return;
            }
             intelligentMove(ocean,r,c); 
             return;
        }
    }
    
    Logger::debug("H @(", r, ",", c, ") will now perform intelligentMove.");
    intelligentMove(ocean, r, c);
    Logger::debug("H @(", r, ",", c, ") finished intelligentMove. Update finished.");
}


char HerbivoreFish::getSymbol() const {
    if (isDead()) return 'x';
    if (energy_ < Config::HERBIVORE_CRITICAL_ENERGY_THRESHOLD / 2) return 'h'; 
    return 'H';
}

EntityType HerbivoreFish::getType() const {
    return EntityType::HERBIVORE;
}