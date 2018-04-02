#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace px
{
	class Transformable
	{
	public:
		Transformable(const Vector3 & position = Vector3(), const Vector3 & scale = Vector3(1.f), const Vector3 & rotation = Vector3());
		~Transformable() = default;

	public:
		void SetPosition(const Vector3 & position);
		void SetRotation(const Vector3 & rotation);
		void SetScale(const Vector3 & scale);
		void SetTransform();
		void SetIdentity();

	public:
		Vector3 GetPosition() const;
		Vector3 GetRotation() const;
		Vector3 GetScale() const;
		Matrix GetTransform() const;

	private:
		Vector3 m_position;
		Vector3 m_scale;
		Vector3 m_rotationAngles;
		Matrix m_world;
	};
}
