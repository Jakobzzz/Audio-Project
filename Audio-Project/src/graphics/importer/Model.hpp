//#pragma once
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include <graphics/importer/Mesh.hpp>
//
//class Model
//{
//public:
//	static void Draw(std::string modelID, std::string shaderID);
//	static void DrawInstanced(std::string modelID, std::string shaderID, UINT instancedStructSize);
//	static bool LoadModel(std::string modelID, const std::string &path);
//	template <typename T>
//	static bool LoadInstancedModel(std::string modelID, const std::string &path, std::vector<T> &instancedData);
//
//private:
//	struct Info
//	{
//		std::string name;
//		std::vector<Mesh> meshes;
//	};
//
//private:
//	static void ProcessNode(aiNode* node, const aiScene* scene, std::string modelID);
//	static Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, std::string modelID);
//	template<typename T>
//	static void ProcessNode(aiNode* node, const aiScene* scene, std::string modelID, std::vector<T> &instancedData);
//	template<typename T>
//	static Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, std::string modelID, std::vector<T> &instancedData);
//	static std::vector<Mesh::TextureInfo> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
//
//private:
//	static std::vector<Mesh::TextureInfo> m_textures_loaded;
//	static std::map<std::string, Info> m_meshes;
//	static std::string m_directory;
//};
//
//template<typename T>
//inline bool Model::LoadInstancedModel(std::string modelID, const std::string & path, std::vector<T> &instancedData)
//{
//	m_meshes[modelID].name = modelID;
//
//	//Read file with Assimp
//	Assimp::Importer importer;
//
//	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace); //Add flags here
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//	{
//		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
//		return false;
//	}
//	else
//		std::cout << "Loaded model at: " << path << std::endl;
//
//	m_directory = path.substr(0, path.find_last_of('/'));
//	ProcessNode(scene->mRootNode, scene, modelID, instancedData);
//	return true;
//}
//
//template<typename T>
//inline void Model::ProcessNode(aiNode * node, const aiScene * scene, std::string modelID, std::vector<T> &instancedData)
//{
//	//Process mesh at current node
//	for (unsigned int i = 0; i < node->mNumMeshes; i++)
//	{
//		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//		m_meshes[modelID].meshes.push_back(ProcessMesh(mesh, scene, modelID, instancedData));
//	}
//
//	//Recursive process over the children
//	for (unsigned int i = 0; i < node->mNumChildren; i++)
//	{
//		ProcessNode(node->mChildren[i], scene, modelID, instancedData);
//	}
//}
//
//template<typename T>
//inline Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene, std::string modelID, std::vector<T> &instancedData)
//{
//	std::vector<Mesh::VertexLayout> vertices;
//	std::vector<DWORD> indices;
//	std::vector<Mesh::TextureInfo> textures;
//
//	//Grab the vertices of the mesh
//	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
//	{
//		Mesh::VertexLayout vertex;
//		Vector3 positions, normals, tangents;
//
//		//Positions
//		positions.x = mesh->mVertices[i].x;
//		positions.y = mesh->mVertices[i].y;
//		positions.z = mesh->mVertices[i].z;
//		vertex.position = positions;
//
//		//Normals
//		normals.x = mesh->mNormals[i].x;
//		normals.y = mesh->mNormals[i].y;
//		normals.z = mesh->mNormals[i].z;
//		vertex.normal = normals;
//
//		//Tangents
//		tangents.x = mesh->mTangents[i].x;
//		tangents.y = mesh->mTangents[i].y;
//		tangents.z = mesh->mTangents[i].z;
//		vertex.tangent = tangents;
//
//		//Texture-coordinates
//		if (mesh->mTextureCoords[0])
//		{
//			Vector2 texture;
//
//			texture.x = mesh->mTextureCoords[0][i].x;
//			texture.y = mesh->mTextureCoords[0][i].y;
//			vertex.texture = texture;
//		}
//		else
//			vertex.texture = Vector2(0.0f, 0.0f);
//
//		//Add the current vertex to vector
//		vertices.push_back(vertex);
//	}
//
//	//Grab the indices of the mesh
//	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
//	{
//		aiFace face = mesh->mFaces[i];
//		for (unsigned int j = 0; j < face.mNumIndices; j++)
//			indices.push_back(face.mIndices[j]);
//	}
//
//	//Materials
//	if (mesh->mMaterialIndex >= 0)
//	{
//		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
//		std::vector<Mesh::TextureInfo> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//
//		std::vector<Mesh::TextureInfo> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//
//		std::vector<Mesh::TextureInfo> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
//		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
//	}
//
//	//Return mesh object with extracted data
//	return Mesh(vertices, indices, instancedData, textures);
//}