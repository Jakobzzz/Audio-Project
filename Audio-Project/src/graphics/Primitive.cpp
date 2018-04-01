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
	}cb;

	Vertex vertices[] =
	{
		Vector4(0.0f, 0.5f, 0.5f, 1.f),
		Vector4(0.5f, -0.5f, 0.5f, 1.f),
		Vector4(-0.5f, -0.5f, 0.5f, 1.f)
	};

	Primitive::Primitive(Camera* camera, Buffer * buffer, Model* model) : m_camera(camera), m_buffer(buffer), m_model(model)
	{
		//Prepare buffers
		//m_buffer->CreateVertexBuffer(vertices, ARRAYSIZE(vertices), sizeof(Vertex), m_vertexBuffer.GetAddressOf(), (D3D11_CPU_ACCESS_FLAG)0);
		m_buffer->CreateConstantBuffer(&cb, sizeof(cb), 1, m_constantBuffer.GetAddressOf(), (D3D11_CPU_ACCESS_FLAG)0);
	}

	void Primitive::Draw()
	{
		//Update constant buffer data
		Matrix world = XMMatrixIdentity();
		cb.WVP = world * m_camera->GetViewProjectionMatrix();
		m_buffer->UpdateConstantBuffer(&cb, m_constantBuffer.GetAddressOf());

		m_buffer->SetConstantBuffer(0, m_constantBuffer.GetAddressOf(), VS);
		m_model->Draw(Models::Cube, Shaders::Basic, Topology::TRIANGLES);
		/*m_shader->SetShaders(Shaders::BASIC, TRIANGLES);
		m_buffer->SetVertexBuffer(m_vertexBuffer.GetAddressOf(), sizeof(Vertex));
		m_buffer->Draw(ARRAYSIZE(vertices));
		m_shader->Unbind(Shaders::BASIC);*/
	}
}