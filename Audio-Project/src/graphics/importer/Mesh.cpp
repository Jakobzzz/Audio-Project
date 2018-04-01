#include <graphics/importer/Mesh.hpp>
#include <utils/Buffer.hpp>

namespace px
{
	Mesh::Mesh(std::vector<VertexLayout> & vertices, std::vector<UINT> & indices, Buffer* buffer, Shader* shader) : m_vertices(vertices), m_indices(indices),
																													m_shader(shader), m_buffer(buffer)
	{
		SetupMesh();
	}

	void Mesh::Draw(const Shaders::ID & id, Topology topology)
	{
		m_shader->SetShaders(id, topology);
		m_buffer->SetVertexBuffer(m_vertexBuffer.GetAddressOf(), sizeof(VertexLayout));
		m_buffer->SetIndexBuffer(m_indexBuffer.GetAddressOf());
		m_buffer->DrawIndexed(m_nrOfIndices);
		m_shader->Unbind(id);
	}

	void Mesh::SetupMesh()
	{
		//Prepare all buffers with the data
		m_buffer->CreateVertexBuffer(&m_vertices[0], m_vertices.size(), sizeof(VertexLayout), m_vertexBuffer.GetAddressOf(), (D3D11_CPU_ACCESS_FLAG)0);
		m_buffer->CreateIndexBuffer(&m_indices[0], m_indices.size(), sizeof(UINT), m_indexBuffer.GetAddressOf(), (D3D11_CPU_ACCESS_FLAG)0);

		//Clear vectors from CPU memory as the memory is already uploaded to the GPU
		m_nrOfIndices = m_indices.size();
		std::vector<Mesh::VertexLayout>().swap(m_vertices);
		std::vector<UINT>().swap(m_indices);
	}
}
