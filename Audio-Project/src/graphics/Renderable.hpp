#pragma once
#include <string>

namespace px
{
	class Model;
	class Shader;

	namespace Shaders
	{
		enum ID;
	}

	namespace Models
	{
		enum ID;
	}

	class Renderable
	{
	public:
		Renderable() = default;
		Renderable(Model* model, const Models::ID & mId, const Shaders::ID & sId, const std::string & name);
		~Renderable() = default;
		void Draw();

	public:
		void SetShader(const Shaders::ID & id);
		void SetModel(const Models::ID & id);
		void SetName(const std::string & name);

	public:
		Shaders::ID GetShader() const;
		Models::ID GetModel() const;
		std::string GetName() const;

	private:
		Model * m_model;

	private:
		Shaders::ID m_shaderID;
		Models::ID m_modelID;
		std::string m_name;
	};
}