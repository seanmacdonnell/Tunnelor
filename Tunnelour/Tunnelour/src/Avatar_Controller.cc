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

#include "Avatar_Controller.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>

#include "Exceptions.h"
#include "String_Helper.h"
#include "Bitmap_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Avatar_Controller::Avatar_Controller() : Controller() {
  m_model = 0;
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_animation_tick = false;
  m_world_settings = 0;
  m_avatar_z_position = -2; // Middleground Z Space is -1
  m_wall_impacting = false;
  m_last_frame_time = 0;
}

//------------------------------------------------------------------------------
Avatar_Controller::~Avatar_Controller() {
  m_model->IgnoreType(this, "Bitmap_Component");
  m_model = 0;
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_animation_tick = false;
  m_world_settings = 0;
  m_floor_tiles.clear();
  m_wall_tiles.clear();
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Init(Component_Composite * const model) {
  if (m_model == 0) {
    m_model = model; 
  }
  Avatar_Controller_Mutator mutator;
  m_model->Apply(&mutator);
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
    m_model->ObserveType(this, "Bitmap_Component");
    m_has_been_initialised = true;
  }

  return m_has_been_initialised;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Run() {
  if (!m_has_been_initialised) { return false; }

  if (m_current_level_name.compare(m_level->GetCurrentLevel().level_name) != 0) {
    ResetAvatar();
  } 

  UpdateTimer();
  if (m_animation_tick) {
    RunAvatarState(); 
  }

  return true;
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
void Avatar_Controller::ResetAvatar() {
  Avatar_Component::Avatar_State initial_state;
  initial_state.direction = "Right";
  float avatar_x_position = m_level->GetCurrentLevel().start_avatar_top_left_x;
  float avatar_y_position = m_level->GetCurrentLevel().start_avatar_top_left_y;
  m_current_level_name = m_level->GetCurrentLevel().level_name;
  m_avatar->SetState(initial_state);
  m_avatar->SetPosition(D3DXVECTOR3(avatar_x_position, 
                                    avatar_y_position,
                                  m_avatar_z_position));
  m_avatar->SetVelocity(D3DXVECTOR3(0,0,0));
  SetAvatarState("Charlie_Standing", "Standing", initial_state.direction);
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Avatar_Controller::CreateAvatar() {
  m_avatar = new Avatar_Component();
  Avatar_Component::Avatar_State initial_state;
  initial_state.direction = "Right";
  float avatar_x_position = m_level->GetCurrentLevel().start_avatar_top_left_x;
  float avatar_y_position = m_level->GetCurrentLevel().start_avatar_top_left_y;
  m_current_level_name = m_level->GetCurrentLevel().level_name;
  m_avatar->SetState(initial_state);
  m_avatar->SetPosition(D3DXVECTOR3(avatar_x_position, 
                                    avatar_y_position,
                                  m_avatar_z_position));  
  SetAvatarState("Charlie_Standing", "Standing", initial_state.direction);
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunAvatarState() {
  std::string current_state = m_avatar->GetState().parent_state;
  m_last_state.state = m_avatar->GetState();
  m_last_state.position = *(m_avatar->GetPosition());

  if (m_avatar->GetTexture()->transparency != 0.0f) {
    if (current_state.compare("Charlie_Standing") == 0) {
      RunStandingState();
    } else if (current_state.compare("Charlie_Walking") == 0) { 
      RunWalkingState();
    } else if (current_state.compare("Charlie_Falling") == 0) {
      RunFallingState();
    } else if (current_state.compare("Charlie_Level_Transitioning") == 0) {
      RunLevelTransitioningState();
    } else if (current_state.compare("Charlie_Running") == 0) {
      RunRunningState();
    } else if (current_state.compare("Charlie_Jumping") == 0) {
      RunJumpingState();
    }
  } else {
    m_avatar->SetPosition(0, 0, 0);
  }

  m_animation_tick = false;
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunStandingState() {
  m_avatar->SetAngle(static_cast<float>(0.0));  // in radians
  m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

  bool has_state_changed = false;

  // If there is a command (Left, Right, Run, Etc.)
  if (current_command.state.compare("") != 0) {
    if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0 ) {
      if (current_command.state.compare("Jumping") == 0) {
        m_avatar->SetAngle(static_cast<float>(1.0));  // in radians
        float y_velocity = 42;
        float x_velocity = 30;
        if (current_command.direction.compare("Right") == 0) {
          m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
        } else {  // Left
          m_avatar->SetVelocity(D3DXVECTOR3((x_velocity * -1), y_velocity, 0));
        }
      }
      SetAvatarState("Charlie_" + current_command.state, current_command.state, current_command.direction);
      has_state_changed = true;
    } else {
      if (current_command.state.compare("Jumping") == 0) {
        m_avatar->SetAngle(static_cast<float>(1.0));  // in radians
        float y_velocity = 42;
        float x_velocity = 30;
        if (current_state.direction.compare("Right") == 0) {
          m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
        } else {  // Left
          m_avatar->SetVelocity(D3DXVECTOR3((x_velocity * -1), y_velocity, 0));
        }
      }
      SetAvatarState("Charlie_" + current_command.state, current_command.state, m_avatar->GetState().direction);
      has_state_changed = true;
    }
    AlignAvatarOnLastAvatarCollisionBlock();
    ClearAvatarState();
  } else { // No command, run the standing animation,
    if (m_level->IsComplete()) {
      SetAvatarState("Charlie_Standing", "Standing_To_Thumbs_up", m_avatar->GetState().direction);
      has_state_changed = true;
      AlignAvatarOnLastAvatarCollisionBlock();
    } else {
      unsigned int state_index = current_state.state_index;
      state_index++;
      if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
        if (m_current_animation_subset.is_repeatable) {
          state_index = 0;
        } else {
          std::string error;
          error = "No handling for this non-repeating animation: " + current_command.state;
          throw Exceptions::init_error(error);
        }
      }

      SetAvatarStateAnimationFrame(state_index);
    }
  }
  
  if (!has_state_changed) {
    // Detect if the avatar is intersecting with a wall
    std::vector<Wall_Collision> *out_colliding_floor_tiles = new std::vector<Wall_Collision>();
    if (IsAvatarWallColliding(out_colliding_floor_tiles)) {
      // Move back avatar
      if ((*out_colliding_floor_tiles->begin()).collision_side.compare("Right") == 0) {
        if (m_avatar->GetState().direction.compare("Left") == 0) {
          AlignAvatarOnLastAvatarCollisionBlock();
          float tile_right_position = (*out_colliding_floor_tiles->begin()).colliding_tile->GetBottomRightPostion().x;
          float avatar_avatar_collision_block_left_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetTopLeftPostion().x;
          D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
          float x_offset = tile_right_position + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
          avatar_position.x = x_offset;
          m_avatar->SetPosition(avatar_position);
        } else {
          float tile_right_position = (*out_colliding_floor_tiles->begin()).colliding_tile->GetBottomRightPostion().x;
          float avatar_avatar_collision_block_left_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetTopLeftPostion().x;
          D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
          float x_offset = tile_right_position + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
          avatar_position.x = x_offset;
          m_avatar->SetPosition(avatar_position);
        }
      } else  if ((*out_colliding_floor_tiles->begin()).collision_side.compare("Left") == 0) {
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          AlignAvatarOnLastAvatarCollisionBlock();
          float tile_left_position = (*out_colliding_floor_tiles->begin()).colliding_tile->GetTopLeftPostion().x;
          float avatar_avatar_collision_block_right_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetBottomRightPostion().x;
          D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
          float x_offset = tile_left_position - (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
          avatar_position.x = x_offset;
          m_avatar->SetPosition(avatar_position);
        } else {
          float tile_left_position = (*out_colliding_floor_tiles->begin()).colliding_tile->GetTopLeftPostion().x;
          float avatar_avatar_collision_block_right_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetBottomRightPostion().x;
          D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
          float x_offset = tile_left_position - (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
          avatar_position.x = x_offset;
          m_avatar->SetPosition(avatar_position);
        }
      }
    }
    // If the avatar is not adjacent to the floor, he should fall
    if (!IsAvatarFloorAdjacent()) {
      SetAvatarState("Charlie_Falling", "Falling_To_Standing", m_avatar->GetState().direction);
      AlignAvatarOnLastAvatarCollisionBlock();
      RunAvatarState();
    }
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunWalkingState() {
  m_avatar->SetAngle(static_cast<float>(0.0));  // in radians
  
  bool has_state_changed = false;

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

  if (current_state.direction.compare("Right") == 0) {
    m_avatar->SetVelocity(D3DXVECTOR3(8, 0 , 0));
  } else {  // Left
    m_avatar->SetVelocity(D3DXVECTOR3(-8, 0 , 0));
  }

  if (current_command.state.compare("") != 0) {
    // There is a command
    // Is the command different from the current state?
    if (current_state.state.compare(current_command.state) != 0 ||
        current_state.direction != current_command.direction) {
      if (current_state.state.compare("Wall_Colliding") == 0) {
        if (current_state.direction != current_command.direction) {
          SetAvatarState("Charlie_" + current_command.state, current_command.state, current_command.direction);
          AlignAvatarOnLastAvatarCollisionBlock();
          has_state_changed = true;
        }
      } else {
        if (current_command.state.compare("Jumping") == 0) {
          m_avatar->SetAngle(static_cast<float>(1.0));  // in radians
          if (current_state.direction.compare("Right") == 0) {
            float y_velocity = 42;
            float x_velocity = 30;
            m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
          } else {  // Left
            float y_velocity = 42;
            float x_velocity = -30;
            m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
          }
        }
        if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0 ) {
          SetAvatarState("Charlie_" + current_command.state, current_command.state, current_command.direction);
        } else {
          SetAvatarState("Charlie_" + current_command.state, current_command.state, m_avatar->GetState().direction);
        }
        has_state_changed = true;
        AlignAvatarOnLastAvatarCollisionBlock();
      }
    } else { // Command is the same as the current state.
      // Continue State
      unsigned int state_index = current_state.state_index;
      state_index++;
      if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
        if (m_current_animation_subset.is_repeatable) {
          state_index = 0;
        } else {
          std::string error;
          error = "No handling for the non-repeating animation: " + current_command.state;
          throw Exceptions::init_error(error);
        }
      }
      if (current_state.state.compare("Walking") == 0) {
        SetAvatarStateAnimationFrame(state_index);
        AlignAvatarOnLastAvatarCollisionBlock();

        D3DXVECTOR3 position = *m_avatar->GetPosition();
        position += m_avatar->GetVelocity();
        m_avatar->SetPosition(position);
      }
    }

    ClearAvatarState();
  } else {
    // No Command, Change to standing
    SetAvatarState("Charlie_Standing", "Standing", m_avatar->GetState().direction);
    AlignAvatarOnLastAvatarCollisionBlock();
    has_state_changed = true;
  }
  
  if (!has_state_changed) {
    // Detect if the avatar is intersecting with a wall
    std::vector<Wall_Collision> *out_colliding_floor_tiles = new std::vector<Wall_Collision>();
    if (IsAvatarWallColliding(out_colliding_floor_tiles)) {
      SetAvatarState("Charlie_Walking", "Wall_Colliding", m_avatar->GetState().direction);
      AlignAvatarOnLastAvatarCollisionBlock();
      // Move back avatar
      if (m_avatar->GetState().direction.compare("Left") == 0) {
        float tile_right_position = out_colliding_floor_tiles->begin()->colliding_tile->GetBottomRightPostion().x;
        float avatar_avatar_collision_block_left_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetTopLeftPostion().x;
        D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
        float x_offset = tile_right_position + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
        avatar_position.x = x_offset;
        m_avatar->SetPosition(avatar_position);
      }
      if (m_avatar->GetState().direction.compare("Right") == 0) {
        D3DXVECTOR3 tile_position = out_colliding_floor_tiles->begin()->colliding_tile->GetTopLeftPostion();
        D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
        float foot_x_offset = GetLowestCollisionBlock(m_avatar->GetState().avatar_collision_blocks).offset_from_avatar_centre.x + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
        avatar_position.x = tile_position.x + (foot_x_offset * -1);
        m_avatar->SetPosition(avatar_position);
      }
    }
  
    // Detect if the avatar is overbalancing from walking
    if (!IsAvatarFloorAdjacent()) {
      m_avatar->SetVelocity(D3DXVECTOR3(4, 0 , 0));
      SetAvatarState("Charlie_Falling", "Walking_To_Falling", m_avatar->GetState().direction);
      AlignAvatarOnLastAvatarCollisionBlock();
    }
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunRunningState() {
  m_avatar->SetAngle(static_cast<float>(0.0));  // in radians
  m_avatar->SetVelocity(D3DXVECTOR3(20, 0 , 0));

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();
  if (current_command.state.compare("") != 0) {
    // There is a command
    // Is the command different from the current state?
    if (current_state.state.compare(current_command.state) != 0 || current_state.direction != current_command.direction) {
      if (current_state.state.compare("Wall_Colliding") == 0) {
        if (current_state.direction != current_command.direction) {
          SetAvatarState("Charlie_"+current_command.state, current_command.state, current_command.direction);
          AlignAvatarOnLastAvatarCollisionBlock();
        }
      } else {
        if (current_command.state.compare("Jumping") == 0) {
          if (current_state.direction.compare("Right") == 0) {
            float y_velocity = 50;
            float x_velocity = 50;
            m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
          } else {  // Left
            float y_velocity = 50;
            float x_velocity = -50;
            m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
          }
        }
        // Set a new state
        SetAvatarState("Charlie_"+current_command.state, current_command.state, current_command.direction);
        AlignAvatarOnLastAvatarCollisionBlock();
      }
    } else { // Command is the same as the current state.
      // Continue State
      unsigned int state_index = current_state.state_index;
      state_index++;
      if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
        if (m_current_animation_subset.is_repeatable) {
          state_index = 0;
        } else {
          std::string error;
          error = "No handling for the non-repeating animation: " + current_command.state;
          throw Exceptions::init_error(error);
        }
      }
      if (current_state.state.compare("Running") == 0) {
        SetAvatarStateAnimationFrame(state_index);
        AlignAvatarOnLastAvatarCollisionBlock();

        D3DXVECTOR3 position = *m_avatar->GetPosition();
        if (current_state.direction.compare("Right") == 0) {
          position += m_avatar->GetVelocity();
        } else {  // Left
          position -= m_avatar->GetVelocity();
        }
        m_avatar->SetPosition(position);
      }
    }

    ClearAvatarState();
  } else {
    // No Command, Change to standing
    SetAvatarState("Charlie_Standing", "Standing", m_avatar->GetState().direction);
    AlignAvatarOnLastAvatarCollisionBlock();
  }
  
  // Detect if the avatar is intersecting with a wall
  std::vector<Wall_Collision> *out_colliding_floor_tiles = new std::vector<Wall_Collision>();
  if (IsAvatarWallColliding(out_colliding_floor_tiles)) {
    // Move back avatar
    if ((*out_colliding_floor_tiles->begin()).collision_side.compare("Right") == 0) {
      if (m_avatar->GetState().direction.compare("Left") == 0) {
        SetAvatarState("Charlie_Running", "Wall_Colliding", m_avatar->GetState().direction);
        AlignAvatarOnLastAvatarCollisionBlock();
        float tile_right_position = (*out_colliding_floor_tiles->begin()).colliding_tile->GetBottomRightPostion().x;
        float avatar_avatar_collision_block_left_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetTopLeftPostion().x;
        D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
        float x_offset = tile_right_position + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
        avatar_position.x = x_offset;
        m_avatar->SetPosition(avatar_position);
      } else {
        float tile_right_position = (*out_colliding_floor_tiles->begin()).colliding_tile->GetBottomRightPostion().x;
        float avatar_avatar_collision_block_left_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetTopLeftPostion().x;
        D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
        float x_offset = tile_right_position + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
        avatar_position.x = x_offset;
        m_avatar->SetPosition(avatar_position);
      }
    } else  if ((*out_colliding_floor_tiles->begin()).collision_side.compare("Left") == 0) {
      if (m_avatar->GetState().direction.compare("Right") == 0) {
        SetAvatarState("Charlie_Running", "Wall_Colliding", m_avatar->GetState().direction);
        AlignAvatarOnLastAvatarCollisionBlock();
        float tile_left_position = (*out_colliding_floor_tiles->begin()).colliding_tile->GetTopLeftPostion().x;
        float avatar_avatar_collision_block_right_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetBottomRightPostion().x;
        D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
        float x_offset = tile_left_position - (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
        avatar_position.x = x_offset;
        m_avatar->SetPosition(avatar_position);
      } else {
        float tile_left_position = (*out_colliding_floor_tiles->begin()).colliding_tile->GetTopLeftPostion().x;
        float avatar_avatar_collision_block_right_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks), *(m_avatar->GetPosition())))->GetBottomRightPostion().x;
        D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
        float x_offset = tile_left_position - (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
        avatar_position.x = x_offset;
        m_avatar->SetPosition(avatar_position);
      }
    }
  }
  
  // Detect if the avatar is overbalancing from walking
  if (!IsAvatarFloorAdjacent()) {
    m_avatar->SetVelocity(D3DXVECTOR3(4, 0 , 0));
    SetAvatarState("Charlie_Falling", "Walking_To_Falling", m_avatar->GetState().direction);
    AlignAvatarOnLastAvatarCollisionBlock();
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunFallingState() {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  std::vector<Wall_Collision> out_colliding_floor_tiles;
  Bitmap_Component* out_avatar_collision_block = new Bitmap_Component();
  if (current_state.state.compare("Falling_To_Standing") == 0) {
    D3DXVECTOR3 position = *m_avatar->GetPosition();
    D3DXVECTOR3 new_velocity = m_avatar->GetVelocity();
    new_velocity.y += ceil(((m_world_settings->GetGravityInPixPerMs() * m_last_frame_time) * -1));
    if ((new_velocity.y/(m_last_frame_time * -1)) >= m_world_settings->GetMaxVelocityInPixPerMs()) { 
      new_velocity.y = ceil((-1 * m_world_settings->GetMaxVelocityInPixPerMs() * m_last_frame_time)); 
    }

    position += new_velocity;
    m_avatar->SetVelocity(new_velocity);
    m_avatar->SetPosition(position);

    std::vector<Wall_Collision> out_colliding_wall_tiles;
    if (IsAvatarFloorColliding(&out_colliding_floor_tiles)) {
      // Avatar has landed, move the avatar to the top of the tile
      bool is_tangent = false;
      float tile_top = 0;

      std::vector<Wall_Collision>::iterator it;
      for (it = out_colliding_floor_tiles.begin(); it != out_colliding_floor_tiles.end(); it++) {
        tile_top = (*it).colliding_tile->GetTopLeftPostion().y;
        float block_bottom = out_avatar_collision_block->GetBottomRightPostion().y;
        if (tile_top == block_bottom) {
          is_tangent = true;
        }
      }

      MoveAvatarFloorAdjacent();

      SetAvatarState("Charlie_Standing", "Standing", m_avatar->GetState().direction);
      m_y_fallen = 0;
    }
    if (IsAvatarWallColliding(&out_colliding_wall_tiles)) {
      if (out_colliding_wall_tiles.begin()->collision_side.compare("Right") == 0) {
        MoveAvatarTileAdjacent("Right", out_colliding_wall_tiles.begin()->colliding_tile);
      } else if (out_colliding_wall_tiles.begin()->collision_side.compare("Left") == 0) {
        MoveAvatarTileAdjacent("Left", out_colliding_wall_tiles.begin()->colliding_tile);
      }
    }
  } else {
    D3DXVECTOR3 position = *m_avatar->GetPosition();
    unsigned int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
      if (m_current_animation_subset.is_repeatable) {
        state_index = 0;
        SetAvatarStateAnimationFrame(state_index);
      } else {
        if (current_state.state.compare("Walking_To_Falling") == 0) {
          SetAvatarState("Charlie_Falling", "Falling_Flip", m_avatar->GetState().direction);
          state_index = 0;
          current_state = m_avatar->GetState();
        } else if (current_state.state.compare("Falling_Flip") == 0) {
          SetAvatarState("Charlie_Falling", "Falling", m_avatar->GetState().direction);
          state_index = 0;
          current_state = m_avatar->GetState();
        } else if (current_state.state.compare("Falling_To_Death") == 0) {
          SetAvatarState("Charlie_Falling", "Death", m_avatar->GetState().direction);
          state_index = 0;
          current_state = m_avatar->GetState();
        } else {
          std::string error;
          error = "No handling for the non-repeating animation: " + current_state.state;
          throw Exceptions::init_error(error);
        }
      }
    } else {
      SetAvatarStateAnimationFrame(state_index);
    }

    if (current_state.state.compare("Walking_To_Falling") == 0) {
      if (m_avatar->GetState().direction.compare("Right") == 0) {
        AlignAvatarOnRightFoot();
      } else {
      if (m_avatar->GetState().direction.compare("Left") == 0) {
        AlignAvatarOnLeftFoot();
      }
      }
    } else if (current_state.state.compare("Falling_To_Death") == 0 || current_state.state.compare("Death") == 0 ) {
      AlignAvatarOnLastAvatarCollisionBlock();
    } else {
      position = *m_avatar->GetPosition();
      D3DXVECTOR3 new_velocity = m_avatar->GetVelocity();
      new_velocity.y += ceil(((m_world_settings->GetGravityInPixPerMs() * m_last_frame_time) * -1));
      if ((new_velocity.y/(m_last_frame_time * -1)) >= m_world_settings->GetMaxVelocityInPixPerMs()) { 
        new_velocity.y = ceil((-1 * m_world_settings->GetMaxVelocityInPixPerMs() * m_last_frame_time)); 
      }

      m_avatar->SetVelocity(new_velocity);
      if (m_avatar->GetState().direction.compare("Right") == 0) {
        position.x = position.x + m_avatar->GetVelocity().x;
      } else if  (m_avatar->GetState().direction.compare("Left") == 0) {
        position.x = position.x - m_avatar->GetVelocity().x;
      }
      position += new_velocity;
      m_y_fallen += m_avatar->GetVelocity().y;
      m_avatar->SetPosition(position);
    }
    std::vector<Wall_Collision> out_colliding_wall_tiles;
    bool is_wall_colliding = false;
    is_wall_colliding = IsAvatarWallColliding(&out_colliding_wall_tiles);
    if (is_wall_colliding) {
      if (m_avatar->GetState().direction.compare("Right") == 0) {
        SetAvatarState("Charlie_Falling", "Falling_Flip", "Left");
        AlignAvatarOnLastAvatarCollisionBlock();
        MoveAvatarTileAdjacent("Left", out_colliding_wall_tiles.begin()->colliding_tile);
        D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
        D3DXVECTOR3 new_velocity = D3DXVECTOR3(old_velocity.x / 2, ceil(old_velocity.y - (m_world_settings->GetGravityInPixPerMs() * m_last_frame_time)), old_velocity.z);
        m_avatar->SetVelocity(new_velocity);
      } else {
        SetAvatarState("Charlie_Falling", "Falling_Flip", "Right");
        AlignAvatarOnLastAvatarCollisionBlock();
        MoveAvatarTileAdjacent("Right", out_colliding_wall_tiles.begin()->colliding_tile);
        D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
        D3DXVECTOR3 new_velocity = D3DXVECTOR3(old_velocity.x / 2, ceil(old_velocity.y - (m_world_settings->GetGravityInPixPerMs() * m_last_frame_time)), old_velocity.z);
        m_avatar->SetVelocity(new_velocity);
      }
    }
    if (current_state.state.compare("Falling") == 0 || current_state.state.compare("Falling_Flip") == 0 || current_state.state.compare("Falling_To_Standing") == 0 ) {
      out_colliding_floor_tiles.clear();
      bool is_colliding = IsAvatarFloorColliding(&out_colliding_floor_tiles);
      if (is_colliding) {
        if (m_y_fallen < -256) {
          SetAvatarState("Charlie_Falling", "Falling_To_Death", m_avatar->GetState().direction);
          AlignAvatarOnLastAvatarCollisionBlock();
          MoveAvatarTileAdjacent("Top", out_colliding_floor_tiles.begin()->colliding_tile);
        } else {
          SetAvatarState("Charlie_Standing", "Standing", m_avatar->GetState().direction);
          AlignAvatarOnLastAvatarCollisionBlock();
          m_y_fallen = 0;
          MoveAvatarTileAdjacent("Top", out_colliding_floor_tiles.begin()->colliding_tile);
        }
      } 
    }

  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunJumpingState() {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

  std::vector<Wall_Collision> out_colliding_floor_tiles;
  Bitmap_Component* out_avatar_collision_block = new Bitmap_Component();
  bool has_state_changed = false;
  if (current_state.state.compare("Jumping") == 0) {
    D3DXVECTOR3 position = *m_avatar->GetPosition();
    D3DXVECTOR3 velocity = m_avatar->GetVelocity();
    position += velocity;
    m_avatar->SetPosition(position);

    unsigned int state_index = current_state.state_index;
    if (state_index == 0) {
      state_index = 1;
    } else if (state_index == 1) {
      state_index = 2;
    } else if (state_index == 2) {
      if (m_avatar->GetVelocity().y < 0) {
        state_index = 3;
      }
    } else if (state_index == 3) {
      state_index = 4;
    } else if  (state_index == 4) {
      state_index = 5;
    } else if   (state_index == 6) {
      if (current_command.state.compare("") != 0) {
        if (current_state.state.compare(current_command.state) != 0 || current_state.direction != current_command.direction) {
          if (current_state.state.compare("Wall_Colliding") == 0) {
            if (current_state.direction != current_command.direction) {
              SetAvatarState("Charlie_" + current_command.state, current_command.state, current_command.direction);
              AlignAvatarOnLastAvatarCollisionBlock();
              has_state_changed = true;
            }
          } else {
            if (current_command.state.compare("Jumping") == 0) {
              m_avatar->SetAngle(static_cast<float>(1.0));  // in radians
              if (current_state.direction.compare("Right") == 0) {
                m_avatar->SetVelocity(D3DXVECTOR3(42, 50, 0));
              } else {  // Left
                m_avatar->SetVelocity(D3DXVECTOR3(-42, 50, 0));
              }
            }
            if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0 ) {
              SetAvatarState("Charlie_" + current_command.state, current_command.state, current_command.direction);
            } else {
              SetAvatarState("Charlie_" + current_command.state, current_command.state, m_avatar->GetState().direction);
            }
            has_state_changed = true;
            AlignAvatarOnLastAvatarCollisionBlock();
          }
        }
      } else {
        // No Command, Change to standing
        SetAvatarState("Charlie_Standing", "Standing", m_avatar->GetState().direction);
        AlignAvatarOnLastAvatarCollisionBlock();
        has_state_changed = true;
      }
    }

    bool is_floor_colliding = IsAvatarFloorColliding(&out_colliding_floor_tiles);
    if (is_floor_colliding) {
      SetAvatarStateAnimationFrame(6);
      state_index = 6; 
      m_avatar->SetAngle(static_cast<float>(0.0));  // in radians
      m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
      MoveAvatarFloorAdjacent();
    }

    if (!has_state_changed) {
      if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
        if (m_current_animation_subset.is_repeatable) {
          state_index = 0;
        } else {
          std::string error;
          error = "No handling for this non-repeating animation: " + m_avatar->GetState().state;
          throw Exceptions::init_error(error);
        }
      }
      if (state_index != 6) {
        SetAvatarStateAnimationFrame(state_index);

        D3DXVECTOR3 new_velocity = m_avatar->GetVelocity();
        new_velocity.y += ceil(((m_world_settings->GetGravityInPixPerMs() * m_last_frame_time) * -1));
        if ((new_velocity.y/(m_last_frame_time * -1)) >= m_world_settings->GetMaxVelocityInPixPerMs()) { 
          new_velocity.y = ceil((-1 * m_world_settings->GetMaxVelocityInPixPerMs() * m_last_frame_time)); 
        }
        
        m_avatar->SetVelocity(new_velocity);
      }
    }
    std::vector<Wall_Collision> out_colliding_wall_tiles;
    if (IsAvatarWallColliding(&out_colliding_wall_tiles)) {
      SetAvatarState("Charlie_Falling", "Falling", current_state.direction);
      AlignAvatarOnLastAvatarCollisionBlock();
      if (current_state.direction.compare("Right") == 0) {
        MoveAvatarTileAdjacent("Left", out_colliding_wall_tiles.begin()->colliding_tile);
      } else {
        MoveAvatarTileAdjacent("Right", out_colliding_wall_tiles.begin()->colliding_tile);
      }
      //RunFallingState();
    }
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunLevelTransitioningState() {
  m_avatar->SetAngle(static_cast<float>(0.0));  // in radians
  m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

  if (current_state.state.compare("Standing_To_Thumbs_up") == 0) {
    unsigned int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
      if (m_current_animation_subset.is_repeatable) {
        state_index = 0;
      } else {
        SetAvatarState("Charlie_Level_Transitioning", "Thumbs_up", m_avatar->GetState().direction);
        state_index = 0;
        current_state = m_avatar->GetState();
      }
    }

    SetAvatarStateAnimationFrame(state_index);
  } else if (current_state.state.compare("Thumbs_up") == 0) {
    unsigned int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
      if (m_current_animation_subset.is_repeatable) {
        state_index = 0;
      } else {
        std::string error;
        error = "No handling for this non-repeating animation: " + current_command.state;
        throw Exceptions::init_error(error);
      }
    }

    SetAvatarStateAnimationFrame(state_index);
  }
}

//------------------------------------------------------------------------------
bool Avatar_Controller::IsAvatarFloorAdjacent() {
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot (Lowest collision Block).
  if (!m_floor_tiles.empty()) {
    // Find the lowest contact block
    // Get the lowest block most right/left block
    Avatar_Component::Avatar_Collision_Block avatar_avatar_collision_block;
    avatar_avatar_collision_block = GetLowestMostForwardFootCollisionBlock(m_avatar->GetState().direction);

    // Make a bitmap for the lowest avatar collision block
    Bitmap_Component avatar_avatar_collision_block_bitmap;
    avatar_avatar_collision_block_bitmap = CollisionBlockToBitmapComponent(avatar_avatar_collision_block, *(m_avatar->GetPosition()));

    // Create a list of floor tiles which are adjacent with the collision block
    std::vector<Tile_Bitmap*> adjacent_tiles;
    std::vector<Tile_Bitmap*>::iterator floor_tile;
    for (floor_tile = m_floor_tiles.begin(); floor_tile != m_floor_tiles.end(); floor_tile++) {
      if (Bitmap_Helper::DoTheseTilesXCollide(*floor_tile, &avatar_avatar_collision_block_bitmap)) {
        if (Bitmap_Helper::AreTheseTilesYAdjacent(*floor_tile, &avatar_avatar_collision_block_bitmap)) {
          adjacent_tiles.push_back(*floor_tile);
        }
      }
    }

    if (adjacent_tiles.empty()) {
      return false;
    }
  } else {
    return false;
  }

  // Adjacent tiles!
  return true;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::IsAvatarFloorColliding(std::vector<Wall_Collision> *out_collisions) {
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot
  if (!m_floor_tiles.empty()) {
    // Get the lowest block
    Avatar_Component::Avatar_Collision_Block lowest_avatar_collision_block;
    lowest_avatar_collision_block = GetLowestCollisionBlock(m_avatar->GetState().avatar_collision_blocks);

    // Make a bitmap for the lowest avatar collision block
    Bitmap_Component out_avatar_collision_block = CollisionBlockToBitmapComponent(lowest_avatar_collision_block, *(m_avatar->GetPosition()));

    // Create a list of floor tiles which are colliding with the collision block
    std::vector<Tile_Bitmap*>::iterator border_tile;
    for (border_tile = m_floor_tiles.begin(); border_tile != m_floor_tiles.end(); border_tile++) {
      if (out_avatar_collision_block.GetBottomRightPostion().y < (*border_tile)->GetTopLeftPostion().y) {
        D3DXVECTOR2 Tile_Top_Left;
        Tile_Top_Left.x = (*border_tile)->GetTopLeftPostion().x;
        Tile_Top_Left.y = (*border_tile)->GetTopLeftPostion().y;
        D3DXVECTOR2 Tile_Top_Right;
        Tile_Top_Right.x = (*border_tile)->GetBottomRightPostion().x;
        Tile_Top_Right.y = (*border_tile)->GetTopLeftPostion().y;
        D3DXVECTOR2 Tile_Bottom_Left;
        Tile_Bottom_Left.x = (*border_tile)->GetTopLeftPostion().x;
        Tile_Bottom_Left.y = (*border_tile)->GetBottomRightPostion().y; 
        D3DXVECTOR2 Tile_Bottom_Right;
        Tile_Bottom_Right.x = (*border_tile)->GetBottomRightPostion().x;
        Tile_Bottom_Right.y = (*border_tile)->GetBottomRightPostion().y; 

        D3DXVECTOR2 Current_Avatar_Position;
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          Current_Avatar_Position.x = out_avatar_collision_block.GetBottomRightPostion().x;
        } else {
          Current_Avatar_Position.x = out_avatar_collision_block.GetTopLeftPostion().x;
        }
        Current_Avatar_Position.y = out_avatar_collision_block.GetBottomRightPostion().y;

        Avatar_Component::Avatar_Collision_Block lowest_last_collision_block;
        lowest_last_collision_block = GetLowestCollisionBlock(m_last_state.state.avatar_collision_blocks);
        Bitmap_Component lowest_last_collision_bitmap = CollisionBlockToBitmapComponent(lowest_last_collision_block, m_last_state.position);
        D3DXVECTOR2 Last_Avatar_Position;
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          Last_Avatar_Position.x = lowest_last_collision_bitmap.GetBottomRightPostion().x;
        } else {
          Last_Avatar_Position.x = lowest_last_collision_bitmap.GetTopLeftPostion().x;
        }
        Last_Avatar_Position.y = lowest_last_collision_bitmap.GetBottomRightPostion().y;

        D3DXVECTOR2 floor_intersection;
        bool isthereafloor_intersection = DoTheseLinesIntersect(Tile_Top_Left, Tile_Top_Right, Current_Avatar_Position, Last_Avatar_Position, &floor_intersection);
        double floor_intersection_disance = WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, floor_intersection);

        D3DXVECTOR2 left_wall_intersection;
        bool istherealeft_wall_intersection = false;
        double left_wall_intersection_disance = 0;
        if ((*border_tile)->IsWall()) {
          istherealeft_wall_intersection = DoTheseLinesIntersect(Tile_Top_Left, Tile_Bottom_Left, Current_Avatar_Position, Last_Avatar_Position, &left_wall_intersection);
          left_wall_intersection_disance = WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, left_wall_intersection);
        }

        D3DXVECTOR2 right_wall_intersection;
        bool istherearight_wall_intersection = false;
        double right_wall_intersection_distance = 0;
        if ((*border_tile)->IsWall()) { 
          istherearight_wall_intersection = DoTheseLinesIntersect(Tile_Top_Right, Tile_Bottom_Right, Current_Avatar_Position, Last_Avatar_Position, &right_wall_intersection);
          right_wall_intersection_distance = WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, right_wall_intersection);
        }

         if (isthereafloor_intersection) {
          if (istherealeft_wall_intersection) {
            if (floor_intersection_disance < left_wall_intersection_disance) {
              Avatar_Controller::Wall_Collision collision;
              collision.colliding_tile = *border_tile;
              std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheTopOrBottom(*border_tile, &out_avatar_collision_block);
              collision.collision_side = collision_side;
              out_collisions->push_back(collision);
            }
          } else if (istherearight_wall_intersection) {
            if (floor_intersection_disance < right_wall_intersection_distance) {
              Avatar_Controller::Wall_Collision collision;
              collision.colliding_tile = *border_tile;
              std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheTopOrBottom(*border_tile, &out_avatar_collision_block);
              collision.collision_side = collision_side;
              out_collisions->push_back(collision);
            }
          } else {
            Avatar_Controller::Wall_Collision collision;
            collision.colliding_tile = *border_tile;
            std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheRightOrLeft(*border_tile, &out_avatar_collision_block);
            collision.collision_side = collision_side;
            out_collisions->push_back(collision);
          }
        }
      }
    }

    // If colliding tiles is not empty
    // This means the avatar is colliding with a tile.
    if (!out_collisions->empty()) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::IsAvatarWallColliding(std::vector<Wall_Collision> *out_collisions) {
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot
  if (!m_wall_tiles.empty()) {
    // Get the lowest block
    Avatar_Component::Avatar_Collision_Block lowest_avatar_collision_block;
    lowest_avatar_collision_block = GetLowestCollisionBlock(m_avatar->GetState().avatar_collision_blocks);

    // Make a bitmap for the lowest avatar collision block
    Bitmap_Component out_avatar_collision_block = CollisionBlockToBitmapComponent(lowest_avatar_collision_block, *(m_avatar->GetPosition()));

    // Create a list of floor tiles which are colliding with the collision block
    std::vector<Tile_Bitmap*>::iterator border_tile;
    for (border_tile = m_wall_tiles.begin(); border_tile != m_wall_tiles.end(); border_tile++) {
       if (m_avatar->GetState().direction.compare("Right") == 0 && (*border_tile)->IsRightWall() && m_avatar->GetBottomRightPostion().x > (*border_tile)->GetTopLeftPostion().x ||
           m_avatar->GetState().direction.compare("Left") == 0 && (*border_tile)->IsLeftWall() && m_avatar->GetTopLeftPostion().x < (*border_tile)->GetBottomRightPostion().x) {
        D3DXVECTOR2 Tile_Top_Left;
        Tile_Top_Left.x = (*border_tile)->GetTopLeftPostion().x;
        Tile_Top_Left.y = (*border_tile)->GetTopLeftPostion().y;
        D3DXVECTOR2 Tile_Top_Right;
        Tile_Top_Right.x = (*border_tile)->GetBottomRightPostion().x;
        Tile_Top_Right.y = (*border_tile)->GetTopLeftPostion().y;
        D3DXVECTOR2 Tile_Bottom_Left;
        Tile_Bottom_Left.x = (*border_tile)->GetTopLeftPostion().x;
        Tile_Bottom_Left.y = (*border_tile)->GetBottomRightPostion().y; 
        D3DXVECTOR2 Tile_Bottom_Right;
        Tile_Bottom_Right.x = (*border_tile)->GetBottomRightPostion().x;
        Tile_Bottom_Right.y = (*border_tile)->GetBottomRightPostion().y; 

        D3DXVECTOR2 Current_Avatar_Position;
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          Current_Avatar_Position.x = out_avatar_collision_block.GetBottomRightPostion().x;
        } else {
          Current_Avatar_Position.x = out_avatar_collision_block.GetTopLeftPostion().x;
        }
        Current_Avatar_Position.y = out_avatar_collision_block.GetBottomRightPostion().y;

        Avatar_Component::Avatar_Collision_Block lowest_last_collision_block;
        lowest_last_collision_block = GetLowestCollisionBlock(m_last_state.state.avatar_collision_blocks);
        Bitmap_Component lowest_last_collision_bitmap = CollisionBlockToBitmapComponent(lowest_last_collision_block, m_last_state.position);
        D3DXVECTOR2 Last_Avatar_Position;
        Last_Avatar_Position.x = lowest_last_collision_bitmap.GetPosition()->x;
        if (m_avatar->GetLastState().direction.compare("Right") == 0) {
          Last_Avatar_Position.x = lowest_last_collision_bitmap.GetBottomRightPostion().x;
        } else {
          Last_Avatar_Position.x = lowest_last_collision_bitmap.GetTopLeftPostion().x;
        }
        Last_Avatar_Position.y = lowest_last_collision_bitmap.GetBottomRightPostion().y;

        D3DXVECTOR2 floor_intersection;
        bool isthereafloor_intersection = DoTheseLinesIntersect(Tile_Top_Left, Tile_Top_Right, Current_Avatar_Position, Last_Avatar_Position, &floor_intersection);
        double floor_intersection_disance = WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, floor_intersection);

        D3DXVECTOR2 left_wall_intersection;
        bool istherealeft_wall_intersection = DoTheseLinesIntersect(Tile_Top_Left, Tile_Bottom_Left, Current_Avatar_Position, Last_Avatar_Position, &left_wall_intersection);
        double left_wall_intersection_disance = WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, left_wall_intersection);;

        D3DXVECTOR2 right_wall_intersection;
        bool istherearight_wall_intersection = DoTheseLinesIntersect(Tile_Top_Right, Tile_Bottom_Right, Current_Avatar_Position, Last_Avatar_Position, &right_wall_intersection);
        double right_wall_intersection_distance = WhatsTheDistanceBetweenThesePoints(Current_Avatar_Position, right_wall_intersection);;

        if (istherealeft_wall_intersection) {
          if (isthereafloor_intersection) {
            if (left_wall_intersection < floor_intersection) {
              Avatar_Controller::Wall_Collision collision;
              collision.colliding_tile = *border_tile;
              std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheRightOrLeft(*border_tile, &out_avatar_collision_block);
              collision.collision_side = collision_side;
              out_collisions->push_back(collision);
            }
          } else {
            Avatar_Controller::Wall_Collision collision;
            collision.colliding_tile = *border_tile;
            std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheRightOrLeft(*border_tile, &out_avatar_collision_block);
            collision.collision_side = collision_side;
            out_collisions->push_back(collision);
          }
        }
        if (istherearight_wall_intersection) {
          if (isthereafloor_intersection) {
            if (right_wall_intersection < floor_intersection) {
              Avatar_Controller::Wall_Collision collision;
              collision.colliding_tile = *border_tile;
              std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheRightOrLeft(*border_tile, &out_avatar_collision_block);
              collision.collision_side = collision_side;
              out_collisions->push_back(collision);
            }
          } else {
            Avatar_Controller::Wall_Collision collision;
            collision.colliding_tile = *border_tile;
            std::string collision_side = Bitmap_Helper::DoesTileACollideOnTheRightOrLeft(*border_tile, &out_avatar_collision_block);
            collision.collision_side = collision_side;
            out_collisions->push_back(collision);
          }
        }
      }
    }

    // If colliding tiles is not empty
    // This means the avatar is colliding with a tile.
    if (!out_collisions->empty()) {
      return true;
    }
  }

  return false;
}

