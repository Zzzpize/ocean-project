#include <iostream>
#include "ocean.hpp"
#include "algae.hpp"
#include "herbivore.hpp"
#include "predator.hpp"
#include "utils/random.hpp"
#include <memory>
#include <chrono>   
#include <thread>  
#include <cstdlib> 
#include <limits>

int main() {
    try {
        Ocean myOcean(15, 30); 

        for(int i=0; i < 40; ++i) {
            myOcean.addEntity(std::make_unique<Algae>(), Random::getInt(0, myOcean.getRows()-1), Random::getInt(0, myOcean.getCols()-1));
        }

        for(int i=0; i < 10; ++i) { 
            myOcean.addEntity(std::make_unique<HerbivoreFish>(), Random::getInt(0, myOcean.getRows()-1), Random::getInt(0, myOcean.getCols()-1));
        }

        for(int i=0; i < 3; ++i) {
             myOcean.addEntity(std::make_unique<PredatorFish>(), Random::getInt(0, myOcean.getRows()-1), Random::getInt(0, myOcean.getCols()-1));
        }
        
        const int num_ticks = 200; 
        for (int i = 0; i < num_ticks; ++i) {
            system("cls"); 
            std::cout << "Tick: " << i + 1 << " (Energy/Age limits: " << Config::HERBIVORE_MAX_ENERGY << "/" << Config::HERBIVORE_MAX_AGE << ")" << std::endl;
            int algae_count = 0, herb_count = 0, pred_count = 0;
            for (int r=0; r < myOcean.getRows(); ++r) for (int c=0; c < myOcean.getCols(); ++c) {
                Entity* e = myOcean.getEntity(r,c);
                if (e) {
                    if (e->getType() == EntityType::ALGAE) algae_count++;
                    else if (e->getType() == EntityType::HERBIVORE) herb_count++;
                    else if (e->getType() == EntityType::PREDATOR) pred_count++;
                }
            }
            std::cout << "Algae: " << algae_count << ", Herbivores: " << herb_count << ", Predators: " << pred_count << std::endl;
            myOcean.tick();    
            myOcean.display(); 

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

    } catch (const std::exception& e) {
        std::cerr << "An exception occurred: " << e.what() << std::endl;
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