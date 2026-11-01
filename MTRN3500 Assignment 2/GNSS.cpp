#include "GNSS.h"

// Constructor to set up members
GNSS::GNSS(SM_ThreadManagement^ SM_TM, SM_GPS^ SM_GPS) {
	SM_TM_ = SM_TM;
	SM_GPS_ = SM_GPS;
	GPS_Stopwatch = gcnew Stopwatch;
}

// Send/Recieve data from shared memory structures
// Process heartbeat updates implementation apatapted from MTRN3500
error_state GNSS::processSharedMemory() {
	//check if the GPS bit in the hearbeat byte is low
	if ((SM_TM_->heartbeat & bit_GPS) == 0)
	{
		//put the LiDAR bit high
		SM_TM_->heartbeat |= bit_GPS;
		//reset the stopwatch
		GPS_Stopwatch->Restart();
	}

	// else check if the time elasped exceed the crash time limit
	else {
		if (GPS_Stopwatch->ElapsedMilliseconds > TM_CRASHLIMIT)
		{
			//shut down all threads
			shutdownModules();
			return error_state::ERR_TM_FAILURE;
		}
	}

	// GPS communcation work
	error_state ret = communicate();
	if (ret == SUCCESS)
	{
		//process the data
	}
	else
	{
		printError(ret);
		return ret;
	}

	return error_state::SUCCESS;
}

// Shutdown all modules in the software
void GNSS::shutdownModules() {
	SM_TM_->shutdown = bit_ALL;
}

// Get Shutdown signal for module, from Thread Management SM
bool GNSS::getShutdownFlag() {
	return SM_TM_->shutdown & bit_GPS;
}

// Thread function for GPS
void GNSS::threadFunction() {
	Console::WriteLine("GNSS Thread is starting");
	error_state ret = connect(WEEDER_ADDRESS, GNSS_PORT);
	if (ret != SUCCESS) {
		printError(ret);
		return;
	}

	//wait for the barrier
	SM_TM_->ThreadBarrier->SignalAndWait();

	//start the stopwatch
	GPS_Stopwatch->Start();

	//start the thread loop
	while (!getShutdownFlag())
	{
		processSharedMemory();
		Thread::Sleep(20);
	}

	// Thread ending, terminate connection
	if (Stream != nullptr) {
		Stream->Close();
		Stream = nullptr;
	}
	if (Client != nullptr) {
		Client->Close();
		Client = nullptr;
	}
	Console::WriteLine("GNSS Thread is terminated");
}

// Establish TCP connection
error_state GNSS::connect(String^ hostName, int portNumber) {
	return error_state::SUCCESS;
}

// Quick check and disconnect
error_state GNSS::disconnect() {
	if (Stream != nullptr) {
		Stream->Close();
		Stream = nullptr;
	}
	if (Client != nullptr) {
		Client->Close();
		Client = nullptr;
	}
	return error_state::SUCCESS;
}

// Communicate with the LiDAR and runs data check
error_state GNSS::communicate() {
	return error_state::SUCCESS;
}