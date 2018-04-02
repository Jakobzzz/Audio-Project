/*
	Model loading class with the Assimp library
*/

#pragma once
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <graphics/importer/Mesh.hpp>

namespace px
{
	class Buffer;

	namespace Models
	{
		enum ID
		{
			Cube
		};
	}

	class Model
	{
	public:
		Model() = default;
		Model(Buffer* buffer, Shader* shader);
		~Model() = default;

	public:
		bool LoadModel(const Models::ID & id, const std::string & filePath);
		void Draw(const Models::ID & mID, const Shaders::ID & id, Topology topology);

	private:
		struct ModelInfo
		{
			std::string filePath;
			std::vector<Mesh> meshes;
		};

	private:
		void ProcessNode(const Models::ID & id, aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

	private:
		Shader * m_shader;
		Buffer * m_buffer;
		std::map<Models::ID, ModelInfo> m_meshes;
	};
}

