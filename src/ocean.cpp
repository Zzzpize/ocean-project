#include "ocean.hpp"
#include "utils/logger.hpp"
#include <iostream> 
#include <stdexcept> 
#include <utility> 

Ocean::Ocean(int rows, int cols) : rows_(rows), cols_(cols) {
    if (rows <= 0 || cols <= 0) {
        Logger::error("Ocean dimensions must be positive. Requested: ", rows, "x", cols);
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
    if (!entity) {
        Logger::warn("Attempted to add a null entity to (", r, ",", c, ").");
        return false;
    }
    if (!isValidCoordinate(r, c)) {
        Logger::warn("Cannot add entity: coordinates (", r, ",", c, ") are outside ocean bounds (", rows_, "x", cols_, "). Entity type: ", static_cast<int>(entity->getType()));
        return false; 
    }
    if (grid_[r][c]) { 
        Logger::debug("Cannot add entity: cell (", r, ",", c, ") is already occupied by type ", static_cast<int>(grid_[r][c]->getType()), ". Requested type: ", static_cast<int>(entity->getType()));
        return false; 
    }
    Logger::debug("Adding entity type ", static_cast<int>(entity->getType()), " to (", r, ",", c, ")");
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
    if (!isValidCoordinate(r, c)) {
        Logger::warn("Cannot remove entity: coordinates (", r, ",", c, ") are outside ocean bounds.");
        return nullptr; 
    }
    if (!grid_[r][c]) {
        Logger::debug("Cannot remove entity: cell (", r, ",", c, ") is already empty.");
        return nullptr; 
    }
    Logger::debug("Removing entity type ", static_cast<int>(grid_[r][c]->getType()), " from (", r, ",", c, ")");
    return std::move(grid_[r][c]); 
}

bool Ocean::moveEntity(int r_from, int c_from, int r_to, int c_to) {
    if (!isValidCoordinate(r_from, c_from) || !isValidCoordinate(r_to, c_to)) {
        Logger::warn("Invalid coordinates for move operation. From (", r_from, ",", c_from, ") to (", r_to, ",", c_to, ").");
        return false;
    }
    if (!grid_[r_from][c_from]) {
        Logger::warn("No entity at source location (", r_from, ",", c_from, ") to move.");
        return false;
    }
    if (r_from == r_to && c_from == c_to) {
        Logger::debug("Attempt to move entity to its current location (", r_from, ",", c_from, "). No action taken.");
        return true; 
    }
    if (grid_[r_to][c_to]) {
        Logger::debug("Destination cell (", r_to, ",", c_to, ") for move is occupied by type ", static_cast<int>(grid_[r_to][c_to]->getType()), ". Source type: ", static_cast<int>(grid_[r_from][c_from]->getType()));
        return false; 
    }
    Logger::debug("Moving entity type ", static_cast<int>(grid_[r_from][c_from]->getType()), " from (", r_from, ",", c_from, ") to (", r_to, ",", c_to, ")");
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
    std::vector<std::pair<int,int>> entities_to_update;
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (grid_[r][c]) {
                entities_to_update.push_back({r,c});
            }
        }
    }
    Logger::debug("Found ", entities_to_update.size(), " entities to update this tick.");

    Random::shuffle(entities_to_update);

    for (const auto& pos : entities_to_update) {
        int r = pos.first;
        int c = pos.second;
        
        if (grid_[r][c]) { 
            Logger::debug("Updating entity at (", r, ",", c, ") of type ", static_cast<int>(grid_[r][c]->getType()));
            grid_[r][c]->update(*this, r, c); 
        }
    } 

    Logger::debug("Starting dead entity removal phase.");
    int removed_count = 0;
    for (int r_scan = 0; r_scan < rows_; ++r_scan) {
        for (int c_scan = 0; c_scan < cols_; ++c_scan) {
            if (grid_[r_scan][c_scan]) {
                if (grid_[r_scan][c_scan]->isDead()) { 
                    Logger::info("Removing dead entity of type ", static_cast<int>(grid_[r_scan][c_scan]->getType()), " at (", r_scan, ",", c_scan, ")");
                    removeEntity(r_scan, c_scan); 
                    removed_count++;
                }
            }
        }
    }
    if (removed_count > 0) {
       Logger::debug("Removed ", removed_count, " dead entities this tick.");
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