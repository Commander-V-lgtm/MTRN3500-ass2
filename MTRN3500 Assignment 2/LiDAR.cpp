#include "LiDAR.h"

// Constructor to set up members
LiDAR::LiDAR(SM_ThreadManagement^ SM_TM, SM_Laser^ SM_lidar) {
	SM_TM_ = SM_TM;
	SM_LASER_ = SM_lidar;
	LASER_Stopwatch = gcnew Stopwatch;
	ReadData = gcnew array<unsigned char>(RecvBufferSize);
	RangeX = gcnew array<double>(STANDARD_LASER_LENGTH);
	RangeY = gcnew array<double>(STANDARD_LASER_LENGTH);
}

// Send/Recieve data from shared memory structures
// Process heartbeat updates implementation apatapted from MTRN3500
error_state LiDAR::processSharedMemory() {
	//check if the LiDAR bit in the hearbeat byte is low
	if ((SM_TM_->heartbeat & bit_LASER) == 0)
	{
		//put the LiDAR bit high
		SM_TM_->heartbeat |= bit_LASER;
		//reset the stopwatch
		LASER_Stopwatch->Restart();
	}

	// else check if the time elasped exceed the crash time limit
	else {
		if (LASER_Stopwatch->ElapsedMilliseconds > TM_CRASHLIMIT)
		{
			//shut down all threads
			shutdownModules();
			return error_state::ERR_TM_FAILURE;
		}
	}

	// LiDAR communcation work
	error_state ret = communicate();
	if (ret == SUCCESS)
	{
		// Push the data to share memory (try)
		Monitor::Enter(SM_LASER_->lockObject);
		try {
			Array::Copy(RangeX, SM_LASER_->x, STANDARD_LASER_LENGTH);
			Array::Copy(RangeY, SM_LASER_->y, STANDARD_LASER_LENGTH);
		}
		catch(Exception^ ex) {
			Console::WriteLine("Push data failed: " + ex->Message);
			return error_state::ERR_INVALID_DATA;
		}
		finally {
			Monitor::Exit(SM_LASER_->lockObject);
		}
	}
	else
	{
		printError(ret);
		return ret;
	}

	return error_state::SUCCESS;
}

// Shutdown all modules in the software
void LiDAR::shutdownModules() {
	SM_TM_->shutdown = bit_ALL;
}

// Get Shutdown signal for module, from Thread Management SM
bool LiDAR::getShutdownFlag() {
	return SM_TM_->shutdown & bit_LASER;
}

// Thread function for LiDAR
void LiDAR::threadFunction() {
	Console::WriteLine("LiDAR Thread is starting");
	error_state ret = connect(WEEDER_ADDRESS, LIDAR_PORT);
	if (ret != SUCCESS) {
		printError(ret);
		return;
	}

	//wait for the barrier
	SM_TM_->ThreadBarrier->SignalAndWait();

	//start the stopwatch
	LASER_Stopwatch->Start();

	//start the thread loop
	while (!getShutdownFlag())
	{
		error_state ret = processSharedMemory();
		if (ret != SUCCESS) {
			printError(ret);
			shutdownModules();
		}
	}

	// Thread ending, terminate connection
	disconnect();
	Console::WriteLine("LiDAR Thread is terminated");
}

// Establish TCP connection
error_state LiDAR::connect(String^ hostName, int portNumber) {
	Client = gcnew TcpClient(hostName, portNumber);
	Client->SendBufferSize = SendBufferSize;
	Client->ReceiveBufferSize = RecvBufferSize;
	Client->SendTimeout = SendTimeout;
	Client->ReceiveTimeout = RecvTimeout;
	Client->NoDelay = NoDelay;
	Stream = Client->GetStream();
	if (Stream == nullptr) {
		disconnect();
		return error_state::ERR_STARTUP;
	}

	// Autentication
	array<uint8_t>^ SendData = Text::Encoding::ASCII->GetBytes(zID);
	Stream->Write(SendData, 0, SendData->Length);
	Threading::Thread::Sleep(10);

	int AuthenRep = Stream->Read(ReadData, 0, ReadData->Length);
	Threading::Thread::Sleep(25);

	if (AuthenRep == 0) return error_state::ERR_AUTHENTICATION;
	String^ AuthenS = Text::Encoding::ASCII->GetString(ReadData, 0, AuthenRep);

	if (AuthenS != AUTH_AFFIRM) {
		// quit connections
		disconnect();
		return error_state::ERR_AUTHENTICATION;
	}

	return error_state::SUCCESS;
}

// Quick check and disconnect
error_state LiDAR::disconnect() {
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
// TCP connection code adapted from assignment coms
// Data processing code adapted from MTRN3500 lecture code
error_state LiDAR::communicate() {
	// Recieve data
	if (Stream == nullptr) return error_state::ERR_CONNECTION;
	array<uint8_t>^ SendData = Text::Encoding::ASCII->GetBytes("Command");
	Stream->Write(SendData, 0, SendData->Length);
	Threading::Thread::Sleep(10);

	int LidarRead = Stream->Read(ReadData, 0, ReadData->Length);
	Threading::Thread::Sleep(25);

	if (LidarRead == 0) return error_state::ERR_NO_DATA;
	LMDscandata = Text::Encoding::ASCII->GetString(ReadData, 0, LidarRead);

	// Process data
	array<wchar_t>^ Space = { ' ' };
	array<String^>^ StringArray = LMDscandata->Split(Space);

	double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
	double Resolution = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
	int NumberData = System::Convert::ToInt32(StringArray[25], 16);
	if (NumberData != STANDARD_LASER_LENGTH) return error_state::ERR_INVALID_DATA;
	array <double >^ Range = gcnew array <double >(NumberData);
	for (int i = 0; i < NumberData; i++)
	{
		Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
		RangeX[i] = Range[i] * sin(i * Resolution + StartAngle);
		RangeY[i] = -Range[i] * cos(i * Resolution + StartAngle);
	}

	return error_state::SUCCESS;
}