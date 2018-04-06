#include <graphics/Renderable.hpp>
#include <graphics/importer/Model.hpp>
#include <utils/Shader.hpp>

namespace px
{
	Renderable::Renderable(Model * model, const Models::ID & mId, const Shaders::ID & sId, const Vector3 & color, const std::string & name) :
							m_model(model), m_modelID(mId), m_shaderID(sId), m_name(name), m_color(color)
	{
	}

	void Renderable::Draw()
	{
		m_model->Draw(m_modelID, m_shaderID, Topology::TRIANGLES);
	}

	void Renderable::SetShader(const Shaders::ID & id)
	{
		m_shaderID = id;
	}

	void Renderable::SetModel(const Models::ID & id)
	{
		m_modelID = id;
	}

	void Renderable::SetName(const std::string & name)
	{
		m_name = name;
	}

	void Renderable::SetColor(const Vector3 & color)
	{
		m_color = color;
	}

	void Renderable::SetAmbientStrength(const float & ambient)
	{
		m_ambientTerm = ambient;
	}

	void Renderable::SetSpecularStrength(const float & specular)
	{
		m_specularTerm = specular;
	}

	Shaders::ID Renderable::GetShader() const
	{
		return m_shaderID;
	}

	Models::ID Renderable::GetModel() const
	{
		return m_modelID;
	}

	std::string Renderable::GetName() const
	{
		return m_name;
	}

	Vector3 Renderable::GetColor() const
	{
		return m_color;
	}

	float Renderable::GetAmbientStrength() const
	{
		return m_ambientTerm;
	}

	float Renderable::GetSpecularStrength() const
	{
		return m_specularTerm;
	}
}