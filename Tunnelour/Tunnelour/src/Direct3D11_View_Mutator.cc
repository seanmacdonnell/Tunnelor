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
  m_camera = 0;
  m_found_camera = false;
  m_found_renderables = false;
}

//------------------------------------------------------------------------------
Direct3D11_View_Mutator::~Direct3D11_View_Mutator() {
  m_camera = 0;
  m_found_camera = false;
  m_found_renderables = false;
}

//------------------------------------------------------------------------------
void Direct3D11_View_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Camera_Component") == 0) {
    // Found Camera_Component
    Tunnelour::Camera_Component *camera = 0;
    camera = static_cast<Tunnelour::Camera_Component*>(component);
    m_camera = camera;
    m_found_camera = true;
  }

  if (component->GetType().compare("Bitmap_Component") == 0) {
    // Found Bitmap_Component
    Tunnelour::Bitmap_Component *bitmap = 0;
    bitmap = static_cast<Tunnelour::Bitmap_Component*>(component);
    if (bitmap->GetPosition()->z > -1) {
      m_renderables.Layer_00.push_back(bitmap);
    } 
    if (bitmap->GetPosition()->z <= -1 && bitmap->GetPosition()->z > -2) {
      m_renderables.Layer_01.push_back(bitmap);
    }
    if (bitmap->GetPosition()->z <= -2) {
      m_renderables.Layer_02.push_back(bitmap);
    }
    m_found_renderables = true;
  }

  if (component->GetType().compare("Text_Component") == 0) {
    // Found Text_Component
    Tunnelour::Text_Component *text = 0;
    text = static_cast<Tunnelour::Text_Component*>(component);
    if (text->GetPosition()->z > -1) {
      m_renderables.Layer_00.push_back(text);
    } 
    if (text->GetPosition()->z <= -1 && text->GetPosition()->z > -2) {
      m_renderables.Layer_01.push_back(text);
    }
    if (text->GetPosition()->z <= -2) {
      m_renderables.Layer_02.push_back(text);
    }
    m_found_renderables = true;
  }
  
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    // Found Game Settings
    Tunnelour::Game_Settings_Component *game_settings = 0;
    game_settings = static_cast<Tunnelour::Game_Settings_Component*>(component);
    m_game_settings = game_settings;
    m_found_game_settings = true;
  }
  
}

//------------------------------------------------------------------------------
bool Direct3D11_View_Mutator::FoundCamera() {
  return m_found_camera;
}

//------------------------------------------------------------------------------
Tunnelour::Camera_Component* const Direct3D11_View_Mutator::GetCamera() {
  return m_camera;
}

//------------------------------------------------------------------------------
bool Direct3D11_View_Mutator::FoundRenderables() {
  return m_found_renderables;
}

//------------------------------------------------------------------------------
Direct3D11_View_Mutator::Renderables& const Direct3D11_View_Mutator::GetRenderables() {
  return m_renderables;
}

//------------------------------------------------------------------------------
bool Direct3D11_View_Mutator::FoundGameSettings() {
  return m_found_game_settings;
}

//------------------------------------------------------------------------------
Tunnelour::Game_Settings_Component* const Direct3D11_View_Mutator::GetGameSettings() {
  return m_game_settings;
}

}  // namespace Tunnelour
