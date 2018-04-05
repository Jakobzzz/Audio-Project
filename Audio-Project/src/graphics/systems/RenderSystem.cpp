#include <graphics/systems/RenderSystem.hpp>
#include <graphics/components/Render.hpp>
#include <graphics/components/Transform.hpp>
#include <utils/D3DUtility.hpp>
#include <utils/Buffer.hpp>
#include <utils/Camera.hpp>
#include <utils/LightManager.hpp>

namespace px
{
	struct vsData
	{
		Matrix WVP;
		Matrix world;
	}cb;

	struct psData
	{
		Vector3 lightPos;
		float ambient;
		Vector3 camPos;
		float specular;
		Vector3 color;
		float pad;
	}lightCb;

	RenderSystem::RenderSystem(Camera* camera, Buffer* buffer, LightManager * lightManager) : m_camera(camera), m_buffer(buffer), m_lightManager(lightManager)
	{
		//Prepare standard constant buffers
		m_buffer->CreateConstantBuffer(&lightCb, sizeof(lightCb), 1, m_lightBuffer.GetAddressOf());
		m_buffer->CreateConstantBuffer(&cb, sizeof(cb), 1, m_constantBuffer.GetAddressOf());
	}

	RenderSystem::~RenderSystem()
	{
	}

	void RenderSystem::update(EntityManager & es, EventManager & events, TimeDelta dt)
	{
		ComponentHandle<Render> render;
		ComponentHandle<Transform> transform;

		for (Entity entity : es.entities_with_components(render, transform))
		{
			//Update constant buffer data
			transform->transform->SetTransform();
			cb.world = transform->transform->GetTransform();
			cb.WVP = transform->transform->GetTransform() * m_camera->GetViewProjectionMatrix();
			m_buffer->UpdateConstantBuffer(&cb, m_constantBuffer.GetAddressOf());

			lightCb.camPos = m_camera->GetPosition(); 
			lightCb.lightPos = m_lightManager->GetLightPosition();
			lightCb.ambient = m_lightManager->GetAmbientStrength();
			lightCb.specular = m_lightManager->GetSpecularStrength();
			lightCb.color = render->object->GetColor();
			m_buffer->UpdateConstantBuffer(&lightCb, m_lightBuffer.GetAddressOf());

			m_buffer->SetConstantBuffer(0, m_constantBuffer.GetAddressOf(), VS);
			m_buffer->SetConstantBuffer(1, m_lightBuffer.GetAddressOf(), PS);
			render->object->Draw();
		}
	}
}