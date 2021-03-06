/*
	Assembly class of the application
*/

#pragma once
#include <wrl.h>
#include <memory>
#include <d3d11.h>

using namespace Microsoft::WRL;

namespace px
{
	class Shader;
	class Buffer;
	class Camera;
	class SoundManager;
	class LightManager;
	class Model;
	class Scene;

	class Application
	{
	public:
		Application();
		~Application();
		void Run();

	private:
		void LoadObjects();
		void LoadModels();
		void LoadAudioFiles();
		void LoadShaders();
		void Update();
		void UpdateSounds();
		void UpdateGUI();
		void PollEvents();
		void RenderScene();
		void CreateDeviceD3D(HWND hWnd);
		void CreateViewport();

	private:
		void CreateRenderTargetAndDepthStencil();
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		std::unique_ptr<Shader> m_shaders;
		std::unique_ptr<Buffer> m_buffer;
		std::unique_ptr<Camera> m_camera;
		std::unique_ptr<Model> m_models;
		std::unique_ptr<Scene> m_scene;
		std::unique_ptr<LightManager> m_lightManager;
		std::unique_ptr<SoundManager> m_soundManager;

	private:
		D3D11_VIEWPORT m_vp;
		WNDCLASSEX m_windowClass;
		ComPtr<ID3D11Device> m_device;
		ComPtr<ID3D11DeviceContext> m_deviceContext;
		ComPtr<IDXGISwapChain> m_swapChain;
		ComPtr<ID3D11RenderTargetView> m_mainRenderTargetView;
		ComPtr<ID3D11RasterizerState> m_noCull;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	};
}