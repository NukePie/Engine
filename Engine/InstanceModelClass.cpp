#include "instancemodelclass.h"

using namespace std;

InstanceModelClass::InstanceModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_instanceBuffer = 0;
	m_TextureArray = 0;
	m_model = 0;
	m_instanceCount = 1;
	m_importer = 0;
}

InstanceModelClass::InstanceModelClass(const InstanceModelClass& other)
{
}

InstanceModelClass::~InstanceModelClass()
{
}

bool InstanceModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext * deviceContext, char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2)
{
	bool result;
	m_animTime = 0.0f;

	m_model = new ModelType[36];

	result = LoadFile("../Engine/test.txt");
	if(!result)
	{
		return false;
	}

	InitializeModelData();
	//InterpolateFrameData(0.0f);
	CalculateModelVectors();

	result = InitializeBuffers(device); //calling initialization functions for vertex and index buffers
	if(!result)
	{
		return false;
	}

	result = LoadTextures(device, textureFilename1, textureFilename2);
	if(!result)
	{
		return false;
	}

	return true;
}

void InstanceModelClass::Shutdown()
{
	ReleaseTextures();

	ShutdownBuffers(); //release the vertex and index buffers

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
	RenderBuffers(deviceContext); //put the vertex and index buffers on the graphics pipeline to prepare them for drawing

	return;
}

int InstanceModelClass::GetIndexCount()
{
	return m_indexCount; //return the number of vertices in the model
}

int InstanceModelClass::GetVertexCount()
{
	return m_vertexCount; //return the number of vertices in the model
}

int InstanceModelClass::GetInstanceCount()
{
	return m_instanceCount; //return the number of vertices in the model
}

void InstanceModelClass::SetInstanceCount(int count)
{
	m_instanceCount = count; //set the number of instances of the model
}

ID3D11ShaderResourceView** InstanceModelClass::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}

bool InstanceModelClass::InitializeBuffers(ID3D11Device* device)
{
	//VertexType* vertices;
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

	//load the vertex array and index array with data
	for(i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);

		indices[i] = i;
	}

	//setting up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;//D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//| D3D11_CPU_ACCESS_READ;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer); //create vertex buffer
	if(FAILED(result))
	{
		return false;
	}

	//setting up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)*m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//give the subresource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer); //create index buffer
	if(FAILED(result))
	{
		return false;
	}

	//deleting the vertex and index arrays since we are done with them
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	//create instance array
	instances = new InstanceType[m_instanceCount];
	if(!instances)
	{
		return false;
	}

	//load the instance array with data
	for(int i = 0; i < m_instanceCount; i++)
	{
		instances[i].position = D3DXVECTOR3(i * 20.0f, 0.0f, 0.0f);
	}

	//set up instance description
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	//give the subresource structure a pointer to the instance data
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	//create buffer
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	delete[] instances;
	instances = 0;
	
	return true;
}

bool InstanceModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
	bool result;

	m_TextureArray = new TextureArrayClass;
	if(!m_TextureArray)
	{
		return false;
	}

	result = m_TextureArray->Initialize(device, filename1, filename2);
	if(!result)
	{
		return false;
	}

	return true;
}

void InstanceModelClass::CalculateModelVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal;	//, normal;

	faceCount = m_vertexCount / 3;
	index = 0;

	for(i = 0; i < faceCount; i++)
	{
		vertex1.x = m_model[index].x;
		vertex1.y = m_model[index].y;
		vertex1.z = m_model[index].z;
		vertex1.tu = m_model[index].tu;
		vertex1.tv = m_model[index].tv;
		vertex1.nx = m_model[index].nx;
		vertex1.ny = m_model[index].ny;
		vertex1.nz = m_model[index].nz;
		index++;

		vertex2.x = m_model[index].x;
		vertex2.y = m_model[index].y;
		vertex2.z = m_model[index].z;
		vertex2.tu = m_model[index].tu;
		vertex2.tv = m_model[index].tv;
		vertex2.nx = m_model[index].nx;
		vertex2.ny = m_model[index].ny;
		vertex2.nz = m_model[index].nz;
		index++;

		vertex3.x = m_model[index].x;
		vertex3.y = m_model[index].y;
		vertex3.z = m_model[index].z;
		vertex3.tu = m_model[index].tu;
		vertex3.tv = m_model[index].tv;
		vertex3.nx = m_model[index].nx;
		vertex3.ny = m_model[index].ny;
		vertex3.nz = m_model[index].nz;
		index++;

		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		//CalculateNormal(tangent, binormal, normal);	//The correct normal is already loaded from the obj file

		//m_model[index-1].nx = normal.x;
		//m_model[index-1].ny = normal.y;
		//m_model[index-1].nz = normal.z;
		m_model[index-1].tx = tangent.x;
		m_model[index-1].ty = tangent.y;
		m_model[index-1].tz = tangent.z;
		m_model[index-1].bx = binormal.x;
		m_model[index-1].by = binormal.y;
		m_model[index-1].bz = binormal.z;
		
		//m_model[index-2].nx = normal.x;
		//m_model[index-2].ny = normal.y;
		//m_model[index-2].nz = normal.z;
		m_model[index-2].tx = tangent.x;
		m_model[index-2].ty = tangent.y;
		m_model[index-2].tz = tangent.z;
		m_model[index-2].bx = binormal.x;
		m_model[index-2].by = binormal.y;
		m_model[index-2].bz = binormal.z;
		
		//m_model[index-3].nx = normal.x;
		//m_model[index-3].ny = normal.y;
		//m_model[index-3].nz = normal.z;
		m_model[index-3].tx = tangent.x;
		m_model[index-3].ty = tangent.y;
		m_model[index-3].tz = tangent.z;
		m_model[index-3].bx = binormal.x;
		m_model[index-3].by = binormal.y;
		m_model[index-3].bz = binormal.z;
	}

	return;
}

void InstanceModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;

	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void InstanceModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;

	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
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

void InstanceModelClass::ShutdownBuffers() //releases the buffers created in the initializebuffers function
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

	/*
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount, groupCount);
	if(!result)
	{
		return false;
	}
	*/
	// m_vertexCount = (faceCount * 3);
	// m_indexCount = (faceCount * 3);
	
	/*
	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
	if(!result)
	{
		return false;
	}
	*/

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
			float difference = timeToPrevious / timeDifference;
			
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