double Avatar_Controller::WhatsTheDistanceBetweenThesePoints(D3DXVECTOR2 point_1, D3DXVECTOR2 point_2)
{
    return sqrt((point_1.x - point_2.y)*(point_1.x - point_2.x) + (point_1.y - point_2.y)*(point_1.y - point_2.y));
}

bool Avatar_Controller::DoTheseLinesIntersect(D3DXVECTOR2 line_a_begin, D3DXVECTOR2 line_a_end, D3DXVECTOR2 line_b_begin, D3DXVECTOR2 line_b_end, D3DXVECTOR2 *out_intersecting_point) {
  if (line_a_begin.y == line_b_begin.y || line_a_begin.y == line_b_end.y) {
    return false;
  }

    float denom = ((line_a_end.y - line_a_begin.y)*(line_b_end.x - line_b_begin.x)) -
                  ((line_a_end.x - line_a_begin.x)*(line_b_end.y - line_b_begin.y));

    float nume_a = ((line_a_end.x - line_a_begin.x)*(line_b_begin.y - line_a_begin.y)) -
                    ((line_a_end.y - line_a_begin.y)*(line_b_begin.x - line_a_begin.x));

    float nume_b = ((line_b_end.x - line_b_begin.x)*(line_b_begin.y - line_a_begin.y)) -
                    ((line_b_end.y - line_b_begin.y)*(line_b_begin.x - line_a_begin.x));

    if(denom == 0.0f)
    {
        if(nume_a == 0.0f && nume_b == 0.0f)
        {
            // COINCIDENT
            return false;
        }
        // PARALLEL
        return false;
    }

    float ua = nume_a / denom;
    float ub = nume_b / denom;

    if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
    {
        // Get the intersection point.
        out_intersecting_point->x = line_b_begin.x + ua*(line_b_end.x - line_b_begin.x);
        out_intersecting_point->y = line_b_begin.y + ua*(line_b_end.y - line_b_begin.y);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
void Avatar_Controller::LoadTilesets(std::wstring wtileset_path) {
  // Running
  m_running_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Running_Animation_Tileset_2_0.txt");
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(m_running_metadata_file_path, &m_running_metadata);
  m_animation_metadata.push_back(m_running_metadata);

  // Walking
  m_walking_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Walking_Animation_Tileset_1_7.txt");
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(m_walking_metadata_file_path, &m_walking_metadata);
  m_animation_metadata.push_back(m_walking_metadata);

  // Standing
  m_standing_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Standing_Animation_Tileset_1_2.txt");
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(m_standing_metadata_file_path, &m_standing_metadata);
  m_animation_metadata.push_back(m_standing_metadata);

  // Falling
  m_falling_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Falling_Animation_Tileset_1_2.txt");
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(m_falling_metadata_file_path, &m_falling_metadata);
  m_animation_metadata.push_back(m_falling_metadata);

  // Thumbs Up (Level Complete)
  m_level_transitioning_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Level_Transition_Animation_Tileset_1_0.txt");
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(m_level_transitioning_metadata_file_path, &m_level_transitioning_metadata);
  m_animation_metadata.push_back(m_level_transitioning_metadata);

  // Jumping
  m_jumping_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Jumping_Animation_Tileset_1_0.txt");
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(m_jumping_metadata_file_path, &m_jumping_metadata);
  m_animation_metadata.push_back(m_jumping_metadata);
}

//------------------------------------------------------------------------------
void Avatar_Controller::AlignAvatarOnRightFoot() {
  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Right_Foot",
                                                   m_avatar->GetState().avatar_collision_blocks);

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Right_Foot",
                                                m_avatar->GetLastState().avatar_collision_blocks);

  D3DXVECTOR3 right_foot_offset;
  right_foot_offset.x = static_cast<float>(last_avatar_collision_block.offset_from_avatar_centre.x -
                                           current_avatar_collision_block.offset_from_avatar_centre.x);
  right_foot_offset.y = static_cast<float>(last_avatar_collision_block.offset_from_avatar_centre.y -
                                           current_avatar_collision_block.offset_from_avatar_centre.y);
  right_foot_offset.z = 0;

  D3DXVECTOR3 right_foot_size_offset = D3DXVECTOR3(0, 0, 0);
  if (last_avatar_collision_block.size != current_avatar_collision_block.size) {
    right_foot_size_offset.x  = (last_avatar_collision_block.size.x /2) - (current_avatar_collision_block.size.x /2);
    right_foot_size_offset.y  = (last_avatar_collision_block.size.y /2) - (current_avatar_collision_block.size.y /2);
    right_foot_size_offset.z  = 0;
  }

  D3DXVECTOR3 current_avatar_position = *m_avatar->GetPosition();
  new_avatar_position = current_avatar_position + (right_foot_offset - right_foot_size_offset);
  m_avatar->SetPosition(new_avatar_position);
}

