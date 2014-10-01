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

#include "Get_Game_Settings_Component_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Get_Game_Settings_Component_Mutator::Get_Game_Settings_Component_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
}

//------------------------------------------------------------------------------
Get_Game_Settings_Component_Mutator::~Get_Game_Settings_Component_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
}

//------------------------------------------------------------------------------
void Get_Game_Settings_Component_Mutator::Mutate(Component *const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    m_game_settings = static_cast<Game_Settings_Component*>(component);
    m_found_game_settings = true;
  }
}

//------------------------------------------------------------------------------
Game_Settings_Component *const Get_Game_Settings_Component_Mutator::GetGameSettings() {
  return m_game_settings;
}

//------------------------------------------------------------------------------
bool Get_Game_Settings_Component_Mutator::WasSuccessful() {
  return  m_found_game_settings;
}


}  // namespace Tunnelour
