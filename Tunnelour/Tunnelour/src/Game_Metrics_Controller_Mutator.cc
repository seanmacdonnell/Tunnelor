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

#include "Game_Metrics_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Game_Metrics_Controller_Mutator::Game_Metrics_Controller_Mutator() {
  m_found_game_settings = false;
  m_found_avatar_component = false;
  m_found_world_settings = false;
  m_game_settings = 0;
  m_avatar_controller = 0;
  m_world_settings = 0;
}

//------------------------------------------------------------------------------
Game_Metrics_Controller_Mutator::~Game_Metrics_Controller_Mutator() {
  m_found_game_settings = false;
  m_found_avatar_component = false;
  m_found_world_settings = false;
  m_game_settings = 0;
  m_avatar_controller = 0;
  m_world_settings = 0;
}

//------------------------------------------------------------------------------
void Game_Metrics_Controller_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    m_game_settings = static_cast<Game_Settings_Component*>(component);
    m_found_game_settings = true;
  } else if (component->GetType().compare("Avatar_Component") == 0) {
    m_avatar_controller = static_cast<Avatar_Component*>(component);
    m_found_avatar_component = true;
  } else if (component->GetType().compare("World_Settings_Component") == 0) {
    m_world_settings = static_cast<World_Settings_Component*>(component);
    m_found_world_settings = true;
  }
}

//------------------------------------------------------------------------------
Game_Settings_Component* const Game_Metrics_Controller_Mutator::GetGameSettings() {
  return m_game_settings;
}

//------------------------------------------------------------------------------
Avatar_Component* const Game_Metrics_Controller_Mutator::GetAvatarComponent() {
  return m_avatar_controller;
}

//------------------------------------------------------------------------------
World_Settings_Component* const Game_Metrics_Controller_Mutator::GetWorldSettings() {
  return m_world_settings;
}

//------------------------------------------------------------------------------
bool Game_Metrics_Controller_Mutator::WasSuccessful() {
  return (m_found_game_settings &&
          m_found_avatar_component &&
          m_found_world_settings);
}
}  // namespace Tunnelour
