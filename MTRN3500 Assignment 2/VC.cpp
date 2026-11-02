#include "VC.h"

// Constructor to set up members
VehicleControl::VehicleControl(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VC_) {

}

// Send/Recieve data from shared memory structures
// Process heartbeat updates
error_state VehicleControl::processSharedMemory() {
	return error_state::SUCCESS;
}

// Shutdown all modules in the software
void VehicleControl::shutdownModules() {
	SM_TM_->shutdown = bit_ALL;
}

// Get Shutdown signal for module, from Thread Management SM
bool VehicleControl::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_VC);
}

// Thread function for TMM
void VehicleControl::threadFunction() {

}

// Establish TCP connection
error_state VehicleControl::connect(String^ hostName, int portNumber) {
	return error_state::SUCCESS;
}

// Quick disconnect of TCP
error_state VehicleControl::disconnect() {
	return error_state::SUCCESS;
}

error_state VehicleControl::communicate() {
	return error_state::SUCCESS;
}