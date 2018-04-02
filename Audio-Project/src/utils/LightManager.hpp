#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace px
{
	class LightManager
	{
	public:
		LightManager();
		~LightManager() = default;

	public:
		void SetLightDirection(const Vector3 & direction);

	public:
		Vector3 GetLightDirection() const;

	private:
		Vector3 m_lightDirection;
	};
}