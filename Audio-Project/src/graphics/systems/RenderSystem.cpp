#include <graphics/systems/RenderSystem.hpp>
#include <graphics/components/Render.hpp>

namespace px
{
	RenderSystem::RenderSystem()
	{
	}

	RenderSystem::~RenderSystem()
	{
	}

	void RenderSystem::update(EntityManager & es, EventManager & events, TimeDelta dt)
	{
		ComponentHandle<Render> render;

		for (Entity entity : es.entities_with_components(render))
		{
			//Shader::SetMatrix4x4(renderable->object->GetShader(), "model", transform->transform->GetTransform());
			render->object->Draw();
			//transform->transform->SetIdentity();
		}
	}
}