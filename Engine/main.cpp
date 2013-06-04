#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System; // Pointer to a System object.
	bool result;

	System = new SystemClass; // Create new System object where the pointer is pointing.

	if(!System) // If system is null.
	{
		return 0;
	}

	result = System->Initialize(); // Run the initialize method in the object that System is pointing at.
	if(result) // If it returned true.
	{
		System->Run(); // Start the program loop by running Run in the object that system is pointing at.
	}

	System->Shutdown(); // Clean System object.
	delete System; // Remove system object.
	System = 0; // Set pointer to null.
	
	return 0; // Exit da program.
}