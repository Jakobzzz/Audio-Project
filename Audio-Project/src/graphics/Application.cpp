#include <graphics/Application.hpp>
#include <graphics/Primitive.hpp>
#include <graphics/importer/Model.hpp>
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

		//Load resources
		LoadObjects();
		LoadShaders();
		LoadModels();
		//LoadAudioFiles();
	}

	Application::~Application()
	{		
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
		m_primitive = std::make_unique<Primitive>(m_camera.get(), m_buffer.get(), m_lightManager.get(), m_models.get());
		//m_soundManager = std::make_unique<SoundManager>();
	}

	void Application::LoadModels()
	{
		m_models->LoadModel(Models::Cube, "src/res/models/cube.obj");
	}

	void Application::LoadAudioFiles()
	{
		m_soundManager->LoadSound(Sounds::Birds, "src/res/sounds/birds.wav", false, false, true);
		m_soundManager->LoadSound(Sounds::Gun, "src/res/sounds/gun.wav", false);
		m_soundManager->Play(Sounds::Birds);
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

			//m_soundManager->Update();
			Input::Update();
			PollEvents();
			m_camera->Update(0.0001f);
			ImGui_ImplDX11_NewFrame();
			UpdateGUI();
			Render();
		}
	}

	void Application::UpdateGUI()
	{
		static int floatPrecision = 3;
		static Vector3 lightDir = m_lightManager->GetLightDirection();
		static float ambient = m_lightManager->GetAmbientStrength();
		static float specular = m_lightManager->GetSpecularStrength();

		//const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;
		ImGui::Begin("Scene");
		ImGui::Text("Light");
		ImGui::Spacing();
		ImGui::InputFloat3("Direction", &lightDir.x, floatPrecision);
		ImGui::DragFloat("Ambient", &ambient, 0.01f, 0.f, 1.f);
		ImGui::DragFloat("Specular", &specular, 0.01f, 0.f, 1.f);
		ImGui::End();

		m_lightManager->SetLightDirection(lightDir);
		m_lightManager->SetAmbientStrength(ambient);
		m_lightManager->SetSpecularStrength(specular);
	}

	void Application::PollEvents()
	{
		if (Input::GetKeyDown(Keyboard::Keys::Escape))
			PostQuitMessage(0);

		/*if (Input::GetMouseButtonDown(Input::MouseButton::LEFT))
			m_soundManager->Play(Sounds::Gun);*/
	}

	void Application::Render()
	{
		m_deviceContext->RSSetViewports(1, &m_vp);
		m_deviceContext->OMSetRenderTargets(1, m_mainRenderTargetView.GetAddressOf(), NULL); //Imgui DX11 sample provides a default depth stencil view*
		m_deviceContext->ClearRenderTargetView(m_mainRenderTargetView.Get(), DirectX::Colors::DarkGray);
		//m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		RenderScene();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		assert(!m_swapChain->Present(0, 0));
	}

	void Application::RenderScene()
	{
		m_primitive->Draw();
	}

	void Application::CreateRenderTargetAndDepthStencil()
	{
		//Create render target
		ID3D11Texture2D* pBackBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		m_device->CreateRenderTargetView(pBackBuffer, nullptr, m_mainRenderTargetView.GetAddressOf());
		SAFE_RELEASE(&pBackBuffer);

		//Create depth stencil view
		//ID3D11Texture2D* depthStencilTexture;
		//D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
		//depthStencilDesc.Width = WIDTH;
		//depthStencilDesc.Height = HEIGHT;
		//depthStencilDesc.MipLevels = 1;
		//depthStencilDesc.ArraySize = 1;
		//depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//depthStencilDesc.SampleDesc.Count = 4;
		//depthStencilDesc.SampleDesc.Quality = 0;
		//depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		//depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//depthStencilDesc.CPUAccessFlags = 0;
		//depthStencilDesc.MiscFlags = 0;

		////Create texture
		//assert(!m_device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilTexture));

		////Create the depth stencil view from the texture and description
		//assert(!m_device->CreateDepthStencilView(depthStencilTexture, NULL, m_depthStencilView.GetAddressOf()));
		//SAFE_RELEASE(&depthStencilTexture);
	}

	void Application::CreateDeviceD3D(HWND hWnd)
	{
		//Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
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