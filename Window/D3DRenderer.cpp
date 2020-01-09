#include "D3DRenderer.h"


void D3DRenderer::AddPoseToDebugRenderer(const std::vector<Joint*> & pose)
{
	for (size_t i = 0; i < pose.size(); i++)
	{
		if (pose[i]->parent_index == -1)
		{
			continue;
		}
		else
		{
			m_DebugRenderer->AddLines(pose[i]->matrix.r[3], pose[pose[i]->parent_index]->matrix.r[3]);
		}
	}
}

void D3DRenderer::DrawMesh(const std::vector<Mesh*>& mesh)
{
	for (size_t i = 0; i < mesh.size(); i++)
	{
		ID3D11ShaderResourceView* textures[] = 
		{
			m_TextureManager->GetShaderResourceView(TextureManager::TM_TYPE::DIFFUSE,	mesh[i]->m_TextureDiffuseID),
			m_TextureManager->GetShaderResourceView(TextureManager::TM_TYPE::NORMAL,	mesh[i]->m_TextureNormalID),
			m_TextureManager->GetShaderResourceView(TextureManager::TM_TYPE::SPECULAR,	mesh[i]->m_TextureSpecularID)
		};
		m_Context->PSSetShaderResources(0, 3, textures);
		m_Context->UpdateSubresource(m_ConstantBuffer[CB_TYPE::WORLD], 0, nullptr, &mesh[i]->m_WorldMatrix, 0, 0);
		m_Context->DrawIndexed(mesh[i]->m_IndexTotal, mesh[i]->m_IndexOffset, mesh[i]->m_VertOffset);
	}
}

void D3DRenderer::DrawDynamicModel(const DynamicModel * model)
{
	if (!model) return;
	unsigned int size = (unsigned int)model->GetBindPose().size();
	size = (size > NUM_JOINTS) ? NUM_JOINTS : size;
	DirectX::XMMATRIX* curr_pose = new DirectX::XMMATRIX[size];
	for (size_t i = 0; i < size; i++)
	{
		curr_pose[i] = model->GetCurrentPose()[i]->matrix;
	}
	m_Context->UpdateSubresource(m_ConstantBuffer[CB_TYPE::CURR_POSE], 0, nullptr, curr_pose, 0, 0);
	delete[] curr_pose;

	DrawMesh(model->GetMeshes());
}

void D3DRenderer::DrawStaticModel(const StaticModel * model)
{
	if (!model) return;
	DrawMesh(model->GetMeshes());
}

void D3DRenderer::CreateFloor(void)
{
	float uv_scale = 3.0f;
	Mesh* floor = new Mesh();
	Vertex v;
	v.position = DirectX::XMFLOAT4(-300.0f, 0.0f, 300.0f, 1.0f);
	v.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	v.texcoord = DirectX::XMFLOAT2(0.0f, 0.0f);
	v.normal = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	v.tangent = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	v.binormal = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
	floor->m_Vertices.push_back(v);
	
	v.position = DirectX::XMFLOAT4(300.0f, 0.0f, 300.0f, 1.0f);
	v.texcoord = DirectX::XMFLOAT2(uv_scale, 0.0f);
	floor->m_Vertices.push_back(v);

	v.position = DirectX::XMFLOAT4(-300.0f, 0.0f, -300.0f, 1.0f);
	v.texcoord = DirectX::XMFLOAT2(0.0f, uv_scale);
	floor->m_Vertices.push_back(v);

	v.position = DirectX::XMFLOAT4(300.0f, 0.0f, 300.0f, 1.0f);
	v.texcoord = DirectX::XMFLOAT2(uv_scale, 0.0f);
	floor->m_Vertices.push_back(v);

	v.position = DirectX::XMFLOAT4(300.0f, 0.0f, -300.0f, 1.0f);
	v.texcoord = DirectX::XMFLOAT2(uv_scale, uv_scale);
	floor->m_Vertices.push_back(v);

	v.position = DirectX::XMFLOAT4(-300.0f, 0.0f, -300.0f, 1.0f);
	v.texcoord = DirectX::XMFLOAT2(0.0f, uv_scale);
	floor->m_Vertices.push_back(v);

	floor->m_VertOffset = m_VertBufferManager->GetOffsets()[0];
	m_VertBufferManager->AddDataToBuffer(floor->m_Vertices, m_VertBufferManager->GetOffsets());

	INT indices[] = { 0, 1, 2, 3, 4, 5 };

	for (size_t i = 0; i < ARRAYSIZE(indices); i++)
	{
		floor->m_Indicies.push_back(indices[i]);
	}

	floor->m_IndexOffset = m_IndexBufferManager->GetOffsets()[0];
	m_IndexBufferManager->AddDataToBuffer(floor->m_Indicies, m_IndexBufferManager->GetOffsets());

	floor->m_VertTotal = (INT)floor->m_Vertices.size();
	floor->m_IndexTotal = (INT)floor->m_Indicies.size();

	floor->m_TextureDiffuseName = "../Assets/floor.fbm/165-diffuse.jpg";
	floor->m_TextureNormalName = "../Assets/floor.fbm/165-normal.jpg";
	floor->m_TextureSpecularName = "../Assets/floor.fbm/165-spec.jpg";
	floor->m_TextureDiffuseID = m_TextureManager->AddTexture(floor->m_TextureDiffuseName.c_str(), TextureManager::TM_TYPE::DIFFUSE);
	floor->m_TextureNormalID = m_TextureManager->AddTexture(floor->m_TextureNormalName.c_str(), TextureManager::TM_TYPE::NORMAL);
	floor->m_TextureSpecularID = m_TextureManager->AddTexture(floor->m_TextureSpecularName.c_str(), TextureManager::TM_TYPE::SPECULAR);

	floor->m_WorldMatrix = DirectX::XMMatrixIdentity();

	m_Floor.push_back(floor);
}

