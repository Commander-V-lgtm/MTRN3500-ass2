#pragma once

#include <NetworkedModule.h>

ref class VehicleControl : public NetworkedModule {
public:
    // Constructor to set up members
    VehicleControl(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VC_);

    // Send/Recieve data from shared memory structures
    // Process heartbeat updates
    error_state processSharedMemory() override;

    // Shutdown all modules in the software
    void shutdownModules();

    // Get Shutdown signal for module, from Thread Management SM
    bool getShutdownFlag() override;

    // Thread function for TMM
    void threadFunction() override;

    error_state connect(String^ hostName, int portNumber) override;	// Establish TCP connection
    error_state disconnect() override;                              // Quick disconnect of TCP
    error_state communicate() override;

private:
    // Additional data members or helper functions
    SM_VehicleControl^ SM_VC_;
    Stopwatch^ VC_Stopwatch;
};