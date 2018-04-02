#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <SimpleMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace px
{
	class Buffer;

	class Transformable
	{
	public:
		Transformable() = default;
		Transformable(Buffer* buffer, const Vector3 & position = Vector3(), const Vector3 & scale = Vector3(1.f));
		~Transformable() = default;

	public:
		void SetPosition(const Vector3 & position);
		void SetRotation(const Vector3 & rotation, const float & angle);
		void SetScale(const Vector3 & scale);
		void SetIdentity();

	public:
		Vector3 GetPosition() const;
		Vector3 GetRotation() const;
		Vector3 GetScale() const;
		Matrix GetTransform() const;

	private:
		ComPtr<ID3D11Buffer> m_transformBuffer;
		Buffer * m_buffer;
		Vector3 m_position;
		Vector3 m_scale;
		Vector3 m_rotationAngles;
		//Quaternion m_orientation;
		Matrix m_world;
	};
}
