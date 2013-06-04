#include "instancemodelclass.h"

using namespace std;

InstanceModelClass::InstanceModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_instanceBuffer = 0;
	m_TextureArray = 0;
	m_model = 0;
	m_instanceCount = 4;
	m_importer = 0;
}

InstanceModelClass::InstanceModelClass(const InstanceModelClass& other)
{
}

InstanceModelClass::~InstanceModelClass()
{
}

bool InstanceModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext * deviceContext, char* modelFilename)
{
	bool result;
	m_animTime = 0.0f;

	result = LoadFile(modelFilename);
	if(!result)
	{
		return false;
	}

	m_model = new ModelType[m_vertexCount];
	InitializeModelData();

	result = InitializeBuffers(device); // Calling initialization functions for vertex and index buffers.
	if(!result)
	{
		return false;
	}

	result = LoadTextures(device);
	if(!result)
	{
		return false;
	}

	return true;
}

void InstanceModelClass::Shutdown()
{
	ReleaseTextures();

	ShutdownBuffers(); // Release the vertex and index buffers.

	ReleaseModel();

	if(m_importer)
	{
		delete m_importer;
		m_importer = 0;
	}

	return;
}

void InstanceModelClass::Frame(ID3D11DeviceContext* deviceContext, float time)
{
 	InterpolateFrameData(time);
	UpdateBuffers(deviceContext);
}

void InstanceModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext); // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.

	return;
}

int InstanceModelClass::GetIndexCount()
{
	return m_indexCount; // Return the number of vertices in the model.
}

int InstanceModelClass::GetVertexCount()
{
	return m_vertexCount; // Return the number of vertices in the model.
}

int InstanceModelClass::GetInstanceCount()
{
	return m_instanceCount; // Return the number of vertices in the model.
}

void InstanceModelClass::SetInstanceCount(int count)
{
	m_instanceCount = count; // Set the number of instances of the model.
}

ID3D11ShaderResourceView** InstanceModelClass::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}

bool InstanceModelClass::InitializeBuffers(ID3D11Device* device)
{
	InstanceType * instances;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData, instanceData;
	VertexType * vertices;
	HRESULT result;
	int i;

	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for(i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);

		indices[i] = i;
	}

	// Setting up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer); // Create vertex buffer.
	if(FAILED(result))
	{
		return false;
	}

	// Setting up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)*m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Deleting the vertex and index arrays since we are done with them.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	// Create instance array.
	instances = new InstanceType[m_instanceCount];
	if(!instances)
	{
		return false;
	}

	// Load the instance array with data.
	for(int i = 0; i < m_instanceCount; i++)
	{
		instances[i].position = D3DXVECTOR3(i * 50.0f, 0.0f, 0.0f);
	}

	// Set up instance description.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	delete[] instances;
	instances = 0;
	
	return true;
}

bool InstanceModelClass::LoadTextures(ID3D11Device* device)
{
	bool result;

	m_TextureArray = new TextureArrayClass;
	if(!m_TextureArray)
	{
		return false;
	}

	char colorTextureName[32];
	char normalTextureName[32];
	char specularTextureName[32];
	
	m_importer->LoadTexture();
	m_importer->GetColorTextureName(colorTextureName);
	m_importer->GetNormalTextureName(normalTextureName);
	m_importer->GetSpecularTextureName(specularTextureName);

	// Make char* into WCHAR* for the buffers.
	size_t size = strlen(colorTextureName) + 1;
	wchar_t* wColorTextureName = new wchar_t[size];
	mbstowcs(wColorTextureName, &colorTextureName[0], size);

	size_t size2 = strlen(normalTextureName) + 1;
	wchar_t* wNormalTextureName = new wchar_t[size2];
	mbstowcs(wNormalTextureName, &normalTextureName[0], size2);

	size_t size3 = 64;
	wchar_t* wSpecularTextureName = new wchar_t[size3];

	// If there's a special specular give that. otherwise give a blank.
	if(strcmp(specularTextureName, "NONE") == 0)
	{
		wSpecularTextureName = L"../Engine/blankNormal.png";
	}
	else
	{
		mbstowcs(wSpecularTextureName, &specularTextureName[0], size3);
	}

	result = m_TextureArray->Initialize(device, wColorTextureName, wNormalTextureName, wSpecularTextureName);
	if(!result)
	{
		return false;
	}

	wNormalTextureName = 0;
	delete wNormalTextureName;

	wColorTextureName = 0;
	delete wColorTextureName;

	wSpecularTextureName = 0;
	delete wSpecularTextureName;

	return true;
}

