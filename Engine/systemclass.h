#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN //a slimmer version of WIN32 to speed up the build process, excludes less used APIs

#include <Windows.h> //the Windows libs for window management and shizz

#include "inputclass.h" //our input class
#include "graphicsclass.h" //our graphics class
#include "timerclass.h"

class SystemClass
{
public:
	SystemClass(); //constructor
	SystemClass(const SystemClass&);
	~SystemClass(); //destructor

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame(unsigned long); //runs an update loop, returns wether it succeeds or not (kind of like Update() in XNA)
	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	unsigned long updateCount;

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	// a pointer to an input class and a graphics class and a timer class
	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	TimerClass* m_Timer;
};

// FUNCTION PROTOTYPES (kind of like abstract in C#)
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// GLOBALS
static SystemClass* ApplicationHandle = 0; //global handle for our window

#endif