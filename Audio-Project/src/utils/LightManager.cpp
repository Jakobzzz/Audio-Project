#include <utils/LightManager.hpp>

namespace px
{
	LightManager::LightManager()  : m_lightDirection(Vector3(5.f, 20.f, -5.f)), m_ambientTerm(0.5f), m_specularTerm(0.5f)
	{
	}

	void LightManager::SetLightDirection(const Vector3 & direction)
	{
		m_lightDirection = direction;
	}

	void LightManager::SetAmbientStrength(const float & ambient)
	{
		m_ambientTerm = ambient;
	}

	void LightManager::SetSpecularStrength(const float & specular)
	{
		m_specularTerm = specular;
	}

	Vector3 LightManager::GetLightDirection() const
	{
		return m_lightDirection;
	}

	float LightManager::GetAmbientStrength() const
	{
		return m_ambientTerm;
	}

	float LightManager::GetSpecularStrength() const
	{
		return m_specularTerm;
	}
}
