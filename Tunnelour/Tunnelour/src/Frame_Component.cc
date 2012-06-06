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

#include "Frame_Component.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Frame_Component::Frame_Component(): Component() {
  m_d3d11device = 0;

  m_frame = new Tunnelour::Frame_Component::Frame();
  m_frame->vertex_buffer = 0;
  m_frame->vertex_count = 0;
  m_frame->index_buffer = 0;
  m_frame->index_count = 0;

  m_position = D3DXVECTOR2(0, 0);
  m_size = D3DXVECTOR2(0, 0);

  m_type = "Frame_Component";
}

//------------------------------------------------------------------------------
Frame_Component::~Frame_Component() {
  // Release the index buffer.
  if (m_frame->index_buffer) {
    m_frame->index_buffer->Release();
    m_frame->index_buffer = 0;
  }

  // Release the vertex buffer.
  if (m_frame->vertex_buffer) {
    m_frame->vertex_buffer->Release();
    m_frame->vertex_buffer = 0;
  }
}

//------------------------------------------------------------------------------
void Frame_Component::Init(ID3D11Device * const d3d11device) {
  m_d3d11device = d3d11device;

  float left, right, top, bottom;
  Vertex_Type* vertices;
  unsigned int* indices;
  D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData, indexData;

  // Set the number of vertices in the vertex array.
  m_frame->vertex_count = 6;

  // Set the number of indices in the index array.
  m_frame->index_count = 6;

  // Create the vertex array.
  vertices = new Vertex_Type[m_frame->vertex_count];
  if (!vertices) {
    throw Tunnelour::Exceptions::init_error("Creating the vertex array Failed!");
  }

  // Create the index array.
  indices = new unsigned int[m_frame->index_count];
  if (!indices) {
    throw Tunnelour::Exceptions::init_error("Creating the index array Failed!");
  }

  // Initialize vertex array to zeros at first.
  memset(vertices, 0, (sizeof(Vertex_Type) * m_frame->vertex_count));

  // Load the index array with data.
  for (int i = 0; i < m_frame->index_count; i++)  {
    indices[i] = i;
  }

  // Calculate the screen coordinates of the left side of the bitmap.
  left = m_position.x - static_cast<float>(m_size.x / 2);

  // Calculate the screen coordinates of the right side of the bitmap.
  right = m_position.x + static_cast<float>(m_size.x / 2);

  // Calculate the screen coordinates of the top of the bitmap.
  top = m_position.y + static_cast<float>(m_size.x / 2);

  // Calculate the screen coordinates of the bottom of the bitmap.
  bottom = m_position.y - static_cast<float>(m_size.x / 2);

  // Load the vertex array with data.
  // First triangle.
  vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
  vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

  vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
  vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

  vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
  vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

  // Second triangle.
  vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
  vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

  vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
  vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

  vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
  vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

  // Set up the description of the static vertex buffer.
  vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  vertexBufferDesc.ByteWidth = sizeof(Vertex_Type) * m_frame->vertex_count;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  // Give the subresource structure a pointer to the vertex data.
  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  // Now create the vertex buffer.
  if (FAILED(m_d3d11device->CreateBuffer(&vertexBufferDesc,
                                         &vertexData,
                                         &(m_frame->vertex_buffer)))) {
    throw Tunnelour::Exceptions::init_error("CreateBuffer (vertex_buffer) Failed!");
  }

  // Set up the description of the static index buffer.
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_frame->index_count;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;
  indexBufferDesc.StructureByteStride = 0;

  // Give the subresource structure a pointer to the index data.
  indexData.pSysMem = indices;
  indexData.SysMemPitch = 0;
  indexData.SysMemSlicePitch = 0;

  // Create the index buffer.
  if (FAILED(m_d3d11device->CreateBuffer(&indexBufferDesc,
                                         &indexData,
                                         &(m_frame->index_buffer)))) {
    throw Tunnelour::Exceptions::init_error("CreateBuffer (index_buffer) Failed!");
  }

  // Release the arrays now that the vertex and index buffers have been created and loaded.
  delete [] vertices;
  vertices = 0;

  delete [] indices;
  indices = 0;

  m_is_initialised = true;
}

//---------------------------------------------------------------------------
Tunnelour::Frame_Component::Frame * const Frame_Component::GetFrame() {
  return m_frame;
}

//---------------------------------------------------------------------------
void Frame_Component::SetFrame(Tunnelour::Frame_Component::Frame * frame) {
  m_frame = frame;
}

//---------------------------------------------------------------------------
D3DXVECTOR2 * const Frame_Component::GetPosition() {
  return &m_position;
}

//---------------------------------------------------------------------------
void Frame_Component::SetPosition(D3DXVECTOR2 * const position) {
  m_position = *position;
}

//---------------------------------------------------------------------------
D3DXVECTOR2 * const Frame_Component::GetSize() {
  return &m_size;
}

//---------------------------------------------------------------------------
void Frame_Component::SetSize(D3DXVECTOR2 * const size) {
  m_size = *size;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