void D3DRenderer::SetupLights(void)
{
	UINT light_index = 0;

	LIGHT ambient;
	LIGHT directional;
	LIGHT point;
	//LIGHT spot;

	ZeroMemory(&ambient, sizeof(LIGHT));
	ambient.position		= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	ambient.color			= DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	ambient.direction		= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_LightList.array[light_index++] = ambient;

	ZeroMemory(&directional, sizeof(LIGHT));
	directional.position	= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	directional.color		= DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	directional.direction	= DirectX::XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);
	m_LightList.array[light_index++] = directional;

	ZeroMemory(&point, sizeof(LIGHT));
	point.position			= DirectX::XMFLOAT4(100.0f, 150.0f, -100.0f, 2.0f);
	point.color				= DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	point.direction			= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	point.cone_ratio		= DirectX::XMFLOAT4(0.0f, 0.0f, 200.0f, 1.0f);
	m_LightList.array[light_index++] = point;

	ZeroMemory(&point, sizeof(LIGHT));
	point.position			= DirectX::XMFLOAT4(-100.0f, 150.0f, -100.0f, 2.0f);
	point.color				= DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	point.direction			= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	point.cone_ratio		= DirectX::XMFLOAT4(0.0f, 0.0f, 200.0f, 1.0f);
	m_LightList.array[light_index++] = point;

	ZeroMemory(&point, sizeof(LIGHT));
	point.position			= DirectX::XMFLOAT4(100.0f, 150.0f, 100.0f, 2.0f);
	point.color				= DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	point.direction			= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	point.cone_ratio		= DirectX::XMFLOAT4(0.0f, 0.0f, 200.0f, 1.0f);
	m_LightList.array[light_index++] = point;

	ZeroMemory(&point, sizeof(LIGHT));
	point.position			= DirectX::XMFLOAT4(-100.0f, 150.0f, 100.0f, 2.0f);
	point.color				= DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	point.direction			= DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	point.cone_ratio		= DirectX::XMFLOAT4(0.0f, 0.0f, 200.0f, 1.0f);
	m_LightList.array[light_index++] = point;

	//ZeroMemory(&spot, sizeof(LIGHT));
	//spot.position			= DirectX::XMFLOAT4(0.0f, 250.0f, 0.0f, 2.0f);
	//spot.color				= DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	//spot.direction			= DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);
	//spot.cone_ratio			= DirectX::XMFLOAT4(0.98f, 0.97f, 300.0f, 1.0f);
	//m_LightList.array[light_index++] = spot;

	m_LightList.data.x		= ((FLOAT)light_index < 100) ? (FLOAT)light_index : 100;
}

