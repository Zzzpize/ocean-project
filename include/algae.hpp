#pragma once
#include "entity.hpp"

class Algae : public Entity {
public:
    Algae();
    ~Algae() override = default;

    void update(Ocean& ocean, int r, int c) override;
    char getSymbol() const override;
    EntityType getType() const override;
};