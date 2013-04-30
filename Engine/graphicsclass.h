#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//INCLUDES
#include <Windows.h>
#include "d3dclass.h"
#include "cameraclass.h"
#include "instancemodelclass.h"
#include "particleshaderclass.h"
#include "particlesystemclass.h"
#include "lightclass.h"
#include "inputclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "terrainclass.h"
#include "rendertextureclass.h"
#include "depthshaderclass.h"
#include "shadowshaderclass.h"
#include "modellistclass.h"
#include "frustumclass.h"
#include "importer.h"

//GLOBALS
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

const int SHADOWMAP_WIDTH = 2048;
const int SHADOWMAP_HEIGHT = 2048;

//CLASS
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, HINSTANCE);
	void Shutdown();
	bool Frame(unsigned long);

private:
	bool HandleInput(float);
	bool RenderSceneToTexture();
	bool Render(unsigned long);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	Importer* m_Importer;
	InstanceModelClass *m_Model, *m_City, *m_Bullet, *m_Sphere, *m_AxisModel;
	LightClass* m_Light;
	ParticleShaderClass* m_ParticleShader;
	ParticleSystemClass* m_ParticleSystem, *m_ParticleSystem2, *m_ParticleSystem3;
	InputClass* m_Input;
	TerrainClass* m_Terrain;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	TerrainShaderClass* m_TerrainShader;

	RenderTextureClass* m_RenderTexture;
	DepthShaderClass* m_DepthShader;
	ShadowShaderClass* m_ShadowShader;

	vector<Importer::cameraDataType> m_Cameras;
	int m_CurrentCam;
	bool m_PreviousZKeyState;

	ModelListClass* m_ModelList;
	FrustumClass* m_Frustrum;
};

#endif
