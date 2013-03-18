#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

//INCLUDES
#include <D3DX10math.h>

//CLASS
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	D3DXVECTOR3 TransformCoordinates(D3DXVECTOR3, float, D3DXMATRIX);
	D3DXMATRIX GetLookAtMatrix(D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR3);
	D3DXVECTOR3 NormalizeVec3(D3DXVECTOR3);
	float LengthVec3(D3DXVECTOR3);
	float DotVec3(D3DXVECTOR3, D3DXVECTOR3);
	D3DXVECTOR3 CrossVec3(D3DXVECTOR3, D3DXVECTOR3);

	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	D3DXMATRIX m_viewMatrix;
};

#endif