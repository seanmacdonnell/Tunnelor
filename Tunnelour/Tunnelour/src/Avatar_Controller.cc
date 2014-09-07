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

#include "Avatar_Controller.h"

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <ctime>

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Avatar_Controller::Avatar_Controller() : Controller() {
  m_model = 0;
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_world_settings = 0;

  m_current_metadata_file_path = "";

  m_frequency = 0;
  m_ticksPerMs = 0;
  m_startTime = 0;
  m_frameTime = 0;
  m_last_frame_time = 0;
  m_animation_tick = false;
  m_current_animation_fps = 0;

  m_y_fallen = 0;

  m_distance_traveled = 0;

  m_currently_grabbed_tile = 0;

  m_is_moving_continuously = false;

  m_vertical_jump_y_initial_velocity = 22;
  m_vertical_jump_x_initial_velocity = 4;
  m_wall_jump_y_initial_velocity = 16;

  m_avatar_initial_direction = "Right";
  m_avatar_initial_parent_state = "Charlie_Standing";
  m_avatar_initial_state = "Initial";
  m_avatar_initial_position = D3DXVECTOR3(0, 0, 0);

  m_avatar_z_position = -2;  // Middleground Z Space is -1

  m_running_file_name = L"Charlie_Running_Animation_Tileset.txt";
  m_standing_file_name = L"Charlie_Standing_Animation_Tileset.txt";
  m_falling_file_name = L"Charlie_Falling_Animation_Tileset.txt";
  m_jumping_file_name = L"Charlie_Jumping_Animation_Tileset.txt";
  m_climbing_file_name = L"Charlie_Climbing_Animation_Tileset.txt";
}

