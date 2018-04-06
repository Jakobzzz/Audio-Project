#include <utils/LightManager.hpp>

namespace px
{
	LightManager::LightManager()
	{
	}

	void LightManager::SetLightPosition(const Vector3 & position)
	{
		m_position = position;
	}

	Vector3 LightManager::GetLightPosition() const
	{
		return m_position;
	}
}
