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

#include "Level_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Level_Controller_Mutator::Level_Controller_Mutator() {
  m_found_game_settings = false;
  m_found_camera = false;
  m_game_settings = 0;
  m_camera = 0;
}

//------------------------------------------------------------------------------
Level_Controller_Mutator::~Level_Controller_Mutator() {
  m_found_game_settings = false;
  m_found_camera = false;
  m_game_settings = 0;
  m_camera = 0;
}

//------------------------------------------------------------------------------
void Level_Controller_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    m_game_settings = static_cast<Game_Settings_Component*>(component);
    m_found_game_settings = true;
  } else if (component->GetType().compare("Camera_Component") == 0) {
    m_camera = static_cast<Camera_Component*>(component);
    m_found_camera = true;
  }
}

//------------------------------------------------------------------------------
Game_Settings_Component* const Level_Controller_Mutator::GetGameSettings() {
  return m_game_settings;
}

//------------------------------------------------------------------------------
Camera_Component* const Level_Controller_Mutator::GetCamera() {
  return m_camera;
}

//------------------------------------------------------------------------------
bool Level_Controller_Mutator::WasSuccessful() {
  return m_found_game_settings && m_found_camera;
}
}  // namespace Tunnelour
