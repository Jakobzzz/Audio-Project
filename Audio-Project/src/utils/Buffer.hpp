/*
	Buffer utility class for resource handling
*/

#pragma once
#include <d3d11.h>

namespace px
{
	enum ShaderType;

	class Buffer
	{
	public:
		Buffer() = default;
		Buffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		~Buffer() = default;

	public:
		void CreateVertexBuffer(const void* data, UINT size, UINT stride, ID3D11Buffer** buffer,
								D3D11_CPU_ACCESS_FLAG flag = (D3D11_CPU_ACCESS_FLAG)0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
		void CreateIndexBuffer(const void* data, UINT size, UINT stride, ID3D11Buffer** buffer,
								D3D11_CPU_ACCESS_FLAG flag = (D3D11_CPU_ACCESS_FLAG)0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
		void CreateConstantBuffer(const void* data, UINT size, UINT stride, ID3D11Buffer** buffer,
								D3D11_CPU_ACCESS_FLAG flag = (D3D11_CPU_ACCESS_FLAG)0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

	public:
		void UpdateConstantBuffer(const void* data, ID3D11Buffer** buffer);

	public:
		void SetVertexBuffer(ID3D11Buffer** buffer, UINT stride);
		void SetIndexBuffer(ID3D11Buffer** buffer);
		void SetConstantBuffer(UINT shaderRegister, ID3D11Buffer** buffer, ShaderType type);
		void Draw(UINT vertexCount);
		void DrawIndexed(UINT indexCount);

	private:
		void CreateBuffer(const void* data, UINT size, UINT stride, ID3D11Buffer** buffer, D3D11_BIND_FLAG bindFlag, 
						  D3D11_CPU_ACCESS_FLAG flag, D3D11_USAGE usage);

	private:
		ID3D11Device * m_device;
		ID3D11DeviceContext* m_deviceContext;
	};
}
