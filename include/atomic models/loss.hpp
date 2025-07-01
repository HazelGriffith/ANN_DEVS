#ifndef __LOSS_HPP__
#define __LOSS_HPP__

// This is an atomic model, meaning it has its' own internal logic/computation
// So, it is necessary to include atomic.hpp
#include "cadmium/modeling/devs/atomic.hpp"
#include <iostream>
#include <limits>
#include <assert.h>
#include <random>
#include <chrono>

using namespace std;

namespace cadmium::ANN_Space {
	
	//enum for representing the states
	enum class Loss_States {    
	Waiting_for_Input,
	Calculating_Loss
	}; //end of the enum


	// Function to convert the enum for states to an enum to the corresponding string
	std::string enumToString(Loss_States state) {
		switch (state) {
			case Loss_States::Waiting_for_Input:
				return "Waiting_for_Input" ;

			case Loss_States::Calculating_Loss:
				return "Calculating_Loss";

			default:
				return "invalid";
		}
	}

	struct LossState {

		// sigma is a mandatory variable, used to advance the time of the simulation
		double sigma;
		Loss_States ___current_state___;

		string loss_function;
		int inputs_received;

		// Set the default values for the state constructor for this specific model
		LossState(): sigma(0), ___current_state___(Loss_States::Waiting_for_Input), loss_function("log_loss"), inputs_received(0){};
	};

	std::ostream& operator<<(std::ostream &out, const LossState& state) {
		out << "[-|name|-]" << "Loss" << "[-|name|-]";
    	out << "[-|state|-]" << enumToString(state.___current_state___) << "[-|state|-]";
    	out << "[-|sigma|-]" << state.sigma << "[-|sigma|-]";
		out << "[-|loss_function|-]" << state.loss_function << "[-|loss_function|-]";
		out << "[-|inputs_received|-]" << state.inputs_received << "[-|inputs_received|-]";
    	return out;
	}

	// Atomic model of Loss
	class Loss: public Atomic<LossState> {
		private:

		public:

			// Declare ports for the model

			vector<Port<double>> inputs;

			Port<double> error;
        	Port<double> output;

			// Declare variables for the model's behaviour

			double bias = 1;

			/**
			 * Constructor function for this atomic model, and its respective state object.
			 *
			 * For this model, both a Loss object and a Loss object
			 * are created, using the same id.
			 *
			 * @param id ID of the new Loss model object, will be used to identify results on the output file
			 */
			Loss(const string& id, int num_inputs, string activation_function_in): Atomic<LossState>(id, LossState()) {
				
				unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
				minstd_rand0 generator(seed1);
				uniform_real_distribution<> probDist(0.0,1.0);

				// Initialize ports for the model
				for (int i = 0; i < num_inputs; i++){
					inputs.push_back(addInPort<double>("Input"+to_string(i)));
					state.weights.push_back(probDist(generator));
				}
				state.weights.push_back(probDist(generator));
				error = addInPort<double>("Error");
				output = addOutPort<double>("Output");

				// Initialize variables for the model's behaviour
				state.___current_state___ = Loss_States::Waiting_for_Input; //Initial state is Waiting_for_Input
				state.activation_function = activation_function_in;
				state.weighted_sum += bias*state.weights.back();

				state.sigma = numeric_limits<double>::infinity();
				
			}

			/**
			 * The transition function is invoked each time the value of
			 * state.sigma reaches 0.
			 *
			 * In this model, the value of state.lightOn is toggled.
			 *
			 * @param state reference to the current state of the model.
			 */
			void internalTransition(LossState& state) const override {
				
				switch(state.___current_state___){
					case Loss_States::Waiting_for_Input:
						break;
					case Loss_States::Activating:
						state.___current_state___ = Loss_States::Learning;
						state.sigma = numeric_limits<double>::infinity();
						break;
					case Loss_States::Learning:
						break;
					default:
						assert(("Not a valid state", false));
						break;
				}
				
			}

			/**
			 * The external transition function is invoked each time external data
			 * is sent to an input port for this model.
			 *
			 * In this model, the value of state.fastToggle is toggled each time the
			 * button connected to the "in" port is pressed.
			 *
			 * The value of state.sigma is then updated depending on the value of
			 * state.fastToggle.  Sigma is not required to be updated in this function,
			 * but we are changing it based on our desired logic for the program.
			 *
			 * @param state reference to the current model state.
			 * @param e time elapsed since the last state transition function was triggered.
			 */
			void externalTransition(LossState& state, double e) const override {
				
				for (int i = 0; i < inputs.size(); i++){
					if(!inputs[i]->empty()){
						state.inputs_received++;
						// The variable x is created to handle the external input values in sequence.
						// The getBag() function is used to get the next input value.
						for( const auto x : inputs[i]->getBag()){
							state.weighted_sum += state.weights[i]*x;
							
						}
					}
				}

				if (state.inputs_received == inputs.size()){
					state.___current_state___ = Loss_States::Activating;
					state.sigma = 1;
				}
				

				if(!error->empty()){

					// The variable x is created to handle the external input values in sequence.
					// The getBag() function is used to get the next input value.
					for( const auto x : error->getBag()){

					}
				}

			}

			/**
			 * This function outputs any desired state values to their associated ports.
			 *
			 * In this model, the value of state.lightOn is sent via the out port.  Once
			 * the value of state.ligthOn reaches the I/O model, that model will update
			 * the status of the LED.
			 *
			 * @param state reference to the current model state.
			 */
			void output(const LossState& state) const override {
				switch(state.___current_state___){
					case Loss_States::Waiting_for_Input:
						break;
					case Loss_States::Activating:
						double result = 0;
						if (state.activation_function.compare("sigmoid") == 0){
							// SIGMOID ACTIVATION FUNCTION
							result = 1/(1+exp(state.weighted_sum));
						} else if (state.activation_function.compare("relu") == 0){
							// RELU ACTIVATION FUNCTION
							result = (state.weighted_sum + abs(state.weighted_sum))/2;
						} else {
							assert(("Not a valid activation function", false));
						}
						output->addMessage(result);
						break;
					case Loss_States::Learning:
						break;
					default:
						assert(("Not a valid state", false));
						break;
				}
			}

			/**
			 * It returns the value of state.sigma for this model.
			 *
			 * This function is the same for all models, and does not need to be changed.
			 *
			 * @param state reference to the current model state.
			 * @return the sigma value.
			 */
			[[nodiscard]] double timeAdvance(const LossState& state) const override {
				return state.sigma;
			}
	};
};
#endif // __LOSS_HPP__