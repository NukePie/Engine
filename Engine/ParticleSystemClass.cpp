#include "particlesystemclass.h"


ParticleSystemClass::ParticleSystemClass()
{
	m_Texture = 0;
	m_particleList = 0;
	m_vertices = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;


}

ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other)
{
}

ParticleSystemClass::~ParticleSystemClass()
{
}

bool ParticleSystemClass::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	result = LoadTexture(device, textureFilename); //load the texture that is used for the particles
	if(!result)
	{
		return false;
	}

	result = InitializeParticleSystem();
	if(!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

void ParticleSystemClass::Shutdown()
{
	ShutdownBuffers();

	ShutdownParticleSystem();

	ReleaseTexture();

	return;
}

bool ParticleSystemClass::Frame(float frameTime, ID3D11DeviceContext* deviceContext, D3DXVECTOR3 emitterPosition, D3DXVECTOR3 camPos)
{
	bool result;

	KillParticles();

	EmitParticles(frameTime, emitterPosition);

	UpdateParticles(frameTime, camPos);

	result = UpdateBuffers(deviceContext); //updating dynamic vertex buffer with new position of each particle
	if(!result)
	{
		return false;
	}

	return true;
}

void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext)
{
		RenderBuffers(deviceContext); //put the vertex and index buffers on teh graphics pipeline to prepare them for drawing

		return;
}

ID3D11ShaderResourceView* ParticleSystemClass::GetTexture() //returns a pointer to the particle texture resource
{
	return m_Texture->GetTexture();
}

int ParticleSystemClass::GetIndexCount() //returns the count of indexes in the index buffer
{
	return m_indexCount;
}

bool ParticleSystemClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass; //create texture object
	if(!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, filename); //initialize texture object
	if(!result)
	{
		return false;
	}

	return true;
}

