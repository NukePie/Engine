#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

///INCLUDES
#include <D3D11.h>
#include <D3DX10math.h>
#include <stdio.h>

#include "textureclass.h"
#include "terrainshaderclass.h"

//GLOBALS
const int TEXTURE_REPEAT = 32;

///CLASS
class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR4 color;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float r, g, b;
		int rIndex, gIndex, bIndex; //used to indicate which material is used for wich poly in the terrain. Index matches up to the colors in the material map
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct MaterialGroupType
	{
		int textureIndex1, textureIndex2, alphaIndex;
		int red, green, blue;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount;
		VertexType* vertices;
		unsigned long* indices;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, char*, char*, char*, char*, WCHAR*);
	void Shutdown();
	bool Render(
		ID3D11DeviceContext*,
		TerrainShaderClass*,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXVECTOR4,
		D3DXVECTOR4,
		D3DXVECTOR3,
		D3DXVECTOR3,
		float,
		float
		);

	int GetIndexCount();
	float GetHeightAtPos(float, float);

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadHeightMap(char*);
	bool LoadColorMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	////////////////////////////////////////////Onödiga?
	void CalculateTextureCoordinates();
	bool LoadCubemapTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();
	void ReleaseCubemapTexture();
	////////////////////////////////////////////


	//functions for loading and unloading the material groups
	bool LoadMaterialFile(char*, char*, ID3D11Device*);
	bool LoadMaterialMap(char*);
	bool LoadMaterialBuffers(ID3D11Device*);
	void ReleaseMaterials();

private:
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	int m_textureCount, m_materialCount;
	float m_divideHeight;

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	HeightMapType *m_heightMap;
	TextureClass *m_Texture, *m_Textures, *m_CubemapTexture;
	MaterialGroupType* m_Materials;
};

#endif