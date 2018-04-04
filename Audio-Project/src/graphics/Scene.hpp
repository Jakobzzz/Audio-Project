#pragma once
#include <entityx/entityx.h>

using namespace entityx;

namespace px
{
	class Buffer;
	class Camera;
	class Model;
	class LightManager;

	class Scene : public EntityX
	{
	public:
		Scene() = default;
		Scene(Camera* camera, Buffer* buffer, Model* model, LightManager * lightManager);
		~Scene();
		void UpdateSystems(double dt);

	public:
		EntityManager & GetEntites();

	private:
		void LoadScene();
		void WriteSceneData();

	private:
		Buffer * m_buffer;
		Model* m_model;
		Camera * m_camera;
		LightManager * m_lightManager;

	private:
		EntityManager m_entities;
		EventManager m_events;
		SystemManager m_systems;
	};
}