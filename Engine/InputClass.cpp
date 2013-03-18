#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	//store the screen size which will be used for positioning the mouse cursor
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//initialize the location of the mouse on the screen
	m_mouseX = 0;
	m_mouseY = 0;

	//initialize the main direct input interface
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	//initialize the direct input interface for the keyboard
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	//set the data format, in this case since it's a kayboard we can use the predefined data format
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	//set teh cooperative level of the keybaord to not shade with other programs
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	//aquire the keyboard
	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	//initialize the direct input interface for the mouse
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	//set the data format for the mouse using the predifined mouse format
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	//set the cooporate level of the mouse to share with other programs
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	//aquire the mouse
	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}
	
	return true;
}

void InputClass::ShutDown()
{
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	//read the current state of the keyboard
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	//read the current state of the mouse
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	//process thechanges in the mouse and keyboard
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;
	//read the keyboard device
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		//if the keyboard lost focus or was not aquired then try to get control back
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


bool InputClass::ReadMouse()
{
	HRESULT result;

	//read the mouse device
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		//if the mouse lost focus or was not acquired the try to get the control back
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	//deal with the changes that have happened since the last frame
	//update the location of the mouse cursor based on the change of the mouse location during the frame
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	//ensure the mouse location doesn't exeed the sceen width and height
	if(m_mouseX < 0) {m_mouseX = 0;}
	if(m_mouseY < 0) {m_mouseY = 0;}

	if(m_mouseX > m_screenWidth) {m_mouseX = m_screenWidth;}
	if(m_mouseY > m_screenHeight) {m_mouseY = m_screenHeight;}

	return;
}

bool InputClass::IsEscapePressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed 0x80 is if it's pressed
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsAPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_A] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsDPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_D] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsWPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_W] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsSPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_S] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsUpPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_UP] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsDownPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_DOWN] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsLeftPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_LEFT] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsRightPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_RIGHT] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsSpacePressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_SPACE] &0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsShiftPressed()
{
	//do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_LSHIFT] &0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;

	return;
}

void InputClass::CenterMouse()
{
	m_mouseX = (int)(m_screenWidth * 0.5);
	m_mouseY = (int)(m_screenHeight * 0.5);
}