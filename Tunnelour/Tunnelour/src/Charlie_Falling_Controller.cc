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

#include "Charlie_Falling_Controller.h"

#include "Exceptions.h"
#include "Avatar_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Charlie_Falling_Controller::Charlie_Falling_Controller() {
  m_avatar_z_position = -2;  // Middleground Z Space is -1
  m_vertical_jump_y_initial_Velocity = 22;
  m_vertical_jump_x_initial_Velocity = 4;
  m_wall_jump_speed_offset = 16;
  m_falling_point_of_safe_landing = -256.0f;
}

//------------------------------------------------------------------------------
Charlie_Falling_Controller::~Charlie_Falling_Controller() {
}

//------------------------------------------------------------------------------
bool Charlie_Falling_Controller::Init(Component_Composite *const model) {
  Avatar_State_Controller::Init(model);
  m_has_been_initialised = true;
  return true;
}

//------------------------------------------------------------------------------
void Charlie_Falling_Controller::Run_Avatar_State() {
  bool has_state_changed = false;

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  // If you are in the Falling state you have no control of the Avatar
  unsigned int state_index = current_state.state_index;
  state_index++;
  if (state_index > (m_current_animation_subset->number_of_frames - 1)) {
    if (m_current_animation_subset->is_repeatable) {
      // Repeat the current state
      state_index = 0;
      Avatar_Helper::SetAvatarStateAnimationFrame(m_avatar, state_index, m_current_animation_subset);
    } else {
      // Find the next state to transfer into
      if (current_state.state.compare("Down_Facing_Falling_To_Death") == 0) {
        // DEAD
        Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Facing_Death", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
        state_index = 0;
        current_state = m_avatar->GetState();
      } else if (current_state.state.compare("Down_Falling_Wall_Impact_Right") == 0) {
        Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Up_Facing_Falling", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(-2, old_velocity.y, 0);
          m_avatar->SetVelocity(new_velocity);
        } else {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(2, old_velocity.y, 0);
          m_avatar->SetVelocity(new_velocity);
        }
        state_index = 0;
        current_state = m_avatar->GetState();
      } else if (current_state.state.compare("Up_Facing_Falling_To_Death") == 0) {
        Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Up_Facing_Death", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
        state_index = 0;
        current_state = m_avatar->GetState();
      } else if (current_state.state.compare("Up_Falling_Wall_Impact_Left") == 0) {
        Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Facing_Falling", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(2, old_velocity.y, 0);
          m_avatar->SetVelocity(new_velocity);
        } else {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(-2, old_velocity.y, 0);
          m_avatar->SetVelocity(new_velocity);
        }
        state_index = 0;
        current_state = m_avatar->GetState();
      } else {
        std::string error;
        error = "No handling for the non-repeating animation: " + current_state.state;
        throw Exceptions::init_error(error);
      }
    }
  } else {
    if (m_avatar->GetState() == m_avatar->GetLastRenderedState()) {
      Avatar_Helper::SetAvatarStateAnimationFrame(m_avatar, state_index, m_current_animation_subset);
      Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
    }
  }

  if (current_state.state.compare("Down_Facing_Falling_To_Death") != 0 &&
      current_state.state.compare("Down_Facing_Death") != 0  &&
      current_state.state.compare("Up_Facing_Falling_To_Death") != 0  &&
      current_state.state.compare("Down_Falling_Wall_Impact_Right") != 0  &&
      current_state.state.compare("Up_Falling_Wall_Impact_Left") != 0  &&
      current_state.state.compare("Up_Facing_Death") != 0) {
    D3DXVECTOR3 position = *m_avatar->GetPosition();
    position = *m_avatar->GetPosition();
    D3DXVECTOR3 new_velocity = m_avatar->GetVelocity();
    new_velocity.y += m_world_settings->GetGravityInPixPerFrame();
    if ((new_velocity.y/((*m_last_frame_time) * -1)) >= m_world_settings->GetMaxVelocityInPixPerMs()) {
      new_velocity.y = ceil((-1 * m_world_settings->GetMaxVelocityInPixPerMs() * (*m_last_frame_time)));
    }

    m_avatar->SetVelocity(new_velocity);
    position += new_velocity;
    (*m_y_fallen) += m_avatar->GetVelocity().y;
    m_avatar->SetPosition(position);
  }

  std::vector<Avatar_Helper::Tile_Collision> out_colliding_wall_tiles;
  bool is_wall_colliding = false;
  is_wall_colliding = Avatar_Helper::IsAvatarWallColliding(m_avatar, &out_colliding_wall_tiles, &m_wall_tiles);
  if (is_wall_colliding) {
    (*m_currently_adjacent_wall_tile) = *(out_colliding_wall_tiles.begin());
    if (out_colliding_wall_tiles.begin()->collision_side.compare("Left") == 0) {
      if (current_state.state.compare("Down_Falling_Wall_Impact_Right") != 0 &&
          m_avatar->GetLastRenderedState().state.compare("Down_Falling_Wall_Impact_Right") != 0 &&
         current_state.state.compare("Up_Falling_Wall_Impact_Left") != 0 &&
         m_avatar->GetLastRenderedState().state.compare("Up_Falling_Wall_Impact_Left") != 0) {
        if (current_state.state.compare("Down_Facing_Falling_To_Death") != 0 &&
            current_state.state.compare("Down_Facing_Death") != 0  &&
            current_state.state.compare("Up_Facing_Falling_To_Death") != 0  &&
            current_state.state.compare("Up_Facing_Death") != 0) {
          if (m_avatar->GetState().direction.compare("Right") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Falling_Wall_Impact_Right", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Up_Falling_Wall_Impact_Left", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
          }
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(0, old_velocity.y, 0);
          m_avatar->SetVelocity(new_velocity);
        }
      }
      Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Left", out_colliding_wall_tiles.begin()->colliding_tile);
    } else {
      if (current_state.state.compare("Down_Falling_Wall_Impact_Right") != 0 &&
          m_avatar->GetLastRenderedState().state.compare("Down_Falling_Wall_Impact_Right") != 0 &&
          current_state.state.compare("Up_Falling_Wall_Impact_Left") != 0 &&
          m_avatar->GetLastRenderedState().state.compare("Up_Falling_Wall_Impact_Left") != 0) {
        if (current_state.state.compare("Down_Facing_Falling_To_Death") != 0 &&
            current_state.state.compare("Down_Facing_Death") != 0  &&
            current_state.state.compare("Up_Facing_Falling_To_Death") != 0  &&
            current_state.state.compare("Up_Facing_Death") != 0) {
          if (m_avatar->GetState().direction.compare("Right") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Up_Falling_Wall_Impact_Left", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Falling_Wall_Impact_Right", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
          }

          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(0, old_velocity.y, 0);
          m_avatar->SetVelocity(new_velocity);
        }
      }
      Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Right", out_colliding_wall_tiles.begin()->colliding_tile);
    }
  }

  std::vector<Avatar_Helper::Tile_Collision> out_colliding_floor_tiles;
  Bitmap_Component* out_avatar_collision_block = new Bitmap_Component();
  bool is_colliding = Avatar_Helper::IsAvatarFloorColliding(m_avatar, &out_colliding_floor_tiles, &m_floor_tiles);
  if (is_colliding) {
    if ((*m_y_fallen) < m_falling_point_of_safe_landing) {
      if (current_state.state.compare("Down_Facing_Falling") == 0 || current_state.state.compare("Down_Falling_Wall_Impact_Right") == 0) {
        Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Facing_Falling_To_Death", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
      } else if (current_state.state.compare("Up_Facing_Falling") == 0 || current_state.state.compare("Up_Falling_Wall_Impact_Left") == 0) {
        Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Up_Facing_Falling_To_Death", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
      }
      Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
      Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Top", out_colliding_floor_tiles.begin()->colliding_tile);
      has_state_changed = true;
    } else {
      Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Standing", "Standing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
      Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
      m_y_fallen = 0;
      Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Top", out_colliding_floor_tiles.begin()->colliding_tile);
      has_state_changed = true;
    }
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