void ParticleSystemClass::ReleaseTexture()
{
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

bool ParticleSystemClass::InitializeParticleSystem()
{
	int i;

	//set how large the area that particles can spawn in is
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	//set the speed and speed variation of particles
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	//set the physical size of the particles
	m_particleSize = 0.2f;

	//set the number of particles to emit per second
	m_particlesPerSecond = 250.0f;

	//set the maximum number of particles allowed in the particle system
	m_maxParticles = 5000;

	//create the particle list
	m_particleList = new ParticleType[m_maxParticles];
	if(!m_particleList)
	{
		return false;
	}

	//initialize the particle list
	for(i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	//initialize the current particle count to zero since none are emitted yet
	m_currentParticleCount = 0;

	//clear the initial accumulated time for the particle per second emmission rate
	m_accumulatedTime = 0.0f;

	return true;
}

void ParticleSystemClass::ShutdownParticleSystem()
{
	if(m_particleList)
	{
		delete [] m_particleList;
		m_particleList = 0;
	}

	return;
}

bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//set the maximum number of vertices in the vertex array
	m_vertexCount = m_maxParticles* 6;

	//set the maximum number of indices in the index array
	m_indexCount = m_vertexCount;

	//create the vertex array for the particles that will be rendered
	m_vertices = new VertexType[m_vertexCount];
	if(!m_vertices)
	{
		return false;
	}

	//create the index array
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	//initialize vertex array to zeros at first
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	//initialize the index array
	for(i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	//set up the description of the dynamic vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//give the subresrouce structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//release the index array since it is no longer needed
	delete [] indices;
	indices = 0;

	return true;
}

void ParticleSystemClass::ShutdownBuffers()
{
	//Release the index buffer
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	//Release the vertexbuffer
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ParticleSystemClass::EmitParticles(float frameTime, D3DXVECTOR3 emitterPosition)//called each frame to emit new particles. The particle array is sorted in correct Z depth order for rendering using an alpha blend so that we do not get any visual artifacs. 
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	//increment the frame time
	m_accumulatedTime += frameTime;

	//set emit particle to false for starters
	emitParticle = false;

	//check if it's time to emit a new particle or not
	if(m_accumulatedTime > (50000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	//if there are particles to emit them emit one per frame
	if((emitParticle == true) && (m_currentParticleCount < (m_maxParticles -1)))
	{
		m_currentParticleCount++;

		//generate the randomized particle properties
		positionX = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationX + emitterPosition.x;
		positionY = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationY + emitterPosition.y;
		positionZ = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationZ + emitterPosition.z;

		velocity = m_particleVelocity + (((float)rand()-(float)rand())/RAND_MAX) * m_particleVelocityVariation;

		/*
		red = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		green = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		blue = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		*/

		red = 1.0f;
		green = 1.0f;
		blue = 1.0f;

		//the particles need to be rendered back to front for blending so we need to sort the particle array
		//we will sort using Z depth
		index = 0;
		found = false;
		while(found)
		{
			if((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		//now that the location where to insert into we will copy the array over by one position from the index to make room for the new particle
		i = m_currentParticleCount;
		j = i - 1;

		while(i !=index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}
		//insert it into the particle array in the correct depth order
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}

	return;
}

void ParticleSystemClass::UpdateParticles(float FrameTime, D3DXVECTOR3 camPos)
{
	int i;
	//D3DXVECTOR3 
	//each frame we will update all particles by making them move using the position, velocity and the frame time
	for(i=0; i<m_currentParticleCount; i++)
	{
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * FrameTime * 0.001f);
		m_particleList[i].m_rotationX = camPos.x;
		m_particleList[i].m_rotationY = camPos.y;
		m_particleList[i].m_rotationZ = camPos.z;
	}

	return;
}

void ParticleSystemClass::KillParticles()
{
	int i, j;

	//kill all the particles that have gone below a certain height range
	for(i=0; i<m_maxParticles; i++)
	{
		if((m_particleList[i].active == true) && (m_particleList[i].positionY < -3.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			//shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly
			for(j=i; j<m_maxParticles-1; j++)
			{
				m_particleList[j].positionX	= m_particleList[j+1].positionX;
				m_particleList[j].positionY	= m_particleList[j+1].positionY;
				m_particleList[j].positionZ	= m_particleList[j+1].positionZ;
				m_particleList[j].red		= m_particleList[j+1].red;
				m_particleList[j].green		= m_particleList[j+1].green;
				m_particleList[j].blue		= m_particleList[j+1].blue;
				m_particleList[j].velocity	= m_particleList[j+1].velocity;
				m_particleList[j].active	= m_particleList[j+1].active;
			}
		}
	}

	return;
}

bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)//is called every frame and rebuilds the entire dynamic vertexbuffer with the updated position of all the particles in the particle system
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	//initialize vertex array to zeroes at first
	memset(m_vertices, 0, (sizeof(VertexType) *m_vertexCount));

	//build the vertex array from the particle list array. each particle is a quad of two tris
	index = 0;

	for(i=0; i<m_currentParticleCount; i++)
	{
		//bottom left
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		
		m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(-m_particleList[i].positionX, -m_particleList[i].positionY, -m_particleList[i].positionZ));

		D3DXVec3TransformCoord(
			&m_vertices[index].position,
			&m_vertices[index].position,	
			&RotateParticle(
				D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ),
				m_vertices[index].position,
				m_particleList[i].m_rotationX,
				m_particleList[i].m_rotationY,
				m_particleList[i].m_rotationZ)
				);

		m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ));

		
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//top left
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
				m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(-m_particleList[i].positionX, -m_particleList[i].positionY, -m_particleList[i].positionZ));

		D3DXVec3TransformCoord(
			&m_vertices[index].position,
			&m_vertices[index].position,
			&RotateParticle(
				D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ),
				m_vertices[index].position,
				m_particleList[i].m_rotationX,
				m_particleList[i].m_rotationY,
				m_particleList[i].m_rotationZ)
				);

		m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ));


		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//bottom right
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
				m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(-m_particleList[i].positionX, -m_particleList[i].positionY, -m_particleList[i].positionZ));

		D3DXVec3TransformCoord(
			&m_vertices[index].position,
			&m_vertices[index].position,
			&RotateParticle(
				D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ),
				m_vertices[index].position,
				m_particleList[i].m_rotationX,
				m_particleList[i].m_rotationY,
				m_particleList[i].m_rotationZ)
				);

		m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ));

		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//bottom right
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
				m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(-m_particleList[i].positionX, -m_particleList[i].positionY, -m_particleList[i].positionZ));

		D3DXVec3TransformCoord(
			&m_vertices[index].position,
			&m_vertices[index].position,
			&RotateParticle(
				D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ),
				m_vertices[index].position,
				m_particleList[i].m_rotationX,
				m_particleList[i].m_rotationY,
				m_particleList[i].m_rotationZ)
				);

		m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ));

		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//top left
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
				m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(-m_particleList[i].positionX, -m_particleList[i].positionY, -m_particleList[i].positionZ));

		D3DXVec3TransformCoord(
			&m_vertices[index].position,
			&m_vertices[index].position,
			&RotateParticle(
				D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ),
				m_vertices[index].position,
				m_particleList[i].m_rotationX,
				m_particleList[i].m_rotationY,
				m_particleList[i].m_rotationZ)
				);

		m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ));

		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		//top right
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
				m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(-m_particleList[i].positionX, -m_particleList[i].positionY, -m_particleList[i].positionZ));

		D3DXVec3TransformCoord(
			&m_vertices[index].position,
			&m_vertices[index].position,
			&RotateParticle(
				D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ),
				m_vertices[index].position,
				m_particleList[i].m_rotationX,
				m_particleList[i].m_rotationY,
				m_particleList[i].m_rotationZ)
				);

		m_vertices[index].position = m_vertices[index].position.operator+(D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ));

		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	//lock the vertex Buffer
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}
	//get a pointer to the data in the vertex buffer
	verticesPtr = (VertexType*)mappedResource.pData;

	//copy the data into the vertex buffer
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) *m_vertexCount));

	//unlock the vertex buffer
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)//is used to draw the particle buffers. it places the geometry on the pipeline so that the shader can render it
{
	unsigned int stride;
	unsigned int offset;

	//set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//set the vertexbuffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//set the index buffer to active in the input assebler so it can be rendered
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

D3DXMATRIX ParticleSystemClass::RotateParticle(D3DXVECTOR3 pPos, D3DXVECTOR3 vPos, float rotX, float rotY, float rotZ)
	{

	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	pitch = rotX * 0.0174532925f;
	yaw   = rotY * 0.0174532925f;
	roll  = rotZ * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	return rotationMatrix;
}