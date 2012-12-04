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
  m_frame->vertices = 0;
  m_frame->index_buffer = 0;
  m_frame->index_count = 0;
  m_frame->indices = 0;

  m_position = D3DXVECTOR3(0, 0, 0);
  m_scale = D3DXVECTOR3(1, 1, 1);
  m_size = D3DXVECTOR2(0, 0);

  m_type = "Frame_Component";
}

//------------------------------------------------------------------------------
Frame_Component::~Frame_Component() {
  if (m_frame->indices) {
    delete m_frame->indices;
  }

  // Release the index buffer.
  if (m_frame->index_buffer) {
    m_frame->index_buffer->Release();
    m_frame->index_buffer = 0;
  }

  if (m_frame->vertices) {
    delete m_frame->vertices;
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
D3DXVECTOR3 * const Frame_Component::GetPosition() {
  return &m_position;
}

//---------------------------------------------------------------------------
void Frame_Component::SetPosition(D3DXVECTOR3 * const position) {
  m_position = *position;
}

//---------------------------------------------------------------------------
D3DXVECTOR3 * const Frame_Component::GetScale() {
  return &m_scale;
}

//---------------------------------------------------------------------------
void Frame_Component::SetScale(D3DXVECTOR3 * const scale) {
  m_scale = *scale;
}

//---------------------------------------------------------------------------
D3DXVECTOR2 * const Frame_Component::GetSize() {
  return &m_size;
}

//---------------------------------------------------------------------------
void Frame_Component::SetSize(D3DXVECTOR2 * const size) {
  m_size = *size;
}

//---------------------------------------------------------------------------
D3DXVECTOR3 const Frame_Component::GetCentre() {
  D3DXVECTOR3 centre = D3DXVECTOR3(0, 0, 0);

  for (int i = 0; i < m_frame->vertex_count; i++) {
    centre.x += m_frame->vertices[i].position.x;
    centre.y += m_frame->vertices[i].position.y;
    centre.z += m_frame->vertices[i].position.z;
  }

  centre.x = centre.x / m_frame->vertex_count;
  centre.y = centre.y / m_frame->vertex_count;
  centre.z = centre.z / m_frame->vertex_count;

  return centre;
}


//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