//------------------------------------------------------------------------------
void Avatar_Controller::AlignAvatarOnLeftFoot() {
  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Left_Foot",
                                                   m_avatar->GetState().avatar_collision_blocks);

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Left_Foot",
                                                m_avatar->GetLastState().avatar_collision_blocks);

  D3DXVECTOR3 right_foot_offset;
  right_foot_offset.x = static_cast<float>(last_avatar_collision_block.offset_from_avatar_centre.x -
                                           current_avatar_collision_block.offset_from_avatar_centre.x);
  right_foot_offset.y = static_cast<float>(last_avatar_collision_block.offset_from_avatar_centre.y -
                                           current_avatar_collision_block.offset_from_avatar_centre.y);
  right_foot_offset.z = 0;

  D3DXVECTOR3 right_foot_size_offset = D3DXVECTOR3(0, 0, 0);
  if (last_avatar_collision_block.size != current_avatar_collision_block.size) {
    right_foot_size_offset.x  = (last_avatar_collision_block.size.x /2) - (current_avatar_collision_block.size.x /2);
    right_foot_size_offset.y  = (last_avatar_collision_block.size.y /2) - (current_avatar_collision_block.size.y /2);
    right_foot_size_offset.z  = 0;
  }

  D3DXVECTOR3 current_avatar_position = *m_avatar->GetPosition();
  new_avatar_position = current_avatar_position + (right_foot_offset + right_foot_size_offset);
  m_avatar->SetPosition(new_avatar_position);
}

