#include "TMM.h"

int main(void) {
	ThreadManagement^ telBOT = gcnew ThreadManagement();

	telBOT->setupSharedMemory();
	telBOT->threadFunction();

	Console::ReadKey();
	return 0;
}