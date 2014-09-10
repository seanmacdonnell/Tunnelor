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

#include "Score_Display_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Score_Display_Controller_Mutator::Score_Display_Controller_Mutator() {
  m_found_game_settings = false;
  m_found_camera = false;
  m_found_input = false;
  m_found_game_metrics = false;
  m_game_settings = 0;
  m_camera = 0;
  m_input = 0;
  m_game_metrics = 0;
}

//------------------------------------------------------------------------------
Score_Display_Controller_Mutator::~Score_Display_Controller_Mutator() {
  m_found_game_settings = false;
  m_found_camera = false;
  m_found_input = false;
  m_found_game_metrics = false;
  m_game_settings = 0;
  m_camera = 0;
  m_game_metrics = 0;
}

//------------------------------------------------------------------------------
void Score_Display_Controller_Mutator::Mutate(Component * const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    m_game_settings = static_cast<Game_Settings_Component*>(component);
    m_found_game_settings = true;
  } else  if (component->GetType().compare("Camera_Component") == 0) {
    m_camera = static_cast<Camera_Component*>(component);
    m_found_camera = true;
  } else  if (component->GetType().compare("Game_Metrics_Component") == 0) {
    m_game_metrics = static_cast<Game_Metrics_Component*>(component);
    m_found_game_metrics = true;
  } else  if (component->GetType().compare("Input_Component") == 0) {
    m_input = static_cast<Input_Component*>(component);
    m_found_input = true;
  }
}

//------------------------------------------------------------------------------
Game_Settings_Component *const Score_Display_Controller_Mutator::GetGameSettings() {
  return m_game_settings;
}

//------------------------------------------------------------------------------
Camera_Component *const Score_Display_Controller_Mutator::GetCamera() {
  return m_camera;
}

//------------------------------------------------------------------------------
Game_Metrics_Component *const Score_Display_Controller_Mutator::GetGameMetrics() {
  return m_game_metrics;
}

//------------------------------------------------------------------------------
Input_Component *const Score_Display_Controller_Mutator::GetInput() {
  return m_input;
}

//------------------------------------------------------------------------------
bool Score_Display_Controller_Mutator::WasSuccessful() {
  return m_found_camera && m_found_game_settings && m_found_game_metrics && m_found_input;
}

}  // namespace Tunnelour
