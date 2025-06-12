#pragma once
#include "entity.hpp"

class HerbivoreFish : public Entity {
public:
    HerbivoreFish();
    ~HerbivoreFish() override = default;

    void update(int r, int c) override;
    char getSymbol() const override;
    EntityType getType() const override;

};