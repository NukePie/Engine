#include "goochshaderclass.h"


Goochshaderclass::Goochshaderclass()
{
	m_vertexShader			= 0;
	m_pixelShader			= 0;
	m_layout				= 0;
	m_matrixBuffer			= 0;
	m_lightPositionBuffer	= 0;
}

Goochshaderclass::Goochshaderclass(const Goochshaderclass& other)
{
}

Goochshaderclass::~Goochshaderclass()
{
}

bool Goochshaderclass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Engine/gooch.vsh", L"../Engine/gooch.psh");
	if(!result)
	{
		return false;
	}

	return true;
}

void Goochshaderclass::Shutdown()
{
	ShutdownShader();

	return;
}

bool Goochshaderclass::Render(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	D3DXVECTOR4 lightPosition
	)
{
	bool result;

	result = SetShaderParameters(
		deviceContext,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		lightPosition
		);
	if(!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

//The layout here must match the VertexType in the gooch.vsh file
bool Goochshaderclass::InitializeShader(
	ID3D11Device* device,
	HWND hwnd,
	WCHAR* vshFilename,
	WCHAR* pshFilename
	)
{
	HRESULT result;
	ID3D10Blob*					errorMessage;
	ID3D10Blob*					vertexShaderBuffer;
	ID3D10Blob*					pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC	polygonLayout[3];
	unsigned int				numElements;
	D3D11_BUFFER_DESC			matrixBufferDesc;
	D3D11_BUFFER_DESC			lightPositionBufferDesc;

	errorMessage		= 0;
	vertexShaderBuffer	= 0;
	pixelShaderBuffer	= 0;

	////////////////////////COMPILE THE VERTEX SHADER CODE//////////////////
	result = D3DX11CompileFromFile(
		vshFilename,
		NULL,
		NULL,
		"GoochVertexShader",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		NULL,
		&vertexShaderBuffer,
		&errorMessage,
		NULL
		);
	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vshFilename);
		}
		else
		{
			MessageBox(hwnd, vshFilename, L"Missing vertex Shader File", MB_OK);
		}

		return false;
	}

	///////////////////COMPILE THE PIXEL SHADER CODE//////////////////
	result = D3DX11CompileFromFile(
		pshFilename,
		NULL,
		NULL,
		"GoochPixelShader",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		NULL,
		&pixelShaderBuffer,
		&errorMessage,
		NULL
		);
	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, pshFilename);
		}
		else
		{
			MessageBox(hwnd, vshFilename, L"Missing pixel Shader File", MB_OK);
		}

		return false;
	}

	/////////CREATE VERTEX SHADER FROM BUFFER///////
	result = device->CreateVertexShader(
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		NULL,
		&m_vertexShader
		);
	if(FAILED(result))
	{
		return false;
	}
	/////////CREATE PIXEL SHADER FROM BUFFER///////
	result = device->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		NULL,
		&m_pixelShader
		);
	if(FAILED(result))
	{
		return false;
	}

	//////SETUP THE LAYOUT OF THE DATA THAT ENTER THE SHADER////////
	polygonLayout[0].SemanticName			= "POSITION";
	polygonLayout[0].SemanticIndex			= 0;
	polygonLayout[0].Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot				= 0;
	polygonLayout[0].AlignedByteOffset		= 0;
	polygonLayout[0].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate	= 0;

	polygonLayout[1].SemanticName			= "NORMAL";
	polygonLayout[1].SemanticIndex			= 0;
	polygonLayout[1].Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot				= 0;
	polygonLayout[1].AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate	= 0;

	polygonLayout[2].SemanticName			= "TEXCOORD";
	polygonLayout[2].SemanticIndex			= 0;
	polygonLayout[2].Format					= DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot				= 0;
	polygonLayout[2].AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate	= 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//////CREATE THE VERTEX INPUT LAYOUT////////
	result = device->CreateInputLayout(
		polygonLayout,
		numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&m_layout
		);
	if(FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//////SETUP THE MATRIX BUFFER DESCRIPTION AND CREATE BUFFER////////
	matrixBufferDesc.Usage					= D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth				= sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags				= 0;
	matrixBufferDesc.StructureByteStride	= 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//////SETUP THE LIGHTPOSITION BUFFER DESCRIPTION AND CREATE BUFFER////////
	lightPositionBufferDesc.Usage					= D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth				= sizeof(LightPositionBufferType);
	lightPositionBufferDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags				= 0;
	lightPositionBufferDesc.StructureByteStride		= 0;

	result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightPositionBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void Goochshaderclass::ShutdownShader()
{
	if(m_lightPositionBuffer)
	{
		m_lightPositionBuffer->Release();
		m_lightPositionBuffer = 0;
	}
	
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void Goochshaderclass::OutputShaderErrorMessage(
	ID3D10Blob* errorMessage,
	HWND hwnd,
	WCHAR* shaderFilename
	)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize(); //get length of message

	fout.open("shader-error.txt");
	for(i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	
	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(
		hwnd, 
		L"Error compiling the shader, check the shader-error txt",
		shaderFilename, 
		MB_OK
		);

	return;
}

bool Goochshaderclass::SetShaderParameters(
	ID3D11DeviceContext* deviceContext,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	D3DXVECTOR4 lightPosition
	)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightPositionBufferType* dataPtr2;
	unsigned int bufferNumber;

	//TRANSPOSE THE MATRICES///
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	////LOCK THE BUFFER AND COPY IN THE MATRICES//////
	result = deviceContext->Map(
		m_matrixBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
		);
	if(FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world			= worldMatrix;
	dataPtr->view			= viewMatrix;
	dataPtr->projection		= projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);
	bufferNumber = 0;

	/////SET THE BUFFER IN THE VERTEX SHADER///////
	deviceContext->VSSetConstantBuffers(
		bufferNumber,
		1,
		&m_matrixBuffer
		);

	////LOCK THE LIGHTPOS BUFFER AND SET IT IN THE SHADER/////
	result = deviceContext->Map(
		m_lightPositionBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
		);
	if(FAILED(result))
	{
		return false;
	}
	dataPtr2 = (LightPositionBufferType*)mappedResource.pData;
	dataPtr2->lightPosition = lightPosition;

	deviceContext->Unmap(m_lightPositionBuffer, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(
		bufferNumber,
		1,
		&m_lightPositionBuffer
		);

	//deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void Goochshaderclass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}