void D3DRenderer::MoveCamera(const float & delta, const float & move_speed, const float & rot_speed_x, const float& rot_speed_y)
{
	if (m_Camera.r[3].m128_f32[1] < 15.0f)
	{
		m_Camera.r[3].m128_f32[1] = 15.0f;
	}
	if (m_InputManager->GetKeyboardButton((int)'W'))
	{
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(0.0f, 0.0f, move_speed * delta);
		m_Camera = DirectX::XMMatrixMultiply(t, m_Camera);
	}
	if (m_InputManager->GetKeyboardButton((int)'S'))
	{
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(0.0f, 0.0f, -move_speed * delta);
		m_Camera = DirectX::XMMatrixMultiply(t, m_Camera);
	}
	if (m_InputManager->GetKeyboardButton((int)'D'))
	{
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(move_speed * delta, 0.0f, 0.0f);
		m_Camera = DirectX::XMMatrixMultiply(t, m_Camera);
	}
	if (m_InputManager->GetKeyboardButton((int)'A'))
	{
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(-move_speed * delta, 0.0f, 0.0f);
		m_Camera = DirectX::XMMatrixMultiply(t, m_Camera);
	}
	if (m_InputManager->GetKeyboardButton(VK_SPACE))
	{
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(0.0f, move_speed * delta, 0.0f);
		m_Camera = DirectX::XMMatrixMultiply(t, m_Camera);
	}
	if (m_InputManager->GetKeyboardButton((int)'X'))
	{
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(0.0f, -move_speed * delta, 0.0f);
		m_Camera = DirectX::XMMatrixMultiply(t, m_Camera);
	}
	if (m_InputManager->GetMouseButton(VK_RBUTTON))
	{
		POINT curr = m_InputManager->GetMousePosition();
		POINT prev = m_InputManager->GetMousePositionPrevious();
		float dx = (float)curr.x - (float)prev.x;
		float dy = (float)curr.y - (float)prev.y;

		DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(dx * delta * rot_speed_x);
		DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(dy * delta * rot_speed_y);

		DirectX::XMVECTOR position = m_Camera.r[3];

		DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0, 1, 0, 0);
		DirectX::XMMATRIX clampMatrix;

		m_Camera.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		clampMatrix = DirectX::XMMatrixMultiply(rotationX, m_Camera);
		clampMatrix = DirectX::XMMatrixMultiply(m_Camera, rotationY);
		clampMatrix.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR dotResult = DirectX::XMVector3Dot(clampMatrix.r[2], worldUp);
		if (dotResult.m128_f32[0] < 0.99f)
		{
			if (dy < 0)
			{
				m_Camera = DirectX::XMMatrixMultiply(rotationX, m_Camera);
			}
			m_Camera = DirectX::XMMatrixMultiply(m_Camera, rotationY);
		}
		if (dotResult.m128_f32[0] > -0.99f)
		{
			if (dy > 0)
			{
				m_Camera = DirectX::XMMatrixMultiply(rotationX, m_Camera);
			}
			m_Camera = DirectX::XMMatrixMultiply(m_Camera, rotationY);
		}
		m_Camera.r[3] = position;
	}

	m_InputManager->SetMousePositionPrevious(m_InputManager->GetMousePosition().x, m_InputManager->GetMousePosition().y);
}

D3DRenderer * D3DRenderer::GetInstance(void)
{
	static D3DRenderer instance;
	return &instance;
}

