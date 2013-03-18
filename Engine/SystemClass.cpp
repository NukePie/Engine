#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	updateCount = 0;
	m_Timer = 0;
}

//creating a copy constructor and a destructor since some compilers will otherwise make their own which could cause unexpected behaviour
SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//initialize width and height to 0
	screenWidth = 0;
	screenHeight = 0;

	//initialize the Windows API
	InitializeWindows(screenWidth, screenHeight);

	m_Input = new InputClass; //create input object, handles keyboard input

	if(!m_Input)
	{
		return false;
	}

	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight); //initializes input object
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object", L"Error", MB_OK);
		return false;
	}

	m_Graphics = new GraphicsClass; //create graphics object, handles graphics rendering
	if(!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd, m_hinstance); //initializes the graphics object
	if(!result)
	{
		return false;
	}

	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	if(m_Input)
	{
		m_Input->ShutDown();
		delete m_Input;
		m_Input = 0;
	}

	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame(0);
			if(!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool SystemClass::Frame(unsigned long updateCount)
{
	bool result;

	result = m_Graphics->Frame(updateCount); //do frame processing for the graphics object
	if(!result) //if frame couldn't run, return false
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, uint, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);

	m_applicationName = L"The Matrix";

	wc.style = CS_HREDRAW | CS_VREDRAW; //the style of the window, these two redraws the window if a movement or size adjustment changes the height or the width of the client area, also other stuff can be here
	wc.lpfnWndProc = WndProc; //the window message receiver
	wc.cbClsExtra = 0; //extra bytes if you would want those for some reason
	wc.cbWndExtra = 0; //extra bytes for the window instance for some purpose, I guess?
	wc.hInstance = m_hinstance; //a handle to the instance that contains the window procedure for the class, don't quite know what that shizz means
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); //these two are icons, for sure
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //the cursor that's used in the application
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //background color
	wc.lpszMenuName = NULL; //if you want a menu bar, this is it
	wc.lpszClassName = m_applicationName; //something something name
	wc.cbSize = sizeof(WNDCLASSEX); //the size of this structure, can only be this

	RegisterClassEx(&wc); //send our settings to the baws (Windows)

	//put the resolution of the desktop into variables
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(FULL_SCREEN)
	{
		//set the application to cover the desktop and use 32 bits
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN); //set it to full screen

		posX = posY = 0; //position the window in the top left corner
	}
	else
	{
		//set the resolution to 800 x 600
		screenWidth = 800;
		screenHeight = 600;

		//place the window in the middle 
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//create dat window
	m_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		m_applicationName,
		m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX,
		posY,
		screenWidth,
		screenHeight,
		NULL,
		NULL,
		m_hinstance,
		NULL
		);

	ShowWindow(m_hwnd, SW_SHOW); //show dat window
	SetForegroundWindow(m_hwnd); //put dat window in the foreground
	SetFocus(m_hwnd); //focus on dat window

	ShowCursor(true); //show dat cursor

	return;
}

//Returns screem settings settings to normal and releases the window and its handles
void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//remove the application instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//release the pointer to this class
	ApplicationHandle = NULL;

	return;
}

//grabs all messages from Windows and quits if the message tells it to, otherwise it sends it to our message handler
LRESULT CALLBACK WndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
	switch (uint)
	{
		case WM_DESTROY: //if its being destroyed
			{
				PostQuitMessage(0);
				return 0;
			}

		case WM_CLOSE: //if its being closed
			{
				PostQuitMessage(0);
				return 0;
			}

		default: //any other message gets sent to our message handler
			{
				return ApplicationHandle->MessageHandler(hwnd, uint, wparam, lparam);
			}
	}
}