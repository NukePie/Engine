#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include <vector>
#include <iostream>
#include <fstream>
#include "common.h"
using namespace std;

class Importer{

public:
	struct cameraDataType
	{
		char name[32];
		float x, y, z; //camera position
		float rotX, rotY, rotZ;
	};

public:

	Importer();
	Importer(const Importer&);
	~Importer();

	void Initialize(char*);
	bool LoadModel();
	bool LoadCamera();
	bool LoadTexture();
	
	void GetColorTextureName(char *);
	void GetNormalTextureName(char *);
	vector<cameraDataType> GetCameraData();
	vector<float> GetKeyFrameTimes();
	vector<vector<ModelType>> GetModelData();

private:
	char* m_filename;
	char m_colorTextureName[32];
	char m_normalTextureName[32];
	vector<vector<ModelType>> m_modelData;
	vector<cameraDataType> m_cameraData;
	vector<float> m_keyFrameTimes;
};
#endif