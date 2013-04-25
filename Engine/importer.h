

#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class Importer{

private:
	struct vertexDataType
	{
		float x, y, z;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float u, v;
	};

	struct cameraDataType
	{
		float x, y, z;//camera position
	};

public:

	Importer();
	Importer(const Importer&);
	~Importer();

	void Initialize(char*);
	bool LoadModel();
	bool LoadCamera();
	
	void GetCameraData();
	
	bool GetModelData();

private:
	char* m_filename;
	vector<vector<vertexDataType>> m_modelData;
	vector<vector<cameraDataType>> m_cameraData;
	vector<float> m_keyFrameTimes;

};
#endif