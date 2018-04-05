#include <graphics/Application.hpp>
#include <graphics/Scene.hpp>
#include <graphics/importer/Model.hpp>
#include <graphics/components/Render.hpp>
#include <graphics/components/Transform.hpp>
#include <utils/D3DUtility.hpp>
#include <utils/Shader.hpp>
#include <utils/Buffer.hpp>
#include <utils/Input.hpp>
#include <utils/Camera.hpp>
#include <utils/LightManager.hpp>
#include <utils/SoundManager.hpp>
#include <tchar.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <DirectXColors.h>

#define WIN32_LEAN_AND_MEAN //Exclude some windows headers
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace px
{
	struct ObjectInfo
	{
		Entity entity;
		bool picked;
	}selectedEntity;

	Application::Application()
	{
		//Create application window
		m_windowClass = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("AP"), NULL };
		RegisterClassEx(&m_windowClass);
		HWND hwnd = CreateWindow(_T("AP"), _T("Audio Project [DX11]"), WS_OVERLAPPEDWINDOW, 200, 75, WIDTH, HEIGHT, NULL, NULL, m_windowClass.hInstance, NULL);

		//Initialize Direct3D
		CreateDeviceD3D(hwnd);
		CreateViewport();

		//Show the window
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);
		Input::Initialize(hwnd);

		//Setup ImGui binding
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui_ImplDX11_Init(hwnd, m_device.Get(), m_deviceContext.Get());
		io.NavFlags |= ImGuiNavFlags_EnableKeyboard;
		ImGui::StyleColorsDark();

		//Create no cull raster state
		D3D11_RASTERIZER_DESC rDesc = {};
		rDesc.FillMode = D3D11_FILL_SOLID;
		rDesc.CullMode = D3D11_CULL_NONE;
		m_device->CreateRasterizerState(&rDesc, m_noCull.GetAddressOf());;

		//Load resources
		LoadObjects();
		LoadShaders();
		LoadModels();
		//LoadAudioFiles();
	}

	Application::~Application()
	{	
		m_scene->WriteSceneData();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();
		m_mainRenderTargetView.Reset();
		//m_depthStencilView.Reset();
		m_swapChain.Reset();
		m_deviceContext.Reset();
		m_device.Reset();
		UnregisterClass(_T("AP"), m_windowClass.hInstance);
	}

	void Application::LoadObjects()
	{
		m_shaders = std::make_unique<Shader>(m_device.Get(), m_deviceContext.Get());
		m_camera = std::make_unique<Camera>(Vector3(0.f, 0.f, -10.f));
		m_buffer = std::make_unique<Buffer>(m_device.Get(), m_deviceContext.Get());
		m_models = std::make_unique<Model>(m_buffer.get(), m_shaders.get());
		m_lightManager = std::make_unique<LightManager>();
		m_scene = std::make_unique<Scene>(m_camera.get(), m_buffer.get(), m_models.get(), m_lightManager.get());
		//m_soundManager = std::make_unique<SoundManager>();
	}

	void Application::LoadModels()
	{
		m_models->LoadModel(Models::Cube, "src/res/models/cube.obj");
	}

	void Application::LoadAudioFiles()
	{
		m_soundManager->LoadSound(Sounds::Bell, "src/res/sounds/gun.wav", true, true, true);
		m_soundManager->LoadSound(Sounds::Nature, "src/res/sounds/ambientNature.wav", false, true, true);
		m_soundManager->LoadSound(Sounds::Footsteps, "src/res/sounds/ambientFootsteps.wav", false, true, true);
		m_soundManager->Play(Sounds::Nature, Vector3(0.f), -15.f);
		m_soundManager->Play(Sounds::Footsteps, Vector3(0.f), -15.f);
		m_soundManager->Play(Sounds::Bell, Vector3(-5.0f, 1.0f, 5.0f));
	}

	void Application::LoadShaders()
	{
		m_shaders->LoadShadersFromFile(Shaders::Basic, "src/res/shaders/Primitive.hlsl", VS | PS);
		m_shaders->CreateInputLayout(Shaders::Basic);
	}

	void Application::Run()
	{
		Update();
	}

	void Application::Update()
	{
		MSG msg = { 0 };
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}

			//Update the camera attributes to unit vectors for the 3d listener
			/*Vector3 forward = m_camera->GetCamForward();
			Vector3 up = m_camera->GetCamUp();
			forward.Normalize();
			up.Normalize();

			m_soundManager->Set3dListenerAndOrientation(m_camera->GetPosition(), forward, up);
			m_soundManager->Update();*/
			Input::Update();
			PollEvents();
			m_camera->Update(0.0001f);
			ImGui_ImplDX11_NewFrame();
			UpdateGUI();
			RenderScene();
		}
	}

	void Application::UpdateGUI()
	{
		static int floatPrecision = 3;
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;

		ImGui::Begin("Scene", NULL,	ImVec2(0, 0), 1.f, flags);

		//ImGui::SetNextTreeNodeOpen(true, 2);
		if (ImGui::CollapsingHeader("Light"))
		{
			static Vector3 lightPos = m_lightManager->GetLightPosition();
			static float ambient = m_lightManager->GetAmbientStrength();
			static float specular = m_lightManager->GetSpecularStrength();

			ImGui::Spacing();
			ImGui::DragFloat3("LightPos", &lightPos.x, 0.1f, -10000.f, 10000.f);
			ImGui::DragFloat("Ambient", &ambient, 0.01f, 0.f, 1.f);
			ImGui::DragFloat("Specular", &specular, 0.01f, 0.f, 1.f);
			ImGui::Spacing();

			m_lightManager->SetLightPosition(lightPos);
			m_lightManager->SetAmbientStrength(ambient);
			m_lightManager->SetSpecularStrength(specular);
		};

		ImGui::SetNextTreeNodeOpen(true, 2);
		if (ImGui::CollapsingHeader("Inspector"))
		{
			if (selectedEntity.picked)
			{
				Vector3 position = selectedEntity.entity.component<Transform>()->transform->GetPosition();
				Vector3 scale = selectedEntity.entity.component<Transform>()->transform->GetScale();
				Vector3 rotation = selectedEntity.entity.component<Transform>()->transform->GetRotation();
				Vector3 material = selectedEntity.entity.component<Render>()->object->GetColor();

				ImGui::Spacing();
				ImGui::Text("Entity: %s", selectedEntity.entity.component<Render>()->object->GetName().c_str());
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::DragFloat3("Position", &position.x, 0.1f, -10000.f, 10000.f);
				ImGui::Spacing();
				ImGui::DragFloat3("Scale", &scale.x, 0.1f, -10000.f, 10000.f);
				ImGui::Spacing();
				ImGui::DragFloat3("Rotation", &rotation.x, 0.1f, -360.f, 360.f);
				ImGui::Spacing();
				ImGui::ColorEdit3("Material", &material.x);
				ImGui::Spacing();

				selectedEntity.entity.component<Transform>()->transform->SetPosition(position);
				selectedEntity.entity.component<Transform>()->transform->SetScale(scale);
				selectedEntity.entity.component<Transform>()->transform->SetRotation(rotation);
				selectedEntity.entity.component<Render>()->object->SetColor(material);
			}
		}

		ImGui::SetNextTreeNodeOpen(true, 2);
		if (ImGui::CollapsingHeader("Entities"))
		{
			static unsigned int selected = 0;
			unsigned int index = 0;
			ComponentHandle<Render> render;
			ComponentHandle<Transform> transform;

			for (Entity entity : m_scene->GetEntites().entities_with_components(render, transform))
			{
				char label[128];
				ImGui::Spacing();
				sprintf(label, render->object->GetName().c_str());
				if (ImGui::Selectable(label, selected == index))
				{
					selectedEntity.entity = entity;
					selectedEntity.picked = true;
					selected = index;
				}
				index++;
			}
		}
		ImGui::End();
	}

	void Application::PollEvents()
	{
		if (Input::GetKeyDown(Keyboard::Keys::Escape))
			PostQuitMessage(0);

		/*if (Input::GetMouseButtonDown(Input::MouseButton::LEFT))
			m_soundManager->Play(Sounds::Gun);*/
	}

	void Application::RenderScene()
	{
		m_deviceContext->RSSetViewports(1, &m_vp);
		m_deviceContext->OMSetRenderTargets(1, m_mainRenderTargetView.GetAddressOf(), m_depthStencilView.Get());
		m_deviceContext->ClearRenderTargetView(m_mainRenderTargetView.Get(), DirectX::Colors::SteelBlue);
		m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		m_deviceContext->RSSetState(m_noCull.Get());
		m_scene->UpdateSystems(0.0);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		assert(!m_swapChain->Present(0, 0));
	}

	void Application::CreateRenderTargetAndDepthStencil()
	{
		//Create render target
		ID3D11Texture2D* pBackBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		m_device->CreateRenderTargetView(pBackBuffer, nullptr, m_mainRenderTargetView.GetAddressOf());
		SAFE_RELEASE(&pBackBuffer);

		//Create depth stencil view
		ID3D11Texture2D* depthStencilTexture;
		D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
		depthStencilDesc.Width = WIDTH;
		depthStencilDesc.Height = HEIGHT;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 8;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//Create texture
		assert(!m_device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilTexture));

		//Create the depth stencil view from the texture and description
		assert(!m_device->CreateDepthStencilView(depthStencilTexture, NULL, m_depthStencilView.GetAddressOf()));
		SAFE_RELEASE(&depthStencilTexture);
	}

	void Application::CreateDeviceD3D(HWND hWnd)
	{
		//Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferCount = 2;
		sd.BufferDesc.Width = WIDTH;
		sd.BufferDesc.Height = HEIGHT;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 8;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		assert(!D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, ARRAYSIZE(featureLevelArray),
			D3D11_SDK_VERSION, &sd, m_swapChain.GetAddressOf(), m_device.GetAddressOf(), &featureLevel, m_deviceContext.GetAddressOf()));

		CreateRenderTargetAndDepthStencil();
	}

	void Application::CreateViewport()
	{
		m_vp.Width = static_cast<FLOAT>(WIDTH);
		m_vp.Height = static_cast<FLOAT>(HEIGHT);
		m_vp.MinDepth = 0.0f;
		m_vp.MaxDepth = 1.0f;
		m_vp.TopLeftX = 0;
		m_vp.TopLeftY = 0;
	}

	LRESULT Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_ACTIVATEAPP:
			Keyboard::ProcessMessage(msg, wParam, lParam);
			Mouse::ProcessMessage(msg, wParam, lParam);
			break;
		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			Mouse::ProcessMessage(msg, wParam, lParam);
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Keyboard::ProcessMessage(msg, wParam, lParam);
			break;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) //Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}