#ifndef _INSTANCEMODELCLASS_H_
#define _INSTANCEMODELCLASS_H_

//INCLUDES
#include <D3D11.h>
#include <D3DX10math.h>

#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

//MY CLASS INCLUDES
#include "importer.h"
#include "texturearrayclass.h"
#include "common.h"


class InstanceModelClass
{
private:
	struct VertexType //this typedef must match the layout in the Shader Class
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};
	
	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	struct ReadVertexType
	{
		float x, y, z;
	};


	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct InstanceType
	{
		D3DXVECTOR3 position;
	};

	
public:
	InstanceModelClass();
	InstanceModelClass(const InstanceModelClass&);
	~InstanceModelClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	int GetVertexCount();
	int GetInstanceCount();

	void Frame(float);
	void SetInstanceCount(int);

	ID3D11ShaderResourceView** GetTextureArray();

	void SetPosition(float, float, float);
	void GetPosition(float&, float&, float&);


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTextures();
	
	void InterpolateFrameData(float);


	bool LoadFile(char*);

	bool ReadFileCounts(
		char*, 
		int&, 
		int&, 
		int&t,
		int&,
		int&
		);

	bool LoadDataStructures(
		char*, 
		int&, 
		int&, 
		int&,
		int&);

	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer, *m_instanceBuffer;
	int m_vertexCount, m_indexCount, m_instanceCount;

	TextureArrayClass* m_TextureArray;

	ModelType* m_model;
	Importer* m_importer;
	vector<vector<ModelType>> m_keyFrameData;
	vector<float> m_keyFrameTimes;

	float m_positionX, m_positionY, m_positionZ;
};

#endif
