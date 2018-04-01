#pragma once
#include <utils/Shader.hpp>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace px
{
	class Buffer;

	class Mesh
	{
	public:
		struct VertexLayout
		{
			Vector3 position;
			Vector3 normal;
		};

	public:
		Mesh(std::vector<VertexLayout> & vertices, std::vector<UINT> & indices, Buffer* buffer, Shader* shader);
		~Mesh();
		void Draw(const Shaders::ID & id, Topology topology);

	private:
		void SetupMesh();

	private:
		Buffer * m_buffer;
		Shader * m_shader;

	private:
		std::vector<VertexLayout> m_vertices;
		std::vector<UINT> m_indices;
		ComPtr<ID3D11Buffer> m_vertexBuffer;
		ComPtr<ID3D11Buffer> m_indexBuffer;
		UINT m_nrOfIndices;
	};
}


