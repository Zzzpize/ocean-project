#include "ocean.hpp"        
#include "utils/logger.hpp" 
#include "utils/random.hpp" 

#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>     
#include <algorithm>    
#include <string> // Для std::to_string

class Ocean::OceanImpl {
public:
    int rows_;
    int cols_;
    std::vector<std::vector<std::unique_ptr<Entity>>> grid_;

    OceanImpl(int rows, int cols) 
        : rows_(rows), cols_(cols) {
        grid_.resize(rows_);
        for (int i = 0; i < rows_; ++i) {
            grid_[i].resize(cols_); 
        }
    }

    bool isValidCoordinateImpl(int r, int c) const {
        return r >= 0 && r < rows_ && c >= 0 && c < cols_;
    }

    bool addEntityImpl(std::unique_ptr<Entity> entity, int r, int c) {
        if (!entity) {
            Logger::warn("Attempted to add a null entity to (", r, ",", c, ").");
            return false;
        }
        if (!isValidCoordinateImpl(r, c)) {
            std::string err_msg = "addEntityImpl: Coordinates (" + std::to_string(r) + "," + std::to_string(c) + 
                                  ") are outside ocean bounds (" + std::to_string(rows_) + "x" + std::to_string(cols_) + ").";
            Logger::error(err_msg);
            throw std::out_of_range(err_msg);
        }
        if (grid_[r][c]) { 
            Logger::debug("Cannot add entity: cell (", r, ",", c, ") is already occupied by type ", static_cast<int>(grid_[r][c]->getType()), ". Requested type: ", static_cast<int>(entity->getType()));
            return false; 
        }
        grid_[r][c] = std::move(entity); 
        return true;
    }

    Entity* getEntityImpl(int r, int c) const {
        if (!isValidCoordinateImpl(r, c)) {
            std::string err_msg = "getEntityImpl: Coordinates (" + std::to_string(r) + "," + std::to_string(c) + 
                                  ") are outside ocean bounds (" + std::to_string(rows_) + "x" + std::to_string(cols_) + ").";
            Logger::error(err_msg);
            throw std::out_of_range(err_msg);
        }
        return grid_[r][c].get(); 
    }

    std::unique_ptr<Entity> removeEntityImpl(int r, int c) {
        if (!isValidCoordinateImpl(r, c)) {
            std::string err_msg = "removeEntityImpl: Coordinates (" + std::to_string(r) + "," + std::to_string(c) + 
                                  ") are outside ocean bounds (" + std::to_string(rows_) + "x" + std::to_string(cols_) + ").";
            Logger::error(err_msg);
            throw std::out_of_range(err_msg);
        }
        if (!grid_[r][c]) {
            return nullptr; 
        }
        return std::move(grid_[r][c]); 
    }

    bool moveEntityImpl(int r_from, int c_from, int r_to, int c_to) {
        if (!isValidCoordinateImpl(r_from, c_from)) {
            std::string err_msg = "moveEntityImpl: Source coordinates (" + std::to_string(r_from) + "," + std::to_string(c_from) + 
                                  ") are outside ocean bounds.";
            Logger::error(err_msg);
            throw std::out_of_range(err_msg);
        }
        if (!isValidCoordinateImpl(r_to, c_to)) {
            std::string err_msg = "moveEntityImpl: Target coordinates (" + std::to_string(r_to) + "," + std::to_string(c_to) + 
                                  ") are outside ocean bounds.";
            Logger::error(err_msg);
            throw std::out_of_range(err_msg);
        }
        if (!grid_[r_from][c_from]) {
            Logger::warn("No entity at source location (", r_from, ",", c_from, ") to move.");
            return false;
        }
        if (r_from == r_to && c_from == c_to) {
            return true; 
        }
        if (grid_[r_to][c_to]) {
            Logger::debug("Destination cell (", r_to, ",", c_to, ") for move is occupied by type ", static_cast<int>(grid_[r_to][c_to]->getType()), ". Source type: ", static_cast<int>(grid_[r_from][c_from]->getType()));
            return false; 
        }
        grid_[r_to][c_to] = std::move(grid_[r_from][c_from]); 
        return true;
    }

