#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

//PRE-PROCESSING DIRECTIVES
//This line is needed to get rid of annoying messages in the compiler about defaulting to version 8
#define DIRECTINPUT_VERSION 0x0800

//LINKING
//these two libraries are needed for direct input to work
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//INCLUDES
#include <dinput.h>

//CLASS
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void ShutDown();
	bool Frame();

	bool IsEscapePressed();
	bool IsUpPressed();
	bool IsDownPressed();
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsWPressed();//Forward
	bool IsAPressed();//Left
	bool IsSPressed();//Back
	bool IsDPressed();//Right
	bool IsSpacePressed();//FlyUp, Up
	bool IsShiftPressed();//Fall down

	void GetMouseLocation(int&, int&);
	void CenterMouse();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif