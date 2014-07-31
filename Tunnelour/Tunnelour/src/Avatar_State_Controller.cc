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

#include "Avatar_State_Controller.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Avatar_State_Controller::Avatar_State_Controller() {
  m_model = 0;
  m_is_finished = false;
  m_has_been_initialised = false;

  m_game_settings = 0;

  m_avatar = 0;
  m_floor_tiles.clear();
  m_wall_tiles.clear();
  m_ledge_tiles.clear();
  m_animation_metadata = 0;
  m_current_animation_subset = 0;
  m_currently_adjacent_wall_tile = 0;
  m_current_metadata_file_path = 0;
  m_current_metadata = 0;
  m_world_settings = 0;
  m_last_frame_time = 0;
  m_y_fallen = 0;
  m_distance_traveled = 0;
  m_is_moving_continuously = 0;
  m_currently_grabbed_tile = 0;
}

//------------------------------------------------------------------------------
Avatar_State_Controller::~Avatar_State_Controller() {
  if (m_model != 0) {
    m_model->IgnoreType(this, "Bitmap_Component");
    m_model = 0;
  }
  m_is_finished = false;
  m_has_been_initialised = false;

  m_game_settings = 0;
  m_avatar = 0;
  m_floor_tiles.clear();
  m_wall_tiles.clear();
  m_ledge_tiles.clear();
  m_animation_metadata = 0;
  m_current_animation_subset = 0;
  m_currently_adjacent_wall_tile = 0;
  m_current_metadata_file_path = 0;
  m_current_metadata = 0;
  m_world_settings = 0;
  m_last_frame_time = 0;
  m_y_fallen = 0;
  m_distance_traveled = 0;
  m_is_moving_continuously = 0;
  m_currently_grabbed_tile = 0;
}

//------------------------------------------------------------------------------
bool Avatar_State_Controller::Init(Component_Composite *const model) {
  if (m_model == 0) {
    m_model = model;
    m_model->ObserveType(this, "Bitmap_Component");
    m_has_been_initialised = true;
  }

  return true;
}

//------------------------------------------------------------------------------
bool Avatar_State_Controller::Run() {
  bool result = false;
  if (m_game_settings != 0 &&
      m_avatar != 0 &&
      m_animation_metadata != 0 &&
      m_current_animation_subset != 0 &&
      m_currently_adjacent_wall_tile != 0 &&
      m_current_metadata_file_path != 0 &&
      m_current_metadata != 0 &&
      m_world_settings != 0 &&
      m_last_frame_time != 0 &&
      m_y_fallen != 0 &&
      m_distance_traveled != 0 &&
      m_is_moving_continuously != 0 &&
      // m_currently_grabbed_tile != 0 &&  // This is 0 when not being grabbed
      !IsFinished()) {
    m_initial_state.state = m_avatar->GetState();
    m_initial_state.position = *(m_avatar->GetPosition());

    Run_Avatar_State();
    result = true;
  }
  return result;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::Run_Avatar_State() {
}

//------------------------------------------------------------------------------
bool Avatar_State_Controller::HasAvatarStateChanged() {
  if (m_avatar->GetState().parent_state.compare(m_initial_state.state.parent_state) != 0 ||
      m_avatar->GetState().state.compare(m_initial_state.state.state) != 0) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetAvatarComponent(Avatar_Component *avatar_component) {
  m_avatar = avatar_component;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetFloorTiles(std::vector<Tile_Bitmap*> floor_tiles) {
  m_floor_tiles = floor_tiles;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetWallTiles(std::vector<Tile_Bitmap*> wall_tiles) {
  m_wall_tiles = wall_tiles;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetLedgeTiles(std::vector<Tile_Bitmap*> ledge_tiles) {
  m_ledge_tiles = ledge_tiles;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetGameSettings(Game_Settings_Component* game_settings) {
  m_game_settings = game_settings;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetAnimationMetadata(std::vector<Tileset_Helper::Animation_Tileset_Metadata> *animation_metadata) {
  m_animation_metadata = animation_metadata;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetCurrentAnimationSubset(Tileset_Helper::Animation_Subset *current_animation_subset) {
  m_current_animation_subset = current_animation_subset;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetCurrentlyAdjacentWallTile(Avatar_Helper::Tile_Collision *currently_adjacent_wall_tile) {
  m_currently_adjacent_wall_tile = currently_adjacent_wall_tile;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetCurrentMetadataFilePath(std::string *current_metadata_file_path) {
  m_current_metadata_file_path = current_metadata_file_path;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetCurrentMetadata(Tileset_Helper::Animation_Tileset_Metadata *current_metadata) {
  m_current_metadata = current_metadata;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetWorldSettings(World_Settings_Component *world_settings) {
  m_world_settings = world_settings;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetLastFrameTime(int *last_frame_time) {
  m_last_frame_time = last_frame_time;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetYFallen(int *y_fallen) {
  m_y_fallen = y_fallen;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetDistanceTraveled(int *distance_traveled) {
  m_distance_traveled = distance_traveled;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetIsMovingContinuously(bool *is_moving_continuously) {
  m_is_moving_continuously = is_moving_continuously;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::SetCurrentlyGrabbedTile(Bitmap_Component *currently_grabbed_tile) {
  m_currently_grabbed_tile = currently_grabbed_tile;
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::HandleEventAdd(Tunnelour::Component * const component) {
  Tile_Bitmap *tile = 0;
  tile = static_cast<Tile_Bitmap*>(component);
  if (tile->IsFloor() && !m_floor_tiles.empty()) {
    m_floor_tiles.push_back(tile);
  }
  if (tile->IsWall() && !m_wall_tiles.empty()) {
    m_wall_tiles.push_back(tile);
  }
  if (tile->IsWall() && tile->IsFloor() && !m_ledge_tiles.empty()) {
    m_ledge_tiles.push_back(tile);
  }
}

//------------------------------------------------------------------------------
void Avatar_State_Controller::HandleEventRemove(Tunnelour::Component * const component) {
  Tunnelour::Tile_Bitmap *target_bitmap = 0;
  target_bitmap = static_cast<Tunnelour::Tile_Bitmap*>(component);
  std::vector<Tile_Bitmap*>::iterator found_bitmap;
  if (target_bitmap->IsWall()) {
    std::vector<Tile_Bitmap*>::iterator bitmap;
    for (bitmap = m_wall_tiles.begin(); bitmap != m_wall_tiles.end(); bitmap++) {
      if ((*bitmap)->GetID() == target_bitmap->GetID()) {
        found_bitmap = bitmap;
      }
    }
    m_wall_tiles.erase(found_bitmap);
  }
  if (target_bitmap->IsFloor()) {
    std::vector<Tile_Bitmap*>::iterator bitmap;
    for (bitmap = m_floor_tiles.begin(); bitmap != m_floor_tiles.end(); bitmap++) {
      if ((*bitmap)->GetID() == target_bitmap->GetID()) {
        found_bitmap = bitmap;
      }
    }
    m_floor_tiles.erase(found_bitmap);
  }
  if (target_bitmap->IsFloor() && target_bitmap->IsWall()) {
    std::vector<Tile_Bitmap*>::iterator bitmap;
    for (bitmap = m_ledge_tiles.begin(); bitmap != m_ledge_tiles.end(); bitmap++) {
      if ((*bitmap)->GetID() == target_bitmap->GetID()) {
        found_bitmap = bitmap;
      }
    }
    m_ledge_tiles.erase(found_bitmap);
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
