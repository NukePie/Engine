#ifndef _SHADOWSHADERCLASS_H_
#define _SHADOWSHADERCLASS_H_

//INCLUDES
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>
using namespace std;

//CLASS
class ShadowShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX lightView;
		D3DXMATRIX lightProjection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 specularColor;
		float specularPower;
		D3DXVECTOR3 lightDirection;
	};

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};

	struct LightBufferType2
	{
		D3DXVECTOR3 lightPosition;
		float padding;
	};

public:
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(
		ID3D11DeviceContext*,
		int,
		int,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		ID3D11ShaderResourceView**,
		ID3D11ShaderResourceView*,
		D3DXVECTOR3,
		D3DXVECTOR4,
		D3DXVECTOR4,
		D3DXVECTOR3,
		D3DXVECTOR4,
		D3DXVECTOR3
		);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(
		ID3D11DeviceContext*,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		ID3D11ShaderResourceView**,
		ID3D11ShaderResourceView*,
		D3DXVECTOR3,
		D3DXVECTOR4,
		D3DXVECTOR4,
		D3DXVECTOR3,
		D3DXVECTOR4,
		D3DXVECTOR3
		);
	void RenderShader(ID3D11DeviceContext*, int, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;

	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer; //constant buffer for the light information (color)
	ID3D11Buffer* m_lightBuffer2;
	ID3D11Buffer* m_cameraBuffer;
};
#endif