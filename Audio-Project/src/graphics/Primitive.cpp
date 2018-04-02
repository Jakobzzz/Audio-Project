#include <graphics/Primitive.hpp>
#include <graphics/importer/Model.hpp>
#include <utils/Buffer.hpp>
#include <utils/Shader.hpp>
#include <utils/Camera.hpp>
#include <utils/D3DUtility.hpp>
#include <utils/LightManager.hpp>

namespace px
{
	struct CBData
	{
		Matrix WVP;
		Matrix world;
	}cb;

	struct LightCB
	{
		Vector3 lightDir;
		float ambient;
		Vector3 camPos;
		float specular;
	}lightCb;


	Primitive::Primitive(Camera* camera, Buffer * buffer, LightManager * lightManager, Model* model) : m_camera(camera), m_buffer(buffer), m_model(model), 
																									   m_lightManager(lightManager)
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
		lightCb.lightDir = m_lightManager->GetLightDirection();
		lightCb.ambient = m_lightManager->GetAmbientStrength();
		lightCb.specular = m_lightManager->GetSpecularStrength();
		m_buffer->UpdateConstantBuffer(&lightCb, m_lightBuffer.GetAddressOf());

		m_buffer->SetConstantBuffer(0, m_constantBuffer.GetAddressOf(), VS);
		m_buffer->SetConstantBuffer(1, m_lightBuffer.GetAddressOf(), PS);
		m_model->Draw(Models::Cube, Shaders::Basic, Topology::TRIANGLES);
	}
}