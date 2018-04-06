/*
	Basic light management class for a point light source
*/

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
		void SetLightPosition(const Vector3 & position);

	public:
		Vector3 GetLightPosition() const;

	private:
		Vector3 m_position;
	};
}