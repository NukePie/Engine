#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	//FILELog::ReportingLevel() = logDEBUG3;
	//FILE* log_fd = fopen( "mylogfile.txt", "w" );
	//Output2FILE::Stream() = log_fd;
	SystemClass* System; //pointer to a System object
	bool result;

	System = new SystemClass; //create new System object where the pointer is pointing

	if(!System) //if system is null
	{
		return 0;
	}

	result = System->Initialize(); //run the initialize method in the object that System is pointing at
	if(result) //if it returned true
	{
		System->Run(); //start the program loop by running Run in the object that system is pointing at
	}

	System->Shutdown(); //clean System object
	delete System; //remove system object
	System = 0; //set pointer to null

	return 0; //exit da program
}