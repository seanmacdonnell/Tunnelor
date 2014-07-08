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

  m_state.state = "";
  m_state.direction = "";
  m_state.state_index = 0;

  m_last_state.state = "";
  m_last_state.direction = "";
  m_last_state.state_index = 0;

  m_last_rendered_state.state = "";
  m_last_rendered_state.direction = "";
  m_last_rendered_state.state_index = 0;
  
  m_type = "Avatar_Component";

  m_command.state = "";
  m_command.direction = "";
  m_command.state_index = 0;
}

//------------------------------------------------------------------------------
Avatar_Component::~Avatar_Component() {
  m_state.state = "";
  m_state.direction = "";
  m_state.state_index = 0;

  m_last_state.state = "";
  m_last_state.direction = "";
  m_last_state.state_index = 0;

  m_last_rendered_state.state = "";
  m_last_rendered_state.direction = "";
  m_last_rendered_state.state_index = 0;

  m_command.state = "";
  m_command.direction = "";
  m_command.state_index = 0;
}

//------------------------------------------------------------------------------
void Avatar_Component::Init() {
  Init_Frame();

  m_is_initialised = true;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_State Avatar_Component::GetState() {
  return m_state;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_State Avatar_Component::GetLastState() {
  return m_last_state;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_State Avatar_Component::GetLastRenderedState() {
  return m_last_rendered_state;
}

//---------------------------------------------------------------------------
void Avatar_Component::SetLastRenderedState(Avatar_Component::Avatar_State last_state) {
  m_last_rendered_state = last_state;
}

//---------------------------------------------------------------------------
void Avatar_Component::SetState(Avatar_Component::Avatar_State state) {
  m_last_state = m_state;
  m_state = state;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_State Avatar_Component::GetCommand() {
  return m_command;
}

//---------------------------------------------------------------------------
void Avatar_Component::SetCommand(Avatar_Component::Avatar_State command) {
  m_command = command;
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

  if (m_state.direction != "Right" && m_state.direction != "Left") {
    throw Tunnelour::Exceptions::init_error("No Direction State! " + GetState().state);
  }

  // Top left
  m_frame->vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
  if (m_state.direction.compare("Right") == 0) {
    m_frame->vertices[0].texture =  D3DXVECTOR2(m_texture->top_left_position.x / m_texture->texture_size.x ,
                                                m_texture->top_left_position.y / m_texture->texture_size.y);
  } else {
    // TOP Left NOW TOP Right
    m_frame->vertices[0].texture = D3DXVECTOR2((m_texture->tile_size.x + m_texture->top_left_position.x) / m_texture->texture_size.x,
                                                m_texture->top_left_position.y / m_texture->texture_size.y);
  }

  // Bottom right
  m_frame->vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
  if (m_state.direction.compare("Right") == 0) {
    m_frame->vertices[1].texture = D3DXVECTOR2((m_texture->tile_size.x + m_texture->top_left_position.x) / m_texture->texture_size.x,
                                               (m_texture->tile_size.y + m_texture->top_left_position.y) / m_texture->texture_size.y);
  } else {
    // BOTTOM Right NOW BOTTOM Left
    m_frame->vertices[1].texture = D3DXVECTOR2(m_texture->top_left_position.x / m_texture->texture_size.x,
                                              (m_texture->tile_size.y + m_texture->top_left_position.y) / m_texture->texture_size.y);
  }

  // Bottom left
  m_frame->vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
  if (m_state.direction.compare("Right") == 0) {
    m_frame->vertices[2].texture = D3DXVECTOR2(m_texture->top_left_position.x / m_texture->texture_size.x,
                                              (m_texture->tile_size.y + m_texture->top_left_position.y) / m_texture->texture_size.y);
  } else {
    // BOTTOM Left NOW BOTTOM Right
    m_frame->vertices[2].texture = D3DXVECTOR2((m_texture->tile_size.x + m_texture->top_left_position.x) / m_texture->texture_size.x,
                                               (m_texture->tile_size.y + m_texture->top_left_position.y) / m_texture->texture_size.y);
  }

  // Second triangle.
  // Top left
  m_frame->vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
  m_frame->vertices[3].texture = m_frame->vertices[0].texture;
  // Top right
  m_frame->vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
  if (m_state.direction.compare("Right") == 0) {
    m_frame->vertices[4].texture = D3DXVECTOR2((m_texture->tile_size.x + m_texture->top_left_position.x) / m_texture->texture_size.x,
                                                m_texture->top_left_position.y / m_texture->texture_size.y);
  } else {
    // TOP Right NOW TOP Left
    m_frame->vertices[4].texture =  D3DXVECTOR2(m_texture->top_left_position.x / m_texture->texture_size.x,
                                                m_texture->top_left_position.y / m_texture->texture_size.y);
  }
  // Bottom right
  m_frame->vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
  m_frame->vertices[5].texture = m_frame->vertices[1].texture;

  m_centre = D3DXVECTOR3(0, 0, 0);

  for (int i = 0; i < m_frame->vertex_count; i++) {
    m_centre.x += m_frame->vertices[i].position.x;
    m_centre.y += m_frame->vertices[i].position.y;
    m_centre.z += m_frame->vertices[i].position.z;
  }

  m_centre.x = m_centre.x / m_frame->vertex_count;
  m_centre.y = m_centre.y / m_frame->vertex_count;
  m_centre.z = m_centre.z / m_frame->vertex_count;

}

}  // namespace Tunnelour
