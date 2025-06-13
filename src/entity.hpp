#pragma once

class Ocean; 

enum class EntityType {
    SAND,
    ALGAE,
    HERBIVORE,
    PREDATOR
};

class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(Ocean& ocean, int r, int c) = 0;
    virtual char getSymbol() const = 0;
    virtual EntityType getType() const = 0;
    virtual bool isDead() const { return false; }
};