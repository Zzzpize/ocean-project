#include "ocean.hpp"
#include <iostream>
#include <stdexcept>

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
        std::cerr << "Error: Cannot add entity outside ocean bounds (" << r << "," << c << ")." << std::endl;
        return false; 
    }
    if (grid_[r][c]) {
        std::cerr << "Error: Cell (" << r << "," << c << ") is already occupied." << std::endl;
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
        std::cerr << "Error: Invalid coordinates for move operation." << std::endl;
        return false;
    }
    if (!grid_[r_from][c_from]) {
        std::cerr << "Error: No entity at source location (" << r_from << "," << c_from << ") to move." << std::endl;
        return false;
    }
    if (grid_[r_to][c_to]) {
         std::cerr << "Error: Destination cell (" << r_to << "," << c_to << ") is occupied." << std::endl;
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
        std::cout << std::endl;
    }
    std::cout << std::endl; 
}

void Ocean::tick() {
    std::cout << "Test2" << std::endl;
}