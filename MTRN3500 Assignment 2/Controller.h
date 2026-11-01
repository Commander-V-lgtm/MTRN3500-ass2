#pragma once

#include <UGVModule.h>

ref class Controller : public UGVModule {
public:
    // Constructor to set up members
    Controller(SM_ThreadManagement^ SM_TM);

    // Send/Recieve data from shared memory structures
    // Process heartbeat updates
    error_state processSharedMemory() override;

    // Shutdown all modules in the software
    void shutdownModules();

    // Get Shutdown signal for module, from Thread Management SM
    bool getShutdownFlag() override;

    // Thread function for TMM
    void threadFunction() override;

private:
    // Additional data members or helper function
    SM_ThreadManagement^ SM_TM_;
    Stopwatch^ CON_Stopwatch;
};