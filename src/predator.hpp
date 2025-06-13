#pragma once
#include "entity.hpp"
#include "utils/random.hpp"

namespace Config {
    const int PREDATOR_INITIAL_ENERGY = 150;
    const int PREDATOR_MAX_ENERGY = 300;
    const int PREDATOR_MAX_AGE = 70;      
    const int PREDATOR_ENERGY_PER_TICK = 4;
    const int PREDATOR_ENERGY_FROM_HERBIVORE = 60;
    
    const int PREDATOR_REPRODUCTION_ENERGY_THRESHOLD = 230; 
    const int PREDATOR_REPRODUCTION_COST = 110;             
    const int PREDATOR_OFFSPRING_INITIAL_ENERGY = 100;      
    const int PREDATOR_REPRODUCTION_CHANCE_PERCENT = 15; 

    const int PREDATOR_SIGHT_RADIUS = 5;
    const int PREDATOR_CRITICAL_ENERGY_THRESHOLD = 75; 
}

class PredatorFish : public Entity {
public:
    explicit PredatorFish(int initial_energy = Config::PREDATOR_INITIAL_ENERGY);
    ~PredatorFish() override = default;

    void update(Ocean& ocean, int r, int c) override;
    char getSymbol() const override;
    EntityType getType() const override;
    bool isDead() const override;

private:
    int energy_;
    int age_;

    bool tryToEat(Ocean& ocean, int current_r, int current_c);
    bool tryToReproduce(Ocean& ocean, int current_r, int current_c);
    void huntOrExplore(Ocean& ocean, int current_r, int current_c); 
};