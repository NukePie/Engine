#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}

D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt, lookAt2;
	float yaw, pitch, roll;
	//D3DXMATRIX rotationMatrix;

	//setup for the up Vector
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//setup where the camera is looking by Default
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//setup for the position of the camera in the world
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	//set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians, 0.0174532925f is one degree in radians
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	
	D3DXMATRIX rotationMatrix
		(
			(float)(cos(yaw) * cos(roll)),
			(float)(cos(yaw) * sin(roll)),
			(float)(-sin(yaw)),
			0.0f,

			(float)(sin(pitch) * sin(yaw) * cos(roll) + cos(pitch) * -sin(roll)),
			(float)(sin(pitch) * sin(yaw) * sin(roll) + cos(pitch) * cos(roll)),
			(float)(sin(pitch) * cos(yaw)),
			0.0f,

			(float)(cos(pitch) * sin(yaw) * cos(roll) + -sin(pitch) * -sin(roll)),
			(float)(cos(pitch) * sin(yaw) * sin(roll) + -sin(pitch) * cos(roll)),
			(float)(cos(pitch) * cos(yaw)),
			0.0f,

			0.0f,
			0.0f,
			0.0f,
			1.0f
		);
		

	//transform the lookAt and Up vector by the rotation matrix so the view is correctly rotated at the origin
	
	lookAt = TransformCoordinates(lookAt, 1, rotationMatrix);
	up = TransformCoordinates(up, 1, rotationMatrix);

	//translate the rotated camera position to the location of the viewer
	lookAt = position + lookAt;

	//create the viewMatrix from the updated Vectors
	m_viewMatrix = GetLookAtMatrix(position, lookAt, up);
	return;
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	
	return;
}

//Create the lookAt matrix
D3DXMATRIX CameraClass::GetLookAtMatrix(D3DXVECTOR3 position, D3DXVECTOR3 lookAt, D3DXVECTOR3 Up)
{
	D3DXVECTOR3 zaxis = NormalizeVec3(lookAt - position);
	D3DXVECTOR3 xaxis = NormalizeVec3(CrossVec3(Up, zaxis));
	D3DXVECTOR3 yaxis = CrossVec3(zaxis, xaxis);

	D3DXMATRIX LookAtMatrix (
		xaxis.x, yaxis.x, zaxis.x, 0,
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0,
		-DotVec3(xaxis, position), -DotVec3(yaxis, position), -DotVec3(zaxis, position), 1
	);

	return LookAtMatrix;
}

float CameraClass::LengthVec3(D3DXVECTOR3 vec3)
{
	return sqrt(vec3.x*vec3.x + vec3.y*vec3.y + vec3.z*vec3.z);
}

D3DXVECTOR3 CameraClass::NormalizeVec3(D3DXVECTOR3 vec3)
{
	D3DXVECTOR3 outVec3;

	float l = LengthVec3(vec3);
	outVec3.x = vec3.x / l;
	outVec3.y = vec3.y / l;
	outVec3.z = vec3.z / l;

	return outVec3;
}

float CameraClass::DotVec3(D3DXVECTOR3 vec3_1, D3DXVECTOR3 vec3_2)
{
	return (vec3_1.x * vec3_2.x) + (vec3_1.y * vec3_2.y) + (vec3_1.z * vec3_2.z);
}

D3DXVECTOR3 CameraClass::CrossVec3(D3DXVECTOR3 vec3_1, D3DXVECTOR3 vec3_2)
{
	return D3DXVECTOR3((vec3_1.y * vec3_2.z) - (vec3_1.z * vec3_2.y), (vec3_1.z * vec3_2.x) - (vec3_1.x * vec3_2.z), (vec3_1.x * vec3_2.y) - (vec3_1.y * vec3_2.x));
}

D3DXVECTOR3 CameraClass::TransformCoordinates(D3DXVECTOR3 vec3, float w, D3DXMATRIX matrix)
{
	D3DXVECTOR3 outVec3;

	outVec3.x = 
		matrix._11 * vec3.x +
		matrix._21 * vec3.y +
		matrix._31 * vec3.z +
		matrix._41 * w;

	outVec3.y =
		matrix._12 * vec3.x +
		matrix._22 * vec3.y +
		matrix._32 * vec3.z +
		matrix._42 * w;

	outVec3.z =
		matrix._13 * vec3.x +
		matrix._23 * vec3.y +
		matrix._33 * vec3.z +
		matrix._43 * w;
	
	return outVec3;
}