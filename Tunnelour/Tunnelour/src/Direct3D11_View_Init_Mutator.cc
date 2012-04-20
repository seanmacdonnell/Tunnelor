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

#include "Direct3D11_View_Init_Mutator.h"
#include "Background_Component.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View_Init_Mutator::Direct3D11_View_Init_Mutator() {
  m_back_buffer_color[0] = 0.0;
  m_back_buffer_color[1] = 0.0;
  m_back_buffer_color[2] = 0.0;
  m_back_buffer_color[3] = 1.0;
  m_found_back_buffer_color = false;
  m_camera_component = 0;
  m_found_camera = false;
  m_mesh_component = 0;
  m_found_mesh = false;
}

//------------------------------------------------------------------------------
Direct3D11_View_Init_Mutator::~Direct3D11_View_Init_Mutator() {
}

//------------------------------------------------------------------------------
void Direct3D11_View_Init_Mutator::Mutate(Tunnelour::Component *component) {
  Tunnelour::Background_Component *background = dynamic_cast<Tunnelour::Background_Component*>(component);
  if (background) {
    //Found Background_Component
    m_back_buffer_color[0] = background->GetRed();
    m_back_buffer_color[1] = background->GetGreen();
    m_back_buffer_color[2] = background->GetBlue();
    m_back_buffer_color[3] = background->GetAlpha();
    m_found_back_buffer_color = true;
  }

  Tunnelour::Camera_Component *camera = dynamic_cast<Tunnelour::Camera_Component*>(component);
  if (camera) {
    //Found Camera_Component
    m_camera_component = camera;
    m_found_camera = true;
  }

  Tunnelour::Mesh_Component *mesh = dynamic_cast<Tunnelour::Mesh_Component*>(component);
  if (mesh) {
    //Found Mesh_Component
    m_mesh_component = mesh;
    m_found_mesh = true;
  }
}

}  // namespace Tunnelour
