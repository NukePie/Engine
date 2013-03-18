#include "positionclass.h"


PositionClass::PositionClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;
	
	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_screenWidth = 0;
	m_screenHeight = 0;
	m_mouseSensitivity = 0.2f;

	m_frameTime = 0.0f;

	m_forwardSpeed   = 0.0f;
	m_backwardSpeed  = 0.0f;
	m_rightSpeed	 = 0.0f;
	m_leftSpeed		 = 0.0f;
	m_upwardSpeed    = 0.0f;
	m_downwardSpeed  = 0.0f;
	m_leftTurnSpeed  = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed    = 0.0f;
	m_lookDownSpeed  = 0.0f;
	
	m_acceleration	 = 0.001f;
	m_deacceleration = 0.0007f;
	m_maxSpeed		 = 0.03f;

	m_rotationAccel	 = 0.005f;
	m_rotationDeaccel= 0.007f;
	m_maxRotSpeed	 = 0.15f;
}

PositionClass::PositionClass(const PositionClass& other)
{
}


PositionClass::~PositionClass()
{
}

void PositionClass::InitializeMouseLook(int screenWidth, int screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	
	return;
}

void PositionClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;

	return;
}

void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;

	return;
}
//Eventuellt inte användbart
//returns the current position and rotation of the camera location
void PositionClass::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;

	return;
}

void PositionClass::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;

	return;
}

void PositionClass::SetFrameTime(float time)//stores the current frame time and is used for movement calculations
{
	m_frameTime = time;

	return;
}

