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

#include "Bitmap_Component.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Bitmap_Component::Bitmap_Component(): Frame_Component() {
  m_texture = new Tunnelour::Bitmap_Component::Texture();
  m_texture->texture = 0;
  m_texture->texture_path = L"";
  m_texture->transparency = 1.0f;
  m_type = "Bitmap_Component";
}

//------------------------------------------------------------------------------
Bitmap_Component::~Bitmap_Component()  {
  // Release the texture resource.
  if (m_texture->texture)  {
    m_texture->texture->Release();
    m_texture->texture = 0;
    delete m_texture;
  }
}

//------------------------------------------------------------------------------
void Bitmap_Component::Init(ID3D11Device * const d3d11device) {
  Frame_Component::Init(d3d11device);
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

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
