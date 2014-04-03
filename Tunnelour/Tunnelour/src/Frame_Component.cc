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
void Frame_Component::Init() {
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
D3DXVECTOR3 const Frame_Component::GetLastPosition() {
  return m_last_position;
}

//---------------------------------------------------------------------------
void Frame_Component::SetPosition(D3DXVECTOR3 position) {
  m_last_position = m_position;
  m_position = position;
  Notify();
}

//---------------------------------------------------------------------------
void Frame_Component::SetPosition(float x, float y, float z) {
  m_position = D3DXVECTOR3(x, y, z);
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
D3DXVECTOR2 Frame_Component::GetSize() {
  D3DXVECTOR3 modified_size;
  modified_size.x = m_size.x * m_scale.x;
  modified_size.y = m_size.y * m_scale.y;
  return modified_size;
}

//---------------------------------------------------------------------------
void Frame_Component::SetSize(D3DXVECTOR2 size) {
  m_size = size;
}

//---------------------------------------------------------------------------
void Frame_Component::SetSize(const float size_x, const float size_y) {
  m_size.x = size_x;
  m_size.y = size_y;
}

//---------------------------------------------------------------------------
D3DXVECTOR3 * const Frame_Component::GetFrameCentre() {
  return &m_centre;
}


//---------------------------------------------------------------------------
D3DXVECTOR3 Frame_Component::GetTopLeftPostion() {
  D3DXVECTOR3 top_left_position;
  top_left_position.x = (m_position.x - ((m_size.x * m_scale.x) / 2));
  top_left_position.y = (m_position.y + ((m_size.y * m_scale.y) / 2));
  top_left_position.z = m_position.z;

  return top_left_position;
}


//---------------------------------------------------------------------------
D3DXVECTOR3 Frame_Component::GetBottomRightPostion() {
  D3DXVECTOR3 bottom_right_position;
  bottom_right_position.x = (m_position.x + ((m_size.x * m_scale.x) / 2));
  bottom_right_position.y = (m_position.y - ((m_size.y * m_scale.y) / 2));
  bottom_right_position.z = m_position.z;

  return bottom_right_position;
}

//---------------------------------------------------------------------------
void Frame_Component::SetBottomPostion(float bottom) {
  float new_x = bottom - ((m_size.x * m_scale.x) / 2);
  SetPosition(new_x, m_position.y, m_position.z);
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
