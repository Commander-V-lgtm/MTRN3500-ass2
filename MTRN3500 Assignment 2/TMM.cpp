#include "TMM.h"
#include "Controller.h"
#include "CrashAvoidance.h"
#include "LiDAR.h"
#include "GNSS.h"
#include "VC.h"
#include "Display.h"

error_state ThreadManagement::setupSharedMemory() {
	SM_TM_ = gcnew SM_ThreadManagement;
	SM_LASER_ = gcnew SM_Laser;
	SM_GPS_ = gcnew SM_GPS;
	SM_VC_ = gcnew SM_VehicleControl;

	return error_state::SUCCESS;
}

// Send/Recieve data from shared memory structures
// Code implementation for processing heartbeat as TM
// Adapted from MTRN3500 lecture code
error_state ThreadManagement::processSharedMemory() {
	for (int i = 0; i < ThreadList->Length; i++)
	{
		// check the heartbeat flag of the ith thread (is it high?)
		if (SM_TM_->heartbeat & ThreadPropertiesList[i]->BitID)
		{
			// if high, put the ith bit (flag) down
			SM_TM_->heartbeat &= ~ThreadPropertiesList[i]->BitID;
			// reset the stopwatch
			TM_StopwatchList[i]->Restart();
		}

		// else check the stopwatch, is the time exceed the crash limit?
		else
		{
			if (TM_StopwatchList[i]->ElapsedMilliseconds > ThreadPropertiesList[i]->CrashLimit)
			{
				//is the ith thread a critical one?
				if (ThreadPropertiesList[i]->Critical) {
					Console::WriteLine(ThreadPropertiesList[i]->ThreadName + " (critical) failure. Shutting down all threads");
					shutdownModules(); // shut down all
					return error_state::ERR_CRITICAL_THREAD_FAILURE;
				}

				else {
					Console::WriteLine(ThreadPropertiesList[i]->ThreadName + " failed...Restarting");
					ThreadList[i]->Abort(); // shut down failed thread
					ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
					SM_TM_->ThreadBarrier = gcnew Barrier(1);
					ThreadList[i]->Start(); // try to restart
				}
			}
		}
	}
	return error_state::SUCCESS;
}

void ThreadManagement::shutdownModules() {
	SM_TM_->shutdown = bit_ALL;
}

bool ThreadManagement::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_TM);
}

// Main code implementation of TM function
// Code adapted from MTRN3500 lecture
void ThreadManagement::threadFunction() {
	Console::WriteLine("TM Thread is starting");

	// make a list of thread properties
	// Moddify here to add or remove threads
	ThreadPropertiesList = gcnew array<ThreadProperties^> {
		gcnew ThreadProperties(gcnew ThreadStart(gcnew LiDAR(SM_TM_, SM_LASER_), &LiDAR::threadFunction), 1000, bit_LASER, "LiDAR Thread"),
		gcnew ThreadProperties(gcnew ThreadStart(gcnew GNSS(SM_TM_, SM_GPS_), &GNSS::threadFunction), 1000, bit_GPS, "GNSS Thread"),
		//gcnew ThreadProperties(gcnew ThreadStart(gcnew VehicleControl(SM_TM_, SM_VC_), &VehicleControl::threadFunction), 1000, bit_VC, "VehicleControl Thread"),
		//gcnew ThreadProperties(gcnew ThreadStart(gcnew Controller(SM_TM_), &Controller::threadFunction), 1000, bit_CONTROLLER, "Controller Thread"),
		//gcnew ThreadProperties(gcnew ThreadStart(gcnew Display(SM_TM_), &Display::threadFunction), 1000, bit_DISPLAY, "Display Thread"),
		//gcnew ThreadProperties(gcnew ThreadStart(gcnew CrashAvoidance(SM_TM_), &CrashAvoidance::threadFunction), 1000, bit_CRASHAVOIDANCE, "CrashAvoidance Thread"),
	};

	// make a list of thread
	ThreadList = gcnew array<Thread^>(ThreadPropertiesList->Length);

	// make the stopwatch list
	TM_StopwatchList = gcnew array<Stopwatch^>(ThreadPropertiesList->Length);

	// make thread barriers
	SM_TM_->ThreadBarrier = gcnew Barrier(ThreadPropertiesList->Length + 1);

	// start all the threads
	for (int i = 0; i < ThreadPropertiesList->Length; i++) {
		TM_StopwatchList[i] = gcnew Stopwatch;
		ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_); // ThreadPropertiesList[0]->ThreadStart_ = ThreadStart(gcnew LiDAR(), &LiDAR::threadFunction)
		ThreadList[i]->Start();
	}

	// wait at the TMT thread barrier
	SM_TM_->ThreadBarrier->SignalAndWait();

	// start all the stopwatches
	for (int i = 0; i < ThreadList->Length; i++)
	{
		TM_StopwatchList[i]->Start();
	}

	// start the thread loop + keep checking the heartbeats
	// Modify here to add shit down conditios
	bool quit = false;
	while (!quit && !getShutdownFlag())
	{
		// Check quit condition
		if (Console::KeyAvailable) {
			ConsoleKeyInfo key = Console::ReadKey(true);
			if (key.KeyChar == 'q') {
				quit = true;
			}
		}
		// Main loop fn
		processSharedMemory();
		Thread::Sleep(50);
	}

	// end the thread loop + shut down all threads
	shutdownModules();

	// join all threads, ensure all quits
	for (int i = 0; i < ThreadPropertiesList->Length; i++)
	{
		ThreadList[i]->Join();
	}

	Console::WriteLine("TM Thread is terminated");
}