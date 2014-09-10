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

#include "Bitmap_Component.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Bitmap_Component::Bitmap_Component(): Frame_Component() {
  m_texture = new Tunnelour::Bitmap_Component::Texture();
  m_texture->texture = 0;
  m_texture->texture_path;
  m_texture->transparency = 1.0f;
  m_texture->top_left_position = D3DXVECTOR2(0, 0);
  m_texture->tile_size = D3DXVECTOR2(0, 0);
  m_texture->texture_size = D3DXVECTOR2(0, 0);
  m_type = "Bitmap_Component";
  m_velocity = D3DXVECTOR3(0.0, 0.0 , 0.0);
  m_angle = 0;
}

//------------------------------------------------------------------------------
Bitmap_Component::~Bitmap_Component()  {
  if (m_texture != 0) {
    // texture is released by the view.
    m_texture->texture = 0;
    m_texture->texture_path;
    m_texture->transparency = 0.0f;
    m_texture->top_left_position = D3DXVECTOR2(0, 0);
    m_texture->tile_size = D3DXVECTOR2(0, 0);
    m_texture->texture_size = D3DXVECTOR2(0, 0);
    m_velocity = D3DXVECTOR3(0, 0 ,0);
    m_angle = 0;

    delete m_texture;
    m_texture = 0;
  }
}

//------------------------------------------------------------------------------
void Bitmap_Component::Init() {
  m_is_initialised = true;
}

//---------------------------------------------------------------------------
Tunnelour::Bitmap_Component::Texture * const Bitmap_Component::GetTexture() {
  return m_texture;
}

//---------------------------------------------------------------------------
void Bitmap_Component::SetTexture(Tunnelour::Bitmap_Component::Texture texture) {
  delete m_texture;
  *m_texture = texture;
}

//---------------------------------------------------------------------------
D3DXVECTOR3 Bitmap_Component::GetVelocity() {
  return m_velocity;
}

//---------------------------------------------------------------------------
void Bitmap_Component::SetVelocity(D3DXVECTOR3 velocity) {
  m_velocity = velocity;
}

//---------------------------------------------------------------------------
float Bitmap_Component::GetAngle() {
  return m_angle;
}

//---------------------------------------------------------------------------
void Bitmap_Component::SetAngle(float angle) {
  m_angle = angle;
}

//---------------------------------------------------------------------------
float Bitmap_Component::GetTransparency() {
  return m_texture->transparency;
}

//---------------------------------------------------------------------------
void Bitmap_Component::SetTransparency(float transparency) {
  m_texture->transparency = transparency;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
