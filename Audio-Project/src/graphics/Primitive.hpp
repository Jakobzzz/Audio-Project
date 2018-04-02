/*
	Basic class to test functionality and render a triangle
*/

#pragma once
#include <wrl.h>

using namespace Microsoft::WRL;

struct ID3D11Buffer;

namespace px
{
	class Buffer;
	class Camera;
	class Model;
	class LightManager;

	class Primitive
	{
	public:
		Primitive() = default;
		Primitive(Camera* camera, Buffer* buffer, LightManager * lightManager, Model* model);
		~Primitive() = default;
		void Draw();

	private:
		Buffer * m_buffer;
		Camera * m_camera;
		Model * m_model;
		LightManager * m_lightManager;
		ComPtr<ID3D11Buffer> m_constantBuffer;
		ComPtr<ID3D11Buffer> m_lightBuffer;
	};
}
