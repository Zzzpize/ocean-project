#pragma once
#include "entity.hpp"

class PredatorFish : public Entity {
public:
    PredatorFish();
    ~PredatorFish() override = default;

    void update(int r, int c) override;
    char getSymbol() const override;
    EntityType getType() const override;

};