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

#include "Direct3D11_View_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View_Mutator::Direct3D11_View_Mutator() {
  m_background = 0;
  m_found_background = false;
  m_camera = 0;
  m_found_camera = false;
  m_found_mesh = false;
  m_bitmap = 0;
  m_found_bitmap = false;
}

//------------------------------------------------------------------------------
Direct3D11_View_Mutator::~Direct3D11_View_Mutator() {
  m_background = 0;
  m_found_background = false;
  m_camera = 0;
  m_found_camera = false;
  m_found_mesh = false;
  m_bitmap = 0;
  m_found_bitmap = false;
}

//------------------------------------------------------------------------------
void Direct3D11_View_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Background_Component") == 0) {
    // Found Background_Component
    Tunnelour::Background_Component * background = 0;
    background = static_cast<Tunnelour::Background_Component*>(component);
    m_background = background;
    m_found_background = true;
  }

  if (component->GetType().compare("Camera_Component") == 0) {
    // Found Camera_Component
    Tunnelour::Camera_Component *camera = 0;
    camera = static_cast<Tunnelour::Camera_Component*>(component);
    m_camera = camera;
    m_found_camera = true;
  }

  if (component->GetType().compare("Bitmap_Component") == 0) {
    // Found Mesh_Component
    Tunnelour::Bitmap_Component *bitmap = 0;
    bitmap = static_cast<Tunnelour::Bitmap_Component*>(component);
    m_bitmap = bitmap;
    m_found_bitmap = true;
  }
}

//------------------------------------------------------------------------------
bool Direct3D11_View_Mutator::FoundBackground() {
  return m_found_background;
}

//------------------------------------------------------------------------------
Tunnelour::Background_Component * const Direct3D11_View_Mutator::GetBackground() {
  return m_background;
}

//------------------------------------------------------------------------------
bool Direct3D11_View_Mutator::FoundCamera() {
  return m_found_camera;
}

//------------------------------------------------------------------------------
Tunnelour::Camera_Component * const Direct3D11_View_Mutator::GetCamera() {
  return m_camera;
}

//------------------------------------------------------------------------------
bool Direct3D11_View_Mutator::FoundBitmap() {
  return m_found_bitmap;
}

//------------------------------------------------------------------------------
Tunnelour::Bitmap_Component * const Direct3D11_View_Mutator::GetBitmap() {
  return m_bitmap;
}


}  // namespace Tunnelour