bool D3DRenderer::Initialize(HWND hWnd, unsigned int nWidth, unsigned int nHeight)
{
	////////////////////////////////////////////////////////////////////////////////
	// Device, Context, SwapChain creation
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.Height = nHeight;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 0;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Width = nWidth;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.Flags = 0;
	swap_chain_desc.OutputWindow = hWnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.Windowed = true;

	HRESULT hr = 0;
	
	UINT flags = D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_SINGLETHREADED;
#if _DEBUG
	flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, NULL, flags,
		nullptr, 0, D3D11_SDK_VERSION, &swap_chain_desc,
		&m_SwapChain, &m_Device, nullptr, &m_Context);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// RenderTargetView creation
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_BackBufferTexture);
	hr = m_Device->CreateRenderTargetView(m_BackBufferTexture, nullptr, &m_RenderTargetView);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// Viewport creation
	m_ViewPort.Height = (float)nHeight;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = (float)nWidth;
	////////////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////////////
	// Depth Texture/StencilView creation
	CD3D11_TEXTURE2D_DESC depth_texture_desc = CD3D11_TEXTURE2D_DESC(
		DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT, 
		nWidth, nHeight, 1, 0, D3D11_BIND_DEPTH_STENCIL);
	hr = m_Device->CreateTexture2D(&depth_texture_desc, nullptr, &m_DepthBufferTexture);
	hr = m_Device->CreateDepthStencilView(m_DepthBufferTexture, nullptr, &m_DepthStencilView);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// Initialize subsystems
	m_BinaryReader = BinaryReader::GetInstance();
	m_BinaryReader->Initialize();

	m_InputManager = InputManager::GetInstance();
	m_InputManager->Initialize();

	m_InputLayoutManager = InputLayoutManager::GetInstance();
	m_InputLayoutManager->Initialize(m_Device);

	m_VertBufferManager = VertexBufferManager::GetInstance();
	m_VertBufferManager->Initialize(m_Device, m_Context);

	m_IndexBufferManager = IndexBufferManager::GetInstance();
	m_IndexBufferManager->Initialize(m_Device, m_Context);

	m_VertShaderManager = VertexShaderManager::GetInstance();
	m_VertShaderManager->Initialize(m_Device);

	m_PixelShaderManager = PixelShaderManager::GetInstance();
	m_PixelShaderManager->Initialize(m_Device);

	m_TextureManager = TextureManager::GetInstance();
	m_TextureManager->Initialize(m_Device, m_Context);

	m_DebugRenderer = DebugRenderer::GetInstance();
	m_DebugRenderer->Initialize();

	m_ModelManager = ModelManager::GetInstance();
	m_ModelManager->Initialize(m_BinaryReader, m_VertBufferManager, m_IndexBufferManager, m_TextureManager);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// Mesh loading
	{
		std::string meshname = "../Assets/sphere.msh";
		m_ModelManager->AddStaticModel("Skybox", meshname);
		StaticModel* model = m_ModelManager->GetStaticModel("Skybox");
		if (model)
		{
			for (size_t i = 0; i < model->GetMeshes().size(); i++)
			{
				model->GetMeshes()[i]->m_TextureDiffuseID = m_TextureManager->AddTexture("../Assets/skybox.fbm/nightsky.dds", TextureManager::TM_TYPE::DIFFUSE);
			}
			model->SetWorldMatrix(DirectX::XMMatrixTranslation(m_Camera.r[3].m128_f32[0], m_Camera.r[3].m128_f32[1], m_Camera.r[3].m128_f32[2]));
		}
	}

	//{
	//	std::string meshname = "../Assets/BattleMage_Run.msh";
	//	std::string posename = "../Assets/BattleMage_Run.pose";
	//	std::string animationnames[2] =
	//	{
	//		"../Assets/BattleMage_Run.anim",
	//	};
	//	std::vector<std::string> animnamevector;
	//	for (size_t i = 0; i < ARRAYSIZE(animationnames); i++)
	//	{
	//		animnamevector.push_back(animationnames[i].c_str());
	//	}

	//	m_ModelManager->AddDynamicModel("BattleMage", meshname, posename, animnamevector);
	//	DynamicModel* model = m_ModelManager->GetDynamicModel("BattleMage");
	//	if (model)
	//	{
	//		//model->SetWorldMatrix(DirectX::XMMatrixTranslation(100.0f, 0.0f, 0.0f));
	//		model->SetWorldMatrix(DirectX::XMMatrixScaling(30.0f, 30.0f, 30.0f));
	//	}
	//}

	{
		std::string meshname = "../Assets/Box_Idle.msh";
		std::string posename = "../Assets/Box_Idle.pose";
		std::string animationnames[2] =
		{
			"../Assets/Box_Idle.anim",
		};
		std::vector<std::string> animnamevector;
		for (size_t i = 0; i < ARRAYSIZE(animationnames); i++)
		{
			animnamevector.push_back(animationnames[i].c_str());
		}

		m_ModelManager->AddDynamicModel("Box_Idle", meshname, posename, animnamevector);
		DynamicModel* model = m_ModelManager->GetDynamicModel("Box_Idle");
		if (model)
		{
			DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f), DirectX::XMMatrixScaling(30.0f, 30.0f, 30.0f));
			model->SetWorldMatrix(result);
		}
	}

	//{
	//	std::string meshname = "../Assets/zombiecop.msh";
	//	std::string posename = "../Assets/zombiecop.pose";
	//	std::string animationnames[2] = 
	//	{
	//		"../Assets/zombiecopidle0.anim",
	//		"../Assets/zombiecopscratch.anim"
	//	};
	//	std::vector<std::string> animnamevector;
	//	for (size_t i = 0; i < ARRAYSIZE(animationnames); i++)
	//	{
	//		animnamevector.push_back(animationnames[i].c_str());
	//	}

	//	m_ModelManager->AddDynamicModel("ZombieCop", meshname, posename, animnamevector);
	//	DynamicModel* model = m_ModelManager->GetDynamicModel("ZombieCop");
	//	if (model)
	//	{ 
	//		model->SetWorldMatrix(DirectX::XMMatrixTranslation(100.0f, 0.0f, 0.0f));
	//	}
	//}

	//{
	//	std::string meshname = "../Assets/kachujin.msh";
	//	std::string posename = "../Assets/kachujin.pose";
	//	std::string animationnames[1] = 
	//	{
	//		"../Assets/kachujinidle.anim",
	//	};
	//	std::vector<std::string> animnamevector;
	//	for (size_t i = 0; i < ARRAYSIZE(animationnames); i++)
	//	{
	//		animnamevector.push_back(animationnames[i].c_str());
	//	}

	//	m_ModelManager->AddDynamicModel("Kachujin", meshname, posename, animnamevector);
	//	DynamicModel* model = m_ModelManager->GetDynamicModel("Kachujin");
	//	if (model)
	//	{ 
	//		model->SetWorldMatrix(DirectX::XMMatrixTranslation(-100.0f, 0.0f, 0.0f));
	//	}
	//}


	CreateFloor();
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// Lights
	SetupLights();
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// debug renderer
	CD3D11_BUFFER_DESC buffer_desc = CD3D11_BUFFER_DESC(
		sizeof(DirectX::XMFLOAT4) * (UINT)m_DebugRenderer->GetCapacity(),
		D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DEFAULT,
		0,
		0,
		0
	);

	D3D11_SUBRESOURCE_DATA buffer_data;
	ZeroMemory(&buffer_data, sizeof(buffer_data));
	buffer_data.pSysMem = m_DebugRenderer->GetData();

	hr = m_Device->CreateBuffer(&buffer_desc, &buffer_data, &m_DebugRendererBuffer);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// view projection setup
	m_Camera = DirectX::XMMatrixInverse(
		nullptr,
		DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(0.0f, 200.0f, 200.0f, 1.0f),	// camera pos
			DirectX::XMVectorSet(0.0f, 125.0f, 0.0f, 0.0f),		// looking at pos
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)		// world up
		)
	);

	m_Projection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(65.0f),					// field of view
		(float)nWidth / (float)nHeight,						// aspect ratio
		0.1f,												// near plane
		100000.0f											// far plane
	);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// constant buffer setup for wvp
	CD3D11_BUFFER_DESC const_buffer_desc = CD3D11_BUFFER_DESC(
		sizeof(DirectX::XMMATRIX),
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DEFAULT,
		0,
		0,
		0
	);

	hr = m_Device->CreateBuffer(&const_buffer_desc, nullptr, &m_ConstantBuffer[CB_TYPE::WORLD]);

	const_buffer_desc.ByteWidth = sizeof(VIEW_PROJ);
	hr = m_Device->CreateBuffer(&const_buffer_desc, nullptr, &m_ConstantBuffer[CB_TYPE::VIEWPROJ]);

	const_buffer_desc.ByteWidth = sizeof(DirectX::XMMATRIX) * NUM_JOINTS;
	hr = m_Device->CreateBuffer(&const_buffer_desc, nullptr, &m_ConstantBuffer[CB_TYPE::CURR_POSE]);

	const_buffer_desc.ByteWidth = sizeof(DirectX::XMFLOAT4);
	hr = m_Device->CreateBuffer(&const_buffer_desc, nullptr, &m_ConstantBuffer[CB_TYPE::CAMERA]);

	const_buffer_desc.ByteWidth = sizeof(CB_LIGHT);
	hr = m_Device->CreateBuffer(&const_buffer_desc, nullptr, &m_ConstantBuffer[CB_TYPE::LIGHTS]);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// sampler state creation
	CD3D11_SAMPLER_DESC sampler_desc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	sampler_desc.Filter   = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	m_Device->CreateSamplerState(&sampler_desc, &m_SamplerState);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// raster state creation
	CD3D11_RASTERIZER_DESC raster_desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	hr = m_Device->CreateRasterizerState(&raster_desc, &m_RasterStates[RS_TYPE::SOLID]);

	raster_desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	hr = m_Device->CreateRasterizerState(&raster_desc, &m_RasterStates[RS_TYPE::WIREFRAME]);
	raster_desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

	raster_desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	hr = m_Device->CreateRasterizerState(&raster_desc, &m_RasterStates[RS_TYPE::CULL_NONE]);

	raster_desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	hr = m_Device->CreateRasterizerState(&raster_desc, &m_RasterStates[RS_TYPE::CULL_NONE_WIREFRAME]);

	////////////////////////////////////////////////////////////////////////////////



	return true;
}

