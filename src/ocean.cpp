#include "ocean.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>

Ocean::Ocean(int rows, int cols) : rows_(rows), cols_(cols) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Ocean dimensions must be positive.");
    }
    grid_.resize(rows_);
    for (int i = 0; i < rows_; ++i) {
        grid_[i].resize(cols_);
    }
}

bool Ocean::isValidCoordinate(int r, int c) const {
    return r >= 0 && r < rows_ && c >= 0 && c < cols_;
}

bool Ocean::addEntity(std::unique_ptr<Entity> entity, int r, int c) {
    if (!isValidCoordinate(r, c)) {
        std::cerr << "Error: Cannot add entity outside ocean bounds (" << r << "," << c << ")." << "\n";
        return false; 
    }
    if (grid_[r][c]) {
        std::cerr << "Error: Cell (" << r << "," << c << ") is already occupied." << "\n";
        return false; 
    }
    grid_[r][c] = std::move(entity);
    return true;
}

Entity* Ocean::getEntity(int r, int c) const {
    if (!isValidCoordinate(r, c)) {
        return nullptr;
    }
    return grid_[r][c].get();
}

std::unique_ptr<Entity> Ocean::removeEntity(int r, int c) {
    if (!isValidCoordinate(r, c) || !grid_[r][c]) {
        return nullptr;
    }
    return std::move(grid_[r][c]); 
}

bool Ocean::moveEntity(int r_from, int c_from, int r_to, int c_to) {
    if (!isValidCoordinate(r_from, c_from) || !isValidCoordinate(r_to, c_to)) {
        std::cerr << "Error: Invalid coordinates for move operation." << "\n";
        return false;
    }
    if (!grid_[r_from][c_from]) {
        std::cerr << "Error: No entity at source location (" << r_from << "," << c_from << ") to move." << "\n";
        return false;
    }
    if (grid_[r_to][c_to]) {
         std::cerr << "Error: Destination cell (" << r_to << "," << c_to << ") is occupied." << "\n";
        return false; 
    }

    grid_[r_to][c_to] = std::move(grid_[r_from][c_from]);
    return true;
}


int Ocean::getRows() const {
    return rows_;
}

int Ocean::getCols() const {
    return cols_;
}

void Ocean::display() const {
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (grid_[r][c]) {
                std::cout << grid_[r][c]->getSymbol() << " ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n"; 
}

void Ocean::tick() {
    std::vector<std::pair<int,int>> entities_to_update;
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (grid_[r][c]) {
                entities_to_update.push_back({r,c});
            }
        }
    }

    Random::shuffle(entities_to_update);


    std::vector<std::pair<int,int>> dead_entities_coords; 

    for (const auto& pos : entities_to_update) {
        int r = pos.first;
        int c = pos.second;
        
        if (grid_[r][c]) { 
            grid_[r][c]->update(*this, r, c); 
        }
    } 

    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (grid_[r][c]) {
                if (grid_[r][c]->isDead()) { 
                    removeEntity(r, c);
                }
            }
        }
    }
}

std::vector<std::pair<int, int>> Ocean::getEmptyAdjacentCells(int r, int c) const {
    std::vector<std::pair<int, int>> emptyCells;
    const int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    const int dc[] = {-1,  0,  1, -1, 1, -1, 0, 1};

    for (int i = 0; i < 8; ++i) {
        int nr = r + dr[i];
        int nc = c + dc[i];

        if (isValidCoordinate(nr, nc) && !grid_[nr][nc]) {
            emptyCells.push_back({nr, nc});
        }
    }
    return emptyCells;
}

std::vector<std::pair<int, int>> Ocean::getAdjacentCellsOfType(int r, int c, EntityType type) const {
    std::vector<std::pair<int, int>> cellsOfType;
    const int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1}; 
    const int dc[] = {-1,  0,  1, -1, 1, -1, 0, 1};

    for (int i = 0; i < 8; ++i) {
        int nr = r + dr[i];
        int nc = c + dc[i];

        if (isValidCoordinate(nr, nc) && grid_[nr][nc] && grid_[nr][nc]->getType() == type) {
            cellsOfType.push_back({nr, nc});
        }
    }
    return cellsOfType;
}