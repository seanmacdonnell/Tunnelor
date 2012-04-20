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
  m_vertexCount = 0;
  m_indexcount = 0;
}

//------------------------------------------------------------------------------
Mesh_Component::~Mesh_Component() {
 // Release the index buffer.
 if (m_indexbuffer)
 {
  m_indexbuffer->Release();
  m_indexbuffer = 0;
 }

 // Release the vertex buffer.
 if (m_vertexbuffer)
 {
  m_vertexbuffer->Release();
  m_vertexbuffer = 0;
 }
}

//------------------------------------------------------------------------------
void Mesh_Component::Init(ID3D11Device* d3d11device) {
 m_d3d11device = d3d11device;
 VertexType* vertices;
 unsigned long* indices;
 D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData, indexData;
 HRESULT result;

 // Set the number of vertices in the vertex array.
 m_vertexCount = 3;

 // Set the number of indices in the index array.
 m_indexcount = 3;

 // Create the vertex array.
 vertices = new VertexType[m_vertexCount];
 if (!vertices)
 {
    throw Tunnelour::Exceptions::initialising_error("Creating the vertex array Failed!");
 }

 // Create the index array.
 indices = new unsigned long[m_indexcount];
 if (!indices)
 {
  throw Tunnelour::Exceptions::initialising_error("Creating the index array Failed!");
 }

 // Load the vertex array with data.
 vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
 vertices[0].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

 vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
 vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

 vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
 vertices[2].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);

 // Load the index array with data.
 indices[0] = 0;  // Bottom left.
 indices[1] = 1;  // Top middle.
 indices[2] = 2;  // Bottom right.

 // Set up the description of the static vertex buffer.
 vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
 vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
 vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 vertexBufferDesc.CPUAccessFlags = 0;
 vertexBufferDesc.MiscFlags = 0;
 vertexBufferDesc.StructureByteStride = 0;

 // Give the subresource structure a pointer to the vertex data.
 vertexData.pSysMem = vertices;
 vertexData.SysMemPitch = 0;
 vertexData.SysMemSlicePitch = 0;

 // Now create the vertex buffer.
 result = m_d3d11device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexbuffer);
 if (FAILED(result))
 {
  throw Tunnelour::Exceptions::initialising_error("Creating the vertex buffer Failed!");
 }

 // Set up the description of the static index buffer.
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexcount;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;
 indexBufferDesc.StructureByteStride = 0;

 // Give the subresource structure a pointer to the index data.
  indexData.pSysMem = indices;
 indexData.SysMemPitch = 0;
 indexData.SysMemSlicePitch = 0;

 // Create the index buffer.
 result = m_d3d11device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexbuffer);
 if (FAILED(result))
 {
  throw Tunnelour::Exceptions::initialising_error("Creating the index buffer Failed!");
 }

 // Release the arrays now that the vertex and index buffers have been created and loaded.
 delete [] vertices;
 vertices = 0;

 delete [] indices;
 indices = 0;

 m_is_initialised = true;
}

//------------------------------------------------------------------------------
ID3D11Buffer* Mesh_Component::GetVertexBuffer() {
  return m_vertexbuffer;
}

//------------------------------------------------------------------------------
ID3D11Buffer* Mesh_Component::GetIndexBuffer() {
  return m_indexbuffer;
}

//------------------------------------------------------------------------------
int Mesh_Component::GetIndexCount() {
  return m_indexcount;
}


//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

} // namespace Tunnelour
