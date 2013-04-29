#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D				= 0;
	m_Camera			= 0;
	m_Model				= 0;
	m_City				= 0;
	m_Bullet			= 0;
	m_Sphere			= 0;
	m_Light				= 0;
	m_ParticleShader	= 0;
	m_ParticleSystem	= 0;
	m_ParticleSystem2	= 0;
	m_ParticleSystem3	= 0;
	m_Input				= 0;
	m_Timer				= 0;
	m_Terrain			= 0;
	m_Position			= 0;
	m_TerrainShader		= 0;
	m_ModelList			= 0;
	m_Frustrum			= 0;
	m_Importer			= 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, HINSTANCE hinstance)
{
	bool result;

	float cameraX, cameraY, cameraZ;

	//create and initialize the input object
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd,L"Could not Initilize the input object, too Bad",L"Error",MB_OK);
		return false;
	}

	m_D3D = new D3DClass; //Create Direct3D object
	if(!m_D3D)
	{
		return false;
	}
	//Initialize the object
	result = m_D3D->Initialize(
		screenWidth,
		screenHeight,
		VSYNC_ENABLED,
		hwnd,
		FULL_SCREEN,
		SCREEN_DEPTH,
		SCREEN_NEAR
		);
	if(!result)
	{
		MessageBox(hwnd,L"Could not Initilize Direct3D, too Bad",L"Error",MB_OK);
		return false;
	}

	//Create Camera object
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 0.0f;

	//Set the initial position of the camera
	m_Camera->SetPosition(cameraX, cameraY, cameraZ);
	m_Camera->SetRotation(0, 0, 0); //this gets overridden by something further down, hehe

	

	m_Importer = new Importer;
	if(!m_Importer)
	{
		return false;
	}

	m_Importer->Initialize("../Engine/cube.txt");

	m_Importer->LoadCamera();
		
	m_Cameras = m_Importer->GetCameraData();

	//create the terrain object
	m_Terrain = new TerrainClass;
	if(!m_Terrain)
	{
		return false;
	}

	//initialize the terrain object
	result = m_Terrain->Initialize(
		m_D3D->GetDevice(),
		"../Engine/heightmap.bmp",
		"../Engine/materials2.txt",
		"../Engine/blending3.bmp",
		"../Engine/coolblend.bmp",
		L"../Engine/cubemap.dds"
		);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	//create the timer object
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	//initialize the timer object
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	//create the position object
	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}

	m_Position->InitializeMouseLook(screenWidth, screenHeight);

	//set the initial position of the viewer to the same as the initial camera position
	m_Position->SetPosition(cameraX, cameraY, cameraZ);


	//create the particle shader object
	m_ParticleShader = new ParticleShaderClass;
	if(!m_ParticleShader)
	{
		return false;
	}

	//initialize the particle shader object
	result = m_ParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the particle shader object.", L"Error", MB_OK);
		return false;
	}

	//create the particle system object
	m_ParticleSystem = new ParticleSystemClass;
	if(!m_ParticleSystem)
	{
		return false;
	}

	//create the particle system object
	m_ParticleSystem2 = new ParticleSystemClass;
	if(!m_ParticleSystem2)
	{
		return false;
	}

	m_ParticleSystem3 = new ParticleSystemClass;
	if(!m_ParticleSystem2)
	{
		return false;
	}

	//initialize the particle system object
	result = m_ParticleSystem->Initialize(m_D3D->GetDevice(), L"../Engine/brick.dds");
	if(!result)
	{
		return false;
	}

	result = m_ParticleSystem2->Initialize(m_D3D->GetDevice(), L"../Engine/boot.dds");
	if(!result)
	{
		return false;
	}

	result = m_ParticleSystem3->Initialize(m_D3D->GetDevice(), L"../Engine/mamma.dds");
	if(!result)
	{
		return false;
	}

	//////////////////////////////////////
	// BEGIN MODELS

	// Create sphere model object.
	m_Sphere = new InstanceModelClass;
	if(!m_Sphere)
	{
		return false;
	}

	// Initialize sphere model object.
	result = m_Sphere->Initialize(m_D3D->GetDevice(), "../Engine/sphere.obj", L"../Engine/brick.dds", L"../Engine/blankNormal.png");
	if(!result)
	{
		return false;
	}

	// Create tomte model object.
	m_Model = new InstanceModelClass;
	if(!m_Model)
	{
		return false;
	}
	m_Model->SetInstanceCount(4);
	//Initialize tomte model object
	result = m_Model->Initialize(m_D3D->GetDevice(),"../Engine/Tomte.obj", L"../Engine/Tomte_Texture.png", L"../Engine/Tomte_Normal.png");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_Model->SetPosition(0.0f, -30.25f, 0.0f);

		//Create model object
	m_City = new InstanceModelClass;
	if(!m_City)
	{
		return false;
	}
	m_City->SetInstanceCount(1);
	//Initialize model object
	result = m_City->Initialize(m_D3D->GetDevice(),"../Engine/Duk.obj", L"../Engine/duk.png", L"../Engine/blankNormal.png");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_City->SetPosition(-20.0f, 0.0f, 10.0f);


		//Create model object
	m_Bullet = new InstanceModelClass;
	if(!m_Bullet)
	{
		return false;
	}
	m_Bullet->SetInstanceCount(2);
	//Initialize model object
	result = m_Bullet->Initialize(m_D3D->GetDevice(),"../Engine/Bullet.obj", L"../Engine/BulletDiffuseMap.png", L"../Engine/blankNormal.png");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_Bullet->SetPosition(0.0f, -27.0f, -30.0f);

	// END MODELS
	///////////////////////////////////////////////
	// BEGIN MODEL LIST

	// Create the model list object.
	m_ModelList = new ModelListClass();
	if(!m_ModelList)
	{
		return false;
	}

	// Initilize the model list object.
	result = m_ModelList->Initialize(25);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// END MOEL LIST
	/////////////////////////////////////////////
	// BEGIN FRUSTRUM

	// Create the frustrum object.
	m_Frustrum = new FrustumClass;
	if(!m_Frustrum)
	{
		return false;
	}

	// END FRUSTRUM
	///////////////////////////////////////////////

	//create the light object
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}
	//Initialize the light object
	m_Light->SetDiffuseColor(1.3f, 1.0f, 1.2f, 1.0f);
	m_Light->SetAmbientColor(0.5f, 0.5f, 0.5f, 1.0f);
	m_Light->SetDirection(0.0f, 1.0f, 0.0f);
	m_Light->SetPosition(-50.0f, 100.0f, 0.0f);

	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);


	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	
	// Create the depth shader object.
	m_DepthShader = new DepthShaderClass;
	if(!m_DepthShader)
	{
		return false;
	}

	// Initialize the depth shader object.
	result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the shadow shader object.
	m_ShadowShader = new ShadowShaderClass;
	if(!m_ShadowShader)
	{
		return false;
	}

	// Initialize the shadow shader object.
	result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	//create the terrain shader object
	m_TerrainShader = new TerrainShaderClass;
	if(!m_TerrainShader)
	{
		return false;
	}
	//initialize the terrain shader object
	result = m_TerrainShader ->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if(m_Importer)
	{
		delete m_Importer;
		m_Importer = 0;
	}

	// Release the shadow shader object.
	if(m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	// Release the depth shader object.
	if(m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// Release the render to texture object.
	if(m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if(m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	if(m_Input)
	{
		m_Input->ShutDown();
		delete m_Input;
		m_Input = 0;
	}
		

	if(m_ParticleSystem)
	{
		m_ParticleSystem->Shutdown();
		delete m_ParticleSystem;
		m_ParticleSystem = 0;
	}

	if(m_ParticleSystem2)
	{
		m_ParticleSystem2->Shutdown();
		delete m_ParticleSystem2;
		m_ParticleSystem2 = 0;
	}

	if(m_ParticleSystem3)
	{
		m_ParticleSystem3->Shutdown();
		delete m_ParticleSystem3;
		m_ParticleSystem3 = 0;
	}

	if(m_ParticleShader)
	{
		m_ParticleShader->Shutdown();
		delete m_ParticleShader;
		m_ParticleShader = 0;
	}


	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	//release the terrain shader object
	if(m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	
	if(m_Bullet)
	{
		m_Bullet->Shutdown();
		delete m_Bullet;
		m_Bullet = 0;
	}

	if(m_Frustrum)
	{
		delete m_Frustrum;
		m_Frustrum = 0;
	}

	if(m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

bool GraphicsClass::Frame(unsigned long updateCount)
{
	bool result;

	
	static float lightPositionX = 0.0f;

	// Update the position of the light each frame.

	D3DXVECTOR3 lightPos = D3DXVECTOR3(50.0f, 100.0f, 50.0f);
	
	float yaw, pitch, roll;
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;

	lightPositionX += 0.0000001f;

	D3DXMATRIX rotationMatrix;

	//pitch = 0.0f * 0.0174532925f;
	yaw   = lightPositionX * 0.0174532925f * 0.1f;
	//roll  = 0.0f * 0.0174532925f;

	

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	D3DXVec3TransformCoord(&lightPos, &lightPos, &rotationMatrix);

	lightPositionX += 2.0f;
	if(lightPositionX > 36000.0f)
	{
		lightPositionX = 0.0f;
	}
	
	// Update the position of the light.
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->SetPosition(lightPos.x, lightPos.y, lightPos.z);
	

	//read the user input
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}

	//check if the user pressed escape and want to exit
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	//update the system stat
	m_Timer->Frame();

	//do the frame input processing
	result = HandleInput(m_Timer->GetTime());
	if(!result)
	{
		return false;
	}

	D3DXVECTOR3 particleEmitterPosition(3.0f, 5.0f, 0.0f);
	D3DXVECTOR3 particleEmitterPosition2 = m_Camera->GetPosition();
	D3DXVECTOR3 particleEmitterPosition3(128.0f, 20.0f, 128.0f);

	//run the frame processing for the particle system
	m_ParticleSystem->Frame(m_Timer->GetTime(), m_D3D->GetDeviceContext(), D3DXVECTOR3(particleEmitterPosition2.x, particleEmitterPosition2.y, particleEmitterPosition2.z -10.0f), m_Camera->GetRotation());
	m_ParticleSystem2->Frame(m_Timer->GetTime(), m_D3D->GetDeviceContext(), D3DXVECTOR3(particleEmitterPosition2.x, particleEmitterPosition2.y + 10.0f, particleEmitterPosition2.z ), m_Camera->GetRotation());
	m_ParticleSystem3->Frame(m_Timer->GetTime(), m_D3D->GetDeviceContext(), particleEmitterPosition3, m_Camera->GetRotation());

	result = Render(updateCount);
	if(!result)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::RenderSceneToTexture()
{
	D3DXMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	float posX, posY, posZ;
	bool result;


	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world matrix from the d3d object.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Get the view and orthographic matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// Setup the translation matrix for the city model.
	m_City->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

	// Render the city model with the depth shader.
	m_City->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_City->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);
	// Setup the translation matrix for the cube model.
	m_Model->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

	// Render the cube model with the depth shader.
	m_Model->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the ground model.
	m_Bullet->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

	// Render the ground model with the depth shader.
	m_Bullet->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_Bullet->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if(!result)
	{
		return false;
	}
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::HandleInput(float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;
	int mousePosX, mousePosY;
	// Set the frame time fort calc the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = m_Input->IsWPressed();
	m_Position->MoveForward(keyDown);

	keyDown = m_Input->IsSPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = m_Input->IsDPressed();
	m_Position->MoveRight(keyDown);

	keyDown = m_Input->IsAPressed();
	m_Position->MoveLeft(keyDown);

	keyDown = m_Input->IsSpacePressed();
	m_Position->MoveUpward(keyDown);

	keyDown = m_Input->IsShiftPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = m_Input->IsUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = m_Input->IsDownPressed();
	m_Position->LookDownward(keyDown);

	if(m_Input->IsZPressed() && !m_PreviousZKeyState)
	{
		if((++m_CurrentCam) > m_Cameras.size() - 1)
		{
			m_CurrentCam = 0;
		}
		m_Position->SetPosition(m_Cameras[m_CurrentCam].x, m_Cameras[m_CurrentCam].y, m_Cameras[m_CurrentCam].z);
		m_PreviousZKeyState = true;
	}
	else if(!m_Input->IsZPressed())
		m_PreviousZKeyState = false;

	m_Input->GetMouseLocation(mousePosX, mousePosY);
	m_Position->MouseLook(mousePosX, mousePosY);
	m_Input->CenterMouse();

	// Get the view point position/rotation.
	
	m_Position->GetRotation(rotX, rotY, rotZ);
	m_Position->GetPosition(posX, posY, posZ);
	// Set position of the camera.
	
	m_Camera->SetRotation(rotX, rotY, rotZ);
	// Set height to terrain.
	float height = m_Terrain->GetHeightAtPos(-50, -50);
	if(height == -1.0f)
	{
		m_Camera->SetPosition(posX, posY, posZ);
	}
	else
	{
		m_Camera->SetPosition(posX, height + 2, posZ);
	}

	return true;
}

bool GraphicsClass::Render(unsigned long updateCount)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	D3DXMATRIX lightViewMatrix, lightProjectionMatrix;
	D3DXVECTOR4 color;
	int modelCount, renderCount, index;
	bool result, renderModel;
	float posX, posY, posZ, radius;
	
	result = RenderSceneToTexture();
	if(!result)
	{
		return false;
	}
	
	// Clear the buffers and set the color to grey.
	m_D3D->BeginScene(0.1f, 0.1f, 0.1f, 1.0f); 

	// Generate the view matrix based on Cam's pos.
	m_Camera->Render();

	m_Light->GenerateViewMatrix();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	
	result = m_Terrain->Render(
		m_D3D->GetDeviceContext(),
		m_TerrainShader,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(),
		m_Light->GetDirection(),
		m_Camera->GetPosition()
		);
		
	if(!result)
	{
		return false;
	}
	

	m_Model->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

	m_Model->Render(m_D3D->GetDeviceContext());

	result = m_ShadowShader->Render(
		m_D3D->GetDeviceContext(),
		m_Model->GetIndexCount(),
		m_Model->GetInstanceCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		lightViewMatrix,
		lightProjectionMatrix,
		m_Model->GetTextureArray(),
		m_RenderTexture->GetShaderResourceView(),
		m_Light->GetPosition(),
		m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor()
		);
	
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	m_City->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

	m_City->Render(m_D3D->GetDeviceContext());
	
	result = m_ShadowShader->Render(
		m_D3D->GetDeviceContext(),
		m_City->GetIndexCount(),
		m_City->GetInstanceCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		lightViewMatrix,
		lightProjectionMatrix,
		m_City->GetTextureArray(),
		m_RenderTexture->GetShaderResourceView(),
		m_Light->GetPosition(),
		m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor()
		);
	
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	m_Bullet->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

	m_Bullet->Render(m_D3D->GetDeviceContext());
	
	result = m_ShadowShader->Render(
		m_D3D->GetDeviceContext(),
		m_Bullet->GetIndexCount(),
		m_Bullet->GetInstanceCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		lightViewMatrix,
		lightProjectionMatrix,
		m_Bullet->GetTextureArray(),
		m_RenderTexture->GetShaderResourceView(),
		m_Light->GetPosition(),
		m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor()
		);
	
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Turn on alpha blending.
	m_D3D->EnableAlphaBlending();

	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ParticleSystem->Render(m_D3D->GetDeviceContext());
	

	// Render the model using the texture shader.
	result = m_ParticleShader->Render(m_D3D->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ParticleSystem->GetTexture());
	if(!result)
	{
		return false;
	}

	m_ParticleSystem2->Render(m_D3D->GetDeviceContext());
	result = m_ParticleShader->Render(m_D3D->GetDeviceContext(), m_ParticleSystem2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ParticleSystem2->GetTexture());
	if(!result)
	{
		return false;
	}

	m_ParticleSystem3->Render(m_D3D->GetDeviceContext());
	result = m_ParticleShader->Render(m_D3D->GetDeviceContext(), m_ParticleSystem3->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ParticleSystem3->GetTexture());
	if(!result)
	{
		return false;
	}
	
	// Turn off alpha blending.
	m_D3D->DisableAlphaBlending();

	////////////////////////////////////////////////////
	// BEGIN FRUSTRUM CULLING

	// Construct the frustrum.
	m_Frustrum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// Get the number of models to be rendered.
	modelCount = m_ModelList->GetModelCount();

	// Initialize the count of models that have been rendered.
	renderCount = 0;

	// Go through all the models and render them only if they can be seen by the camera view.
	for(index = 0; index < modelCount; index++)
	{
		// Get the position and color of the sphere model at this index.
		m_ModelList->GetData(index, posX, posY, posZ, color);

		// Set the radius of the sphere to 1.0 since this is already known.
		// NOTE: Change this to a lower value, like 0.1 to see the spheres get culled.
		radius = 1.0f;

		// Check if the sphere model is in the view frustrum.
		renderModel = m_Frustrum->CheckSphere(posX, posY, posZ, radius);

		// If it can be seen then render it, if not skip this model and check the next sphere.
		if(renderModel)
		{
			// Move the model to the location it should be rendered it.
			D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

			// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
			m_Sphere->Render(m_D3D->GetDeviceContext());

			// Render the model using the shadow shader
			result = m_ShadowShader->Render(
				m_D3D->GetDeviceContext(),
				m_Sphere->GetIndexCount(),
				m_Sphere->GetInstanceCount(),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				lightViewMatrix,
				lightProjectionMatrix,
				m_Sphere->GetTextureArray(),
				m_RenderTexture->GetShaderResourceView(),
				m_Light->GetPosition(),
				m_Light->GetAmbientColor(),
				m_Light->GetDiffuseColor()
				);
			if(!result)
			{
				return false;
			}

			//Reset to the original world matrix.
			m_D3D->GetWorldMatrix(worldMatrix);

			// Since this model was rendered then increase the count for this frame.
			renderCount++;
		}
	}

	// END FRUSTRUM CULLING
	//////////////////////////////////////////////////////

	// Show the rendered result on the sceen.
	m_D3D->EndScene();
	return true;
}