//------------------------------------------------------------------------------
void Avatar_Controller::AlignAvatarOnLastAvatarCollisionBlock() {
  if (m_avatar->GetLastState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks);
  Bitmap_Component current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(m_avatar->GetPosition()));

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetLastState().avatar_collision_blocks);
  Bitmap_Component last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, *(m_avatar->GetPosition()));

  if (current_collision_bitmap.GetBottomRightPostion().y != last_collision_bitmap.GetBottomRightPostion().y) {
    D3DXVECTOR3 right_foot_offset;
    right_foot_offset.x = static_cast<float>(last_avatar_collision_block.size.x -
                                             current_avatar_collision_block.size.x);
    if (m_avatar->GetState().direction.compare("Left") == 0) { right_foot_offset.x = right_foot_offset.x * -1; }
    right_foot_offset.y = static_cast<float>(last_avatar_collision_block.size.y -
                                             current_avatar_collision_block.size.y);
    right_foot_offset.z = 0;


    D3DXVECTOR3 current_avatar_position = *m_avatar->GetPosition();
    new_avatar_position.x = current_avatar_position.x - (right_foot_offset.x / 2);
    new_avatar_position.y = current_avatar_position.y - (right_foot_offset.y / 2);
    new_avatar_position.z = current_avatar_position.z;

    m_avatar->SetPosition(new_avatar_position);

    D3DXVECTOR3 avatar_position_offset;
    avatar_position_offset.x = current_avatar_collision_block.offset_from_avatar_centre.x;
    avatar_position_offset.y = current_avatar_collision_block.offset_from_avatar_centre.y;
    avatar_position_offset.z = 0;

    current_avatar_position = *m_avatar->GetPosition();
    new_avatar_position.x = current_avatar_position.x + avatar_position_offset.x;
    new_avatar_position.y = current_avatar_position.y - avatar_position_offset.y;
    new_avatar_position.z = current_avatar_position.z;

    m_avatar->SetPosition(new_avatar_position);
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::AlignAvatarOnLastAvatarCollisionBlockX() {
  if (m_avatar->GetLastState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Avatar_Collision_Block current_avatar_collision_block;
  current_avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks);
  Bitmap_Component current_collision_bitmap = CollisionBlockToBitmapComponent(current_avatar_collision_block, *(m_avatar->GetPosition()));

  Avatar_Component::Avatar_Collision_Block last_avatar_collision_block;
  last_avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetLastState().avatar_collision_blocks);
  Bitmap_Component last_collision_bitmap = CollisionBlockToBitmapComponent(last_avatar_collision_block, *(m_avatar->GetPosition()));

  if (current_collision_bitmap.GetBottomRightPostion().y != last_collision_bitmap.GetBottomRightPostion().y) {
    D3DXVECTOR3 difference;
    difference.x = 0;
    difference.y = current_collision_bitmap.GetBottomRightPostion().y - last_collision_bitmap.GetBottomRightPostion().y;
    difference.z = 0;

    D3DXVECTOR3 new_avatar_position = *m_avatar->GetPosition();
    new_avatar_position += difference;

    m_avatar->SetPosition(new_avatar_position);
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::MoveAvatarTileAdjacent(std::string direction, Bitmap_Component* tile) {
  D3DXVECTOR3 new_avatar_position = *m_avatar->GetPosition();
  if (direction.compare("Right") == 0) {
    D3DXVECTOR3 tile_position = tile->GetBottomRightPostion();
    Avatar_Component::Avatar_Collision_Block avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks);
    float foot_x_offset = avatar_collision_block.offset_from_avatar_centre.x + (avatar_collision_block.size.x / 2);
    new_avatar_position.x = tile_position.x + foot_x_offset;
  } else if (direction.compare("Left") == 0) {
    D3DXVECTOR3 tile_position = tile->GetTopLeftPostion();
    float foot_x_offset = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).offset_from_avatar_centre.x + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.x / 2);
    new_avatar_position.x = tile_position.x - foot_x_offset;
  } else if (direction.compare("Top") == 0) {
    D3DXVECTOR3 tile_position = tile->GetTopLeftPostion();
    float foot_x_offset = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).offset_from_avatar_centre.y + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks).size.y / 2);
    new_avatar_position.y = tile_position.y + foot_x_offset;
  }
  m_avatar->SetPosition(new_avatar_position);
}

