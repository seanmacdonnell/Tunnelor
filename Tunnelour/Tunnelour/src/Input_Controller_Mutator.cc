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

#include "Input_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Input_Controller_Mutator::Input_Controller_Mutator() {
  m_found_game_settings = false;
  m_found_avatar_component = false;
  m_game_settings = 0;
  m_avatar_controller = 0;
}

//------------------------------------------------------------------------------
Input_Controller_Mutator::~Input_Controller_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
}

//------------------------------------------------------------------------------
void Input_Controller_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    // Found Game Settings
    Tunnelour::Game_Settings_Component *game_settings = 0;
    game_settings = static_cast<Tunnelour::Game_Settings_Component*>(component);
    m_game_settings = game_settings;
    m_found_game_settings = true;
  } else if (component->GetType().compare("Avatar_Component") == 0) {
    if (!m_found_avatar_component)  {
      m_avatar_controller = static_cast<Tunnelour::Avatar_Component*>(component);
      m_found_avatar_component = true;
    }
  }
}

//------------------------------------------------------------------------------
bool Input_Controller_Mutator::FoundGameSettings() {
  return m_found_game_settings;
}

//------------------------------------------------------------------------------
Tunnelour::Game_Settings_Component* const Input_Controller_Mutator::GetGameSettings() {
  return m_game_settings;
}

//------------------------------------------------------------------------------
bool Input_Controller_Mutator::FoundAvatarComponent() {
  return m_found_avatar_component;
}

//------------------------------------------------------------------------------
Tunnelour::Avatar_Component* const Input_Controller_Mutator::GetAvatarComponent() {
  return m_avatar_controller;
}

}  // namespace Tunnelour
