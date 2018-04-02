#include <graphics/Transformable.hpp>
#include <utils/Buffer.hpp>

namespace px
{
	Transformable::Transformable(Buffer * buffer, const Vector3 & position, const Vector3 & scale) : m_buffer(buffer), m_position(position), m_scale(scale)
						
	{
	}

	void Transformable::SetPosition(const Vector3 & position)
	{
		m_position = position;
	}

	void Transformable::SetRotation(const Vector3 & rotation, const float & angle)
	{
		//TODO
	}

	void Transformable::SetScale(const Vector3 & scale)
	{
		m_scale = scale;
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
		//m_world = XMMatrixScalingFromVector(m_scale) * XMMatrixTranslationFromVector(m_position);
		return m_world;
	}
}