//------------------------------------------------------------------------------
void Avatar_Controller::MoveAvatarFloorAdjacent() {
  std::vector<Wall_Collision> out_colliding_wall_tiles;
  bool is_floor_colliding = IsAvatarFloorColliding(&out_colliding_wall_tiles);
  if (is_floor_colliding) {
    D3DXVECTOR3 new_avatar_position = *m_avatar->GetPosition();
    D3DXVECTOR3 tile_position = (*out_colliding_wall_tiles.begin()).colliding_tile->GetTopLeftPostion();
    Avatar_Component::Avatar_Collision_Block avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks);
    float foot_x_offset = avatar_collision_block.offset_from_avatar_centre.y + (avatar_collision_block.size.y / 2);
    new_avatar_position.y = tile_position.y + foot_x_offset;
    m_avatar->SetPosition(new_avatar_position);
  }
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
    milliseconds_per_frame = static_cast<int>(1000/24);
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

//------------------------------------------------------------------------------
void Avatar_Controller::SetAvatarState(std::string new_state_parent_name, std::string new_state_name, std::string direction) {
  Avatar_Component::Avatar_State new_state;
  Tileset_Helper::Animation_Tileset_Metadata new_state_metadata;
  Tileset_Helper::Animation_Subset new_animation_subset;

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  std::vector<Tileset_Helper::Animation_Tileset_Metadata>::iterator metadata;
  for (metadata = m_animation_metadata.begin(); metadata != m_animation_metadata.end(); metadata++) {
    if (metadata->name.compare(new_state_parent_name) == 0) {
      std::vector<Tileset_Helper::Animation_Subset>::iterator tileset;
      for (tileset = (*metadata).subsets.begin(); tileset != (*metadata).subsets.end(); tileset++) {
        if (tileset->name.compare(new_state_name) == 0) {
          new_animation_subset = *tileset;
          new_state_metadata = (*metadata);
          new_state.direction = direction;
          new_state.state = new_state_name;
          new_state.parent_state = metadata->name;
          new_state.max_state_index = tileset->number_of_frames;
        }
      }
    }
  }

  if (new_state_metadata.name.compare("") == 0) {
    std::string error;
    error = "Animation " + new_state_name;
    error += " not found in State " + new_state_parent_name + " Metadata";
    throw Exceptions::init_error(error);
  }

  new_state.state_index = 0;
  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(new_state_metadata.filename);

  m_avatar->GetTexture()->texture_path = texture_path;
  m_avatar->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(new_state_metadata.size_x),
                                                     static_cast<float>(new_state_metadata.size_y));
  m_avatar->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(new_animation_subset.tile_size_x),
                                                  static_cast<float>(new_animation_subset.tile_size_y));
  m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(new_animation_subset.top_left_x),
                                                          static_cast<float>((new_animation_subset.top_left_y) * -1));
  m_avatar->SetSize(D3DXVECTOR2(static_cast<float>(new_animation_subset.tile_size_x),
                                static_cast<float>(new_animation_subset.tile_size_y)));

  // Find the initial frame of the new tile set state
  Tileset_Helper::Frame_Metadata initial_frame;
  std::vector<Tileset_Helper::Frame_Metadata>::iterator frame;
  for (frame = new_animation_subset.frames.begin(); frame != new_animation_subset.frames.end(); frame++) {
    if (frame->id == 1) {
      initial_frame = (*frame);
    }
  }

    // Create new collision block from the initial frame collision block
  std::vector<Tileset_Helper::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = initial_frame.avatar_collision_blocks.begin(); avatar_collision_block != initial_frame.avatar_collision_blocks.end(); avatar_collision_block++) {
    Avatar_Component::Avatar_Collision_Block new_avatar_collision_block;
    new_avatar_collision_block = TilesetCollisionBlockToAvatarCollisionBlock((*avatar_collision_block),
                                                                      new_animation_subset.top_left_y,
                                                                      new_state.state_index);
    new_state.avatar_collision_blocks.push_back(new_avatar_collision_block);
  }

  m_avatar->SetState(new_state);
  m_current_animation_fps = new_animation_subset.frames_per_second;

  m_avatar->GetTexture()->texture = 0;
  m_avatar->GetFrame()->vertex_buffer = 0;
  m_avatar->GetFrame()->index_buffer = 0;
  m_avatar->Init();

  m_current_metadata_file_path = new_state_metadata.filename;
  m_current_metadata = new_state_metadata;
  m_current_animation_subset = new_animation_subset;
}

