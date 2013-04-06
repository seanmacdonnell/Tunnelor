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

#include "Avatar_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Avatar_Controller_Mutator::Avatar_Controller_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
  m_found_avatar_component = true;
  m_avatar_component = 0;
}

//------------------------------------------------------------------------------
Avatar_Controller_Mutator::~Avatar_Controller_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
}

//------------------------------------------------------------------------------
void Avatar_Controller_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    // Found Game Settings
    Tunnelour::Game_Settings_Component *game_settings = 0;
    game_settings = static_cast<Tunnelour::Game_Settings_Component*>(component);
    m_game_settings = game_settings;
    m_found_game_settings = true;
  } else {
    if (!m_found_avatar_component) {
      m_avatar_component = dynamic_cast<Avatar_Component*>(component);
      if (m_avatar_component != 0) {
        m_found_avatar_component = true;
      }
    }
  }
}

//------------------------------------------------------------------------------
bool Avatar_Controller_Mutator::FoundGameSettings() {
  return m_found_game_settings;
}

//------------------------------------------------------------------------------
Tunnelour::Game_Settings_Component* const Avatar_Controller_Mutator::GetGameSettings() {
  return m_game_settings;
}


//------------------------------------------------------------------------------
bool Avatar_Controller_Mutator::FoundAvatarComponent() {
  return m_found_avatar_component;
}

//------------------------------------------------------------------------------
Tunnelour::Avatar_Component* const Avatar_Controller_Mutator::GetAvatarComponent() {
  return m_avatar_component;
}

}  // namespace Tunnelour