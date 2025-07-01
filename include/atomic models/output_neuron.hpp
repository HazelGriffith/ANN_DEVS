#ifndef __NEURON_HPP__
#define __NEURON_HPP__

// This is an atomic model, meaning it has its' own internal logic/computation
// So, it is necessary to include atomic.hpp
#include "cadmium/modeling/devs/atomic.hpp"
#include <iostream>
#include <limits>
#include <assert.h>
#include <random>
#include <chrono>

using namespace std;

namespace cadmium::Neuron_Space {
	
	//enum for representing the states
	enum class Neuron_States {    
	Forward_Pass,
	Activating,
	Backward_Pass,
	Updating
	}; //end of the enum


	// Function to convert the enum for states to an enum to the corresponding string
	std::string enumToString(Neuron_States state) {
		switch (state) {
			case Neuron_States::Forward_Pass:
				return "Forward_Pass" ;

			case Neuron_States::Activating:
				return "Activating" ;
			
			case Neuron_States::Backward_Pass:
				return "Backward_Pass" ;

			case Neuron_States::Updating:
				return "Updating" ;

			default:
				return "invalid";
		}
	}

	struct NeuronState {

		// sigma is a mandatory variable, used to advance the time of the simulation
		double sigma;
		Neuron_States ___current_state___;

		string activation_function;
		string loss_function;
		double weighted_sum;
		double prediction;
		double error;
		int inputs_received;
		
		vector<double> weights;

		// Set the default values for the state constructor for this specific model
		NeuronState(): sigma(0), ___current_state___(Neuron_States::Forward_Pass), activation_function("sigmoid"), loss_function("MSE"), weights({}), weighted_sum(0), inputs_received(0), prediction(0){};
	};

	std::ostream& operator<<(std::ostream &out, const NeuronState& state) {
		out << "[-|name|-]" << "Neuron" << "[-|name|-]";
    	out << "[-|state|-]" << enumToString(state.___current_state___) << "[-|state|-]";
    	out << "[-|sigma|-]" << state.sigma << "[-|sigma|-]";
		out << "[-|activation_function|-]" << state.activation_function << "[-|activation_function|-]";
		out << "[-|weighted_sum|-]" << state.weighted_sum << "[-|weighted_sum|-]";
		out << "[-|inputs_received|-]" << state.inputs_received << "[-|inputs_received|-]";
    	return out;
	}

	// Atomic model of Neuron
	class Neuron: public Atomic<NeuronState> {
		private:

		public:

			// Declare ports for the model

			vector<Port<double>> forward_inputs;
			vector<Port<double>> backward_inputs;

        	Port<double> forward_output;
			Port<double> backward_output;

			// Declare variables for the model's behaviour

			double bias = 1;
			int num_of_inputs = 0;
			int num_of_outputs = 0;

			/**
			 * Constructor function for this atomic model, and its respective state object.
			 *
			 * For this model, both a Neuron object and a Neuron object
			 * are created, using the same id.
			 *
			 * @param id ID of the new Neuron model object, will be used to identify predictions on the output file
			 */
			Neuron(const string& id, int num_inputs, int num_outputs, string activation_function_in, string loss_function_in): Atomic<NeuronState>(id, NeuronState()) {
				
				unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
				minstd_rand0 generator(seed1);
				uniform_real_distribution<> probDist(0.0,1.0);

				// Initialize ports for the model
				if (num_inputs < 1){
					assert(("Must have at least 1 input", false));
				} else {
					for (int i = 0; i < num_inputs; i++){
						forward_inputs.push_back(addInPort<double>("FInput"+to_string(i)));
						state.weights.push_back(probDist(generator));
					}
					state.weights.push_back(probDist(generator));
				}

				if (num_outputs < 0){
					assert(("Cannot have negative number of outputs", false));
				} else {
					for (int i = 0; i < num_outputs; i++){
						backward_inputs.push_back(addInPort<double>("BInput"+to_string(i)));
					}
				}
				

				forward_output = addOutPort<double>("FOutput");
				backward_output = addOutPort<double>("BOutput");

				// Initialize variables for the model's behaviour
				num_of_inputs = num_inputs;
				num_of_outputs = num_outputs;
				state.___current_state___ = Neuron_States::Forward_Pass; //Initial state is Forward_Pass
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
			void internalTransition(NeuronState& state) const override {
				
				switch(state.___current_state___){
					case Neuron_States::Forward_Pass:
						break;
					case Neuron_States::Activating:
						state.___current_state___ = Neuron_States::Backward_Pass;
						state.inputs_received = 0;
						state.weighted_sum = 0;
						if (num_of_outputs > 0){
							state.sigma = numeric_limits<double>::infinity();
						} else {
							if (state.loss_function.compare("MSE") == 0){
								state.error = 
							}
						}
						
						break;
					case Neuron_States::Backward_Pass:
						break;
					case Neuron_States::Updating:
						state.___current_state___ = Neuron_States::Forward_Pass;
						state.weighted_sum += bias*state.weights.back();
						state.inputs_received = 0;
						state.sigma = numeric_limits<double>::infinity();
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
			void externalTransition(NeuronState& state, double e) const override {
				
				if (state.___current_state___ == Neuron_States::Forward_Pass){
					for (int i = 0; i < forward_inputs.size(); i++){
						if(!forward_inputs[i]->empty()){
							state.inputs_received++;
							// The variable x is created to handle the external input values in sequence.
							// The getBag() function is used to get the next input value.
							for( const auto x : forward_inputs[i]->getBag()){
								state.weighted_sum += state.weights[i]*x;
								
							}
						}
					}
					if (state.inputs_received == forward_inputs.size()){
						state.___current_state___ = Neuron_States::Activating;
						if (state.activation_function.compare("sigmoid") == 0){
							// SIGMOID ACTIVATION FUNCTION
							state.prediction = 1/(1+exp(state.weighted_sum));
						} else if (state.activation_function.compare("relu") == 0){
							// RELU ACTIVATION FUNCTION
							state.prediction = (state.weighted_sum + abs(state.weighted_sum))/2;
						} else {
							assert(("Not a valid activation function", false));
						}
						state.sigma = 1;
					}
				} else if (state.___current_state___ == Neuron_States::Backward_Pass){
					for (int i = 0; i < backward_inputs.size(); i++){
						if(!backward_inputs[i]->empty()){
							state.inputs_received++;
							// The variable x is created to handle the external input values in sequence.
							// The getBag() function is used to get the next input value.
							for( const auto x : backward_inputs[i]->getBag()){
								state.weighted_sum += x;
							}
						}
					}
					if (state.inputs_received == backward_inputs.size()){
						state.___current_state___ = Neuron_States::Updating;
						state.sigma = 1;
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
			void output(const NeuronState& state) const override {
				switch(state.___current_state___){
					case Neuron_States::Forward_Pass:
						break;
					case Neuron_States::Activating:
						if (num_of_outputs > 0){
							forward_output->addMessage(state.prediction);
						}
						break;
					case Neuron_States::Backward_Pass:
						break;
					case Neuron_States::Updating:
						backward_output->addMessage(state.error);
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
			[[nodiscard]] double timeAdvance(const NeuronState& state) const override {
				return state.sigma;
			}
	};
};
#endif // __NEURON_HPP__