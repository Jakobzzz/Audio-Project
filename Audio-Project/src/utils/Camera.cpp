#include <utils/Camera.hpp>
#include <utils/Input.hpp>
#include <utils/D3DUtility.hpp>
#include <iostream>

namespace px
{
	Camera::Camera() = default;

	Camera::Camera(const Vector3 & camPosition, const Vector3 & camTarget, const Vector3 & camUp, float speed, float sensitivity) :
				   m_cameraPos(camPosition), m_camTarget(camTarget), m_camUp(camUp), m_movementSpeed(speed), m_mouseSensivity(sensitivity),
				   m_camYaw(0.f), m_camPitch(0.f), m_fov(45.0f)
	{
		m_currentMousePos = Vector2(static_cast<float>(Input::GetMousePositionX()), static_cast<float>(Input::GetMousePositionY()));
		m_lastMousePos = m_currentMousePos;
	}

	void Camera::Update(float dt)
	{
		MoveCamera(dt);
		RotateCamera();
	}

	void Camera::MoveCamera(float dt)
	{
		m_cameraPos.y = 1.5f;
		Input::ResetScrollWheelValue();
		float velocity = m_movementSpeed * dt;

		//Move camera
		if (Input::GetKey(Keyboard::Keys::W))
			m_cameraPos += m_camTarget * velocity;
		if (Input::GetKey(Keyboard::Keys::S))
			m_cameraPos -= m_camTarget * velocity;
		if (Input::GetKey(Keyboard::Keys::D))
			m_cameraPos += XMVector3Normalize(XMVector3Cross(m_camUp, m_camTarget)) * velocity;
		if (Input::GetKey(Keyboard::Keys::A))
			m_cameraPos -= XMVector3Normalize(XMVector3Cross(m_camUp, m_camTarget)) * velocity;

		//Adjust camera position depending on mouse scroll
		if (Input::GetMouseScrollWheel() > 0)
			m_cameraPos += m_camTarget * 5.f;

		if (Input::GetMouseScrollWheel() < 0)
			m_cameraPos -= m_camTarget * 5.f;
	}

	void Camera::RotateCamera()
	{
		m_lastMousePos = m_currentMousePos;
		m_currentMousePos = Vector2(static_cast<float>(Input::GetMousePositionX()), static_cast<float>(Input::GetMousePositionY()));

		if (Mouse::MODE_RELATIVE)
		{
			//As we use relative mouse-coordinates delta is already calculated
			Vector2 mouseDelta = m_currentMousePos * m_mouseSensivity;
			m_camYaw += mouseDelta.x;
			m_camPitch += mouseDelta.y;

			//Restrict pitch angle
			if (m_camPitch > 89.0f)
				m_camPitch = 89.0f;

			if (m_camPitch < -89.0f)
				m_camPitch = -89.0f;
		}
		//if (Input::GetMouseButton(Input::MouseButton::RIGHT))
		//{
		//	//Calculate delta
		//	Vector2 mouseDelta = (m_currentMousePos - m_lastMousePos) * m_mouseSensivity;
		//	m_camYaw += mouseDelta.x;
		//	m_camPitch += mouseDelta.y;

		//	//Restrict pitch angle
		//	if (m_camPitch > 89.0f)
		//		m_camPitch = 89.0f;

		//	if (m_camPitch < -89.0f)
		//		m_camPitch = -89.0f;
		//}

		//Calculate target vector with Euler angles
		m_rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_camPitch), XMConvertToRadians(m_camYaw), 0.f);
		m_camTarget = XMVector3TransformCoord(Vector3(0.0f, 0.0f, 1.0f), m_rotationMatrix);
		m_camTarget = XMVector3Normalize(m_camTarget);

		m_viewMatrix = XMMatrixLookAtLH(m_cameraPos, m_camTarget + m_cameraPos, m_camUp);
	}

	void Camera::SetPosition(const Vector3 & position)
	{
		m_cameraPos = position;
	}

	void Camera::SetFOV(const float & angle)
	{
		m_fov = angle;
	}

	void Camera::SetYaw(const float & yaw)
	{
		m_camYaw = yaw;
	}

	void Camera::SetPitch(const float & pitch)
	{
		m_camPitch = pitch;
	}

	Matrix Camera::GetViewMatrix() const
	{
		return m_viewMatrix;
	}

	Matrix Camera::GetViewProjectionMatrix() const
	{
		return m_viewMatrix * GetProjectionMatrix();
	}

	Matrix Camera::GetProjectionMatrix() const
	{
		return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fov), static_cast<float>(WIDTH / HEIGHT), 0.1f, 1000.f);
	}

	Vector3 Camera::GetPosition() const
	{
		return m_cameraPos;
	}

	Vector3 Camera::GetCamForward() const
	{
		return m_camTarget;
	}

	Vector3 Camera::GetCamUp() const
	{
		return m_camUp;
	}

	float Camera::GetFOV() const
	{
		return m_fov;
	}

	float Camera::GetYaw() const
	{
		return m_camYaw;
	}

	float Camera::GetPitch() const
	{
		return m_camPitch;
	}
}

