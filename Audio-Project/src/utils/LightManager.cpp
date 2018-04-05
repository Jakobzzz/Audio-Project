#include <utils/LightManager.hpp>

namespace px
{
	LightManager::LightManager()  : m_position(Vector3(1.2f, 2.5f, 2.f)), m_ambientTerm(0.5f), m_specularTerm(0.5f)
	{
	}

	void LightManager::SetLightPosition(const Vector3 & direction)
	{
		m_position = direction;
	}

	void LightManager::SetAmbientStrength(const float & ambient)
	{
		m_ambientTerm = ambient;
	}

	void LightManager::SetSpecularStrength(const float & specular)
	{
		m_specularTerm = specular;
	}

	Vector3 LightManager::GetLightPosition() const
	{
		return m_position;
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
