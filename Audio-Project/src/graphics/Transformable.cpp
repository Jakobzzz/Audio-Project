#include <graphics/Transformable.hpp>

namespace px
{
	Transformable::Transformable(const Vector3 & position, const Vector3 & scale, const Vector3 & rotation) : m_position(position), m_scale(scale), 
																											  m_rotationAngles(rotation)
						
	{
	}

	void Transformable::SetPosition(const Vector3 & position)
	{
		m_position = position;
	}

	void Transformable::SetRotation(const Vector3 & rotation)
	{
		m_rotationAngles = rotation;
	}

	void Transformable::SetScale(const Vector3 & scale)
	{
		m_scale = scale;
	}

	void Transformable::SetTransform()
	{
		SetIdentity();
		m_world = XMMatrixScalingFromVector(m_scale) * XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rotationAngles.x), XMConvertToRadians(m_rotationAngles.y),
				  XMConvertToRadians(m_rotationAngles.z)) * XMMatrixTranslationFromVector(m_position);
	}

	void Transformable::SetIdentity()
	{
		m_world = XMMatrixIdentity();
	}

	Vector3 Transformable::GetPosition() const
	{
		return m_position;
	}

	Vector3 Transformable::GetRotation() const
	{
		return m_rotationAngles;
	}

	Vector3 Transformable::GetScale() const
	{
		return m_scale;
	}

	Matrix Transformable::GetTransform() const
	{	
		return m_world;
	}
}