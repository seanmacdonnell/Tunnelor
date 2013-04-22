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

#include "Debug_Data_Display_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Debug_Data_Display_Controller_Mutator::Debug_Data_Display_Controller_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
  m_found_avatar_component = false;
  m_avatar_component = 0;
  m_found_camera = false;
  m_camera = 0;
}

//------------------------------------------------------------------------------
Debug_Data_Display_Controller_Mutator::~Debug_Data_Display_Controller_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
}

//------------------------------------------------------------------------------
void Debug_Data_Display_Controller_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    // Found Game Settings
    Tunnelour::Game_Settings_Component *game_settings = 0;
    game_settings = static_cast<Tunnelour::Game_Settings_Component*>(component);
    m_game_settings = game_settings;
    m_found_game_settings = true;
  } else if (component->GetType().compare("Bitmap_Component") == 0) {
    if (!m_found_avatar_component) {
      m_avatar_component = dynamic_cast<Avatar_Component*>(component);
      if (m_avatar_component != 0) {
        m_found_avatar_component = true;
      }
    }
  } else  if (component->GetType().compare("Camera_Component") == 0) {
    // Found Camera_Component
    Tunnelour::Camera_Component *camera = 0;
    camera = static_cast<Tunnelour::Camera_Component*>(component);
    m_camera = camera;
    m_found_camera = true;
  }
}

//------------------------------------------------------------------------------
bool Debug_Data_Display_Controller_Mutator::FoundGameSettings() {
  return m_found_game_settings;
}

//------------------------------------------------------------------------------
Tunnelour::Game_Settings_Component* const Debug_Data_Display_Controller_Mutator::GetGameSettings() {
  return m_game_settings;
}


//------------------------------------------------------------------------------
bool Debug_Data_Display_Controller_Mutator::FoundAvatarComponent() {
  return m_found_avatar_component;
}

//------------------------------------------------------------------------------
Tunnelour::Avatar_Component* const Debug_Data_Display_Controller_Mutator::GetAvatarComponent() {
  return m_avatar_component;
}

//------------------------------------------------------------------------------
bool Debug_Data_Display_Controller_Mutator::FoundCamera() {
  return m_found_camera;
}

//------------------------------------------------------------------------------
Tunnelour::Camera_Component* const Debug_Data_Display_Controller_Mutator::GetCamera() {
  return m_camera;
}


}  // namespace Tunnelour
