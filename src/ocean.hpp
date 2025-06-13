#pragma once

#include <memory>
#include <vector>
#include <utility>
#include "entity.hpp" 

class Ocean {
public:
    Ocean(int rows, int cols);
    ~Ocean(); 

    Ocean(const Ocean&) = delete;
    Ocean& operator=(const Ocean&) = delete;

    Ocean(Ocean&& other) noexcept;
    Ocean& operator=(Ocean&& other) noexcept;

    bool addEntity(std::unique_ptr<Entity> entity, int r, int c);
    Entity* getEntity(int r, int c) const;
    std::unique_ptr<Entity> removeEntity(int r, int c);
    bool moveEntity(int r_from, int c_from, int r_to, int c_to);

    int getRows() const;
    int getCols() const;

    void display() const;
    void tick(); 

    bool isValidCoordinate(int r, int c) const;
    std::vector<std::pair<int, int>> getEmptyAdjacentCells(int r, int c) const;
    std::vector<std::pair<int, int>> getAdjacentCellsOfType(int r, int c, EntityType type) const;
    std::pair<int, int> getDirectionToNearestTarget(int start_r, int start_c, EntityType target_type, int radius) const;

private:
    class OceanImpl; 
    std::unique_ptr<OceanImpl> pImpl_; 
};