//------------------------------------------------------------------------------
Avatar_Controller::~Avatar_Controller() {
  if (m_model != 0) {
    m_model->IgnoreType(this, "Bitmap_Component");
    m_model = 0;
  }
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_world_settings = 0;

  m_current_metadata_file_path = "";

  m_animation_metadata.clear();

  m_frequency = 0;
  m_ticksPerMs = 0;
  m_startTime = 0;
  m_frameTime = 0;
  m_last_frame_time = 0;
  m_animation_tick = false;
  m_current_animation_fps = 0;

  m_floor_tiles.clear();
  m_wall_tiles.clear();
  m_ledge_tiles.clear();

  m_y_fallen = 0;

  m_distance_traveled = 0;

  m_currently_grabbed_tile = 0;

  m_is_moving_continuously = false;

  m_vertical_jump_y_initial_velocity = 22;
  m_vertical_jump_x_initial_velocity = 4;
  m_wall_jump_y_initial_velocity = 16;

  m_avatar_initial_direction = "Right";
  m_avatar_initial_parent_state = "Charlie_Standing";
  m_avatar_initial_state = "Initial";
  m_avatar_initial_position = D3DXVECTOR3(0, 0, 0);
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Init(Component_Composite * const model) {
  if (!m_has_been_initialised) {
    Avatar_Controller_Mutator mutator;
    if (m_model == 0) {
      m_model = model;
      m_model->Apply(&mutator);
    }
    if (mutator.WasSuccessful()) {
      InitTimer();
      m_game_settings = mutator.GetGameSettings();
      m_world_settings = mutator.GetWorldSettings();
      m_level = mutator.GetLevel();
      LoadTilesets(m_game_settings->GetTilesetPath());
      CreateAvatar();
      m_model->Add(m_avatar);
      m_floor_tiles = mutator.GetFloorTiles();
      m_wall_tiles = mutator.GetWallTiles();
      m_ledge_tiles = mutator.GetLedgeTiles();
      m_model->ObserveType(this, "Bitmap_Component");
      m_has_been_initialised = true;
    } else {
      m_model = 0;
      m_has_been_initialised = false;
    }
  }

  return m_has_been_initialised;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Run() {
  bool result = false;
  if (m_has_been_initialised) {
    UpdateTimer();
    if (m_animation_tick) {
      RunAvatarState();
    }

    result = true;
  }

  return result;
}

//------------------------------------------------------------------------------
void Avatar_Controller::HandleEventAdd(Tunnelour::Component * const component) {
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
}

//------------------------------------------------------------------------------
void Avatar_Controller::HandleEventRemove(Tunnelour::Component * const component) {
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
void Avatar_Controller::HideAvatar() {
  m_avatar->GetTexture()->transparency = 0.0f;
}

//------------------------------------------------------------------------------
void Avatar_Controller::ShowAvatar() {
  m_avatar->GetTexture()->transparency = 1.0f;
}

//------------------------------------------------------------------------------
void Avatar_Controller::ResetAvatarToDefaults() {
  m_y_fallen = 0;
  Avatar_Component::Avatar_State initial_state;
  initial_state.direction = m_avatar_initial_direction;
  float avatar_x_position = m_level->GetCurrentLevel().start_avatar_top_left_x;
  float avatar_y_position = m_level->GetCurrentLevel().start_avatar_top_left_y;
  m_avatar->SetState(initial_state);
  m_avatar->SetPosition(D3DXVECTOR3(avatar_x_position,
                                    avatar_y_position,
                                    m_avatar_z_position));
  m_avatar->SetVelocity(m_avatar_initial_position);
  Avatar_Helper::SetAvatarState(m_avatar,
                                m_game_settings->GetTilesetPath(),
                               &m_animation_metadata,
                                m_avatar_initial_parent_state,
                                m_avatar_initial_state,
                                initial_state.direction,
                                &m_current_metadata_file_path,
                                &m_current_metadata,
                                &m_current_animation_subset);
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Avatar_Controller::CreateAvatar() {
  m_avatar = new Avatar_Component();
  ResetAvatarToDefaults();
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunAvatarState() {
  std::string current_state = m_avatar->GetState().parent_state;

  if (m_avatar->GetTexture()->transparency != 0.0f) {
    if (current_state.compare("Charlie_Standing") == 0) {
      if (!m_charlie_standing_controller.HasBeenInitalised()) {
        m_charlie_standing_controller.Init(m_model);
        m_charlie_standing_controller.SetAvatarComponent(m_avatar);
        m_charlie_standing_controller.SetFloorTiles(m_floor_tiles);
        m_charlie_standing_controller.SetWallTiles(m_wall_tiles);
        m_charlie_standing_controller.SetLedgeTiles(m_ledge_tiles);
        m_charlie_standing_controller.SetGameSettings(m_game_settings);
        m_charlie_standing_controller.SetAnimationMetadata(&m_animation_metadata);
        m_charlie_standing_controller.SetCurrentAnimationSubset(&m_current_animation_subset);
        m_charlie_standing_controller.SetCurrentlyAdjacentWallTile(&m_adjacent_wall);
        m_charlie_standing_controller.SetCurrentMetadataFilePath(&m_current_metadata_file_path);
        m_charlie_standing_controller.SetCurrentMetadata(&m_current_metadata);
        m_charlie_standing_controller.SetWorldSettings(m_world_settings);
        m_charlie_standing_controller.SetLastFrameTime(&m_last_frame_time);
        m_charlie_standing_controller.SetYFallen(&m_y_fallen);
        m_charlie_standing_controller.SetDistanceTraveled(&m_distance_traveled);
        m_charlie_standing_controller.SetIsMovingContinuously(&m_is_moving_continuously);
        m_charlie_standing_controller.SetCurrentlyGrabbedTile(m_currently_grabbed_tile);
      }

      if (m_charlie_standing_controller.HasBeenInitalised()) {
        m_charlie_standing_controller.Run();
      }

      if (m_charlie_standing_controller.HasAvatarStateChanged()) {
        RunAvatarState();
      }
    } else if (current_state.compare("Charlie_Falling") == 0) {
      if (!m_charlie_falling_controller.HasBeenInitalised()) {
        m_charlie_falling_controller.Init(m_model);
        m_charlie_falling_controller.SetAvatarComponent(m_avatar);
        m_charlie_falling_controller.SetFloorTiles(m_floor_tiles);
        m_charlie_falling_controller.SetWallTiles(m_wall_tiles);
        m_charlie_falling_controller.SetLedgeTiles(m_ledge_tiles);
        m_charlie_falling_controller.SetGameSettings(m_game_settings);
        m_charlie_falling_controller.SetAnimationMetadata(&m_animation_metadata);
        m_charlie_falling_controller.SetCurrentAnimationSubset(&m_current_animation_subset);
        m_charlie_falling_controller.SetCurrentlyAdjacentWallTile(&m_adjacent_wall);
        m_charlie_falling_controller.SetCurrentMetadataFilePath(&m_current_metadata_file_path);
        m_charlie_falling_controller.SetCurrentMetadata(&m_current_metadata);
        m_charlie_falling_controller.SetWorldSettings(m_world_settings);
        m_charlie_falling_controller.SetLastFrameTime(&m_last_frame_time);
        m_charlie_falling_controller.SetYFallen(&m_y_fallen);
        m_charlie_falling_controller.SetDistanceTraveled(&m_distance_traveled);
        m_charlie_falling_controller.SetIsMovingContinuously(&m_is_moving_continuously);
        m_charlie_falling_controller.SetCurrentlyGrabbedTile(m_currently_grabbed_tile);
      }

      if (m_charlie_falling_controller.HasBeenInitalised()) {
        m_charlie_falling_controller.Run();
      }

      if (m_charlie_falling_controller.HasAvatarStateChanged()) {
        RunAvatarState();
      }
    } else if (current_state.compare("Charlie_Running") == 0) {
      if (!m_charlie_running_controller.HasBeenInitalised()) {
        m_charlie_running_controller.Init(m_model);
        m_charlie_running_controller.SetAvatarComponent(m_avatar);
        m_charlie_running_controller.SetFloorTiles(m_floor_tiles);
        m_charlie_running_controller.SetWallTiles(m_wall_tiles);
        m_charlie_running_controller.SetLedgeTiles(m_ledge_tiles);
        m_charlie_running_controller.SetGameSettings(m_game_settings);
        m_charlie_running_controller.SetAnimationMetadata(&m_animation_metadata);
        m_charlie_running_controller.SetCurrentAnimationSubset(&m_current_animation_subset);
        m_charlie_running_controller.SetCurrentlyAdjacentWallTile(&m_adjacent_wall);
        m_charlie_running_controller.SetCurrentMetadataFilePath(&m_current_metadata_file_path);
        m_charlie_running_controller.SetCurrentMetadata(&m_current_metadata);
        m_charlie_running_controller.SetWorldSettings(m_world_settings);
        m_charlie_running_controller.SetLastFrameTime(&m_last_frame_time);
        m_charlie_running_controller.SetYFallen(&m_y_fallen);
        m_charlie_running_controller.SetDistanceTraveled(&m_distance_traveled);
        m_charlie_running_controller.SetIsMovingContinuously(&m_is_moving_continuously);
        m_charlie_running_controller.SetCurrentlyGrabbedTile(m_currently_grabbed_tile);
      }

      if (m_charlie_running_controller.HasBeenInitalised()) {
        m_charlie_running_controller.Run();
      }

      if (m_charlie_running_controller.HasAvatarStateChanged()) {
        RunAvatarState();
      }
    } else if (current_state.compare("Charlie_Jumping") == 0) {
      if (!m_charlie_jumping_controller.HasBeenInitalised()) {
        m_charlie_jumping_controller.Init(m_model);
        m_charlie_jumping_controller.SetAvatarComponent(m_avatar);
        m_charlie_jumping_controller.SetFloorTiles(m_floor_tiles);
        m_charlie_jumping_controller.SetWallTiles(m_wall_tiles);
        m_charlie_jumping_controller.SetLedgeTiles(m_ledge_tiles);
        m_charlie_jumping_controller.SetGameSettings(m_game_settings);
        m_charlie_jumping_controller.SetAnimationMetadata(&m_animation_metadata);
        m_charlie_jumping_controller.SetCurrentAnimationSubset(&m_current_animation_subset);
        m_charlie_jumping_controller.SetCurrentlyAdjacentWallTile(&m_adjacent_wall);
        m_charlie_jumping_controller.SetCurrentMetadataFilePath(&m_current_metadata_file_path);
        m_charlie_jumping_controller.SetCurrentMetadata(&m_current_metadata);
        m_charlie_jumping_controller.SetWorldSettings(m_world_settings);
        m_charlie_jumping_controller.SetLastFrameTime(&m_last_frame_time);
        m_charlie_jumping_controller.SetYFallen(&m_y_fallen);
        m_charlie_jumping_controller.SetDistanceTraveled(&m_distance_traveled);
        m_charlie_jumping_controller.SetIsMovingContinuously(&m_is_moving_continuously);
        m_charlie_jumping_controller.SetCurrentlyGrabbedTile(m_currently_grabbed_tile);
      }

      if (m_charlie_jumping_controller.HasBeenInitalised()) {
        m_charlie_jumping_controller.Run();
      }

      if (m_charlie_jumping_controller.HasAvatarStateChanged()) {
        RunAvatarState();
      }
    } else if (current_state.compare("Charlie_Climbing") == 0) {
      if (!m_charlie_climbing_controller.HasBeenInitalised()) {
        m_charlie_climbing_controller.Init(m_model);
        m_charlie_climbing_controller.SetAvatarComponent(m_avatar);
        m_charlie_climbing_controller.SetFloorTiles(m_floor_tiles);
        m_charlie_climbing_controller.SetWallTiles(m_wall_tiles);
        m_charlie_climbing_controller.SetLedgeTiles(m_ledge_tiles);
        m_charlie_climbing_controller.SetGameSettings(m_game_settings);
        m_charlie_climbing_controller.SetAnimationMetadata(&m_animation_metadata);
        m_charlie_climbing_controller.SetCurrentAnimationSubset(&m_current_animation_subset);
        m_charlie_climbing_controller.SetCurrentlyAdjacentWallTile(&m_adjacent_wall);
        m_charlie_climbing_controller.SetCurrentMetadataFilePath(&m_current_metadata_file_path);
        m_charlie_climbing_controller.SetCurrentMetadata(&m_current_metadata);
        m_charlie_climbing_controller.SetWorldSettings(m_world_settings);
        m_charlie_climbing_controller.SetLastFrameTime(&m_last_frame_time);
        m_charlie_climbing_controller.SetYFallen(&m_y_fallen);
        m_charlie_climbing_controller.SetDistanceTraveled(&m_distance_traveled);
        m_charlie_climbing_controller.SetIsMovingContinuously(&m_is_moving_continuously);
        m_charlie_climbing_controller.SetCurrentlyGrabbedTile(m_currently_grabbed_tile);
      }

      if (m_charlie_climbing_controller.HasBeenInitalised()) {
        m_charlie_climbing_controller.Run();
      }

      if (m_charlie_climbing_controller.HasAvatarStateChanged()) {
        RunAvatarState();
      }
    }
  } else {
    m_avatar->SetPosition(0, 0, 0);
  }

  m_animation_tick = false;
}

//------------------------------------------------------------------------------
void Avatar_Controller::LoadTilesets(std::wstring wtileset_path) {
  // Running
  std::string running_metadata_file_path = String_Helper::WStringToString(wtileset_path + m_running_file_name);
  Tileset_Helper::Animation_Tileset_Metadata running_metadata;
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(running_metadata_file_path, &running_metadata);
  m_animation_metadata.push_back(running_metadata);

  // Standing
  std::string standing_metadata_file_path = String_Helper::WStringToString(wtileset_path + m_standing_file_name);
  Tileset_Helper::Animation_Tileset_Metadata standing_metadata;
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(standing_metadata_file_path, &standing_metadata);
  m_animation_metadata.push_back(standing_metadata);

  // Falling
  std::string falling_metadata_file_path = String_Helper::WStringToString(wtileset_path + m_falling_file_name);
  Tileset_Helper::Animation_Tileset_Metadata falling_metadata;
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(falling_metadata_file_path, &falling_metadata);
  m_animation_metadata.push_back(falling_metadata);

  // Jumping
  std::string jumping_metadata_file_path = String_Helper::WStringToString(wtileset_path + m_jumping_file_name);
  Tileset_Helper::Animation_Tileset_Metadata jumping_metadata;
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(jumping_metadata_file_path, &jumping_metadata);
  m_animation_metadata.push_back(jumping_metadata);

  // Climbing
  std::string climbing_metadata_file_path = String_Helper::WStringToString(wtileset_path + m_climbing_file_name);
  Tileset_Helper::Animation_Tileset_Metadata climbing_metadata;
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(climbing_metadata_file_path, &climbing_metadata);
  m_animation_metadata.push_back(climbing_metadata);
}

//------------------------------------------------------------------------------
bool Avatar_Controller::InitTimer() {
  // Check to see if this system supports high performance timers.
  QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));

  if (m_frequency == 0) {
    return false;
  }

  // Find out how many times the frequency counter ticks every millisecond.
  m_ticksPerMs = static_cast<float>(m_frequency / 1000);

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTime));

  return true;
}

//------------------------------------------------------------------------------
void Avatar_Controller::UpdateTimer() {
  int milliseconds_per_frame = 0;
  // 1000/24 is the miliseconds per frame from 24 frames per second
  if (m_game_settings->IsDebugMode()) {
    milliseconds_per_frame = static_cast<int>(1000/1);
  } else {
    milliseconds_per_frame = static_cast<int>(1000/18);
  }

  INT64 currentTime;
  float timeDifference;

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

  timeDifference = static_cast<float>(currentTime - m_startTime);

  m_frameTime = timeDifference / m_ticksPerMs;

  if (m_frameTime >= milliseconds_per_frame) {
    m_last_frame_time = static_cast<int>(m_frameTime);
    if (m_last_frame_time % 2) {
      /* x is odd */
      m_last_frame_time += 1;
    }
    m_startTime = currentTime;
    m_frameTime = static_cast<float>(m_frameTime - milliseconds_per_frame);
    m_animation_tick = true;
  }

  if (m_game_settings->IsDebugMode()) {
    m_last_frame_time = 64;
  }
}

}  // namespace Tunnelour
