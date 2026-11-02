#include "TMM.h"

int main(void) {
	ThreadManagement^ telBOT = gcnew ThreadManagement();

	telBOT->setupSharedMemory();
	telBOT->threadFunction();

	Console::WriteLine("Program finished. Press any key to continue...");
	Console::ReadKey();
	return 0;
}