//------------------------------------------------------------------------------
void Avatar_Controller::SetAvatarStateAnimationFrame(int new_state_index) {
  Avatar_Component::Avatar_State incremented_state;

  incremented_state.state = m_avatar->GetState().state;
  incremented_state.direction = m_avatar->GetState().direction;
  incremented_state.state_index = new_state_index; 
  incremented_state.parent_state = m_avatar->GetState().parent_state;

  // Set new bitmap frame location on the Tileset
  m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(m_current_animation_subset.top_left_x + (new_state_index * m_current_animation_subset.tile_size_x)),
                                                          static_cast<float>(m_current_animation_subset.top_left_y * -1));

  // Find the new frame of the current tile set
  Tileset_Helper::Frame_Metadata new_frame;
  std::vector<Tileset_Helper::Frame_Metadata>::iterator frame;
  for (frame = m_current_animation_subset.frames.begin(); frame != m_current_animation_subset.frames.end(); frame++) {
    if (frame->id == (new_state_index + 1)) {
      new_frame = (*frame);
    }
  }

  // Create new collision block from the initial frame collision block
  std::vector<Tileset_Helper::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = new_frame.avatar_collision_blocks.begin(); avatar_collision_block != new_frame.avatar_collision_blocks.end(); avatar_collision_block++) {
    Avatar_Component::Avatar_Collision_Block new_avatar_collision_block;
    new_avatar_collision_block = TilesetCollisionBlockToAvatarCollisionBlock((*avatar_collision_block),
                                                                      m_current_animation_subset.top_left_y,
                                                                      new_state_index);
    incremented_state.avatar_collision_blocks.push_back(new_avatar_collision_block);
  }

  m_avatar->SetState(incremented_state);

  m_avatar->GetTexture()->texture = 0;
  m_avatar->GetFrame()->vertex_buffer = 0;
  m_avatar->GetFrame()->index_buffer = 0;
  m_avatar->Init();
}

