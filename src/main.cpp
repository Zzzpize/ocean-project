#include <iostream>
#include "ocean.hpp"
#include "algae.hpp"
#include "herbivore.hpp"
#include <memory>
#include <chrono>   
#include <thread>  
#include <cstdlib> 

int main() {
    try {
        Ocean myOcean(15, 30); 

        for(int i=0; i < 20; ++i) {
            myOcean.addEntity(std::make_unique<Algae>(), Random::getInt(0, myOcean.getRows()-1), Random::getInt(0, myOcean.getCols()-1));
        }

        for(int i=0; i < 5; ++i) { 
            myOcean.addEntity(std::make_unique<HerbivoreFish>(), Random::getInt(0, myOcean.getRows()-1), Random::getInt(0, myOcean.getCols()-1));
        }
        
        const int num_ticks = 100; 
        for (int i = 0; i < num_ticks; ++i) {
            system("cls"); 
            std::cout << "Tick: " << i + 1 << " (Energy/Age limits: " 
                      << Config::HERBIVORE_MAX_ENERGY << "/" << Config::HERBIVORE_MAX_AGE << ")" << std::endl;
            
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