bool D3DRenderer::Update(const float & delta)
{
	// TODO: add timer class to window.cpp
	// this is per frame incrementation for rotating this test mesh
	static float elapsed_time = 0.0f;
	//elapsed_time += delta;

	////////////////////////////////////////////////////////////////////////////////
	// update camera and projection matrix
	MoveCamera(delta, 80.0f, 0.10f, 0.15f);
	m_ViewProj.view = DirectX::XMMatrixInverse(nullptr, m_Camera);
	m_ViewProj.proj = m_Projection;
	m_Context->UpdateSubresource(m_ConstantBuffer[CB_TYPE::VIEWPROJ], 0, nullptr, &m_ViewProj, 0, 0);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// update world matrix for the skybox
	StaticModel* model = m_ModelManager->GetStaticModel("Skybox");
	if (model)
	{
		float x = m_Camera.r[3].m128_f32[0];
		float y = m_Camera.r[3].m128_f32[1];
		float z = m_Camera.r[3].m128_f32[2];
		model->SetWorldMatrix(DirectX::XMMatrixTranslation(x, y, z));
	}
	////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	float delta_inc = delta * 0.5f;
	if (m_bPauseAnimation)
	{
		delta_inc = 0.0f;
	}

	for (size_t j = 0; j < m_ModelManager->GetDynamicModels().size(); j++)
	{
		DynamicModel* model = m_ModelManager->GetDynamicModel((unsigned int)j);
		unsigned int size = (unsigned int)model->GetBindPose().size();
		size = (size > NUM_JOINTS) ? NUM_JOINTS : size;

		std::vector<Joint*> result = model->GetAnimationInterpolator().Interpolate(delta_inc);
		for (size_t i = 0; i < size; i++)
		{
			DirectX::XMMATRIX inv_bindpose = DirectX::XMMatrixInverse(nullptr, model->GetBindPose()[i]->matrix);
			DirectX::XMMATRIX curr_joint = result[i]->matrix;
			DirectX::XMMATRIX curr_pose_matrix = DirectX::XMMatrixMultiply(inv_bindpose, curr_joint);

			model->GetCurrentPose()[i]->name = result[i]->name;
			model->GetCurrentPose()[i]->parent_index = result[i]->parent_index;
			model->GetCurrentPose()[i]->matrix = curr_pose_matrix;

			result[i]->matrix = DirectX::XMMatrixMultiply(result[i]->matrix, model->GetWorldMatrix());
		}
		AddPoseToDebugRenderer(result);

		for (size_t i = 0; i < result.size(); i++)
		{
			SAFE_DELETE(result[i]);
		}
	}


	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// update camera
	DirectX::XMMATRIX camera = DirectX::XMMatrixInverse(nullptr, m_ViewProj.view);
	DirectX::XMVECTOR camera_pos = camera.r[3];
	m_Context->UpdateSubresource(m_ConstantBuffer[CB_TYPE::CAMERA], 0, nullptr, &camera_pos, 0, 0);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// update the light list
	for (size_t i = 0; i < ARRAYSIZE(m_LightList.array); i++)
	{
		if (m_LightList.array[i].position.w == 1)
		{
			DirectX::XMVECTOR direction = DirectX::XMVectorSet(
				m_LightList.array[i].direction.x,
				m_LightList.array[i].direction.y,
				m_LightList.array[i].direction.z,
				0.0f
			);

			direction = DirectX::XMVector4Transform(direction, DirectX::XMMatrixRotationY(delta*0));
			direction.m128_f32[3] = m_LightList.array[i].direction.w;

			DirectX::XMStoreFloat4(&m_LightList.array[i].direction, direction);
		}
		if (m_LightList.array[i].position.w == 2)
		{
			DirectX::XMVECTOR position = DirectX::XMVectorSet(
				m_LightList.array[i].position.x,
				m_LightList.array[i].position.y,
				m_LightList.array[i].position.z,
				1.0f);

			position = DirectX::XMVector4Transform(position, DirectX::XMMatrixRotationY(delta));
			position.m128_f32[3] = m_LightList.array[i].position.w;

			DirectX::XMStoreFloat4(&m_LightList.array[i].position, position);
		}
	}

	m_Context->UpdateSubresource(m_ConstantBuffer[CB_TYPE::LIGHTS], 0, nullptr, &m_LightList, 0, 0);
	////////////////////////////////////////////////////////////////////////////////


	if (m_InputManager->GetKeyboardButton((int)'P'))
	{
		m_bWireFrame = false;
	}
	if (m_InputManager->GetKeyboardButton((int)'L'))
	{
		m_bWireFrame = true;
	}


	if (m_InputManager->GetKeyboardButton((int)'O'))
	{
		m_bPauseAnimation = false;
	}
	if (m_InputManager->GetKeyboardButton((int)'K'))
	{
		m_bPauseAnimation = true;
	}

	return true;
}

