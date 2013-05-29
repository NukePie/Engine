#include "terrainclass.h"


TerrainClass::TerrainClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_heightMap = 0;
	m_Texture = 0;
	m_Textures = 0;
	m_CubemapTexture = 0;
	m_Materials = 0;
	m_divideHeight = 16.0f;
}

TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}

bool TerrainClass::Initialize(
	ID3D11Device* device,
	char* heightMapFilename,
	char* materialsFilename,
	char* materialMapFilename,
	char* colorMapFilename,
	WCHAR* cubemapFilename
	)
{
	bool result;

	//load in the height map for the terrain
	result = LoadHeightMap(heightMapFilename);
	if(!result)
	{
		return false;
	}

	//normalize the height of the height map
	NormalizeHeightMap();

	//calculate the normals for the terrain data
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	CalculateTextureCoordinates();

	result=LoadColorMap(colorMapFilename);
	if(!result)
	{
		return false;
	}

	//the material groups array is initialized instead of single vertex and index buffer, each material group will have its own vertex and index array with its own set of textures
	//Initialize the material groups for the terrain
	result = LoadMaterialFile(materialsFilename, materialMapFilename, device);
	if(!result)
	{
		return false;
	}

	result = LoadCubemapTexture(device, cubemapFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TerrainClass::LoadCubemapTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// create the texture object
	m_CubemapTexture = new TextureClass;
	if(!m_CubemapTexture)
	{
		return false;
	}

	// initialize the texture object
	result = m_CubemapTexture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

void TerrainClass::Shutdown()
{
	//release the materials
	ReleaseMaterials();

	//release the texture
	ReleaseTexture();

	//release the height map data
	ShutdownHeightMap();

	//release the cubemap texture
	ReleaseCubemapTexture();

	return;
}

int TerrainClass::GetIndexCount()//returns the number of indices in the terrain model. Called by the shaders that render the terrain. (they need the data)
{
	return m_indexCount;
}

float TerrainClass::GetHeightAtPos(float x, float z)
{
	float y = 0.0f;
	int heightMapIndex = 0;
	if(x < 0 || z < 0 || x > m_terrainWidth || z > m_terrainHeight)
	{
		return -1.0f;
	}

	heightMapIndex = ((int)x) + ((int)z * m_terrainWidth);
	y = m_heightMap[heightMapIndex].y;

	return y;
}

ID3D11ShaderResourceView* TerrainClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool TerrainClass::LoadHeightMap(char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, iX, iY, placeInFile, index;
	unsigned char* bitmapImage;
	unsigned char height;

	//open the height map file in binary
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	//read in the file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	//read in the bitmap info header
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	//save the dimensions of the terrain
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	//calculate the size of the bitmap image data
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	//allocate memory for the bitmap image data
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	//move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//read in the bitmap image data
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	//close the file
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	//create the structure to hold the height map data
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	//initialize the position in the image data buffer
	placeInFile = 0;

	//read the image data into the height map
	for(iY = 0; iY < m_terrainHeight; iY++)
	{
		for(iX = 0; iX < m_terrainWidth; iX++)
		{
			height = bitmapImage[placeInFile];

			index = (m_terrainHeight * iY) + iX;

			m_heightMap[index].x = (float)iX;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)iY;

			//we only want every third color value in the file since we are using a greyscale image
			placeInFile += 3;
		}
	}

	//release the bitmap image data
	delete []bitmapImage;
	bitmapImage = 0;

	return true;
}

bool TerrainClass::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;

	//create a temporary array to hold the un-normalized normal vectors
	normals = new VectorType[ (m_terrainHeight - 1) * (m_terrainWidth - 1) ];
	if(!normals)
	{
		return false;
	}

	//go through all the faces in the mesh and calculate their normals
	for(j = 0; j < (m_terrainWidth - 1); j++)
	{
		for(i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			//get three vertices from the face
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			//calculate the two vectors for this face
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			//calcuate the cross product of those two vectors to get the un-normalized value for this face normal
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	//now go through all the vertices and take an average of each face normal
	//that the vertex touches to get the averaged normal for that vertex
	for(j = 0; j < m_terrainHeight; j++)
	{
		for(i = 0; i < m_terrainWidth; i++)
		{
			//initialize the sum
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			//initialize the count
			count = 0;

			//bottom left face
			if(((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			//bottom right face
			if((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			//upper left face
			if(((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			//upper right face
			if((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			//take the average of the faces touching this vertex
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			//calculate the length of this normal
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			//get an index to the vertex location in the height map array
			index = (j * m_terrainHeight) + i;

			//normalize the final shared normal for this vertex and store it in the height map array
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	//release the temporary normals
	delete []normals;
	normals = 0;

	return true;
}

void TerrainClass::NormalizeHeightMap()
{
	int x, y;

	for(y = 0; y < m_terrainHeight; y++)
	{
		for(x = 0; x < m_terrainWidth; x++)
		{
			m_heightMap[(m_terrainHeight * y) + x].y /= m_divideHeight;
		}
	}

	return;
}

void TerrainClass::ShutdownHeightMap()
{
	if(m_heightMap)
	{
		delete [] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void TerrainClass::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;

	//calculate how much to increment the texture coordinates by
	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;

	//calculate how many times to repeat the texture
	incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	//initialize the tu and tv coordinate values
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	//initialize the tu and tv coordinate indexes
	tuCount = 0;
	tvCount = 0;

	//loop through the entire height map and calculate the tu and tv texture coordinates for each vertex
	for(j = 0; j < m_terrainHeight; j++)
	{
		for(i = 0; i < m_terrainWidth; i++)
		{
			//store hte texture coordinate in the height map
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			//increment the tu texture coordinate by the increment value and increment the index by one
			tuCoordinate += incrementValue;
			tuCount++;

			//check if at the far right end of the texture and if so the start at the beginning again
			if(tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}
		//increment the tv texture coordinate by the increment value and increment the index by one
		tvCoordinate -= incrementValue;
		tvCount++;

		//check if at the top of the texture and if so the start at the bottom again
		if(tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

void TerrainClass::ReleaseTexture()
{
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

void TerrainClass::ReleaseCubemapTexture()
{
	if(m_CubemapTexture)
	{
		m_CubemapTexture->Shutdown();
		delete m_CubemapTexture;
		m_CubemapTexture = 0;
	}

	return;
}

bool TerrainClass::LoadColorMap(char* filename)
{
	int error, imageSize, i, j, k, index, colorMapWidth, colorMapHeight;
	FILE* filePtr;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;

	//open the color map file
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}
	//read in the file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	//read in the bitmap info Header
	//read in the file header
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	//make sure the color map dimensions are the same as the terrain dimensions
	colorMapWidth = bitmapInfoHeader.biWidth;
	colorMapHeight = bitmapInfoHeader.biHeight;

	if((colorMapWidth != m_terrainWidth) || (colorMapHeight != m_terrainHeight))
	{
		return false;
	}

	//calculate the size of the bitmap image data
	imageSize = colorMapWidth * colorMapHeight * 3;

	//allocate memory for the bitmap image data
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	//move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//read in the bitmap image data
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	//close the file
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	//Initialize the position in the image data buffer
	k = 0;

	//read the image data into the color map portion of the height map structure
	for(j = 0; j < colorMapHeight; j++)
	{
		for(i = 0; i < colorMapWidth; i++)
		{
			index = (colorMapHeight * j) +i;

			m_heightMap[index].b = (float)bitmapImage[k] / 255.0f;
			m_heightMap[index].g = (float)bitmapImage[k + 1] / 255.0f;
			m_heightMap[index].r = (float)bitmapImage[k + 2] / 255.0f;

			k += 3;
		}
	}

	//release the bitmap image data
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;

}

bool TerrainClass::LoadMaterialFile(char* filename, char* materialMapFilename, ID3D11Device* device)
{
	ifstream fin;
	char input;
	int i, error;
	char inputFilename[128];
	unsigned int stringLength;
	WCHAR textureFilename[128];
	bool result;

	//open the materials info txtfile
	fin.open(filename);
	if(fin.fail())
	{
		return false;
	}

	//read up to the value of the texture count
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	//read in the texture count
	fin >> m_textureCount;

	//create the texture objects array
	m_Textures = new TextureClass[m_textureCount];
	if(!m_Textures)
	{
		return false;
	}

	//load each of the textures in
	for(i= 0; i<m_textureCount; i++)
	{
		fin.get(input);
		while(input != ':')
		{
			fin.get(input);
		}

		fin >> inputFilename;

		//convert the characterfilename to WCHAR
		error = mbstowcs_s(&stringLength, textureFilename, 128, inputFilename, 128);
		if (error != 0)
		{
			return false;
		}

		//load the texture alpha map
		result = m_Textures[i].Initialize(device, textureFilename);
		if(!result)
		{
			return false;
		}
	}
	
	//read up to the value of the material count
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	// read in the material count
	fin >> m_materialCount;

	//create the material group array
	m_Materials = new MaterialGroupType[m_materialCount];
	if(!m_Materials)
	{
		return false;
	}

	//initialize the material group array
	for(i=0; i<m_materialCount; i++)
	{
		m_Materials[i].vertexBuffer = 0;
		m_Materials[i].indexBuffer = 0;
		m_Materials[i].vertices = 0;
		m_Materials[i].indices = 0;
	}

	//load each of the material group indexes in
	for(i = 0; i < m_materialCount; i++)
	{
		fin.get(input);
		while(input != ':')
		{
			fin.get(input);
		}

		fin >> m_Materials[i].textureIndex1 >> m_Materials[i].textureIndex2 >> m_Materials[i].alphaIndex;
		fin >> m_Materials[i].red >> m_Materials[i].green >> m_Materials[i].blue;	
	}

	//close the material info file
	fin.close();

	//load the material index map
	result = LoadMaterialMap(materialMapFilename);
	if(!result)
	{
		return false;
	}

	//load the vertex buffer for each material group with the terrain data
	result = LoadMaterialBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TerrainClass::LoadMaterialMap(char* filename)
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;

	// open the material map file in binary
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// read in the file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// read in the bitmap info header
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// make sure the material index map dimensions are the same as the terrain dimensions for 1 to 1 mapping
	if((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
	{
		return false;
	}

	// calculate the size of teh bitmap image data
	imageSize = bitmapInfoHeader.biWidth * bitmapInfoHeader.biHeight * 3;

	// allocate memory for the bitmap image data
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// read in the bitmap image data
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// close the file
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// initialize the position in the image data buffer so each vertice has a material index associated with it
	k = 0;

	// read the material index data into the height map structure
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].rIndex = (int)bitmapImage[k + 2];
			m_heightMap[index].gIndex = (int)bitmapImage[k + 1];
			m_heightMap[index].bIndex = (int)bitmapImage[k + 0];

			k += 3;
		}
	}

	// release the bitmap image data
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

bool TerrainClass::LoadMaterialBuffers(ID3D11Device* device)
{
	int maxVertexCount, maxIndexCount, i, j, index1, index2, index3, index4, redIndex, greenIndex, blueIndex, index, vIndex;
	bool found;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// create the value for the maximum number of vertices a material group could possibly have
	maxVertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	//set the index count ot the same as the maximum vertex count
	maxIndexCount = maxVertexCount;

	// initialize vertex and index arrays for each material group to the maximum size
	for(i = 0; i < m_materialCount; i++)
	{
		// create the temporary vertex array for this material group
		m_Materials[i].vertices = new VertexType[maxVertexCount];
		if(!m_Materials[i].vertices)
		{
			return false;
		}

		// create the temporary index array for this material group
		m_Materials[i].indices = new unsigned long[maxIndexCount];
		if(!m_Materials[i].indices)
		{
			return false;
		}

		// initialize the counts to zero
		m_Materials[i].vertexCount = 0;
		m_Materials[i].indexCount = 0;
	}

	// now loop through the terrain and build the vertex arrays for each material group
	for(j = 0; j < (m_terrainHeight - 1); j++)
	{
		for(i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (m_terrainHeight * j) + i;				// bottom left
			index2 = (m_terrainHeight * j) + (i + 1);		// bottom right
			index3 = (m_terrainHeight * (j + 1)) + i;		// upper left
			index4 = (m_terrainHeight * (j + 1)) + (i + 1);	// upper right

			// query the upper left corner vertex for the material index
			redIndex = m_heightMap[index3].rIndex;
			greenIndex = m_heightMap[index3].gIndex;
			blueIndex = m_heightMap[index3].bIndex;

			// find which material group this vertex belongs to
			index = 0;
			found = false;
			while(!found)
			{
				if((redIndex == m_Materials[index].red) && (greenIndex == m_Materials[index].green) && (blueIndex == m_Materials[index].blue))
				{
					found = true;
				}
				else
				{
					index++;
				}
			}

			// set the index position in the vertex and index array to the count
			vIndex = m_Materials[index].vertexCount;

			// upper left
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(0.0f, 0.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index3].r, m_heightMap[index3].g, m_heightMap[index3].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// upper right
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(1.0f, 0.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// bottom left
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(0.0f, 1.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// bottom left
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(0.0f, 1.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// upper right
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(1.0f, 0.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// bottom right
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(1.0f, 1.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index2].r, m_heightMap[index2].g, m_heightMap[index2].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// increment the vertex and index array counts
			m_Materials[index].vertexCount += 6;
			m_Materials[index].indexCount += 6;
		}
	}

	// now create the vertex and index buffers from the vertex and index arrays for each material group
	for(i = 0; i < m_materialCount; i++)
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_Materials[i].vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		vertexData.pSysMem = m_Materials[i].vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_Materials[i].vertexBuffer);
		if(FAILED(result))
		{
			return false;
		}

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_Materials[i].indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		indexData.pSysMem = m_Materials[i].indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_Materials[i].indexBuffer);
		if(FAILED(result))
		{
			return false;
		}

		delete [] m_Materials[i].vertices;
		m_Materials[i].vertices = 0;

		delete [] m_Materials[i].indices;
		m_Materials[i].indices = 0;
	}

	return true;
}

void TerrainClass::ReleaseMaterials()
{
	int i;

	// release the material groups
	if(m_Materials)
	{
		for(i = 0; i < m_materialCount; i++)
		{
			if(m_Materials[i].vertexBuffer)
			{
				m_Materials[i].vertexBuffer->Release();
				m_Materials[i].vertexBuffer = 0;
			}

			if(m_Materials[i].indexBuffer)
			{
				m_Materials[i].indexBuffer->Release();
				m_Materials[i].indexBuffer = 0;
			}

			if(m_Materials[i].vertices)
			{
				delete [] m_Materials[i].vertices;
				m_Materials[i].vertices = 0;
			}

			if(m_Materials[i].indices)
			{
				delete [] m_Materials[i].indices;
				m_Materials[i].indices = 0;
			}
		}

		delete [] m_Materials;
		m_Materials = 0;
	}

	// release the terrain textures and alpha maps
	if(m_Textures)
	{
		for(i = 0; i < m_textureCount; i++)
		{
			m_Textures[i].Shutdown();
		}

		delete [] m_Textures;
		m_Textures = 0;
	}

	return;
}

bool TerrainClass::Render(
	ID3D11DeviceContext* deviceContext,
	TerrainShaderClass* shader,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	D3DXVECTOR4 ambientColor,
	D3DXVECTOR4 diffuseColor,
	D3DXVECTOR3 lightDirection,
	D3DXVECTOR3 camPosition,
	float fogStart,
	float fogEnd
	)
{
	unsigned int stride;
	unsigned int offset;
	int i;
	bool result;

	// set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	// set the shader parameters that it will use for rendering
	result = shader->SetShaderParameters(
		deviceContext,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		ambientColor,
		diffuseColor,
		lightDirection,
		fogStart,
		fogEnd
		);
	if(!result)
	{
		return false;
	}

	// set the type of primitive that should be rendered from the vertex buffers, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// render each material group
	for(i = 0; i < m_materialCount; i++)
	{
		// set the vertex buffer to active in the input assembler so it can be rendered
		deviceContext->IASetVertexBuffers(0, 1, &m_Materials[i].vertexBuffer, &stride, &offset);

		// set the index buffer to active in the input assembler so it can be rendered
		deviceContext->IASetIndexBuffer(m_Materials[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//if the material group has a valid second texture index then this is a blended terrain polygon
		if(m_Materials[i].textureIndex2 != -1)
		{
			result = shader->SetShaderTextures(
				deviceContext,
				m_Textures[m_Materials[i].textureIndex1].GetTexture(),
				m_Textures[m_Materials[i].textureIndex2].GetTexture(),
				m_Textures[m_Materials[i].alphaIndex].GetTexture(),
				m_CubemapTexture->GetTexture(),
				camPosition,
				true
				);
		}

		// if not the its just a single textured polygon
		else
		{
			result = shader->SetShaderTextures(
				deviceContext,
				m_Textures[m_Materials[i].textureIndex1].GetTexture(),
				NULL,
				NULL,
				m_CubemapTexture->GetTexture(),
				camPosition,
				false
				);
		}

		// check if the textures were set or not
		if(!result)
		{
			return false;
		}

		// now render the prepared buffers with the shader
		shader->RenderShader(deviceContext, m_Materials[i].indexCount);
	}

	return true;
}