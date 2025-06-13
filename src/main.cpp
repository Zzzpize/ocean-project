#include <iostream>
#include <vector>
#include <memory>
#include <chrono>   
#include <thread>  
#include <cstdlib> 
#include <limits> 
#include <string> 
#include <stdexcept> 

#include "ocean.hpp"
#include "algae.hpp"
#include "herbivore.hpp"
#include "predator.hpp"
#include "utils/random.hpp" 
#include "utils/logger.hpp" 
#include "utils/resource_wrapper.hpp" 

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
    Logger::info("Simulation starting...");

    demonstrateRuleOfFive(); 

    std::cout << "\nPress Enter to start ocean simulation...\n";
    std::cin.get(); 

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
        for(int i=0; i < 4; ++i) { 
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
                            case EntityType::ALGAE: algae_count++; break;
                            case EntityType::HERBIVORE: herb_count++; break;
                            case EntityType::PREDATOR: pred_count++; break;
                            default: break;
                        }
                    }
                }
            }
            std::cout << "Statistics: Algae: " << algae_count 
                      << ", Herbivores: " << herb_count 
                      << ", Predators: " << pred_count 
                      << " (Total: " << total_entities << ")" << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
        }
        Logger::info("Simulation finished after ", num_ticks, " ticks.");

    } catch (const std::out_of_range& oor) {
        Logger::error("FATAL: Out of Range error caught in main: ", oor.what());
    } catch (const std::invalid_argument& ia) {
        Logger::error("FATAL: Invalid Argument error caught in main: ", ia.what());
    } catch (const std::runtime_error& rte) {
        Logger::error("FATAL: Runtime error caught in main: ", rte.what());
    } catch (const std::exception& e) {
        Logger::error("FATAL: An unexpected standard exception occurred in main: ", e.what());
    } catch (...) {
        Logger::error("FATAL: An unknown exception occurred in main.");
    }

    if (std::current_exception()) { 
        std::cout << "An error occurred. Press Enter to exit..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return 1;
    }

    std::cout << "Simulation finished successfully. Press Enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    std::cin.get(); 
    return 0;
}