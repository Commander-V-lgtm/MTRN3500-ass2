#include "CrashAvoidance.h"

// Constructor to set up members
CrashAvoidance::CrashAvoidance(SM_ThreadManagement^ SM_TM) {

}

// Send/Recieve data from shared memory structures
// Process heartbeat updates
error_state CrashAvoidance::processSharedMemory() {
	return error_state::SUCCESS;
}

// Shutdown all modules in the software
void CrashAvoidance::shutdownModules() {
	SM_TM_->shutdown = bit_ALL;
}

// Get Shutdown signal for module, from Thread Management SM
bool CrashAvoidance::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_CRASHAVOIDANCE);
}

// Thread function for TMM
void CrashAvoidance::threadFunction() {

}