void PositionClass::MoveForward(bool keydown)//calculates the forward speed and movement of the camera
{
	float radians;

	//update the forward speed movement based on the frame time and whether the user is holding down the key or not
	if(keydown)
	{
		m_forwardSpeed += m_frameTime * m_acceleration;

		if(m_forwardSpeed > (m_frameTime * m_maxSpeed))
		{
			m_forwardSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime * m_deacceleration;

		if(m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	//convert degrees to radians
	radians = m_rotationY *  0.0174532925f;

	//update the position
	m_positionX += sinf(radians) * m_forwardSpeed;
	m_positionZ += cosf(radians) * m_forwardSpeed;

	return;
}

void PositionClass::MoveBackward(bool keydown)
{
	float radians;

	//update the backwards speed movement based on the frame time and whether the user is holding down the key or not
	if(keydown)
	{
		m_backwardSpeed += m_frameTime * m_acceleration;

		if(m_backwardSpeed > (m_frameTime * m_maxSpeed))
		{
			m_backwardSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * m_deacceleration;

		if(m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	//convert degrees to radians
	radians = m_rotationY *  0.0174532925f;

	//update the position
	m_positionX -= sinf(radians) * m_backwardSpeed;
	m_positionZ -= cosf(radians) * m_backwardSpeed;

	return;
}

void PositionClass::MoveRight(bool keydown)//calculates the speed and movement of the camera
{
	float radians;

	//update the speed movement based on the frame time and whether the user is holding down the key or not
	if(keydown)
	{
		m_rightSpeed += m_frameTime * m_acceleration;

		if(m_rightSpeed > (m_frameTime * m_maxSpeed))
		{
			m_rightSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_rightSpeed -= m_frameTime * m_deacceleration;

		if(m_rightSpeed < 0.0f)
		{
			m_rightSpeed = 0.0f;
		}
	}

	//convert degrees to radians
	radians = (m_rotationY + 90) *  0.0174532925f;

	//update the position
	m_positionX += sinf(radians) * m_rightSpeed;
	m_positionZ += cosf(radians) * m_rightSpeed;

	return;
}

void PositionClass::MoveLeft(bool keydown)//calculates the speed and movement of the camera
{
	float radians;

	//update the speed movement based on the frame time and whether the user is holding down the key or not
	if(keydown)
	{
		m_leftSpeed += m_frameTime * m_acceleration;

		if(m_leftSpeed > (m_frameTime * m_maxSpeed))
		{
			m_leftSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_leftSpeed -= m_frameTime * m_deacceleration;

		if(m_leftSpeed < 0.0f)
		{
			m_leftSpeed = 0.0f;
		}
	}

	//convert degrees to radians
	radians = (m_rotationY -90) *  0.0174532925f;

	//update the position
	m_positionX += sinf(radians) * m_leftSpeed;
	m_positionZ += cosf(radians) * m_leftSpeed;

	return;
}

void PositionClass::MoveUpward(bool keydown)
{
	//update the upward speed movement based on the frame time and whether the user is holding down the key or not
	if(keydown)
	{
		m_upwardSpeed += m_frameTime * m_acceleration;

		if(m_upwardSpeed > (m_frameTime * m_maxSpeed))
		{
			m_upwardSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * m_deacceleration;

		if(m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}
	//update the position
	m_positionY += m_upwardSpeed;

	return;
}

void PositionClass::MoveDownward(bool keydown)
{
	//update the downward speed movement based on the frame time and whether the user is holding down the key or not
	if(keydown)
	{
		m_downwardSpeed += m_frameTime * m_acceleration;

		if(m_downwardSpeed > (m_frameTime * m_maxSpeed))
		{
			m_downwardSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_downwardSpeed -= m_frameTime * m_deacceleration;

		if(m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}
	//update the position
	m_positionY -= m_downwardSpeed;

	return;
}

void PositionClass::TurnLeft(bool keydown)
{
	if(keydown)
	{
		m_leftTurnSpeed += m_frameTime * m_rotationAccel;

		if(m_leftTurnSpeed > (m_frameTime * m_maxRotSpeed))
		{
			m_leftTurnSpeed = m_frameTime * m_maxRotSpeed;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime * m_rotationDeaccel;

		if(m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}
	//update the rotation
	m_rotationY -= m_leftTurnSpeed;

	//keep the rotation in the 0-360 range
	if(m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}

	return;
}

void PositionClass::TurnRight(bool keydown)
{
	if(keydown)
	{
		m_rightTurnSpeed += m_frameTime * m_rotationAccel;

		if(m_rightTurnSpeed > (m_frameTime * m_maxRotSpeed))
		{
			m_rightTurnSpeed = m_frameTime * m_maxRotSpeed;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime * m_rotationDeaccel;

		if(m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}
	//update the rotation
	m_rotationY += m_rightTurnSpeed;

	//keep the rotation in the 0-360 range
	if(m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}

	return;
}

void PositionClass::LookUpward(bool keydown)
{
	if(keydown)
	{
		m_lookUpSpeed += m_frameTime * m_rotationAccel;

		if(m_lookUpSpeed > (m_frameTime * m_maxRotSpeed))
		{
			m_lookUpSpeed = m_frameTime * m_maxRotSpeed;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime * m_rotationDeaccel;

		if(m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}
	//update the rotation
	m_rotationX -= m_lookUpSpeed;

	//keep the rotation in the 0-360 range
	if(m_rotationX > 90.0f)
	{
		m_rotationX = 90.0f;
	}

	return;
}

void PositionClass::LookDownward(bool keydown)
{
	if(keydown)
	{
		m_lookDownSpeed += m_frameTime * m_rotationAccel;

		if(m_lookDownSpeed > (m_frameTime * m_maxRotSpeed))
		{
			m_lookDownSpeed = m_frameTime * m_maxRotSpeed;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime * m_rotationDeaccel;

		if(m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}
	//update the rotation
	m_rotationX += m_lookDownSpeed;

	//keep the rotation in the 0-360 range
	if(m_rotationX < -90.0f)
	{
		m_rotationX = -90.0f;
	}

	return;
}

void PositionClass::MouseLook(int posX, int posY)
{
	m_rotationY += (posX - m_screenWidth * 0.5f) * m_mouseSensitivity;

	m_rotationX += (posY - m_screenHeight * 0.5f) * m_mouseSensitivity;
}
