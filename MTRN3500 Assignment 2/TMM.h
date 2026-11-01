#pragma once

#include <UGVModule.h>

ref struct ThreadProperties
{
	ThreadStart^ ThreadStart_;
	bool Critical;
    uint16_t CrashLimit;
	String^ ThreadName;
	uint8_t BitID;

	ThreadProperties(ThreadStart^ start, uint16_t crash_limit, uint8_t bit_id, String^ threadname)
	{
		ThreadStart_ = start;
		Critical = bit_id & CRITICALMASK;
        CrashLimit = crash_limit;
		BitID = bit_id;
		ThreadName = threadname;
	}
};

ref class ThreadManagement : public UGVModule {
public:
    // Create shared memory objects
    error_state setupSharedMemory();

    // Send/Recieve data from shared memory structures
    // Process heartbeats update
    error_state processSharedMemory() override;

    // Shutdown all modules in the software
    void shutdownModules();

    // Get Shutdown signal for module, from Thread Management SM
    bool getShutdownFlag() override;

    // Thread function for TMM
    void threadFunction() override;

private:
    // Additional data members or helper functions
    SM_Laser^ SM_LASER_;
    SM_GPS^ SM_GPS_;
    SM_VehicleControl^ SM_VC_;
    array<Stopwatch^>^ TM_StopwatchList;
    array<Thread^>^ ThreadList;
    array<ThreadProperties^>^ ThreadPropertiesList;
};
