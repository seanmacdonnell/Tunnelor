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

#include "Camera_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Camera_Controller_Mutator::Camera_Controller_Mutator() {
  m_found_game_settings = false;
  m_found_avatar_component = false;
  m_game_settings = 0;
  m_avatar_controller = 0;
}

//------------------------------------------------------------------------------
Camera_Controller_Mutator::~Camera_Controller_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
}

//------------------------------------------------------------------------------
void Camera_Controller_Mutator::Mutate(Tunnelour::Component * const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    m_game_settings = static_cast<Game_Settings_Component*>(component);
    m_found_game_settings = true;
  } else if (component->GetType().compare("Avatar_Component") == 0) {
    m_avatar_controller = static_cast<Avatar_Component*>(component);
    m_found_avatar_component = true;
  } else if (component->GetType().compare("Bitmap_Component") == 0) {
    Tile_Bitmap *tile = 0;
    tile = static_cast<Tile_Bitmap*>(component);
    if (tile->IsFloor()) {
      m_floor_tiles.push_back(tile);
    }
  }
}

//------------------------------------------------------------------------------
Game_Settings_Component* const Camera_Controller_Mutator::GetGameSettings() {
  return m_game_settings;
}

//------------------------------------------------------------------------------
Avatar_Component* const Camera_Controller_Mutator::GetAvatarComponent() {
  return m_avatar_controller;
}

//------------------------------------------------------------------------------
bool Camera_Controller_Mutator::WasSuccessful() {
  return (m_found_game_settings && m_found_avatar_component);
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Camera_Controller_Mutator::GetFloorTiles() {
  return m_floor_tiles;
}

}  // namespace Tunnelour
