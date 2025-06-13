#include <iostream>
#include <vector>
#include <memory>
#include <chrono>   
#include <thread>  
#include <cstdlib> 
#include <limits> 

#include "ocean.hpp"
#include "algae.hpp"
#include "herbivore.hpp"
#include "predator.hpp"
#include "utils/random.hpp" 
#include "utils/logger.hpp" 

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

int main() {
    LoggerInitializer logger_guard; 

    Logger::info("Simulation starting...");

    try {
        Ocean myOcean(15, 30); 
        Logger::info("Ocean created with size ", myOcean.getRows(), "x", myOcean.getCols(), ".");

        int initial_algae_count = 0;
        for(int i=0; i < 40; ++i) {
            int r = Random::getInt(0, myOcean.getRows()-1);
            int c = Random::getInt(0, myOcean.getCols()-1);
            if(myOcean.addEntity(std::make_unique<Algae>(), r, c)) {
                initial_algae_count++;
            }
        }
        Logger::info("Added ", initial_algae_count, " Algae initially.");

        int initial_herb_count = 0;
        for(int i=0; i < 10; ++i) { 
            int r = Random::getInt(0, myOcean.getRows()-1);
            int c = Random::getInt(0, myOcean.getCols()-1);
            if(myOcean.addEntity(std::make_unique<HerbivoreFish>(), r, c)) {
                initial_herb_count++;
            }
        }
        Logger::info("Added ", initial_herb_count, " HerbivoreFish initially.");

        int initial_pred_count = 0;
        for(int i=0; i < 3; ++i) { 
            int r = Random::getInt(0, myOcean.getRows()-1);
            int c = Random::getInt(0, myOcean.getCols()-1);
            if(myOcean.addEntity(std::make_unique<PredatorFish>(), r, c)) {
                initial_pred_count++;
            }
        }
        Logger::info("Added ", initial_pred_count, " PredatorFish initially.");
        
        const int num_ticks = 200; 
        Logger::info("Starting simulation for ", num_ticks, " ticks.");

        for (int i = 0; i < num_ticks; ++i) {
            system("cls"); 
            
            std::cout << "Tick: " << i + 1 << " / " << num_ticks << std::endl;
            Logger::debug("Processing Tick: ", i + 1);
            
            myOcean.tick();    
            myOcean.display(); 

            int algae_count = 0;
            int herb_count = 0;
            int pred_count = 0;
            int total_entities = 0;

            for (int r_idx = 0; r_idx < myOcean.getRows(); ++r_idx) {
                for (int c_idx = 0; c_idx < myOcean.getCols(); ++c_idx) {
                    Entity* e = myOcean.getEntity(r_idx, c_idx);
                    if (e) {
                        total_entities++;
                        switch (e->getType()) {
                            case EntityType::ALGAE:
                                algae_count++;
                                break;
                            case EntityType::HERBIVORE:
                                herb_count++;
                                break;
                            case EntityType::PREDATOR:
                                pred_count++;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            std::cout << "Statistics: Algae: " << algae_count 
                      << ", Herbivores: " << herb_count 
                      << ", Predators: " << pred_count 
                      << " (Total: " << total_entities << ")" << std::endl;
            Logger::info("Tick ", i+1, " Stats - Algae: ", algae_count, ", Herbivores: ", herb_count, ", Predators: ", pred_count);

            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
        }
        Logger::info("Simulation finished after ", num_ticks, " ticks.");

    } catch (const std::exception& e) {
        Logger::error("An exception occurred: ", e.what());
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return 1;
    }
    std::cout << "Simulation finished. Press Enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    std::cin.get(); 
    return 0;
}