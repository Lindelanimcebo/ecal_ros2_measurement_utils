#ifndef __DESERIALIZER__
#define __DESERIALIZER__

#include <ecalhdf5/eh5_meas.h>
#include <iostream>
#include "msgs.hpp"

namespace m2s2{ namespace ecal{ namespace deserializer{
        
class Deserializer
{
    public:
    inline explicit Deserializer(std::string meas_path, std::string channel_name){
        this->meas_ = new eCAL::eh5::HDF5Meas(meas_path);
        if (!meas_->IsOk()){
            std::cout << "Problem with measurement file." << std::endl;
        }
        this->meas_->GetEntriesInfo(channel_name, this->entry_info_set);
    }                
    virtual ~Deserializer() = default;

    virtual struct BaseMsg deserialize_message(
        eCAL::eh5::HDF5Meas* meas_,
        long long ID
    ) = 0;
    
    virtual void process_message(struct BaseMsg* msg) = 0;
    
    inline void process_all(){
        for (auto it=this->entry_info_set.begin(); it!=this->entry_info_set.end();it++)
        {   
            std::cout << "-------------------------------" << std::endl;
            struct BaseMsg msg = deserialize_message(this->meas_, it->ID);
            process_message(&msg);
        }
    }
    private:
    // Measurement reading variables
    eCAL::eh5::HDF5Meas* meas_;
    eCAL::eh5::EntryInfoSet entry_info_set;
};

}}} // namespace m2s2 -> ecal -> deserializer

#endif