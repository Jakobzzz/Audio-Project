#pragma once
#include <entityx/entityx.h>
#include <wrl.h>

using namespace Microsoft::WRL;
using namespace entityx;

struct ID3D11Buffer;

namespace px
{
	class Buffer;
	class Camera;
	class LightManager;

	class RenderSystem : public System<RenderSystem>
	{
	public:
		explicit RenderSystem(Camera* camera, Buffer* buffer, LightManager * lightManager);
		~RenderSystem();

	public:
		void update(EntityManager & es, EventManager & events, TimeDelta dt) override;

	private:
		Buffer * m_buffer;
		Camera * m_camera;
		LightManager * m_lightManager;
		ComPtr<ID3D11Buffer> m_constantBuffer;
		ComPtr<ID3D11Buffer> m_lightBuffer;
	};
}
