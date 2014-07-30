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

#include "Charlie_Jumping_Controller.h"
#include "Avatar_Helper.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Charlie_Jumping_Controller::Charlie_Jumping_Controller() {
  // Constant Variables
  m_avatar_z_position = -2; // Middleground Z Space is -1
  m_vertical_jump_y_initial_Velocity = 22;
  m_vertical_jump_x_initial_Velocity = 4;
  m_wall_jump_speed_offset = 16;
}

//------------------------------------------------------------------------------
Charlie_Jumping_Controller::~Charlie_Jumping_Controller() {
}

//------------------------------------------------------------------------------
bool Charlie_Jumping_Controller::Init(Component_Composite *const model) {
  Avatar_State_Controller::Init(model);
  m_has_been_initialised = true;
  return true;
}

//------------------------------------------------------------------------------
void Charlie_Jumping_Controller::Run_Avatar_State() {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State last_state = m_avatar->GetLastRenderedState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();
  bool has_state_changed = false;

  if (current_command.state.compare("Down") == 0 || current_command.state.compare("Looking") == 0 ) {
    current_command.state = "";
  }

  int state_index = current_state.state_index;
  state_index++;
  if (current_state == last_state) {
    if (state_index > (m_current_animation_subset->number_of_frames - 1)) {
      if (m_current_animation_subset->is_repeatable) {
        if (current_state.state.compare("Wall_Jump_Rising") == 0) {
          if (m_avatar->GetVelocity().y <= 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Fall_Arc", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
            has_state_changed = true;
          }
        } else if (current_state.state.compare("Wall_Jump_Falling") == 0) {
          if ((*m_y_fallen) < -1280) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Facing_Falling", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
            has_state_changed = true;
          }
        } else if (current_state.state.compare("Gap_Jump_Falling") == 0) {
          if ((*m_y_fallen) < -1280) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Facing_Falling", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
            has_state_changed = true;
          }
        } else if (current_state.state.compare("Vertical_Jump_Arc") == 0) {
          if ((*m_y_fallen) < -1280) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Facing_Falling", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
            has_state_changed = true;
          }
        }
        state_index = 0;
      } else {
        if (current_state.state.compare("Wall_Jump_Takeoff") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Rise_Arc", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state.compare("Wall_Jump_Rise_Arc") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Rising", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state.compare("Wall_Jump_Fall_Arc") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Falling", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true; 
        } else if (current_state.state.compare("Vertical_Jump_Takeoff") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Vertical_Jump_Arc", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state.compare("Vertical_Jump_Landing") == 0 ||
                   current_state.state.compare("Wall_Jump_Landing") == 0) {
          m_avatar->SetVelocity(D3DXVECTOR3(0 ,0, 0));

          if (current_command.state.compare("Jumping") == 0) {
            if (current_state.direction.compare("Right") == 0) {
              float y_velocity = 26;
              float x_velocity = 4;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
            } else {  // Left
              float y_velocity = 26;
              float x_velocity = -4;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
            }

            if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0 ) {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Vertical_Jump_Takeoff", current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            } else {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Vertical_Jump_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            }
            if (m_avatar->GetState().direction.compare("Right") == 0) {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            } else {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            }
            has_state_changed = true;
          } else if (current_command.state.compare("Running") == 0) {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Standing_To_Running", current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              if (current_command.direction.compare("Right") == 0) {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Standing", "Standing", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            if (m_avatar->GetState().direction.compare("Right") == 0) {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            } else {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            }
          }
          has_state_changed = true;
          (*m_y_fallen) = 0;
          (*m_distance_traveled) = 0;
        } else if (current_state.state.compare("Gap_Jump_Takeoff") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Arc_Rise", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state.compare("Gap_Jump_Arc_Rise") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Arc_Fall", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state.compare("Gap_Jump_Arc_Fall") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Falling", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state.compare("Gap_Jump_Landing") == 0) {
          m_avatar->SetVelocity(D3DXVECTOR3(0 ,0, 0));
          if (current_command.state.compare("Jumping") == 0) {
            if (current_command.direction.compare(current_state.direction) == 0) {
              D3DXVECTOR3 position = *m_avatar->GetPosition();
              position += m_avatar->GetVelocity();
              m_avatar->SetPosition(position); 

              if (current_command.direction.compare("Right") == 0) {
                float y_velocity = 24;
                float x_velocity = 24;
                m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
              } else {  // Left
                float y_velocity = 24;
                float x_velocity = -24;
                m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
              }

              if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0) {
                Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Takeoff", current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              } else {
                Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              }
              if (m_avatar->GetState().direction.compare("Right") == 0) {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
            } else {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Stopping", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              if (m_avatar->GetState().direction.compare("Right") == 0) {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
              has_state_changed = true;
              (*m_is_moving_continuously) = false;
            }
          } else if (current_command.state.compare("Running") == 0) {
            if (current_command.direction.compare(current_state.direction) == 0) {
              (*m_is_moving_continuously) = true;
              if (current_state.direction.compare("Right") == 0) {
                m_avatar->SetVelocity(D3DXVECTOR3(32, 0 , 0));
              } else {  // Left
                m_avatar->SetVelocity(D3DXVECTOR3(-32, 0 , 0));
              }
              D3DXVECTOR3 position = *m_avatar->GetPosition();
              D3DXVECTOR3 velocity = m_avatar->GetVelocity();
              position += velocity;
              (*m_y_fallen) += m_avatar->GetVelocity().y;
              m_avatar->SetPosition(position);

              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_" + current_command.state, current_command.state, current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              if (m_avatar->GetState().direction.compare("Right") == 0) {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
            } else {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Stopping", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              if (m_avatar->GetState().direction.compare("Right") == 0) {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
              has_state_changed = true;
              (*m_is_moving_continuously) = false;
            }
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Standing", "Standing", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            if (m_avatar->GetState().direction.compare("Right") == 0) {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            } else {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            }
          }
          has_state_changed = true;
          (*m_y_fallen) = 0;
        } else {
          std::string error;
          error = "No handling for the non-repeating animation: " + current_command.state;
          throw Exceptions::init_error(error);
        }
      }
    }
  }

  if (!has_state_changed) {
    if (current_state == last_state) {
      Avatar_Helper::SetAvatarStateAnimationFrame(m_avatar, state_index, m_current_animation_subset);
      if (current_state.state.compare("Wall_Jump_Takeoff") == 0 ||
          current_state.state.compare("Vertical_Jump_Landing") == 0 ||
          current_state.state.compare("Vertical_Jump_Takeoff") == 0 ||
          current_state.state.compare("Gap_Jump_Takeoff") == 0 ||
          current_state.state.compare("Gap_Jump_Landing") == 0) {
        Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
      } else if (current_state.state.compare("Wall_Jump_Rise_Arc") == 0 ||
                 current_state.state.compare("Wall_Jump_Rising") == 0 ||
                 current_state.state.compare("Wall_Jump_Fall_Arc") == 0 ||
                 current_state.state.compare("Wall_Jump_Falling") == 0 ) {
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, m_currently_adjacent_wall_tile->collision_side, m_currently_adjacent_wall_tile->colliding_tile);
      } else {
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
        } else {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
        }
      }
    }

    if (current_state.state.compare("Vertical_Jump_Arc") == 0 ||
        current_state.state.compare("Gap_Jump_Arc_Rise") == 0 ||
        current_state.state.compare("Gap_Jump_Arc_Fall") == 0 ||
        current_state.state.compare("Gap_Jump_Falling") == 0 ||
        current_state.state.compare("Wall_Jump_Rise_Arc") == 0 ||
        current_state.state.compare("Wall_Jump_Rising") == 0 ||
        current_state.state.compare("Wall_Jump_Fall_Arc") == 0 ||
        current_state.state.compare("Wall_Jump_Falling") == 0) {
      D3DXVECTOR3 position = *m_avatar->GetPosition();
      D3DXVECTOR3 velocity = m_avatar->GetVelocity();
      position += velocity;
      (*m_y_fallen) += m_avatar->GetVelocity().y;
      m_avatar->SetPosition(position);

      D3DXVECTOR3 new_velocity = m_avatar->GetVelocity();
      new_velocity.y += m_world_settings->GetGravityInPixPerFrame();
      if (new_velocity.y < m_world_settings->GetMaxVelocityInPixPerFrame()) { 
        new_velocity.y = m_world_settings->GetMaxVelocityInPixPerFrame(); 
      }
      
      m_avatar->SetVelocity(new_velocity);
    } else if (current_state.state.compare("Gap_Jump_Landing") == 0) {
      D3DXVECTOR3 position = *m_avatar->GetPosition();
      position.x += m_avatar->GetVelocity().x;
      m_avatar->SetPosition(position);
    }

    std::vector<Avatar_Helper::Tile_Collision> *out_colliding_ledge_tiles = new std::vector<Avatar_Helper::Tile_Collision>();
    bool can_avatar_grab_a_ledge = Avatar_Helper::CanAvatarGrabALedge(m_avatar, out_colliding_ledge_tiles, &m_ledge_tiles);
    std::vector<Avatar_Helper::Tile_Collision> *out_colliding_wall_tiles = new std::vector<Avatar_Helper::Tile_Collision>();
    bool is_wall_colliding = false;
    is_wall_colliding = Avatar_Helper::IsAvatarWallColliding(m_avatar, out_colliding_wall_tiles, &m_wall_tiles);
    if (is_wall_colliding) {
      (*m_currently_adjacent_wall_tile) = *(out_colliding_wall_tiles->begin());
      if (current_state.state.compare("Vertical_Jump_Takeoff") == 0 ||
          current_state.state.compare("Vertical_Jump_Arc") == 0 ||
          current_state.state.compare("Vertical_Jump_Landing") == 0 ||
          current_state.state.compare("Wall_Jump_Takeoff") == 0 ||
          current_state.state.compare("Wall_Jump_Rise_Arc") == 0 ||
          current_state.state.compare("Wall_Jump_Rising") == 0 ||
          current_state.state.compare("Wall_Jump_Fall_Arc") == 0 ||
          current_state.state.compare("Wall_Jump_Falling") == 0 ||
          current_state.state.compare("Wall_Jump_Landing") == 0) {
        if (current_state.direction.compare("Right") == 0) {
          float y_velocity = m_avatar->GetVelocity().y;
          float x_velocity = 0;
          m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
        } else {  // Left
          float y_velocity = m_avatar->GetVelocity().y;
          float x_velocity = 0;
          m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
        }
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, out_colliding_wall_tiles->begin()->collision_side, out_colliding_wall_tiles->begin()->colliding_tile);
      } else if (can_avatar_grab_a_ledge) {
        Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Vertical_Jump_To_Hanging", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        Avatar_Helper::AlignAvatarOnLastLedgeEdge(m_avatar, *(out_colliding_ledge_tiles->begin()));
        m_currently_grabbed_tile = out_colliding_ledge_tiles->begin()->colliding_tile;
        has_state_changed = true;
        (*m_y_fallen) = 0;
        (*m_distance_traveled) = 0;
      } else if (current_state.state.compare("Gap_Jump_Landing") == 0) {          
        Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding_From_Mid_Speed_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, m_currently_adjacent_wall_tile->collision_side, m_currently_adjacent_wall_tile->colliding_tile);
        (*m_distance_traveled) = 0;
        has_state_changed = true;
      } else {
        if (current_command.state.compare("Jumping") == 0) {
          float y_velocity = m_wall_jump_speed_offset;
          float x_velocity = 4;
          if (m_avatar->GetVelocity().y > 0) {
              if ((*m_distance_traveled) > 128) {
              if (current_state.state.compare("Gap_Jump_Takeoff") == 0 ||
                 (current_state.state.compare("Gap_Jump_Arc_Rise") == 0 && (current_state.state_index == 0 || current_state.state_index == 1))) {
                y_velocity = m_vertical_jump_y_initial_Velocity + m_wall_jump_speed_offset;
              } else {
                y_velocity = m_avatar->GetVelocity().y + m_wall_jump_speed_offset;
              }
            }
          }

          if (current_state.direction.compare("Left") == 0) {
            x_velocity = (x_velocity * -1);
          }

          m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
          if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0 ) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          }
          has_state_changed = true;
          if (out_colliding_wall_tiles->begin()->collision_side.compare("Right") == 0) {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
          } else {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
          }
          if (out_colliding_wall_tiles->begin()->collision_side.compare("Left") == 0) {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Left", out_colliding_wall_tiles->begin()->colliding_tile);
          } else {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Right", out_colliding_wall_tiles->begin()->colliding_tile);
          }
        } else if ((*m_y_fallen) < 1280) {
          if (m_avatar->GetState().direction.compare("Right") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Vertical_Jump_Arc", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            has_state_changed = true;
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Vertical_Jump_Arc", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            has_state_changed = true;
          }
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(0, 0, 0);
          m_avatar->SetVelocity(new_velocity);
          if (out_colliding_wall_tiles->begin()->collision_side.compare("Left") == 0) {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Left", out_colliding_wall_tiles->begin()->colliding_tile);
          } else {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Right", out_colliding_wall_tiles->begin()->colliding_tile);
          }
        } else {
          if (m_avatar->GetState().direction.compare("Right") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Falling_Wall_Impact_Right", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            has_state_changed = true;
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Up_Falling_Wall_Impact_Left", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            has_state_changed = true;
          }
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(0, old_velocity.y ,0);
          m_avatar->SetVelocity(new_velocity);   
          if (out_colliding_wall_tiles->begin()->collision_side.compare("Left") == 0) {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Left", out_colliding_wall_tiles->begin()->colliding_tile);
          } else {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Right", out_colliding_wall_tiles->begin()->colliding_tile);
          }
        }
      }
    }
    delete out_colliding_wall_tiles;
    delete out_colliding_ledge_tiles;

    if (current_state.state.compare("Gap_Jump_Arc_Fall") == 0 ||
        current_state.state.compare("Gap_Jump_Falling") == 0 ||
        current_state.state.compare("Vertical_Jump_Arc") == 0 ||
        current_state.state.compare("Wall_Jump_Fall_Arc") == 0 ||
        current_state.state.compare("Wall_Jump_Falling") == 0 ||
        current_state.state.compare("Wall_Jump_Rise_Arc") == 0 ||
        current_state.state.compare("Wall_Jump_Rising") == 0) {

      std::vector<Tile_Bitmap*> *adjacent_tiles = new std::vector<Tile_Bitmap*>();
      std::vector<Avatar_Helper::Tile_Collision> *out_colliding_floor_tiles = new std::vector<Avatar_Helper::Tile_Collision>();
      bool is_floor_colliding = Avatar_Helper::IsAvatarFloorColliding(m_avatar, out_colliding_floor_tiles, &m_floor_tiles);
      if (is_floor_colliding) {
        if ((*m_y_fallen) < -1280) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Facing_Falling_To_Death", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        } else {
          if (current_state.state.compare("Vertical_Jump_Arc") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Vertical_Jump_Landing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          } else if (current_state.state.compare("Gap_Jump_Arc_Fall") == 0 ||
                     current_state.state.compare("Gap_Jump_Falling") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Landing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          } else if (current_state.state.compare("Wall_Jump_Falling") == 0 ||
                     current_state.state.compare("Wall_Jump_Fall_Arc") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Landing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          }
        }
 
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
        } else {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
        }
        has_state_changed = true;
 
        (*m_y_fallen) = 0;
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Top", out_colliding_floor_tiles->begin()->colliding_tile);
        D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
        D3DXVECTOR3 new_velocity = D3DXVECTOR3(old_velocity.x, 0 ,0);
        m_avatar->SetVelocity(new_velocity);
      } else if (Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tiles, &m_floor_tiles)) {
        if ((*m_y_fallen) < -1280) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Falling", "Down_Facing_Falling_To_Death", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        } else {
          if (current_state.state.compare("Vertical_Jump_Arc") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Vertical_Jump_Landing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          } else if (current_state.state.compare("Gap_Jump_Arc_Fall") == 0 ||
                     current_state.state.compare("Gap_Jump_Falling") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Landing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          } else if (current_state.state.compare("Wall_Jump_Falling") == 0 ||
                     current_state.state.compare("Wall_Jump_Rising") == 0 ||
                     current_state.state.compare("Wall_Jump_Rise_Arc") == 0 ||
                     current_state.state.compare("Wall_Jump_Fall_Arc") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Landing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          }
        }
        
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Top", *(adjacent_tiles->begin()));

        has_state_changed = true;
        (*m_y_fallen) = 0;
        D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
        D3DXVECTOR3 new_velocity = D3DXVECTOR3(old_velocity.x, 0 ,0);
        m_avatar->SetVelocity(new_velocity);
      }
      delete adjacent_tiles;
      delete out_colliding_floor_tiles;
      
    }

    if (current_state.state.compare("Vertical_Jump_Takeoff") == 0 ||
        current_state.state.compare("Vertical_Jump_Arc") == 0 ||
        current_state.state.compare("Vertical_Jump_Landing") == 0 ||
        current_state.state.compare("Gap_Jump_Takeoff") == 0 ||
        current_state.state.compare("Gap_Jump_Arc_Rise") == 0 ||
        current_state.state.compare("Gap_Jump_Arc_Fall") == 0 ||
        current_state.state.compare("Gap_Jump_Falling") == 0 ||
        current_state.state.compare("Wall_Jump_Takeoff") == 0 ||
        current_state.state.compare("Wall_Jump_Rise_Arc") == 0 ||
        current_state.state.compare("Wall_Jump_Fall_Arc") == 0 ||
        current_state.state.compare("Wall_Jump_Rising") == 0 ||
        current_state.state.compare("Wall_Jump_Falling") == 0) {
      std::vector<Avatar_Helper::Tile_Collision> *out_colliding_floor_tiles = new std::vector<Avatar_Helper::Tile_Collision>();
      bool can_avatar_grab_a_ledge = Avatar_Helper::CanAvatarGrabALedge(m_avatar, out_colliding_floor_tiles, &m_ledge_tiles);
      if (can_avatar_grab_a_ledge) {
          if (current_state.state.compare("Wall_Jump_Rise_Arc") == 0 ||
              current_state.state.compare("Wall_Jump_Fall_Arc") == 0 ||
              current_state.state.compare("Wall_Jump_Rising") == 0 ||
              current_state.state.compare("Wall_Jump_Falling") == 0) {            
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Wall_Jump_To_Hanging", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastLedgeEdge(m_avatar, *(out_colliding_floor_tiles->begin()));
            m_currently_grabbed_tile = out_colliding_floor_tiles->begin()->colliding_tile;
            has_state_changed = true;
            (*m_y_fallen) = 0;
            (*m_distance_traveled) = 0;
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Vertical_Jump_To_Hanging", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastLedgeEdge(m_avatar, *(out_colliding_floor_tiles->begin()));
            m_currently_grabbed_tile = out_colliding_floor_tiles->begin()->colliding_tile;
            has_state_changed = true;
            (*m_y_fallen) = 0;
            (*m_distance_traveled) = 0;
          }
      }
      delete out_colliding_floor_tiles;
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
