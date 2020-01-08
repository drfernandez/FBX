#pragma once

// includes
#include "header.h"

// systems includes
#include "BinaryReader.h"
#include "InputManager.h"
#include "InputLayoutManager.h"
#include "VertexBufferManager.h"
#include "IndexBufferManager.h"
#include "VertexShaderManager.h"
#include "PixelShaderManager.h"
#include "TextureManager.h"
#include "DebugRenderer.h"
#include "AnimationInterpolator.h"
#include "ModelManager.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }
#define NUM_JOINTS 66


class D3DRenderer
{
public:
	enum CB_TYPE { WORLD = 0, VIEWPROJ, CURR_POSE, CAMERA, LIGHTS, CB_COUNT };
	enum RS_TYPE { SOLID = 0, WIREFRAME, CULL_NONE, CULL_NONE_WIREFRAME, RS_COUNT };
private:
	// Singleton closure of functions
	D3DRenderer() = default;
	~D3DRenderer() = default;
	D3DRenderer(const D3DRenderer& copy) = default;
	D3DRenderer& operator=(const D3DRenderer& copy) = default;

	// Data Members
	ID3D11Device*							m_Device = nullptr;
	ID3D11DeviceContext*					m_Context = nullptr;
	IDXGISwapChain*							m_SwapChain = nullptr;

	ID3D11Texture2D*						m_BackBufferTexture = nullptr;
	ID3D11RenderTargetView*					m_RenderTargetView = nullptr;
	ID3D11Texture2D*						m_DepthBufferTexture = nullptr;
	ID3D11DepthStencilView*					m_DepthStencilView = nullptr;

	ID3D11Buffer*							m_ConstantBuffer[CB_TYPE::CB_COUNT] = { nullptr };
	ID3D11Buffer*							m_DebugRendererBuffer = nullptr;

	ID3D11SamplerState*						m_SamplerState = nullptr;

	ID3D11RasterizerState*					m_RasterStates[RS_TYPE::RS_COUNT] = { nullptr };
	BOOL									m_bWireFrame = false;

	D3D11_VIEWPORT							m_ViewPort;

	// meshes
	AnimationInterpolator					m_AnimationInterpolator;

	std::vector<Mesh*>						m_Floor;

	bool									m_bPauseAnimation = false;
	unsigned int							m_AnimationIndex = 1;

	// camera
	DirectX::XMMATRIX						m_Camera;
	DirectX::XMMATRIX						m_Projection;
	VIEW_PROJ								m_ViewProj;

	// lights for the scene
	CB_LIGHT								m_LightList;

	// Subsystems used with the renderer
	BinaryReader*							m_BinaryReader = nullptr;
	InputManager*							m_InputManager = nullptr;
	InputLayoutManager*						m_InputLayoutManager = nullptr;
	VertexBufferManager*					m_VertBufferManager = nullptr;
	IndexBufferManager*						m_IndexBufferManager = nullptr;
	VertexShaderManager*					m_VertShaderManager = nullptr; 
	PixelShaderManager*						m_PixelShaderManager = nullptr;
	TextureManager*							m_TextureManager = nullptr;
	DebugRenderer*							m_DebugRenderer = nullptr;
	ModelManager*							m_ModelManager = nullptr;


	// private functions
	void AddPoseToDebugRenderer(const std::vector<Joint*>& pose);

	void DrawMesh(const std::vector<Mesh*>& mesh);
	void DrawDynamicModel(const DynamicModel* model);
	void DrawStaticModel(const StaticModel* model);

	void CreateFloor(void);
	void SetupLights(void);

	void MoveCamera(const float& delta, const float& move_speed, const float& rot_speed_x, const float& rot_speed_y);


public:
	// Singleton access
	static D3DRenderer* GetInstance(void);

	// Public functions
	bool Initialize(HWND hWnd, unsigned int nWidth, unsigned int nHeight);
	bool Update(const float& delta);
	bool Draw(UINT vSync);
	void Shutdown(void);

	inline void SetKeyboardKeyDown(int key) { m_InputManager->SetKeyboardButtonOn(key); }
	inline void SetKeyboardKeyUp(int key) { m_InputManager->SetKeyboardButtonOff(key); }
	inline void SetMouseKeyDown(int key) { m_InputManager->SetMouseButtonOn(key); }
	inline void SetMouseKeyUp(int key) { m_InputManager->SetMouseButtonOff(key); }
	inline void SetMousePosition(int x, int y) { m_InputManager->SetMousePosition(x, y); }
};

