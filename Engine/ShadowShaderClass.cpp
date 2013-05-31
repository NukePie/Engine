#include "shadowshaderclass.h"


ShadowShaderClass::ShadowShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleStateWrap = 0;
	m_sampleStateClamp = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
	m_lightBuffer2 = 0;
	m_cameraBuffer = 0;
}


ShadowShaderClass::ShadowShaderClass(const ShadowShaderClass& other)
{
}

ShadowShaderClass::~ShadowShaderClass()
{
}

bool ShadowShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Engine/shadow.vsh", L"../Engine/shadow.psh"); //initialize the shaders
	if(!result)
	{
		return false;
	}

	return true;
}

void ShadowShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool ShadowShaderClass::Render(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	int instanceCount,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	D3DXMATRIX lightViewMatrix,
	D3DXMATRIX lightProjectionMatrix,
	ID3D11ShaderResourceView** textureArray,
	ID3D11ShaderResourceView* depthMapTexture,
	D3DXVECTOR3 lightPosition,
	D3DXVECTOR4 ambientColor,
	D3DXVECTOR4 diffuseColor,
	D3DXVECTOR3 cameraPosition,
	D3DXVECTOR4 specularColor,
	D3DXVECTOR3 lightDirection
	)
{
	bool result;

	result = SetShaderParameters( //set the shader parameters that it will use for rendering
		deviceContext,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		lightViewMatrix,
		lightProjectionMatrix,
		textureArray,
		depthMapTexture,
		lightPosition,
		ambientColor,
		diffuseColor,
		cameraPosition,
		specularColor,
		lightDirection
		);
	if(!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, instanceCount); //render the prepared buffers with the shader

	return true;
}

//the layout will need the match the VertexType in the modelclass.h file as well as the one defined in the color.vsh file
bool ShadowShaderClass::InitializeShader(
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[6];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightColorBufferDesc;
	D3D11_BUFFER_DESC lightPositionBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile( //compile the vertex shader code
		vshFilename,
		NULL,
		NULL,
		"ShadowVertexShader",
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
		"ShadowPixelShader",
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	polygonLayout[5].SemanticName = "NORMAL";
	polygonLayout[5].SemanticIndex = 1;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[5].InputSlot = 1;
	polygonLayout[5].AlignedByteOffset = 0;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[5].InstanceDataStepRate = 1;

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

	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap); //creating the texture sampler state
	if(FAILED(result))
	{
		return false;
	}

	//create a clamp texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
	if(FAILED(result))
	{
		return false;
	}

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
	//The setup of the description for the light constant buffer. To satisfy the CreateBuffer function who require a size which is a multiple by 16 we need to add a little padding to make it work
	lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightColorBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightColorBufferDesc.MiscFlags = 0;
	lightColorBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightColorBufferDesc, NULL, &m_lightBuffer);
	if(FAILED(result))
	{
		return false;
	}

	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth = sizeof(LightBufferType2);
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightBuffer2);
	if(FAILED(result))
	{
		return false;
	}

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void ShadowShaderClass::ShutdownShader()
{
	
	if(m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}
	
	if(m_lightBuffer2)
	{
		m_lightBuffer2->Release();
		m_lightBuffer2 = 0;
	}

	if(m_sampleStateWrap)
	{
		m_sampleStateWrap->Release();
		m_sampleStateWrap = 0;
	}

	if(m_sampleStateClamp)
	{
		m_sampleStateClamp->Release();
		m_sampleStateClamp = 0;
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

	if(m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}
	return;
}

void ShadowShaderClass::OutputShaderErrorMessage(
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

bool ShadowShaderClass::SetShaderParameters( //this function is here to make it easier to set global variables for the shaders
	ID3D11DeviceContext* deviceContext,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	D3DXMATRIX lightViewMatrix,
	D3DXMATRIX lightProjectionMatrix, 
	ID3D11ShaderResourceView** textureArray,
	ID3D11ShaderResourceView* depthMapTexture,
	D3DXVECTOR3 lightPosition,
	D3DXVECTOR4 ambientColor,
	D3DXVECTOR4 diffuseColor,
	D3DXVECTOR3 cameraPosition,
	D3DXVECTOR4 specularColor,
	D3DXVECTOR3 lightDirection
	)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	LightBufferType2* dataPtr3;
	CameraBufferType* dataPtr4;

	//transpose the matrices to prepare them for the shader, this is a requirement for DX11
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	D3DXMatrixTranspose(&lightViewMatrix, &lightViewMatrix);
	D3DXMatrixTranspose(&lightProjectionMatrix, &lightProjectionMatrix);

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
	dataPtr->lightView = lightViewMatrix;
	dataPtr->lightProjection = lightProjectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0); //unlock the constant buffer

	bufferNumber = 0; //set the position of the constant buffer in the vertex shader

	deviceContext->VSSetConstantBuffers( //finally set the constant buffer in the vertex shader with the updated values
		bufferNumber,
		1,
		&m_matrixBuffer
		);

	deviceContext->PSSetShaderResources(1, 3, textureArray);
	deviceContext->PSSetShaderResources(0, 1, &depthMapTexture);

	//lock the light position constant buffer so it can be written to
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	dataPtr2 = (LightBufferType*)mappedResource.pData;
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	//Lock the light Constant Buffer so we can write to it
	result = deviceContext->Map(m_lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	dataPtr3 = (LightBufferType2*)mappedResource.pData; //a pointer to the data in the constant buffer

	//copy the variables into the constant buffer
	dataPtr3->lightPosition = lightPosition;
	dataPtr3->padding = 0.0f;

	//unlock it
	deviceContext->Unmap(m_lightBuffer2, 0);

	//set the position of the light buffer in the pixelshader
	bufferNumber = 1;

	//Set it in the pixel shader with updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer2);

	//lock the camera buffer
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr4 = (CameraBufferType*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	dataPtr4->cameraPosition = cameraPosition;

	// Unlock the matrix constant buffer.
	deviceContext->Unmap(m_cameraBuffer, 0);

	// Set the position of the camera constant buffer in the vertex shader as the second buffer.
	bufferNumber = 2;

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	return true;
}

void ShadowShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount)
{
	deviceContext->IASetInputLayout(m_layout); //set the vertex input layout

	//set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleStateClamp); //set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateWrap);

	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0); //render the triangle

	return;
}
