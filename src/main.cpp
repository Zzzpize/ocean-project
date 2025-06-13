#define OLC_PGE_APPLICATION
#include "olc/olcPixelGameEngine.h"

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <limits>

#include "ocean.hpp"
#include "algae.hpp"
#include "herbivore.hpp"
#include "predator.hpp"
#include "utils/random.hpp"
#include "utils/logger.hpp"
#include "utils/resource_wrapper.hpp"

const int SPRITE_SIZE = 16; 

class OceanGame : public olc::PixelGameEngine {
public:
    OceanGame(int ocean_rows, int ocean_cols) : ocean_rows_(ocean_rows), ocean_cols_(ocean_cols) {
        sAppName = "Living Ocean";
        pge_ocean_ = nullptr; 
    }

private:
    Ocean* pge_ocean_; 
    int ocean_rows_;
    int ocean_cols_;
    float time_accumulator_ = 0.0f;
    const float TICK_INTERVAL = 0.5f; 

    std::unique_ptr<olc::Sprite> sprSand;
    std::unique_ptr<olc::Sprite> sprAlgae;
    std::unique_ptr<olc::Sprite> sprHerbivore;
    std::unique_ptr<olc::Sprite> sprPredator;
    std::unique_ptr<olc::Sprite> sprHerbivoreHungry;
    std::unique_ptr<olc::Sprite> sprPredatorHungry;


public:
    bool OnUserCreate() override {
    Logger::info("PGE: OnUserCreate called.");
    pge_ocean_ = new Ocean(ocean_rows_, ocean_cols_);
    Logger::info("Ocean created with size ", pge_ocean_->getRows(), "x", pge_ocean_->getCols(), ".");
    SetPixelMode(olc::Pixel::ALPHA);
    Logger::info("PGE: Attempting to load sprites...");
    sprSand = std::make_unique<olc::Sprite>("./assets/sand.png");
    if (!sprSand || sprSand->width == 0 || sprSand->height == 0) {
        Logger::error("PGE: Failed to load sand.png or its dimensions are zero.");
        return false;
    }

    sprAlgae = std::make_unique<olc::Sprite>("./assets/algae.png");
    if (!sprAlgae || sprAlgae->width == 0 || sprAlgae->height == 0) {
        Logger::error("PGE: Failed to load algae.png or its dimensions are zero.");
        return false;
    }

    sprHerbivore = std::make_unique<olc::Sprite>("./assets/herbivore.png");
    if (!sprHerbivore || sprHerbivore->width == 0 || sprHerbivore->height == 0) {
        Logger::error("PGE: Failed to load herbivore.png or its dimensions are zero.");
        return false;
    }

    sprPredator = std::make_unique<olc::Sprite>("./assets/predator.png");
    if (!sprPredator || sprPredator->width == 0 || sprPredator->height == 0) { 
        Logger::error("PGE: Failed to load predator.png or its dimensions are zero.");
        return false;
    }
    
    Logger::info("PGE: Base sprites loaded successfully.");

    sprHerbivoreHungry = std::make_unique<olc::Sprite>(sprHerbivore->width, sprHerbivore->height);
    if (!sprHerbivoreHungry || sprHerbivoreHungry->width == 0 || sprHerbivoreHungry->height == 0) {
        Logger::error("PGE: Failed to create sprHerbivoreHungry or its dimensions are zero.");
        return false; 
    } else {
        for (int32_t y = 0; y < sprHerbivore->height; ++y) { 
            for (int32_t x = 0; x < sprHerbivore->width; ++x) {
                olc::Pixel p = sprHerbivore->GetPixel(x, y);
                if (p.a > 0) {
                    sprHerbivoreHungry->SetPixel(x, y, olc::Pixel(p.r / 2, p.g / 2, p.b / 2, p.a));
                } else {
                    sprHerbivoreHungry->SetPixel(x, y, p);
                }
            }
        }
        Logger::info("PGE: sprHerbivoreHungry processed.");
    }
    sprPredatorHungry = std::make_unique<olc::Sprite>(sprPredator->width, sprPredator->height);
    if (!sprPredatorHungry || sprPredatorHungry->width == 0 || sprPredatorHungry->height == 0) {
        Logger::error("PGE: Failed to create sprPredatorHungry or its dimensions are zero.");
        return false; 
    } else {
        for (int32_t y = 0; y < sprPredator->height; ++y) {
            for (int32_t x = 0; x < sprPredator->width; ++x) {
                olc::Pixel p = sprPredator->GetPixel(x, y);
                if (p.a > 0) { 
                    sprPredatorHungry->SetPixel(x, y, olc::Pixel(p.r / 2, p.g / 2, p.b / 2, p.a));
                } else {
                    sprPredatorHungry->SetPixel(x, y, p);
                }
            }
        }
        Logger::info("PGE: sprPredatorHungry processed.");
    }
    
    Logger::info("PGE: Hungry sprites created and modified.");


    int initial_algae_count = 0;
    for(int i = 0; i < ocean_rows_ * ocean_cols_ / 15; ++i) { // вместо ocean_rows_ * ocean_cols_ / 15 можно написать точное число водорослей
        int r = Random::getInt(0, pge_ocean_->getRows()-1);
        int c = Random::getInt(0, pge_ocean_->getCols()-1);
        if(pge_ocean_->addEntity(std::make_unique<Algae>(), r, c)) {
            initial_algae_count++;
        }
    }
    Logger::info("Added ", initial_algae_count, " Algae initially.");

    int initial_herb_count = 0;
    for(int i=0; i < ocean_rows_ * ocean_cols_ / 100; ++i) { // вместо ocean_rows_ * ocean_cols_ / 100 можно написать точное число травоядных
        int r = Random::getInt(0, pge_ocean_->getRows()-1);
        int c = Random::getInt(0, pge_ocean_->getCols()-1);
        if(pge_ocean_->addEntity(std::make_unique<HerbivoreFish>(), r, c)) {
            initial_herb_count++;
        }
    }
    Logger::info("Added ", initial_herb_count, " HerbivoreFish initially.");

    int initial_pred_count = 0;
    for(int i=0; i < ocean_rows_ * ocean_cols_ / 300; ++i) { // вместо ocean_rows_ * ocean_cols_ / 300 можно написать точное число хищников
        int r = Random::getInt(0, pge_ocean_->getRows()-1);
        int c = Random::getInt(0, pge_ocean_->getCols()-1);
        if(pge_ocean_->addEntity(std::make_unique<PredatorFish>(), r, c)) {
            initial_pred_count++;
        }
    }
    Logger::info("Added ", initial_pred_count, " PredatorFish initially.");
    Logger::info("PGE: OnUserCreate finished.");
    return true;
}

