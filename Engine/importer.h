

#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class Importer{

public:
	struct cameraDataType
	{
		char name[32];
		float x, y, z; //camera position
	};

private:
	struct vertexDataType
	{
		float x, y, z;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float u, v;
	};

public:

	Importer();
	Importer(const Importer&);
	~Importer();

	void Initialize(char*);
	bool LoadModel();
	bool LoadCamera();
	
	vector<cameraDataType> GetCameraData();
	
	bool GetModelData();

private:
	char* m_filename;
	vector<vector<vertexDataType>> m_modelData;
	vector<cameraDataType> m_cameraData;
	vector<float> m_keyFrameTimes;

};
#endif