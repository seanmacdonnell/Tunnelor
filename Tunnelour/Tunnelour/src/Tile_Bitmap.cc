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

#include "Tile_Bitmap.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Tile_Bitmap::Tile_Bitmap(): Bitmap_Component() {
  m_position = D3DXVECTOR3(0, 0, 0);
  m_size = D3DXVECTOR2(0, 0);
  m_texture->texture_path = L"";
  m_is_floor = false;
  m_is_top_edge = false;
  m_is_bottom_edge = false;
  m_is_right_edge = false;
  m_is_left_edge = false;
}

//------------------------------------------------------------------------------
Tile_Bitmap::~Tile_Bitmap() {
}

//------------------------------------------------------------------------------
void Tile_Bitmap::Init() {
  Init_Frame();

  m_is_initialised = true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Tile_Bitmap::Init_Frame() {
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

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsFloor() {
  return m_is_floor;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsFloor(bool is_floor) {
  m_is_floor = is_floor;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsRoof() {
  return m_is_roof;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsRoof(bool is_roof) {
  m_is_roof = is_roof;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsWall() {
  return m_is_wall;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsWall(bool is_wall) {
  m_is_wall = is_wall;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsTopEdge() {
  return m_is_top_edge;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsTopEdge(bool is_top_edge) {
  m_is_top_edge = is_top_edge;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsBottomEdge() {
  return m_is_bottom_edge;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsBottomEdge(bool is_bottom_edge) {
  m_is_bottom_edge = is_bottom_edge;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsRightEdge() {
  return m_is_right_edge;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsRightEdge(bool is_right_edge) {
  m_is_right_edge = is_right_edge;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsLeftEdge() {
  return m_is_left_edge;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsLeftEdge(bool is_left_edge) {
  m_is_left_edge = is_left_edge;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsEdge() {
  return IsTopEdge() || IsBottomEdge() || IsRightEdge() || IsLeftEdge() ||
         IsFloor() || IsRoof() || IsWall();
}
}  // namespace Tunnelour