void InstanceModelClass::ReleaseTextures()
{
	if(m_TextureArray)
	{
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = 0;
	}

	return;
}

void InstanceModelClass::ShutdownBuffers() // Releases the buffers created in the initializebuffers function.
{
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	
	if(m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	return;
}

void InstanceModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) //sending buffers to GPU so that shaders can work their magic upon them
{
	unsigned int stride[2];
	unsigned int offset[2];
	ID3D11Buffer* bufferPointer[2];

	stride[0] = sizeof(VertexType);
	stride[1] = sizeof(InstanceType);

	offset[0] = 0;
	offset[1] = 0;

	bufferPointer[0] = m_vertexBuffer;
	bufferPointer[1] = m_instanceBuffer;

	deviceContext->IASetVertexBuffers(0, 2, bufferPointer, stride, offset);
	
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void InstanceModelClass::ReleaseModel()
{
	if(m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}

void InstanceModelClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void InstanceModelClass::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
	return;
}

bool InstanceModelClass::LoadFile(char* filename)
{	
	bool result;
	int vertexCount, textureCount, normalCount, faceCount, groupCount;

	m_importer = new Importer();
	if(!m_importer)
	{
		return false;
	}

	m_importer->Initialize(filename);

	result = m_importer->LoadModel();
	if(!result)
	{
		return false;
	}
	
	m_keyFrameData = m_importer->GetModelData();
	m_keyFrameTimes = m_importer->GetKeyFrameTimes();

	m_vertexCount = m_keyFrameData[0].size();
	m_indexCount = m_vertexCount;

	return true;
}

bool InstanceModelClass::ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount, int& groupCount)
{	
	ifstream fin;
	char input;

	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	fin.open(filename);
	if(fin.fail() == true)
	{
		return false;
	}

	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ') 
			{
				vertexCount++;
			}

			if(input == 't')
			{
				textureCount++;
			}

			if(input == 'n')
			{
				normalCount++;
			}
		}

		if(input == 'g')
		{
			groupCount++;
		}

		if(input == 'f')
		{
			faceCount++;
		}

		while(input != '\n')
		{
			fin.get(input);
		}
		
		fin.get(input);
	}

	fin.close();

	return true;
}

