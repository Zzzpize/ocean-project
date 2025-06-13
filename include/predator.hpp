#pragma once
#include "entity.hpp"
#include "utils/random.hpp"

namespace Config {
    const int PREDATOR_INITIAL_ENERGY = 180;
    const int PREDATOR_MAX_ENERGY = 350;
    const int PREDATOR_MAX_AGE = 90;      
    const int PREDATOR_ENERGY_PER_TICK = 3;
    const int PREDATOR_ENERGY_FROM_HERBIVORE = 75;
    
    const int PREDATOR_REPRODUCTION_ENERGY_THRESHOLD = 280; 
    const int PREDATOR_REPRODUCTION_COST = 130;             
    const int PREDATOR_OFFSPRING_INITIAL_ENERGY = 120;      
    const int PREDATOR_REPRODUCTION_CHANCE_PERCENT = 10; 

    const int PREDATOR_SIGHT_RADIUS = 6;
    const int PREDATOR_CRITICAL_ENERGY_THRESHOLD = 90; 
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