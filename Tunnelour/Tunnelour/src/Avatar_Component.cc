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

#include "Avatar_Component.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Avatar_Component::Avatar_Component(): Bitmap_Component() {
  m_position = D3DXVECTOR3(0, 0, 0);
  m_size = D3DXVECTOR2(0, 0);
  m_texture->texture_path = L"";
  m_state = "";
  m_state_index = 0;
  m_last_command = "";
  m_current_command = "";
  m_next_command = "";
}

//------------------------------------------------------------------------------
Avatar_Component::~Avatar_Component() {
}

//------------------------------------------------------------------------------
void Avatar_Component::Init() {
  Init_Frame();

  m_is_initialised = true;
}

//---------------------------------------------------------------------------
std::string Avatar_Component::GetState() {
  return m_state;
}

//---------------------------------------------------------------------------
void Avatar_Component::SetState(std::string state) {
  m_state = state;
}

//---------------------------------------------------------------------------
unsigned int Avatar_Component::GetStateIndex() {
  return m_state_index;
}

//---------------------------------------------------------------------------
void Avatar_Component::SetStateIndex(unsigned int state_index) {
  m_state_index = state_index;
}

//---------------------------------------------------------------------------
std::string Avatar_Component::GetLastCommand() {
  return m_last_command;
}

//---------------------------------------------------------------------------
void Avatar_Component::SetLastCommand(std::string last_command) {
  m_last_command = last_command;
}

//---------------------------------------------------------------------------
std::string Avatar_Component::GetCurrentCommand() {
  return m_current_command;
}

//---------------------------------------------------------------------------
void Avatar_Component::SetCurrentCommand(std::string current_command) {
  m_current_command = current_command;
}

//---------------------------------------------------------------------------
std::string Avatar_Component::GetNextCommand() {
  return m_next_command;
}

//---------------------------------------------------------------------------
void Avatar_Component::SetNextCommand(std::string next_command) {
  m_next_command = next_command;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Avatar_Component::Init_Frame() {
  float left, right, top, bottom;

  // Set the number of vertices in the vertex array.
  m_frame->vertex_count = 6;

  // Set the number of indices in the index array.
  m_frame->index_count = 6;

  // Create the vertex array.
  m_frame->vertices = new Vertex_Type[m_frame->vertex_count];
  if (!m_frame->vertices) {
    throw Tunnelour::Exceptions::init_error("Creating the vertex array Failed!");
  }

  // Create the index array.
  m_frame->indices = new unsigned int[m_frame->index_count];
  if (!m_frame->indices) {
    throw Tunnelour::Exceptions::init_error("Creating the index array Failed!");
  }

  // Initialize vertex array to zeros at first.
  memset(m_frame->vertices, 0, (sizeof(Vertex_Type) * m_frame->vertex_count));

  // Load the index array with data.
  for (int i = 0; i < m_frame->index_count; i++)  {
    m_frame->indices[i] = i;
  }

  // Calculate the screen coordinates of the left side of the bitmap.
  left = m_position.x - static_cast<float>(m_size.x / 2);

  // Calculate the screen coordinates of the right side of the bitmap.
  right = m_position.x + static_cast<float>(m_size.x / 2);

  // Calculate the screen coordinates of the top of the bitmap.
  top = m_position.y + static_cast<float>(m_size.x / 2);

  // Calculate the screen coordinates of the bottom of the bitmap.
  bottom = m_position.y - static_cast<float>(m_size.x / 2);

  // Load the vertex array with data
  // First triangle
  // Top left
  m_frame->vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
  m_frame->vertices[0].texture =  D3DXVECTOR2(m_texture->top_left_position.x / m_texture->texture_size.x,
                                              m_texture->top_left_position.y / m_texture->texture_size.y);

  // Bottom right
  m_frame->vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
  m_frame->vertices[1].texture = D3DXVECTOR2((m_texture->tile_size.x + m_texture->top_left_position.x) / m_texture->texture_size.x,
                                             (m_texture->tile_size.y + m_texture->top_left_position.y) / m_texture->texture_size.y);

  // Bottom left
  m_frame->vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
  m_frame->vertices[2].texture = D3DXVECTOR2(m_texture->top_left_position.x / m_texture->texture_size.x,
                                            (m_texture->tile_size.y + m_texture->top_left_position.y) / m_texture->texture_size.y);

  // Second triangle.
  // Top left
  m_frame->vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
  m_frame->vertices[3].texture = m_frame->vertices[0].texture;
  // Top right
  m_frame->vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
  m_frame->vertices[4].texture = D3DXVECTOR2((m_texture->tile_size.x + m_texture->top_left_position.x) / m_texture->texture_size.x,
                                              m_texture->top_left_position.y / m_texture->texture_size.y);
  // Bottom right
  m_frame->vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
  m_frame->vertices[5].texture = m_frame->vertices[1].texture;
}
}  // namespace Tunnelour
