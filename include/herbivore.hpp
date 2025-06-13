#pragma once
#include "entity.hpp"
#include "utils/random.hpp"

namespace Config {
    const int HERBIVORE_INITIAL_ENERGY = 120;
    const int HERBIVORE_MAX_ENERGY = 250;
    const int HERBIVORE_MAX_AGE = 70; 
    const int HERBIVORE_ENERGY_PER_TICK = 2; 
    const int HERBIVORE_ENERGY_FROM_ALGAE = 30;

    const int HERBIVORE_REPRODUCTION_ENERGY_THRESHOLD = 180; 
    const int HERBIVORE_REPRODUCTION_COST = 80;              
    const int HERBIVORE_OFFSPRING_INITIAL_ENERGY = 90;       
    const int HERBIVORE_REPRODUCTION_CHANCE_PERCENT = 25;

    const int HERBIVORE_SIGHT_RADIUS = 4; 
    const int HERBIVORE_CRITICAL_ENERGY_THRESHOLD = 70;
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

    bool tryToEat(Ocean& ocean, int current_r, int current_c);
    bool tryToReproduce(Ocean& ocean, int current_r, int current_c);
    void intelligentMove(Ocean& ocean, int current_r, int current_c);
};