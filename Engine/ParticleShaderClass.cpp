#include "particleshaderclass.h"


ParticleShaderClass::ParticleShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}

ParticleShaderClass::ParticleShaderClass(const ParticleShaderClass& other)
{
}

ParticleShaderClass::~ParticleShaderClass()
{
}

bool ParticleShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Engine/particle.vsh", L"../Engine/particle.psh"); //initialize the shaders
	if(!result)
	{
		return false;
	}

	return true;
}

void ParticleShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool ParticleShaderClass::Render(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture
	)
{
	bool result;

	result = SetShaderParameters( //set the shader parameters that it will use for rendering
		deviceContext,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		texture
		);
	if(!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount); //render the prepared buffers with the shader

	return true;
}

//the layout will need the match the VertexType in the modelclass.h file as well as the one defined in the color.vsh file
bool ParticleShaderClass::InitializeShader(
	ID3D11Device* device,
	HWND hwnd,
	WCHAR* vshFilename,
	WCHAR* pshFilename
	)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile( //compile the vertex shader code
		vshFilename,
		NULL,
		NULL,
		"ParticleVertexShader",
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
			MessageBox(hwnd, vshFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(//compile the pixel shader code
		pshFilename,
		NULL,
		NULL,
		"ParticlePixelShader",
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
			MessageBox(hwnd, pshFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = device->CreateVertexShader( //create a vertex shader from vertex buffer
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		NULL,
		&m_vertexShader
		);
	if(FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader( //create a pixel shader from pixel buffer
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		NULL,
		&m_pixelShader
		);
	if(FAILED(result))
	{
		return false;
	}

	//now setup the layout of the data that goes into the shader, D3D11_APPEND_ALIGNED_ELEMENT figures out the spacing between the data in the buffer
	//this setup needs to match the vertextype structure in the modelclass and in the shader
	//create a three component layout for the particle shader, the third component being the individual color of each particle

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "COLOR";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]); 

	result = device->CreateInputLayout( //create vertex input layout
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

	//release the vertex and shader buffer
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //needs to be dynamic since it'll be updating each frame
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer); //create the constant buffer pointer so we can access the vertex shader constant buffer from within this class
	if(FAILED(result))
	{
		return false;
	}

	//here are all the settings for how to sample the texture
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void ParticleShaderClass::ShutdownShader()
{
	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
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

void ParticleShaderClass::OutputShaderErrorMessage(
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

	fout.open("shader-error.txt"); //open the file

	for(i = 0; i < bufferSize; i++) //write the error message to file
	{
		fout << compileErrors[i];
	}

	fout.close(); //close the file

	errorMessage->Release(); //release error message
	errorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool ParticleShaderClass::SetShaderParameters( //this function is here to make it easier to set global variables for the shaders
	ID3D11DeviceContext* deviceContext,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture
	)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//transpose the matrices to prepare them for the shader, this is a requirement for DX11
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = deviceContext->Map( //lock the constant buffer so it can be written to
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

	dataPtr = (MatrixBufferType*)mappedResource.pData; //get a pointer to the data in the constant buffer

	//copy the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0); //unlock the constant buffer

	bufferNumber = 0; //set the position of the constant buffer in the vertex shader

	deviceContext->VSSetConstantBuffers( //finally set the constant buffer in the vertex shader with the updated values
		bufferNumber,
		1,
		&m_matrixBuffer
		);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void ParticleShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout); //set the vertex input layout

	//set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState); //set the sampler state in the pixel shader

	deviceContext->DrawIndexed(indexCount, 0, 0); //render the triangle

	return;
}