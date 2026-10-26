#pragma once

#include <NetworkedModule.h>

ref class VehicleControl : public NetworkedModule {
public:
    // Create shared memory objects
    error_state setupSharedMemory();

    // Send/Recieve data from shared memory structures
    error_state processSharedMemory() override;

    // Shutdown all modules in the software
    void shutdownModules();

    // Get Shutdown signal for module, from Thread Management SM
    bool getShutdownFlag() override;

    // Thread function for TMM
    void threadFunction() override;

    error_state connect(String^ hostName, int portNumber) override;	// Establish TCP connection
    error_state communicate() override;

private:
    // Add any additional data members or helper functions here
};