#pragma once
#include <string>
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

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
		Renderable(Model* model, const Models::ID & mId, const Shaders::ID & sId, const Vector3 & color, const std::string & name);
		~Renderable() = default;
		void Draw();

	public:
		void SetShader(const Shaders::ID & id);
		void SetModel(const Models::ID & id);
		void SetName(const std::string & name);
		void SetColor(const Vector3 & color);
		void SetAmbientStrength(const float & ambient);
		void SetSpecularStrength(const float & specular);

	public:
		Shaders::ID GetShader() const;
		Models::ID GetModel() const;
		std::string GetName() const;
		Vector3 GetColor() const;
		float GetAmbientStrength() const;
		float GetSpecularStrength() const;

	private:
		Model * m_model;
		Shaders::ID m_shaderID;
		Models::ID m_modelID;
		std::string m_name;
		Vector3 m_color;
		float m_ambientTerm;
		float m_specularTerm;
	};
}