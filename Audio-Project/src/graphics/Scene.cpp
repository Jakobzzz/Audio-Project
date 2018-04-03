#include <graphics/Scene.hpp>
#include <graphics/systems/RenderSystem.hpp>
#include <graphics/importer/Model.hpp>
#include <graphics/components/Render.hpp>
#include <graphics/components/Transform.hpp>
#include <utils/D3DUtility.hpp>
#include <utils/Buffer.hpp>
#include <utils/Camera.hpp>
#include <utils/LightManager.hpp>

namespace px
{
	Scene::Scene(Camera * camera, Buffer * buffer, Model* model, LightManager * lightManager) : m_camera(camera), m_buffer(buffer), m_lightManager(lightManager), m_model(model),
																								m_entities(m_events), m_systems(m_entities, m_events)
	{	
		//Add a basic entity for test
		auto entity = m_entities.create();		
		entity.assign<Render>(std::make_unique<Renderable>(m_model, Models::Cube, Shaders::Basic, "Cube"));
		entity.assign<Transform>(std::make_unique<Transformable>());

		auto e = m_entities.create();
		e.assign<Render>(std::make_unique<Renderable>(m_model, Models::Cube, Shaders::Basic, "Cube1"));
		e.assign<Transform>(std::make_unique<Transformable>(Vector3(0.f, 3.f, 0.f)));

		//Systems
		m_systems.add<RenderSystem>(m_camera, m_buffer, m_lightManager);
		m_systems.configure();
	}

	Scene::~Scene()
	{
		ComponentHandle<Render> render;

		for (Entity entity : m_entities.entities_with_components(render))
		{
			entity.destroy();
		}
	}

	void Scene::UpdateSystems(double dt)
	{
		m_systems.update<RenderSystem>(dt);
	}

	EntityManager & Scene::GetEntites()
	{
		return m_entities;
	}
}