bool D3DRenderer::Draw(UINT vSync)
{
	HRESULT hr = 0;

	////////////////////////////////////////////////////////////////////////////////
	// set the rendertarget and depthstencilview
	// clear both
	m_Context->RSSetViewports(1, &m_ViewPort);
	const float color[] = { 0.078f, 0.078f, 1.0f, 1.0f };
	m_Context->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	m_Context->ClearRenderTargetView(m_RenderTargetView, color);
	m_Context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0x0);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// set the strides and offet for the vertex buffer
	// set the index buffer
	// set the constant buffer
	UINT stride[] = 
	{ 
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT2),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4)
	};
	UINT offset[] = 
	{ 
		0, 
		0, 
		0, 
		0,
		0, 
		0,
		0,
		0
	};
	ID3D11Buffer* vertex_buffer[] =
	{
		m_VertBufferManager->GetVertexBuffer(VertexBufferManager::VB_TYPE::POSITION),
		m_VertBufferManager->GetVertexBuffer(VertexBufferManager::VB_TYPE::COLOR),
		m_VertBufferManager->GetVertexBuffer(VertexBufferManager::VB_TYPE::TEXCOORD),
		m_VertBufferManager->GetVertexBuffer(VertexBufferManager::VB_TYPE::NORMAL),
		m_VertBufferManager->GetVertexBuffer(VertexBufferManager::VB_TYPE::TANGENT),
		m_VertBufferManager->GetVertexBuffer(VertexBufferManager::VB_TYPE::BINORMAL),
		m_VertBufferManager->GetVertexBuffer(VertexBufferManager::VB_TYPE::JOINTS),
		m_VertBufferManager->GetVertexBuffer(VertexBufferManager::VB_TYPE::WEIGHTS)
	};
	ID3D11Buffer* index_buffer = 
		m_IndexBufferManager->GetIndexBuffer(IndexBufferManager::IB_TYPE::UINT32);
	ID3D11Buffer* constant_buffer[] =
	{
		m_ConstantBuffer[CB_TYPE::WORLD],
		m_ConstantBuffer[CB_TYPE::VIEWPROJ],
		m_ConstantBuffer[CB_TYPE::CURR_POSE]
	};
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// setup the pipeline
	m_Context->IASetVertexBuffers(0, ARRAYSIZE(vertex_buffer), vertex_buffer, stride, offset);
	m_Context->IASetIndexBuffer(index_buffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_Context->IASetInputLayout(m_InputLayoutManager->GetInputLayout(InputLayoutManager::IL_TYPE::PCTNTBJW));
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffer), constant_buffer);
	m_Context->PSSetConstantBuffers(0, 1, &m_ConstantBuffer[CB_TYPE::CAMERA]);
	m_Context->PSSetConstantBuffers(1, 1, &m_ConstantBuffer[CB_TYPE::LIGHTS]);
	m_Context->PSSetSamplers(0, 1, &m_SamplerState);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// draw meshes here
	if (m_bWireFrame) {	m_Context->RSSetState(m_RasterStates[RS_TYPE::CULL_NONE_WIREFRAME]); }
	else { m_Context->RSSetState(m_RasterStates[RS_TYPE::CULL_NONE]); }

	m_Context->VSSetShader(m_VertShaderManager->GetVertexShader(VertexShaderManager::VS_TYPE::SKYBOX), nullptr, 0);
	m_Context->PSSetShader(m_PixelShaderManager->GetPixelShader(PixelShaderManager::PS_TYPE::SKYBOX), nullptr, 0);
	StaticModel* model = m_ModelManager->GetStaticModel("Skybox");
	if (model)
	{
		DrawStaticModel(model);
	}
	m_Context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0x0);


	if (m_bWireFrame) {	m_Context->RSSetState(m_RasterStates[RS_TYPE::WIREFRAME]); } 
	else { m_Context->RSSetState(m_RasterStates[RS_TYPE::SOLID]); }

	m_Context->VSSetShader(m_VertShaderManager->GetVertexShader(VertexShaderManager::VS_TYPE::PCTNTBJW_ANIM), nullptr, 0);
	m_Context->PSSetShader(m_PixelShaderManager->GetPixelShader(PixelShaderManager::PS_TYPE::PCTNTBJW), nullptr, 0);
	for (size_t i = 0; i < m_ModelManager->GetDynamicModels().size(); i++)
	{
		const DynamicModel* model = m_ModelManager->GetDynamicModel((unsigned int)i);
		DrawDynamicModel(model);
	}

	m_Context->VSSetShader(m_VertShaderManager->GetVertexShader(VertexShaderManager::VS_TYPE::PCTNTBJW), nullptr, 0);
	for (size_t i = 0; i < m_ModelManager->GetStaticModels().size(); i++)
	{
		const StaticModel* model = m_ModelManager->GetStaticModel((unsigned int)i);
		if (model->GetName() == "Skybox") continue;
		DrawStaticModel(model);
	}

	DrawMesh(m_Floor);
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// debug renderer
	if (m_bWireFrame)
	{
		m_Context->UpdateSubresource(m_DebugRendererBuffer, 0, nullptr, m_DebugRenderer->GetData(), 0, 0);
		m_Context->IASetVertexBuffers(0, 1, &m_DebugRendererBuffer, stride, offset);
		m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		m_Context->VSSetShader(m_VertShaderManager->GetVertexShader(VertexShaderManager::VS_TYPE::DEBUGLINES), nullptr, 0);
		m_Context->PSSetShader(m_PixelShaderManager->GetPixelShader(PixelShaderManager::PS_TYPE::DEBUGLINES), nullptr, 0);
		m_Context->Draw((UINT)m_DebugRenderer->GetVertexCount(), 0);
	}
	////////////////////////////////////////////////////////////////////////////////

	m_DebugRenderer->ClearLines();

	hr = m_SwapChain->Present(vSync, 0);
	return true;
}

