#include <iostream>
#include "ocean.hpp"
#include "algae.hpp"
#include "herbivore.hpp"
#include "predator.hpp"
#include <memory>

int main() {
    try {
        Ocean myOcean(10, 20); 

        myOcean.addEntity(std::make_unique<Algae>(), 2, 2);
        myOcean.addEntity(std::make_unique<Algae>(), 2, 3);
        myOcean.addEntity(std::make_unique<Algae>(), 5, 10);

        myOcean.addEntity(std::make_unique<HerbivoreFish>(), 3, 5);

        myOcean.addEntity(std::make_unique<PredatorFish>(), 7, 15);

        std::cout << "Initial: " << std::endl;
        myOcean.display();

        std::cout << "Moving herbivore from (3,5) to (3,6)..." << std::endl;
        if (myOcean.moveEntity(3, 5, 3, 6)) {
             std::cout << "Move successful." << std::endl;
        } else {
             std::cout << "Move failed." << std::endl;
        }
        myOcean.display();

        std::cout << "Removing algae from (2,2)..." << std::endl;
        std::unique_ptr<Entity> removedAlgae = myOcean.removeEntity(2,2);
        if (removedAlgae) {
             std::cout << "Algae removed successfully. Type: " << static_cast<int>(removedAlgae->getType()) << std::endl;
        } else {
             std::cout << "Failed to remove algae or cell was empty." << std::endl;
        }
        myOcean.display();

        myOcean.tick();


    } catch (const std::exception& e) {
        std::cerr << "An exception occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}