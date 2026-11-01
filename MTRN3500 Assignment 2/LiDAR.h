#pragma once

#include <NetworkedModule.h>

ref class LiDAR : public NetworkedModule {
public:
    // Constructor to set up members
    LiDAR(SM_ThreadManagement^ SM_TM, SM_Laser^ SM_lidar);
    
    // Send/Recieve data from shared memory structures
    // Also process heartbeat updates
    error_state processSharedMemory() override;

    // Shutdown all modules in the software
    void shutdownModules();

    // Get Shutdown signal for module, from Thread Management SM
    bool getShutdownFlag() override;

    // Thread function for LiDAR
    void threadFunction() override;

    error_state connect(String^ hostName, int portNumber) override;	// Establish TCP connection
    error_state disconnect() override;                              // Quick disconnect of TCP
    error_state communicate() override;

private:
    // Additional data members or helper functions
    SM_Laser^ SM_LASER_;
    Stopwatch^ LASER_Stopwatch;
    String^ LMDscandata;
    array<double>^ RangeX;
    array<double>^ RangeY;
};