#pragma once

#include <vector>
#include <memory>
#include "entity.hpp"

class Ocean {
public:
    Ocean(int rows, int cols);

    ~Ocean() = default;

    Ocean(const Ocean&) = delete;
    Ocean& operator=(const Ocean&) = delete;

    Ocean(Ocean&&) noexcept = default;
    Ocean& operator=(Ocean&&) noexcept = default;


    bool addEntity(std::unique_ptr<Entity> entity, int r, int c);

    Entity* getEntity(int r, int c) const;

    
    std::unique_ptr<Entity> removeEntity(int r, int c);
    
    bool moveEntity(int r_from, int c_from, int r_to, int c_to);

    int getRows() const;
    int getCols() const;

    void display() const;

    void tick(); 

private:
    int rows_; 
    int cols_;
    std::vector<std::vector<std::unique_ptr<Entity>>> grid_;

    bool isValidCoordinate(int r, int c) const;
};