//  Copyright 2012 Sean MacDonnell
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include "Mesh_Component.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Mesh_Component::Mesh_Component(): Component() {
  m_d3d11device = 0;
  m_vertexbuffer = 0;
  m_indexbuffer = 0;
  m_vertex_count = 0;
  m_index_count = 0;
  m_texture = 0;
  m_texture_name = 0;
  m_type = "Mesh_Component";
}

//------------------------------------------------------------------------------
Mesh_Component::~Mesh_Component() {
  // Release the index buffer.
  if (m_indexbuffer) {
    m_indexbuffer->Release();
    m_indexbuffer = 0;
  }

  // Release the vertex buffer.
  if (m_vertexbuffer) {
    m_vertexbuffer->Release();
    m_vertexbuffer = 0;
  }

	// Release the texture resource.
	if(m_texture)	{
		m_texture->Release();
		m_texture = 0;
	}
}

//------------------------------------------------------------------------------
void Mesh_Component::Init(ID3D11Device * const d3d11device) {
  m_d3d11device = d3d11device;

  Init_Mesh();
  Init_Texture();

  m_is_initialised = true;
}

//------------------------------------------------------------------------------
ID3D11Buffer * const Mesh_Component::GetVertexBuffer() {
  return m_vertexbuffer;
}

//------------------------------------------------------------------------------
ID3D11Buffer * const Mesh_Component::GetIndexBuffer() {
  return m_indexbuffer;
}

//------------------------------------------------------------------------------
int Mesh_Component::GetIndexCount() {
  return m_index_count;
}

//---------------------------------------------------------------------------
ID3D11ShaderResourceView * const Mesh_Component::GetTexture() {
  return m_texture;
}

//---------------------------------------------------------------------------
void Mesh_Component::SetTexture(ID3D11ShaderResourceView * texture) {
  m_texture = texture;
}

//---------------------------------------------------------------------------
WCHAR * const Mesh_Component::GetTextureName() {
  return m_texture_name;
}

//---------------------------------------------------------------------------
void Mesh_Component::SetTextureName(WCHAR * texture_name) {
  m_texture_name = texture_name;
}


//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Mesh_Component::Init_Mesh() {
  Vertex_Type* vertices;
  unsigned long* indices;
  D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
  D3D11_SUBRESOURCE_DATA vertex_data, inde_data;

  // Set the number of vertices in the vertex array.
  m_vertex_count = 3;

  // Set the number of indices in the index array.
  m_index_count = 3;

  // Create the vertex array.
  vertices = new Vertex_Type[m_vertex_count];
  if (!vertices) {
    throw Tunnelour::Exceptions::init_error("Creating the vertex array Failed!");
  }

  // Create the index array.
  indices = new unsigned long[m_index_count];
  if (!indices) {
    throw Tunnelour::Exceptions::init_error("Creating the index array Failed!");
  }

  // Load the vertex array with data.
  vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
  vertices[0].texture  = D3DXVECTOR2(0.0f, 1.0f);

  vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
  vertices[1].texture  = D3DXVECTOR2(0.5f, 0.0f);

  vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
  vertices[2].texture  = D3DXVECTOR2(1.0f, 1.0f);

  // Load the index array with data.
  indices[0] = 0;  // Bottom left.
  indices[1] = 1;  // Top middle.
  indices[2] = 2;  // Bottom right.

  // Set up the description of the static vertex buffer.
  vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  vertex_buffer_desc.ByteWidth = sizeof(Vertex_Type) * m_vertex_count;
  vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertex_buffer_desc.CPUAccessFlags = 0;
  vertex_buffer_desc.MiscFlags = 0;
  vertex_buffer_desc.StructureByteStride = 0;

  // Give the subresource structure a pointer to the vertex data.
  vertex_data.pSysMem = vertices;
  vertex_data.SysMemPitch = 0;
  vertex_data.SysMemSlicePitch = 0;

  // Now create the vertex buffer.
  if (FAILED(m_d3d11device->CreateBuffer(&vertex_buffer_desc,
                                         &vertex_data,
                                         &m_vertexbuffer))) {
    throw Tunnelour::Exceptions::init_error("Creating the vertex buffer Failed!");
  }

  // Set up the description of the static index buffer.
  index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  index_buffer_desc.ByteWidth = sizeof(unsigned long) * m_index_count;
  index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  index_buffer_desc.CPUAccessFlags = 0;
  index_buffer_desc.MiscFlags = 0;
  index_buffer_desc.StructureByteStride = 0;

  // Give the subresource structure a pointer to the index data.
  inde_data.pSysMem = indices;
  inde_data.SysMemPitch = 0;
  inde_data.SysMemSlicePitch = 0;

  // Create the index buffer.
  if (FAILED(m_d3d11device->CreateBuffer(&index_buffer_desc,
                                         &inde_data,
                                         &m_indexbuffer))) {
    throw Tunnelour::Exceptions::init_error("Creating the index buffer Failed!");
  }

  // Release the arrays now that the vertex and index buffers have been created and loaded.
  delete [] vertices;
  vertices = 0;

  delete [] indices;
  indices = 0;

}

//------------------------------------------------------------------------------
void Mesh_Component::Init_Texture() {
  m_texture_name = L"resource/Seafloor_Texture_001.dds";

	// Load the texture in.
	if(FAILED(D3DX11CreateShaderResourceViewFromFile(m_d3d11device,
                                                   m_texture_name,
                                                   NULL,
                                                   NULL,
                                                   &m_texture,
                                                   NULL)))	{
		throw Tunnelour::Exceptions::init_error("D3DX11CreateShaderResourceViewFromFile Failed!");
	}


}

}  // namespace Tunnelour
