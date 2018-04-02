#include <graphics/Primitive.hpp>
#include <graphics/importer/Model.hpp>
#include <utils/Buffer.hpp>
#include <utils/Shader.hpp>
#include <utils/Camera.hpp>
#include <utils/D3DUtility.hpp>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace px
{
	struct Vertex
	{
		Vector4 position;
	};

	struct CBData
	{
		Matrix WVP;
		Matrix world;
	}cb;

	struct LightCB
	{
		Vector3 lightDir;
		float pad;
		Vector3 camPos;
		float pad1;
	}lightCb;

	Vertex vertices[] =
	{
		Vector4(0.0f, 0.5f, 0.5f, 1.f),
		Vector4(0.5f, -0.5f, 0.5f, 1.f),
		Vector4(-0.5f, -0.5f, 0.5f, 1.f)
	};

	Primitive::Primitive(Camera* camera, Buffer * buffer, Model* model) : m_camera(camera), m_buffer(buffer), m_model(model)
	{
		//Prepare constant buffers
		m_buffer->CreateConstantBuffer(&lightCb, sizeof(lightCb), 1, m_lightBuffer.GetAddressOf(), (D3D11_CPU_ACCESS_FLAG)0);
		m_buffer->CreateConstantBuffer(&cb, sizeof(cb), 1, m_constantBuffer.GetAddressOf(), (D3D11_CPU_ACCESS_FLAG)0);
	}

	void Primitive::Draw()
	{
		//Update constant buffer data
		Matrix world = XMMatrixIdentity();
		cb.world = world;
		cb.WVP = world * m_camera->GetViewProjectionMatrix();
		m_buffer->UpdateConstantBuffer(&cb, m_constantBuffer.GetAddressOf());

		lightCb.camPos = m_camera->GetCameraPosition();
		lightCb.lightDir = Vector3(-5.f, 2.f, 3.0f);
		m_buffer->UpdateConstantBuffer(&lightCb, m_lightBuffer.GetAddressOf());

		m_buffer->SetConstantBuffer(0, m_constantBuffer.GetAddressOf(), VS);
		m_buffer->SetConstantBuffer(1, m_lightBuffer.GetAddressOf(), PS);
		m_model->Draw(Models::Cube, Shaders::Basic, Topology::TRIANGLES);
	}
}