    void displayImpl() const {
        for (int r_idx = 0; r_idx < rows_; ++r_idx) {
            for (int c_idx = 0; c_idx < cols_; ++c_idx) {
                if (grid_[r_idx][c_idx]) {
                    std::cout << grid_[r_idx][c_idx]->getSymbol() << " ";
                } else {
                    std::cout << ". "; 
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl; 
    }

    void tickImpl(Ocean& ocean_ref) {
        std::vector<std::pair<int,int>> entities_to_update;
        for (int r_idx = 0; r_idx < rows_; ++r_idx) {
            for (int c_idx = 0; c_idx < cols_; ++c_idx) {
                if (grid_[r_idx][c_idx]) {
                    entities_to_update.push_back({r_idx,c_idx});
                }
            }
        }
        Random::shuffle(entities_to_update);

        for (const auto& pos : entities_to_update) {
            int r = pos.first;
            int c = pos.second;
            if (isValidCoordinateImpl(r,c) && grid_[r][c]) { 
                grid_[r][c]->update(ocean_ref, r, c); 
            }
        } 

        int removed_count = 0;
        for (int r_scan = 0; r_scan < rows_; ++r_scan) {
            for (int c_scan = 0; c_scan < cols_; ++c_scan) {
                if (isValidCoordinateImpl(r_scan,c_scan) && grid_[r_scan][c_scan]) {
                    if (grid_[r_scan][c_scan]->isDead()) { 
                        EntityType dead_entity_type = grid_[r_scan][c_scan]->getType(); 
                        grid_[r_scan][c_scan].reset(); 
                        Logger::info("Removed dead entity of type ", static_cast<int>(dead_entity_type), " at (", r_scan, ",", c_scan, ")");
                        removed_count++;
                    }
                }
            }
        }
    }

    std::vector<std::pair<int, int>> getEmptyAdjacentCellsImpl(int r_param, int c_param) const {
        std::vector<std::pair<int, int>> emptyCells;
        const int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1};
        const int dc[] = {-1,  0,  1, -1, 1, -1, 0, 1};

        for (int i = 0; i < 8; ++i) {
            int nr = r_param + dr[i];
            int nc = c_param + dc[i];
            if (isValidCoordinateImpl(nr, nc) && !grid_[nr][nc]) { 
                emptyCells.push_back({nr, nc});
            }
        }
        return emptyCells;
    }

    std::vector<std::pair<int, int>> getAdjacentCellsOfTypeImpl(int r_param, int c_param, EntityType type) const {
        std::vector<std::pair<int, int>> cellsOfType;
        const int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1}; 
        const int dc[] = {-1,  0,  1, -1, 1, -1, 0, 1};

        for (int i = 0; i < 8; ++i) {
            int nr = r_param + dr[i];
            int nc = c_param + dc[i];
            if (isValidCoordinateImpl(nr, nc) && grid_[nr][nc] && grid_[nr][nc]->getType() == type) {
                cellsOfType.push_back({nr, nc});
            }
        }
        return cellsOfType;
    }

    std::pair<int, int> getDirectionToNearestTargetImpl(int start_r, int start_c, EntityType target_type, int radius) const {
        std::pair<int, int> best_target_pos = {-1, -1};
        int min_dist_sq = (radius * radius) * 2 + 1; 

        for (int r_check = start_r - radius; r_check <= start_r + radius; ++r_check) {
            for (int c_check = start_c - radius; c_check <= start_c + radius; ++c_check) {
                if (r_check == start_r && c_check == start_c) continue;

                if (isValidCoordinateImpl(r_check, c_check)) {
                    Entity* entity_ptr = grid_[r_check][c_check].get();
                    if (entity_ptr && entity_ptr->getType() == target_type) {
                        int dr_dist = r_check - start_r;
                        int dc_dist = c_check - start_c;
                        int dist_sq = dr_dist * dr_dist + dc_dist * dc_dist; 

                        if (dist_sq < min_dist_sq) {
                            min_dist_sq = dist_sq;
                            best_target_pos = {r_check, c_check};
                        }
                    }
                }
            }
        }

        if (best_target_pos.first != -1) { 
            int move_dr = 0;
            if (best_target_pos.first < start_r) move_dr = -1;
            else if (best_target_pos.first > start_r) move_dr = 1;

            int move_dc = 0;
            if (best_target_pos.second < start_c) move_dc = -1;
            else if (best_target_pos.second > start_c) move_dc = 1;
            
            return {move_dr, move_dc};
        }
        return {0, 0}; 
    }
};

Ocean::Ocean(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        std::string err_msg = "Ocean constructor: dimensions must be positive. Requested: " + std::to_string(rows) + "x" + std::to_string(cols);
        Logger::error(err_msg);
        throw std::invalid_argument(err_msg);
    }
    pImpl_ = std::make_unique<OceanImpl>(rows, cols);
    Logger::info("Ocean (PImpl) created with size ", rows, "x", cols, ".");
}

Ocean::~Ocean() = default; 

Ocean::Ocean(Ocean&& other) noexcept = default;
Ocean& Ocean::operator=(Ocean&& other) noexcept = default;

bool Ocean::addEntity(std::unique_ptr<Entity> entity, int r, int c) {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::addEntity called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->addEntityImpl(std::move(entity), r, c);
}

Entity* Ocean::getEntity(int r, int c) const {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::getEntity called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->getEntityImpl(r, c);
}

std::unique_ptr<Entity> Ocean::removeEntity(int r, int c) {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::removeEntity called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->removeEntityImpl(r, c);
}
    
bool Ocean::moveEntity(int r_from, int c_from, int r_to, int c_to) {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::moveEntity called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->moveEntityImpl(r_from, c_from, r_to, c_to);
}

int Ocean::getRows() const {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::getRows called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->rows_; 
}

int Ocean::getCols() const {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::getCols called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->cols_; 
}

void Ocean::display() const {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::display called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    pImpl_->displayImpl();
}

void Ocean::tick() {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::tick called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    pImpl_->tickImpl(*this); 
}
    
bool Ocean::isValidCoordinate(int r, int c) const {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::isValidCoordinate called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->isValidCoordinateImpl(r, c);
}

std::vector<std::pair<int, int>> Ocean::getEmptyAdjacentCells(int r, int c) const {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::getEmptyAdjacentCells called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->getEmptyAdjacentCellsImpl(r, c);
}

std::vector<std::pair<int, int>> Ocean::getAdjacentCellsOfType(int r, int c, EntityType type) const {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::getAdjacentCellsOfType called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->getAdjacentCellsOfTypeImpl(r, c, type);
}

std::pair<int, int> Ocean::getDirectionToNearestTarget(int start_r, int start_c, EntityType target_type, int radius) const {
    if (!pImpl_) { 
        std::string err_msg = "Ocean::getDirectionToNearestTarget called on an invalid (moved-from or uninitialized) Ocean object.";
        Logger::error(err_msg);
        throw std::runtime_error(err_msg);
    }
    return pImpl_->getDirectionToNearestTargetImpl(start_r, start_c, target_type, radius);
}