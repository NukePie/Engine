#include "importer.h"

Importer::Importer(){}
Importer::Importer(const Importer&){}
Importer::~Importer(){}

void Importer::Initialize(char* filename)
{
	m_filename = filename;
}
bool Importer::LoadModel()
{
	ifstream fin;
	char input;

	fin.open(m_filename);
	if(fin.fail() == true)
	{
		return false;
	}

	fin.get(input);
	while(!fin.eof())
	{
		//char test[256];
		//fin >> test;

		fin.get(input);
		if(input == 'v')
		{
			fin.get(input);
			if(input == 'l')
			{
				int tempCount = 0;
				//int count;
				vector<vertexDataType> frameData;
				vertexDataType vertexData;

				while(input != 'c')
				{
					fin.get(input);
					if(input == 'p')
					{
						fin >> vertexData.x;
						fin >> vertexData.y;
						fin >> vertexData.z;
						tempCount++;
					}

					if(input == 'n')
					{
						fin >> vertexData.nx;
						fin >> vertexData.ny;
						fin >> vertexData.nz;
						tempCount++;				
					}

					if(input == 't')
					{
						fin >> vertexData.tx;
						fin >> vertexData.ty;
						fin >> vertexData.tz;
						tempCount++;				
					}
					
					if(input == 'b')
					{
						fin >> vertexData.bx;
						fin >> vertexData.by;
						fin >> vertexData.bz;
						tempCount++;				
					}

					if(input == 'u')
					{
						fin >> vertexData.u;
						fin >> vertexData.v;
						tempCount++;				
					}

					if(tempCount == 5)
					{
						frameData.push_back(vertexData);
						tempCount = 0;
					}
					
				}

				m_modelData.push_back(frameData);
			}
		}

		if(input == 't')
		{
			fin.get(input);
			if(input == 'i')
			{
				float temptime;
				fin >> temptime;
				m_keyFrameTimes.push_back(temptime);
			}
		}
	}

	fin.close();

	return true;
}

bool Importer::LoadCamera()
{

	ifstream fin;
	char input;

	fin.open(m_filename);
	if(fin.fail() == true)
	{
		return false;
	}

	fin.get(input);
	while(!fin.eof())
	{
		fin.get(input);
		if(input == 'c')
		{
			fin.get(input);
			if(input == 'e')
			{
				cameraDataType cameraData;
				vector<cameraDataType> frameCamData;

				//int frameCount

				if(input == 'e')
				{
					fin >> cameraData.x;
					fin >> cameraData.y;
					fin >> cameraData.z;

					//Count++;
				}

				/*if(Count == 4)
				{
					frameCamData.push_back(cameraData);
					Count = 0;
				}

				fin.get(input);*/
			

				m_cameraData.push_back(frameCamData);
			}
		}
	}
	
	fin.close();
}