bool InstanceModelClass::LoadDataStructures(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ReadVertexType* vertices;
	ReadVertexType* texcoords;
	ReadVertexType* normals;
	FaceType* faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	fin.open(filename);


	m_model = new ModelType[faceCount * 3];
	if(!m_model)
	{
		return false;
	}

	// Initialize the four data structures.
	vertices = new ReadVertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	texcoords = new ReadVertexType[textureCount];
	if(!texcoords)
	{
		return false;
	}

	normals = new ReadVertexType[normalCount];
	if(!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if(!faces)
	{
		return false;
	}

	if(fin.fail() == true)
	{
		return false;
	}

	fin.get(input);

	
	while(!fin.eof())
	{
		fin.get(input);
		if(input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if(input == ' ') 
			{ 
				fin >> vertices[vertexIndex].x;
				fin >> vertices[vertexIndex].y;
				fin >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++; 
			}

			// Read in the texture uv coordinates.
			if(input == 't') 
			{ 
				fin >> texcoords[texcoordIndex].x;
				fin >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++; 
			}

			// Read in the normals.
			if(input == 'n') 
			{ 
				fin >> normals[normalIndex].x;
				fin >> normals[normalIndex].y;
				fin >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++; 
			}
		}

		// Read in the faces.
		if(input == 'f') 
		{
			fin.get(input);
			if(input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
				    >> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
				    >> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}
	}
	

	// Close the file.
	fin.close();


	int i = 0;
	int j = 0;
	bool done = false;
	
	while(!done)
	{		
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		m_model[j].x = vertices[vIndex].x;
		m_model[j].y = vertices[vIndex].y;
		m_model[j].z = vertices[vIndex].z;
		m_model[j].tu = texcoords[tIndex].x;
		m_model[j].tv = texcoords[tIndex].y;
		m_model[j].nx = normals[nIndex].x;
		m_model[j].ny = normals[nIndex].y;
		m_model[j].nz = normals[nIndex].z;
		j++;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;
		
		m_model[j].x = vertices[vIndex].x;
		m_model[j].y = vertices[vIndex].y;
		m_model[j].z = vertices[vIndex].z;
		m_model[j].tu = texcoords[tIndex].x;
		m_model[j].tv = texcoords[tIndex].y;
		m_model[j].nx = normals[nIndex].x;
		m_model[j].ny = normals[nIndex].y;
		m_model[j].nz = normals[nIndex].z;
		j++;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		m_model[j].x = vertices[vIndex].x;
		m_model[j].y = vertices[vIndex].y;
		m_model[j].z = vertices[vIndex].z;
		m_model[j].tu = texcoords[tIndex].x;
		m_model[j].tv = texcoords[tIndex].y;
		m_model[j].nx = normals[nIndex].x;
		m_model[j].ny = normals[nIndex].y;
		m_model[j].nz = normals[nIndex].z;
		j++;

		i++;

		if((i == faceIndex) || (j == m_vertexCount))
		{
			done = true;
		}
	}
	

	if(vertices)
	{
		delete [] vertices;
		vertices = 0;
	}

	if(texcoords)
	{
		delete [] texcoords;
		texcoords = 0;
	}

	if(normals)
	{
		delete [] normals;
		normals = 0;
	}

	if(faces)
	{
		delete [] faces;
		faces = 0;
	}

	return true;
}

bool InstanceModelClass::InterpolateFrameData(float time)
{
	m_animTime += time;
	float animLength = m_keyFrameTimes[m_keyFrameTimes.size() - 1] * 1000 - m_keyFrameTimes[0] * 1000;
	float currAnimTime = fmod(m_animTime, animLength);
	int previousKeyIndex; // The place in the array in which the previous key is at

	//currAnimTime = 0.0f;

	for(unsigned int i = 0; m_keyFrameTimes.size() > i; i++)
	{
		if(m_keyFrameTimes[i] * 1000 >= currAnimTime)
		{
			if(i == 0)
			{
				previousKeyIndex = 0;
			}
			else
			{
				previousKeyIndex = i - 1;
			}

			float timeDifference = m_keyFrameTimes[i] * 1000 - m_keyFrameTimes[previousKeyIndex] * 1000;
			float timeToPrevious = currAnimTime - m_keyFrameTimes[previousKeyIndex] * 1000;
			float difference = 0.0f;
			if(timeDifference > 0)
			{
				difference = timeToPrevious / timeDifference;
			}
			
			for (int k = 0; k < m_vertexCount; k++)
			{
				m_model[k].x = m_keyFrameData[previousKeyIndex][k].x + ( (m_keyFrameData[previousKeyIndex + 1][k].x - m_keyFrameData[previousKeyIndex][k].x) * difference);
				m_model[k].y = m_keyFrameData[previousKeyIndex][k].y + ( (m_keyFrameData[previousKeyIndex + 1][k].y - m_keyFrameData[previousKeyIndex][k].y) * difference);
				m_model[k].z = m_keyFrameData[previousKeyIndex][k].z + ( (m_keyFrameData[previousKeyIndex + 1][k].z - m_keyFrameData[previousKeyIndex][k].z) * difference);
			}

			break;
		}
	}

	return true;
}

bool InstanceModelClass::InitializeModelData()
{
	for (int k = 0; k < m_vertexCount; k++)
	{
		m_model[k].x = m_keyFrameData[0][k].x;
		m_model[k].y = m_keyFrameData[0][k].y;
		m_model[k].z = m_keyFrameData[0][k].z;
		
		m_model[k].bx = m_keyFrameData[0][k].bx;
		m_model[k].by = m_keyFrameData[0][k].by;
		m_model[k].bz = m_keyFrameData[0][k].bz;

		m_model[k].nx = m_keyFrameData[0][k].nx;
		m_model[k].ny = m_keyFrameData[0][k].ny;
		m_model[k].nz = m_keyFrameData[0][k].nz;

		m_model[k].tu = m_keyFrameData[0][k].tu;
		m_model[k].tv = m_keyFrameData[0][k].tv;

		m_model[k].tx = m_keyFrameData[0][k].tx;
		m_model[k].ty = m_keyFrameData[0][k].ty;
		m_model[k].tz = m_keyFrameData[0][k].tz;
	}

	return true;
}

bool InstanceModelClass::UpdateBuffers(ID3D11DeviceContext * deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hResult;
	VertexType * vertices;
	vertices = new VertexType[m_vertexCount];

	//initialize vertex array to zeroes at first
	memset(vertices, 0, (sizeof(VertexType) *m_vertexCount));

	for(int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);
		vertices[i].normal   = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].tangent  = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].texture  = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
	}
	
	hResult = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if(hResult != S_OK)
	{
		return false;
	}

	VertexType * dataPtr = (VertexType*)resource.pData;

	memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) *m_vertexCount));

	deviceContext->Unmap(m_vertexBuffer, 0);
	
	delete vertices;
	vertices = 0;

	return true;
}