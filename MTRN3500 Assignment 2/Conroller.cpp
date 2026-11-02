#include "Controller.h"

// Constructor to set up members
Controller::Controller(SM_ThreadManagement^ SM_TM) {

}

// Send/Recieve data from shared memory structures
// Process heartbeat updates
error_state Controller::processSharedMemory() {
	return error_state::SUCCESS;
}

// Shutdown all modules in the software
void Controller::shutdownModules() {
	SM_TM_->shutdown = bit_ALL;
}

// Get Shutdown signal for module, from Thread Management SM
bool Controller::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_CONTROLLER);
}

// Thread function for TMM
void Controller::threadFunction() {

}