void D3DRenderer::Shutdown(void)
{
	////////////////////////////////////////////////////////////////////////////////
	// cleanup of dynamic memory
	for (int i = 0; i < m_Floor.size(); i++)
	{
		SAFE_DELETE(m_Floor[i]);
	}
	m_Floor.clear();
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// cleanup of managers
	m_ModelManager->Shutdown();
	m_DebugRenderer->Shutdown();
	m_TextureManager->Shutdown();
	m_PixelShaderManager->Shutdown();
	m_VertShaderManager->Shutdown();
	m_IndexBufferManager->Shutdown();
	m_VertBufferManager->Shutdown();
	m_InputLayoutManager->Shutdown();
	m_InputManager->Shutdown();
	m_BinaryReader->Shutdown();
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// cleanup of any directx 11 interface objects
	for (size_t i = 0; i < RS_TYPE::RS_COUNT; i++)
	{
		SAFE_RELEASE(m_RasterStates[i]);
	}
	SAFE_RELEASE(m_SamplerState);
	SAFE_RELEASE(m_DebugRendererBuffer);
	for (size_t i = 0; i < CB_TYPE::CB_COUNT; i++)
	{
		SAFE_RELEASE(m_ConstantBuffer[i]);
	}
	SAFE_RELEASE(m_DepthStencilView);
	SAFE_RELEASE(m_DepthBufferTexture);
	SAFE_RELEASE(m_RenderTargetView);
	SAFE_RELEASE(m_BackBufferTexture);
	SAFE_RELEASE(m_SwapChain);
	SAFE_RELEASE(m_Context);

#ifndef _DEBUG
	{
		ID3D11Debug* debug;
		m_Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug));
		if (debug)
		{
			debug->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
		}
		SAFE_RELEASE(debug);
	}
#endif

	SAFE_RELEASE(m_Device);
	////////////////////////////////////////////////////////////////////////////////
}