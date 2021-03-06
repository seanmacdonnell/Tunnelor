//  Copyright 2014 Sean MacDonnell
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
  m_is_floor = false;
  m_is_right_wall = false;
  m_is_left_wall = false;
  m_is_roof = false;
  m_is_top_edge = false;
  m_is_bottom_edge = false;
  m_is_right_edge = false;
  m_is_left_edge = false;
  m_is_collidable = false;

  m_is_right_floor_end = false;
  m_is_left_floor_end = false;
  m_is_right_roof_end = false;
  m_is_left_roof_end = false;
  m_is_top_right_wall_end = false;
  m_is_bot_right_wall_end = false;
  m_is_top_left_wall_end = false;
  m_is_bot_left_wall_end = false;

  m_is_left_exit = false;
  m_is_right_exit = false;

  m_is_middleground = false;
  m_is_background = false;
}

//------------------------------------------------------------------------------
Tile_Bitmap::~Tile_Bitmap() {
  m_is_floor = false;
  m_is_right_wall = false;
  m_is_left_wall = false;
  m_is_roof = false;
  m_is_top_edge = false;
  m_is_bottom_edge = false;
  m_is_right_edge = false;
  m_is_left_edge = false;
  m_is_collidable = false;

  m_is_middleground = false;
  m_is_background = false;
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
  m_frame->vertices[0].texture =  D3DXVECTOR2((m_texture->top_left_position.x / m_texture->texture_size.x),
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

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsFloor() {
  return m_is_floor;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsFloor(bool is_floor) {
  m_is_floor = is_floor;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsRightFloorEnd() {
  return m_is_right_floor_end;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsRightFloorEnd(bool is_right_floor_end) {
  m_is_right_floor_end = is_right_floor_end;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsLeftFloorEnd() {
  return m_is_left_floor_end;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsLeftFloorEnd(bool is_left_floor_end) {
  m_is_left_floor_end = is_left_floor_end;
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
bool Tile_Bitmap::IsRightRoofEnd() {
  return m_is_right_roof_end;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsRightRoofEnd(bool is_right_roof_end) {
  m_is_right_roof_end = is_right_roof_end;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsLeftRoofEnd() {
  return m_is_left_roof_end;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsLeftRoofEnd(bool is_left_roof_end) {
  m_is_left_roof_end = is_left_roof_end;
}


//------------------------------------------------------------------------------
bool Tile_Bitmap::IsWall() {
  if (m_is_right_wall) {
    return true;
  } else if (m_is_left_wall) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsRightWall() {
  return m_is_right_wall;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsRightWall(bool is_wall) {
  m_is_right_wall = is_wall;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsTopRightWallEnd() {
  return m_is_top_right_wall_end;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsTopRightWallEnd(bool is_top_right_wall_end) {
  m_is_top_right_wall_end = is_top_right_wall_end;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsBotRightWallEnd() {
  return m_is_bot_right_wall_end;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsBotRightWallEnd(bool is_bot_right_wall_end) {
  m_is_bot_right_wall_end = is_bot_right_wall_end;
}


//------------------------------------------------------------------------------
bool Tile_Bitmap::IsLeftWall() {
  return m_is_left_wall;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsLeftWall(bool is_wall) {
  m_is_left_wall = is_wall;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsTopLeftWallEnd() {
  return m_is_top_left_wall_end;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsTopLeftWallEnd(bool is_top_left_wall_end) {
  m_is_top_left_wall_end = is_top_left_wall_end;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsBotLeftWallEnd() {
  return m_is_bot_left_wall_end;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsBotLeftWallEnd(bool is_bot_left_wall_end) {
  m_is_bot_left_wall_end = is_bot_left_wall_end;
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
bool Tile_Bitmap::IsCollidable() {
  return m_is_collidable;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsCollidable(bool is_collidable) {
  m_is_collidable = is_collidable;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsEdge() {
  return IsTopEdge() || IsBottomEdge() || IsRightEdge() || IsLeftEdge() ||
         IsFloor() || IsRoof() || IsWall();
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsLeftExit() {
  return m_is_left_exit;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsLeftExit(bool is_left_exit) {
  m_is_left_exit = is_left_exit;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsRightExit() {
  return m_is_right_exit;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsRightExit(bool is_right_exit) {
  m_is_right_exit = is_right_exit;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsMiddleground() {
  return m_is_middleground;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsMiddleground(bool is_middleground) {
  m_is_middleground = is_middleground;
}

//------------------------------------------------------------------------------
bool Tile_Bitmap::IsBackground() {
  return m_is_background;
}

//------------------------------------------------------------------------------
void Tile_Bitmap::SetIsBackground(bool is_background) {
  m_is_background = is_background;
}

}  // namespace Tunnelour
