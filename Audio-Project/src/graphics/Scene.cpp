#include <graphics/Scene.hpp>
#include <graphics/systems/RenderSystem.hpp>
#include <graphics/importer/Model.hpp>
#include <graphics/components/Render.hpp>
#include <graphics/components/Transform.hpp>
#include <utils/D3DUtility.hpp>
#include <utils/Buffer.hpp>
#include <utils/Camera.hpp>
#include <utils/LightManager.hpp>
#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace px
{
	inline Vector3 FromVec3Json(std::vector<float> values)
	{
		Vector3 result = { values[0], values[1], values[2] };
		return result;
	}

	inline std::vector<float> ToVec3Json(const Vector3 & values)
	{
		std::vector<float> result = { values.x, values.y, values.z };
		return result;
	}

	Scene::Scene(Camera * camera, Buffer * buffer, Model* model, LightManager * lightManager) : m_camera(camera), m_buffer(buffer), m_lightManager(lightManager), m_model(model),
																								m_entities(m_events), m_systems(m_entities, m_events)
	{
		LoadScene();

		//Systems
		m_systems.add<RenderSystem>(m_camera, m_buffer, m_lightManager);
		m_systems.configure();
	}

	Scene::~Scene()
	{
		//Destroy entities
		ComponentHandle<Render> render;

		for (Entity entity : m_entities.entities_with_components(render))
		{
			entity.destroy();
		}
	}

	void Scene::LoadScene()
	{
		//Read scene data from json file
		std::ifstream i("src/res/metadata/scene.json");
		json reader; i >> reader; i.close();

		//Camera
		m_camera->SetPosition(FromVec3Json(reader["Camera"]["position"]));
		m_camera->SetYaw(reader["Camera"]["yaw"]);
		m_camera->SetPitch(reader["Camera"]["pitch"]);

		//Light
		m_lightManager->SetLightPosition(FromVec3Json(reader["Light"]["position"]));
		m_lightManager->SetAmbientStrength(reader["Light"]["ambient"]);
		m_lightManager->SetSpecularStrength(reader["Light"]["specular"]);

		//Entities
		for (unsigned int i = 0; i < reader["Scene"]["count"]; ++i)
		{
			std::string name = reader["Scene"]["names"][i];
			auto entity = m_entities.create();
			entity.assign<Render>(std::make_unique<Renderable>(m_model, reader[name]["model"], reader[name]["shader"], name));
			entity.assign<Transform>(std::make_unique<Transformable>(FromVec3Json(reader[name]["position"]), FromVec3Json(reader[name]["scale"]),
																	 FromVec3Json(reader[name]["rotation"])));
		}
	}

	void Scene::WriteSceneData()
	{
		//Write scene information to json file
		json data;
		data["Scene"]["count"] = m_entities.size();
		data["Camera"]["position"] = ToVec3Json(m_camera->GetPosition());
		data["Camera"]["yaw"] = m_camera->GetYaw();
		data["Camera"]["pitch"] = m_camera->GetPitch();
		data["Light"]["position"] = ToVec3Json(m_lightManager->GetLightPosition());
		data["Light"]["ambient"] = m_lightManager->GetAmbientStrength();
		data["Light"]["specular"] = m_lightManager->GetSpecularStrength();

		unsigned int i = 0;
		ComponentHandle<Transform> transform;
		ComponentHandle<Render> render;

		for (Entity entity : m_entities.entities_with_components(render, transform))
		{
			data["Scene"]["names"][i] = render->object->GetName();
			data[render->object->GetName()]["model"] = render->object->GetModel();
			data[render->object->GetName()]["shader"] = render->object->GetShader();
			data[render->object->GetName()]["position"] = ToVec3Json(transform->transform->GetPosition());
			data[render->object->GetName()]["rotation"] = ToVec3Json(transform->transform->GetRotation());
			data[render->object->GetName()]["scale"] = ToVec3Json(transform->transform->GetScale());
			i++;
		}

		//Dump information
		std::ofstream o("src/res/metadata/scene.json");
		o << std::setw(3) << data << std::endl;
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