#include <graphics/importer/Model.hpp>
#include <assimp/Importer.hpp>
#include <iostream>
#include <utils/Buffer.hpp>

namespace px
{
	Model::Model(Buffer * buffer, Shader * shader) : m_buffer(buffer), m_shader(shader)
	{
	}

	bool Model::LoadModel(const Models::ID & id, const std::string & filePath)
	{
		ModelInfo info;
		info.filePath = filePath;

		//Read file with Assimp
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(info.filePath, aiProcess_Triangulate | aiProcess_GenNormals); //Add optional flags here
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return false;
		}

		auto inserted = m_meshes.insert(std::make_pair(id, info));
		assert(inserted.second);
		ProcessNode(id, scene->mRootNode, scene);
		return true;
	}

	void Model::Draw(const Models::ID & mID, const Shaders::ID & id, Topology topology)
	{
		auto model = m_meshes.find(mID);

		for (auto & mesh : model->second.meshes)
			mesh.Draw(id, topology);
	}

	void Model::ProcessNode(const Models::ID & id, aiNode * node, const aiScene * scene)
	{
		auto model = m_meshes.find(id);

		//Process mesh at current node
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			model->second.meshes.push_back(ProcessMesh(mesh, scene));
		}

		//Recursive process over the children
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(id, node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
	{
		std::vector<Mesh::VertexLayout> vertices;
		std::vector<UINT> indices;

		//Grab the vertex data from the mesh
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			Mesh::VertexLayout vertex;
			Vector3 positions, normals;

			vertex.position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			vertices.push_back(vertex);
		}

		//Grab the indices of the mesh
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
				indices.push_back(face.mIndices[j]);
		}

		//Return mesh object with extracted data
		return Mesh(vertices, indices, m_buffer, m_shader);
	}
}
