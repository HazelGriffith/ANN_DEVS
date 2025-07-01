#ifndef __VC_COUPLED_MODEL_HPP__
#define __VC_COUPLED_MODEL_HPP__

// This is a coupled model, meaning it has no internal computation, and is
// used to connect atomic models.  So, it is necessary to include coupled.hpp
#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"
#include <iostream>
#include <string>
#include <cassert>

// We include any models that are directly contained within this coupled model
#include "../atomic_models/customerGenerator.hpp"
#include "../atomic_models/vendor.hpp"

// data structures

using namespace std;

namespace cadmium::vendor_Space {
    class VCCoupled : public Coupled {
        public:

		Port<int> inVendorReq1;
		Port<int> inVendorReq2;
		Port<int> inVendorProd1;
		Port<int> inVendorProd2;

		Port<int> outVendorReq1;
		Port<int> outVendorReq2;
		Port<int> outVendorProd1;
		Port<int> outVendorProd2;

		
		
		
        VCCoupled(const std::string& id, int vendorNumber, int initial_stock): Coupled(id){


             // Declare and initialize all controller models (non-input/output)
			string vendorName = "vendor"+to_string(vendorNumber);
			string customerName = "customer"+to_string(vendorNumber);
			shared_ptr<Vendor> vendor = addComponent<Vendor>(vendorName, initial_stock);
			shared_ptr<Customer> fi = addComponent<Customer>(customerName);
			
			// initialize coupled model ports
			inVendorReq1 = addInPort<int>("inVendorReq1");
			inVendorReq2 = addInPort<int>("inVendorReq2");
			inVendorProd1 = addInPort<int>("inVendorProd1");
			inVendorProd2 = addInPort<int>("inVendorProd2");

			outVendorReq1 = addOutPort<int>("outVendorReq1");
			outVendorReq2 = addOutPort<int>("outVendorReq2");
			outVendorProd1 = addOutPort<int>("outVendorProd1");
			outVendorProd2 = addOutPort<int>("outVendorProd2");
			
            // Connect the atomic models with Internal Couplings
			addIC(vendorName, "Message_Customer", customerName, "Vendor_Message");
			addIC(vendorName, "Product_to_Customer", customerName, "Product_Received");
			addIC(customerName, "Purchase_Request", vendorName, "Customer_Request");
			addIC(customerName, "Money_Sent", vendorName, "Money_Input");
			
			// Connect the inputs of the coupled model with the inputs of some atomic model
			addEIC("inVendorReq1", vendorName, "Vendor_Request1");
			addEIC("inVendorReq2", vendorName, "Vendor_Request2");
			addEIC("inVendorProd1", vendorName, "Product_Input");
			addEIC("inVendorProd2", vendorName, "Product_Input");
			
			// Connect the outputs of the coupled model with the outputs of some atomic model
			addEOC(vendorName, "Message_Vendor1", "outVendorReq1");
			addEOC(vendorName, "Message_Vendor2", "outVendorReq2");
			addEOC(vendorName, "Product_to_Vendor1", "outVendorProd1");
			addEOC(vendorName, "Product_to_Vendor2", "outVendorProd2");
        }
    };
} // namespace cadmium::assignment1

#endif // __VC_COUPLED_MODEL_HPP__
