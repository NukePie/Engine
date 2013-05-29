#include "terrainshaderclass.h"
//#include "log.h"

TerrainShaderClass::TerrainShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
	m_textureInfoBuffer = 0;
	m_fogBuffer = 0;
}


TerrainShaderClass::TerrainShaderClass(const TerrainShaderClass& other)
{
}

TerrainShaderClass::~TerrainShaderClass()
{
}

bool TerrainShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Engine/terrain.vsh", L"../Engine/terrain.psh"); //initialize the shaders
	if(!result)
	{
		return false;
	}

	return true;
}

void TerrainShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}


//the layout will need the match the VertexType in the modelclass.h file as well as the one defined in the color.vsh file
bool TerrainShaderClass::InitializeShader(
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];	//testin testing - should be 4
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC textureInfoBufferDesc;
	D3D11_BUFFER_DESC fogBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile( //compile the vertex shader code
		vshFilename,
		NULL,
		NULL,
		"TerrainVertexShader",
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
		"LightPixelShader",
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

	polygonLayout[3].SemanticName = "COLOR";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	//////////////////////////////////////
	polygonLayout[4].SemanticName = "NORMAL";
	polygonLayout[4].SemanticIndex = 1;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 1;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;
	/////////////////////////////////////////

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

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState); //creating the texture sampler state
	if(FAILED(result))
	{
		return false;
	}

	//The setup of the description for the light constant buffer. To satisfy the CreateBuffer function who require a size which is a multiple by 16 we need to add a little padding to make it work
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//setup the description of the texture info constant buffer that's in the pixel shader
	textureInfoBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureInfoBufferDesc.ByteWidth = sizeof(TextureInfoBufferType);
	textureInfoBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	textureInfoBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureInfoBufferDesc.MiscFlags = 0;
	textureInfoBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&textureInfoBufferDesc, NULL, &m_textureInfoBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic fog constatnt buffer that is in the vertex shader.
	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(FogBufferType);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;

	// Create the fog buffer pointer so we can access the vertex shader fog constant buffer from within this class.
	result = device->CreateBuffer(&fogBufferDesc, NULL, &m_fogBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void TerrainShaderClass::ShutdownShader()
{
	if(m_fogBuffer)
	{
		m_fogBuffer->Release();
		m_fogBuffer = 0;
	}

	if(m_textureInfoBuffer)
	{
		m_textureInfoBuffer->Release();
		m_textureInfoBuffer = 0;
	}

	if(m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

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

void TerrainShaderClass::OutputShaderErrorMessage(
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

//this function is here to make it easier to set global variables for the shaders
bool TerrainShaderClass::SetShaderParameters(
	ID3D11DeviceContext* deviceContext,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	D3DXVECTOR4 ambientColor,
	D3DXVECTOR4 diffuseColor,
	D3DXVECTOR3 lightDirection,
	float fogStart,
	float fogEnd
	)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	FogBufferType* dataPtr3;

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

	//Lock the light Constant Buffer so we can write to it
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	dataPtr2 = (LightBufferType*)mappedResource.pData; //a pointer to the data in the constant buffer

	//copy the variables into the constant buffer
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	//dataPtr2->camPosition = camPosition;	//testing
	//dataPtr2->padding = D3DXVECTOR2(0.0f, 0.0f);
	
	//FILE_LOG(logWARNING) << "Ops, variable x should be " << camPosition.x << ":" << camPosition.y << ":" << camPosition.z;

	//unlock it
	deviceContext->Unmap(m_lightBuffer, 0);

	//set the position of the light buffer in the pixelshader
	bufferNumber = 0;

	//Set it in the pixel shader with updated values
	deviceContext->PSSetConstantBuffers(bufferNumber, 1,&m_lightBuffer);

	// Lock the fog constant buffer so it can be written to.
	result = deviceContext->Map(m_fogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (FogBufferType*)mappedResource.pData;

	// Copy the fog information into the fog constant buffer.
	dataPtr3->fogStart = fogStart;
	dataPtr3->fogEnd = fogEnd;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_fogBuffer, 0);

	// Set the position of the fog constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the fog buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_fogBuffer);

	return true;
}

bool TerrainShaderClass::SetShaderTextures(
	ID3D11DeviceContext* deviceContext, 
	ID3D11ShaderResourceView* texture1,
	ID3D11ShaderResourceView* texture2,
	ID3D11ShaderResourceView* alphaMap,
	ID3D11ShaderResourceView* cubemap,
	D3DXVECTOR3 camPosition,	//testing
	bool useAlpha
	)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	TextureInfoBufferType* dataPtr;
	unsigned int bufferNumber;
	//set the primary texture in the pixel shader first
	//set shader texture resources in the pixelshader

	deviceContext->PSSetShaderResources(0, 1, &texture1);

	//if this is a blended polygon then also set the second texture and the alpha map for blending
	if(useAlpha)
	{
		deviceContext->PSSetShaderResources(1, 1, &texture2);
		deviceContext->PSSetShaderResources(2, 1, &alphaMap);
	}
	

	deviceContext->PSSetShaderResources(3, 1, &cubemap);

	//ID3DX11EffectShaderResourceVariable* Cubemap;

	//set the boolean value to se if blend or not
	//lock the buffer
	result= deviceContext->Map(m_textureInfoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	//get a pointer to the data in the buffer
	dataPtr = (TextureInfoBufferType*)mappedResource.pData;
	dataPtr->camPosition = camPosition;	//testing
	//copy the texture info variables into the buffer
	dataPtr->useAlpha = useAlpha;
	//dataPtr->padding2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//unlock the buffer
	deviceContext->Unmap(m_textureInfoBuffer, 0);

	//set the position of the texture info const buffer in the pixelshader
	bufferNumber = 1;

	//set the texture info const buffer in the pixelshader with the updated values
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_textureInfoBuffer);

	return true;
}

void TerrainShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout); //set the vertex input layout

	//set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState); //set the sampler state in the pixel shader

	deviceContext->DrawIndexed(indexCount, 0, 0); //render the triangle

	return;
}