//------------------------------------------------------------------------------
Avatar_Component::Avatar_Collision_Block Avatar_Controller::GetLowestCollisionBlock(std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks) {
  Avatar_Component::Avatar_Collision_Block lowest_avatar_collision_block;

  lowest_avatar_collision_block = (*avatar_collision_blocks.begin());

  std::vector<Avatar_Component::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = avatar_collision_blocks.begin(); avatar_collision_block != avatar_collision_blocks.end(); avatar_collision_block++) {
    float avatar_collision_block_bottom = avatar_collision_block->offset_from_avatar_centre.y - avatar_collision_block->size.y;
    float lowest_avatar_collision_block_bottom = lowest_avatar_collision_block.offset_from_avatar_centre.y - lowest_avatar_collision_block.size.y;
    if (avatar_collision_block_bottom < lowest_avatar_collision_block_bottom) {
      lowest_avatar_collision_block = (*avatar_collision_block);
    }
  }

  return lowest_avatar_collision_block;
}

//------------------------------------------------------------------------------
Avatar_Component::Avatar_Collision_Block Avatar_Controller::GetLowestMostForwardFootCollisionBlock(std::string direction) {
  Avatar_Component::Avatar_Collision_Block lowest_avatar_collision_block;

  std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks;
  avatar_collision_blocks = m_avatar->GetState().avatar_collision_blocks;

  std::vector<Avatar_Component::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = avatar_collision_blocks.begin(); avatar_collision_block != avatar_collision_blocks.end(); avatar_collision_block++) {
    if ((avatar_collision_block->id.compare("Right_Foot") == 0 || avatar_collision_block->id.compare("Left_Foot") == 0)) {
      if (lowest_avatar_collision_block.id.compare("") == 0) { 
        lowest_avatar_collision_block = (*avatar_collision_block); 
      } else {
        float avatar_collision_block_bottom = avatar_collision_block->offset_from_avatar_centre.y - avatar_collision_block->size.y;
        float lowest_avatar_collision_block_bottom = lowest_avatar_collision_block.offset_from_avatar_centre.y - lowest_avatar_collision_block.size.y;
        if (avatar_collision_block_bottom < lowest_avatar_collision_block_bottom) {
          lowest_avatar_collision_block = (*avatar_collision_block);
        }
        if (avatar_collision_block_bottom == lowest_avatar_collision_block_bottom) {
          if (direction.compare("Right") == 0) {
            float avatar_collision_block_right = avatar_collision_block->offset_from_avatar_centre.x + avatar_collision_block->size.x;
            float lowest_avatar_collision_block_right = lowest_avatar_collision_block.offset_from_avatar_centre.x + lowest_avatar_collision_block.size.x;
            if (avatar_collision_block_right > lowest_avatar_collision_block_right) {
              lowest_avatar_collision_block = (*avatar_collision_block);
            }
          }
          if (direction.compare("Left") == 0) {
            float avatar_collision_block_right = avatar_collision_block->offset_from_avatar_centre.x - avatar_collision_block->size.x;
            float lowest_avatar_collision_block_right = lowest_avatar_collision_block.offset_from_avatar_centre.x - lowest_avatar_collision_block.size.x;
            if (avatar_collision_block_right < lowest_avatar_collision_block_right) {
              lowest_avatar_collision_block = (*avatar_collision_block);
            }
          }
        }
      }
    }
  }

  return lowest_avatar_collision_block;
}

