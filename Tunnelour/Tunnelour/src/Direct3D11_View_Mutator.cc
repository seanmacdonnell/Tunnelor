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

#include "Direct3D11_View_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Direct3D11_View_Mutator::Direct3D11_View_Mutator() {
  m_camera = 0;
  m_found_camera = false;
  m_game_metrics = 0;
  m_found_game_metics = false;
  m_avatar = 0;
  m_found_avatar = false;
  m_game_settings = 0;
  m_found_game_settings = false;
}

//------------------------------------------------------------------------------
Direct3D11_View_Mutator::~Direct3D11_View_Mutator() {
  m_camera = 0;
  m_found_camera = false;
  m_game_metrics = 0;
  m_found_game_metics = false;
  m_avatar = 0;
  m_found_avatar = false;
  m_game_settings = 0;
  m_found_game_settings = false;
}

//------------------------------------------------------------------------------
void Direct3D11_View_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Camera_Component") == 0) {
    m_camera = static_cast<Tunnelour::Camera_Component*>(component);
    m_found_camera = true;
  } else if (component->GetType().compare("Avatar_Component") == 0) {
    m_avatar = static_cast<Tunnelour::Avatar_Component*>(component);
    m_found_avatar = true;
  } if (component->GetType().compare("Game_Settings_Component") == 0) {
    m_game_settings = static_cast<Tunnelour::Game_Settings_Component*>(component);
    m_found_game_settings = true;
  } if (component->GetType().compare("Game_Metrics_Component") == 0) {
    m_game_metrics = static_cast<Tunnelour::Game_Metrics_Component*>(component);
    m_found_game_metics = true;
  }
}

//------------------------------------------------------------------------------
bool Direct3D11_View_Mutator::WasSuccessful() {
  return m_found_camera && m_found_game_settings;
}

//------------------------------------------------------------------------------
Tunnelour::Camera_Component* const Direct3D11_View_Mutator::GetCamera() {
  return m_camera;
}

//------------------------------------------------------------------------------
Tunnelour::Game_Settings_Component* const Direct3D11_View_Mutator::GetGameSettings() {
  return m_game_settings;
}

//------------------------------------------------------------------------------
Tunnelour::Game_Metrics_Component* const Direct3D11_View_Mutator::GetGameMetrics() {
  return m_game_metrics;
}

//------------------------------------------------------------------------------
Tunnelour::Avatar_Component* const Direct3D11_View_Mutator::GetAvatar() {
  return m_avatar;
}

}  // namespace Tunnelour
