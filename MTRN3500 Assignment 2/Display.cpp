#include "Display.h"

// Constructor to set up members
Display::Display(SM_ThreadManagement^ SM_TM) {

}

// Send/Recieve data from shared memory structures
// Process heartbeat updates
error_state Display::processSharedMemory() {
	return error_state::SUCCESS;
}

// Shutdown all modules in the software
void Display::shutdownModules() {
	SM_TM_->shutdown = bit_ALL;
}

// Get Shutdown signal for module, from Thread Management SM
bool Display::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_DISPLAY);
}

// Thread function for TMM
void Display::threadFunction() {
}

// Establish TCP connection
error_state Display::connect(String^ hostName, int portNumber) {
	return error_state::SUCCESS;
}

// Quick disconnect of TCP
error_state Display::disconnect() {
	return error_state::SUCCESS;
}

error_state Display::communicate() {
	return error_state::SUCCESS;
}