//------------------------------------------------------------------------------
Bitmap_Component Avatar_Controller::CollisionBlockToBitmapComponent(Avatar_Component::Avatar_Collision_Block avatar_collision_block, D3DXVECTOR3 position) {
  Bitmap_Component collision_bitmap;

  D3DXVECTOR3 collision_bitmap_position;
  collision_bitmap_position.x = position.x + avatar_collision_block.offset_from_avatar_centre.x;
  collision_bitmap_position.y = position.y + avatar_collision_block.offset_from_avatar_centre.y;
  collision_bitmap_position.z = position.z;

  collision_bitmap.SetPosition(collision_bitmap_position);

  collision_bitmap.SetSize(avatar_collision_block.size.x, avatar_collision_block.size.y);

  return collision_bitmap;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_Collision_Block Avatar_Controller::GetNamedCollisionBlock(std::string id, std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks) {
  Avatar_Component::Avatar_Collision_Block found_avatar_collision_block;

  Avatar_Component::Avatar_Collision_Block* current_right_foot_avatar_collision_block = 0;

  std::vector<Avatar_Component::Avatar_Collision_Block>::iterator avatar_collision_block;
  for (avatar_collision_block = avatar_collision_blocks.begin(); avatar_collision_block != avatar_collision_blocks.end(); avatar_collision_block++) {
    if (avatar_collision_block->id.compare(id) == 0) {
      found_avatar_collision_block = (*avatar_collision_block);
    }
  }

  return found_avatar_collision_block;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_Collision_Block Avatar_Controller::TilesetCollisionBlockToAvatarCollisionBlock(Tileset_Helper::Avatar_Collision_Block tileset_avatar_collision_block, 
                                                                                                 float tileset_animation_top_left_y,
                                                                                                 int state_index) {
  Avatar_Component::Avatar_Collision_Block new_avatar_collision_block;

  // Create new collision block from the initial frame collision block
  new_avatar_collision_block.id = tileset_avatar_collision_block.id;
  new_avatar_collision_block.is_contacting = tileset_avatar_collision_block.is_contacting;
  new_avatar_collision_block.size.x = static_cast<float>(tileset_avatar_collision_block.size_x);
  new_avatar_collision_block.size.y = static_cast<float>(tileset_avatar_collision_block.size_y);

  // Need to change the coordinate information from the "animation sheet" to
  // the coordinate system in the game world.
  // Create a frame for the avatar foot
  Frame_Component avatar_contact_foot;
  D3DXVECTOR3 avatar_collision_block_tilesheet_centre;
  avatar_collision_block_tilesheet_centre.x = static_cast<float>(tileset_avatar_collision_block.top_left_x + (tileset_avatar_collision_block.size_x /2));
  avatar_collision_block_tilesheet_centre.y = static_cast<float>(tileset_avatar_collision_block.top_left_y - (tileset_avatar_collision_block.size_y /2));

  // Need to account for different positions in the frame and the avatar
  // Work out the center of the avatar frame (128x128 block) int the Tileset
  // frame is 128x128 so get the frame # and times it by 128/2 for y
  D3DXVECTOR3 animation_frame_centre;
  animation_frame_centre.x = static_cast<float>(((state_index + 1) * 128) - (128 / 2));
  // and 128/2 for x
  animation_frame_centre.y = static_cast<float>(((tileset_animation_top_left_y) - (128 / 2)));
  animation_frame_centre.z = static_cast<float>(-2);

  // store the distance from x and y to the center of the animation frame
  new_avatar_collision_block.offset_from_avatar_centre.x = avatar_collision_block_tilesheet_centre.x - animation_frame_centre.x;
  new_avatar_collision_block.offset_from_avatar_centre.y = avatar_collision_block_tilesheet_centre.y - animation_frame_centre.y;

  if (m_avatar->GetState().direction.compare("Left") == 0) {
    // We need to reverse the x on the collision block.
    if (new_avatar_collision_block.id.compare("Left_Foot") == 0) {
      new_avatar_collision_block.id = "Right_Foot";
      new_avatar_collision_block.offset_from_avatar_centre.x = (new_avatar_collision_block.offset_from_avatar_centre.x * -1);
    } else if (new_avatar_collision_block.id.compare("Right_Foot") == 0) {
      new_avatar_collision_block.id = "Left_Foot";
      new_avatar_collision_block.offset_from_avatar_centre.x = (new_avatar_collision_block.offset_from_avatar_centre.x * -1);
    }
  }

  return new_avatar_collision_block;
}

void Avatar_Controller::ClearAvatarState() {
  Avatar_Component::Avatar_State cleared_state;
  cleared_state.state = "";
  cleared_state.direction = "";
  cleared_state.state_index = 0;
  m_avatar->SetCommand(cleared_state);
}

}  // Tunnelour
