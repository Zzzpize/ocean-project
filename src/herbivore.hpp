#pragma once
#include "entity.hpp"
#include "utils/random.hpp"

namespace Config {
    const int HERBIVORE_INITIAL_ENERGY = 100;
    const int HERBIVORE_MAX_ENERGY = 200;
    const int HERBIVORE_MAX_AGE = 50; 
    const int HERBIVORE_ENERGY_PER_TICK = 2; 
    const int HERBIVORE_ENERGY_FROM_ALGAE = 20;

    const int HERBIVORE_REPRODUCTION_ENERGY_THRESHOLD = 150;
    const int HERBIVORE_REPRODUCTION_COST = 70;              
    const int HERBIVORE_OFFSPRING_INITIAL_ENERGY = 80;       
    const int HERBIVORE_REPRODUCTION_CHANCE_PERCENT = 25;    
}

class HerbivoreFish : public Entity {
public:
    explicit HerbivoreFish(int initial_energy = Config::HERBIVORE_INITIAL_ENERGY);
    ~HerbivoreFish() override = default;

    void update(Ocean& ocean, int r, int c) override;
    char getSymbol() const override;
    EntityType getType() const override;
    bool isDead() const override;

private:
    int energy_;
    int age_;

    void move(Ocean& ocean, int current_r, int current_c);
    bool eat(Ocean& ocean, int current_r, int current_c);
    void reproduce(Ocean& ocean, int current_r, int current_c);
};