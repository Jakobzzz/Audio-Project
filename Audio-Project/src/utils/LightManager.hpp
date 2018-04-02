/*
	Basic light management class for a directional light source
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
		void SetLightDirection(const Vector3 & direction);
		void SetAmbientStrength(const float & ambient);
		void SetSpecularStrength(const float & specular);

	public:
		Vector3 GetLightDirection() const;
		float GetAmbientStrength() const;
		float GetSpecularStrength() const;

	private:
		Vector3 m_lightDirection;
		float m_ambientTerm;
		float m_specularTerm;
	};
}