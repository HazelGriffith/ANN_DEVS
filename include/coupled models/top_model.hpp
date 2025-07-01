#ifndef __TOP_MODEL_HPP__
#define __TOP_MODEL_HPP__

// This is a coupled model, meaning it has no internal computation, and is
// used to connect atomic models.  So, it is necessary to include coupled.hpp
#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"
#include <iostream>
#include <string>
#include <cassert>

// We include any models that are directly contained within this coupled model
#include "VCCoupledModel.hpp"

// data structures

using namespace std;

namespace cadmium::vendor_Space {
    class Top_model : public Coupled {
        public:
        Top_model(const std::string& id, int initial_stock, int depth): Coupled(id){





            vector<shared_ptr<VCCoupled>> VCs;
            VCs.push_back(addComponent<VCCoupled>("VC"+to_string(1), 1, initial_stock));
            int count = 1;
            for (int i = 2; i <= depth; i++){
                for (int j = 1; j <= i; j++){
                    VCs.push_back(addComponent<VCCoupled>("VC"+to_string(count+j), count+j, initial_stock));
                    if (j > 1){
                        addCoupling(VCs[count+j-1]->outVendorProd1, VCs[count+j-i-1]->inVendorProd2);
                        addCoupling(VCs[count+j-i-1]->outVendorReq2, VCs[count+j-1]->inVendorReq1);
                    } 
                    if (j < i){
                        addCoupling(VCs[count+j-1]->outVendorProd2, VCs[count+j-i]->inVendorProd1);
                        addCoupling(VCs[count+j-i]->outVendorReq1, VCs[count+j-1]->inVendorReq1);
                    }
                }
                count+=i;
            }
        }
    };
} // namespace cadmium::vendor_Space

#endif // __TOP_MODEL_HPP__
