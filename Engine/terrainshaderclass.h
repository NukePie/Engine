#ifndef _TERRAINSHADERCLASS_H_
#define _TERRAINSHADERCLASS_H_

//INCLUDES
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>
using namespace std;

//CLASS
class TerrainShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct FogBufferType
	{
		float fogStart;
		float fogEnd;
		float padding1, padding2;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;//testing
		float padding;
		//D3DXVECTOR2 padding;//Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements
	};

	struct TextureInfoBufferType
	{
		D3DXVECTOR3 camPosition;
		bool useAlpha;
	};

public:
	TerrainShaderClass();
	TerrainShaderClass(const TerrainShaderClass&);
	~TerrainShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool SetShaderParameters(
		ID3D11DeviceContext*,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXVECTOR4,
		D3DXVECTOR4,
		D3DXVECTOR3,
		float,
		float
		);
	bool SetShaderTextures(
		ID3D11DeviceContext*,
		ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*,
		D3DXVECTOR3,
		bool
		);


	void RenderShader(ID3D11DeviceContext*, int);
	
private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_textureInfoBuffer;
	ID3D11Buffer* m_fogBuffer;
};

#endif