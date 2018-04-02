#include <utils/LightManager.hpp>

namespace px
{
	LightManager::LightManager()  : m_lightDirection(Vector3(10.f, 50.f, 25.f))
	{
	}

	void LightManager::SetLightDirection(const Vector3 & direction)
	{
		m_lightDirection = direction;
	}

	Vector3 LightManager::GetLightDirection() const
	{
		return m_lightDirection;
	}
}
