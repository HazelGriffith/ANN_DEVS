#include "cadmium/simulation/root_coordinator.hpp"
#include <limits>
#include "cadmium/simulation/logger/csv.hpp"
#include <iostream>
#include <string>

#include "include/coupled models/top_model.hpp"

using namespace cadmium::SimpleHelo;

int main(int argc,char* argv[]){

    // Declare and initialize the top model
    auto model = std::make_shared<Top_model>("top_model");
    auto rootCoordinator = cadmium::RootCoordinator(model);
	
    //set the name of the output file
	rootCoordinator.setLogger<cadmium::CSVLogger>("simulation_results/results.csv",",");

    rootCoordinator.start();

    rootCoordinator.simulate(250.0);

    rootCoordinator.stop();
    return 0;
}
