#pragma once

#include <NetworkedModule.h>

ref class GNSS : public NetworkedModule {
public:
    // Constructor to set up members
    GNSS(SM_ThreadManagement^ SM_TM, SM_GPS^ SM_GPS);

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
    SM_GPS^ SM_GPS_;
    Stopwatch^ GPS_Stopwatch;
};