    bool OnUserUpdate(float fElapsedTime) override {
        time_accumulator_ += fElapsedTime;

        if (GetKey(olc::Key::ESCAPE).bPressed) {
            return false; 
        }
        
        if (time_accumulator_ >= TICK_INTERVAL) {
            time_accumulator_ -= TICK_INTERVAL;
            if (pge_ocean_) {
                pge_ocean_->tick();
            }
        }

        Clear(olc::BLACK);

        if (pge_ocean_) {
            for (int r = 0; r < pge_ocean_->getRows(); ++r) {
                for (int c = 0; c < pge_ocean_->getCols(); ++c) {
                    olc::Sprite* spriteToDraw = sprSand.get();
                    Entity* entity = nullptr;
                    try {
                        entity = pge_ocean_->getEntity(r, c);
                    } catch (const std::out_of_range& e) {
                        Logger::error("Error getting entity at (", r, ",", c, "): ", e.what());
                        continue; 
                    }
                    
                    if (entity) {
                        switch (entity->getType()) {
                            case EntityType::ALGAE:
                                spriteToDraw = sprAlgae.get();
                                break;
                            case EntityType::HERBIVORE:
                                {
                                    HerbivoreFish* hf = dynamic_cast<HerbivoreFish*>(entity);
                                    if (hf && hf->getSymbol() == 'h') spriteToDraw = sprHerbivoreHungry.get();
                                    else spriteToDraw = sprHerbivore.get();
                                }
                                break;
                            case EntityType::PREDATOR:
                                {
                                    PredatorFish* pf = dynamic_cast<PredatorFish*>(entity);
                                    if (pf && pf->getSymbol() == 'p') spriteToDraw = sprPredatorHungry.get();
                                    else spriteToDraw = sprPredator.get();
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    if (spriteToDraw) {
                        DrawSprite(c * SPRITE_SIZE, r * SPRITE_SIZE, spriteToDraw);
                    }
                }
            }
        }
        return true;
    }

    bool OnUserDestroy() override {
        Logger::info("PGE: OnUserDestroy called.");
        delete pge_ocean_;
        pge_ocean_ = nullptr;
        return true;
    }
};

class LoggerInitializer {
public:
    LoggerInitializer() { 
        Logger::init(); 
        Logger::info("Logger system initialized."); 
    }
    ~LoggerInitializer() { 
        Logger::info("Logger system shutting down."); 
        Logger::shutdown(); 
    }
};

void demonstrateRuleOfFive() {
    Logger::info("--- Demonstrating Rule of Five with ResourceWrapper ---");
    std::cout << "\n--- Demonstrating Rule of Five with ResourceWrapper ---\n";

    std::cout << "\n1. Construction:\n";
    ResourceWrapper r1("Hello");
    r1.print("r1: ");
    ResourceWrapper r2_empty; 
    r2_empty.print("r2_empty: ");

    std::cout << "\n2. Copy Construction:\n";
    ResourceWrapper r3 = r1; 
    r1.print("r1 (after r3=r1): "); 
    r3.print("r3 (copy of r1): "); 

    std::cout << "\n3. Copy Assignment:\n";
    ResourceWrapper r2_assign("Original R2");
    r2_assign.print("r2_assign (before assignment): ");
    r2_assign = r1;
    r1.print("r1 (after r2_assign=r1): "); 
    r2_assign.print("r2_assign (now copy of r1): "); 

    std::cout << "\n4. Move Construction:\n";
    ResourceWrapper r4 = std::move(r1); 
    r1.print("r1 (after move to r4): "); 
    r4.print("r4 (moved from r1): "); 

    std::cout << "\n5. Move Assignment:\n";
    ResourceWrapper r5("Original R5");
    r5.print("r5 (before assignment): ");
    r5 = std::move(r3); 
    r3.print("r3 (after move to r5): ");
    r5.print("r5 (moved from r3): "); 
    
    std::cout << "\n6. Self-assignment check (copy):\n";
    ResourceWrapper r_self_copy("SelfCopyTest");
    r_self_copy.print("r_self_copy (before): ");
    r_self_copy = r_self_copy; 
    r_self_copy.print("r_self_copy (after self-copy): ");

    std::cout << "\n7. Self-assignment check (move):\n";
    ResourceWrapper r_self_move("SelfMoveTest");
    r_self_move.print("r_self_move (before): ");
    r_self_move = std::move(r_self_move); 
    r_self_move.print("r_self_move (after self-move): ");

    std::cout << "\n--- End of Rule of Five Demonstration (scopes ending) ---\n";
}


int main() {
    LoggerInitializer logger_guard; 
    Logger::info("Main: Program starting...");

    demonstrateRuleOfFive(); 

    std::cout << "\nPress Enter to start ocean simulation...\n";
    std::cin.get(); 
    std::cin.clear(); 
    std::cin.sync();  


    int ocean_sim_rows = 50; 
    int ocean_sim_cols = 50; 

    OceanGame game(ocean_sim_rows, ocean_sim_cols);
    if (game.Construct(ocean_sim_cols * SPRITE_SIZE, ocean_sim_rows * SPRITE_SIZE, 4, 4)) {
        Logger::info("Main: PGE Constructed, starting game loop.");
        game.Start();
    } else {
        Logger::error("Main: PGE Failed to construct!");
    }
    
    Logger::info("Main: Program finished.");
    return 0;
}