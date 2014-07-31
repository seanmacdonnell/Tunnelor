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

#include "Avatar_Controller_Mutator.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Avatar_Controller_Mutator::Avatar_Controller_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
  m_floor_tiles.clear();
  m_found_world_settings = false;
  m_world_settings = 0;
  m_wall_tiles.clear();
  m_ledge_tiles.clear();
  m_found_level = false;
  m_level = 0;
}

//------------------------------------------------------------------------------
Avatar_Controller_Mutator::~Avatar_Controller_Mutator() {
  m_found_game_settings = false;
  m_game_settings = 0;
  m_floor_tiles.clear();
  m_found_world_settings = false;
  m_world_settings = 0;
  m_wall_tiles.clear();
  m_ledge_tiles.clear();
  m_found_level = false;
  m_level = 0;
}

//------------------------------------------------------------------------------
void Avatar_Controller_Mutator::Mutate(Component * const component) {
  if (component->GetType().compare("Game_Settings_Component") == 0) {
    Game_Settings_Component *game_settings = 0;
    game_settings = static_cast<Game_Settings_Component*>(component);
    m_game_settings = game_settings;
    m_found_game_settings = true;
  } else if (component->GetType().compare("Bitmap_Component") == 0) {
    Tile_Bitmap *tile = 0;
    tile = static_cast<Tile_Bitmap*>(component);
    if (tile->IsFloor()) {
      m_floor_tiles.push_back(tile);
    }
    if (tile->IsWall()) {
      m_wall_tiles.push_back(tile);
    }
    if (tile->IsWall() && tile->IsFloor()) {
      m_ledge_tiles.push_back(tile);
    }
  } else if (component->GetType().compare("World_Settings_Component") == 0) {
    m_world_settings = static_cast<World_Settings_Component*>(component);
    m_found_world_settings = true;
  } else if (component->GetType().compare("Level_Component") == 0) {
    m_level = static_cast<Level_Component*>(component);
    m_found_level = true;
  }
}

//------------------------------------------------------------------------------
Game_Settings_Component* const Avatar_Controller_Mutator::GetGameSettings() {
  return m_game_settings;
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Avatar_Controller_Mutator::GetFloorTiles() {
  return m_floor_tiles;
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Avatar_Controller_Mutator::GetWallTiles() {
  return m_wall_tiles;
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Avatar_Controller_Mutator::GetLedgeTiles() {
  return m_ledge_tiles;
}

//------------------------------------------------------------------------------
World_Settings_Component* Avatar_Controller_Mutator::GetWorldSettings() {
  return m_world_settings;
}

//------------------------------------------------------------------------------
Level_Component* Avatar_Controller_Mutator::GetLevel() {
  return m_level;
}

//------------------------------------------------------------------------------
bool Avatar_Controller_Mutator::WasSuccessful() {
  return m_found_game_settings && m_found_world_settings && m_found_level;
}

}  // namespace Tunnelour
