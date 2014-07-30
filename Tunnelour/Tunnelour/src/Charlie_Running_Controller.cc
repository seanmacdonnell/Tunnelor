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

#include "Charlie_Running_Controller.h"
#include "Avatar_Helper.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Charlie_Running_Controller::Charlie_Running_Controller() {
  // Constant Variables
  m_avatar_z_position = -2; // Middleground Z Space is -1
  m_vertical_jump_y_initial_Velocity = 22;
  m_vertical_jump_x_initial_Velocity = 4;
  m_wall_jump_speed_offset = 16;
}

//------------------------------------------------------------------------------
Charlie_Running_Controller::~Charlie_Running_Controller() {
}

//------------------------------------------------------------------------------
bool Charlie_Running_Controller::Init(Component_Composite *const model) {
  Avatar_State_Controller::Init(model);
  m_has_been_initialised = true;
  return true;
}

//------------------------------------------------------------------------------
void Charlie_Running_Controller::Run_Avatar_State() {
  bool has_state_changed = false;

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State last_state = m_avatar->GetLastRenderedState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

  if (current_state.state.compare("Running") == 0 && current_state == last_state) {
    if (current_state.direction.compare("Right") == 0) {
      m_avatar->SetVelocity(D3DXVECTOR3(18, 0 , 0));
    } else {  // Left
      m_avatar->SetVelocity(D3DXVECTOR3(-18, 0 , 0));
    }
    if ((*m_distance_traveled) > 128 || (*m_is_moving_continuously)) {
      (*m_is_moving_continuously) = true;
      if (current_state.direction.compare("Right") == 0) {
        m_avatar->SetVelocity(D3DXVECTOR3(32, 0 , 0));
      } else {  // Left
        m_avatar->SetVelocity(D3DXVECTOR3(-32, 0 , 0));
      }
    }
  } else if (current_state.state.compare("Stopping") == 0)  {
    if (current_state.direction.compare("Right") == 0) {
      float new_x_velocity = m_avatar->GetVelocity().x - ceil(m_avatar->GetVelocity().x/2);
      m_avatar->SetVelocity(D3DXVECTOR3(new_x_velocity, 0 , 0));
    } else {  // Left
      float new_x_velocity = m_avatar->GetVelocity().x + ceil((-1*(m_avatar->GetVelocity().x))/2);
      m_avatar->SetVelocity(D3DXVECTOR3(new_x_velocity, 0 , 0));
    }
  } else if (current_state.state.compare("Wall_Colliding") == 0)  {
     m_avatar->SetVelocity(D3DXVECTOR3(0, 0 , 0));
  } else if (current_state.state.compare("Standing_To_Running") == 0) {
    m_avatar->SetVelocity(D3DXVECTOR3(0, 0 , 0));
  } else if (current_state.state.compare("Slow_Running") == 0) {
    if (current_state.direction.compare("Right") == 0) {
      m_avatar->SetVelocity(D3DXVECTOR3(24, 0 , 0));
    } else {  // Left
      m_avatar->SetVelocity(D3DXVECTOR3(-24, 0 , 0));
    }
  }

  if ((current_state.state.compare(current_command.state) != 0 || current_state.direction != current_command.direction) &&
       (current_state.state.compare("Stopping") != 0 || current_state.state_index == (m_current_animation_subset->number_of_frames -1)) &&
       current_state.state.compare("Wall_Colliding_From_Mid_Speed_Arc") != 0 &&
       (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Landing") != 0 || current_state.state_index == (m_current_animation_subset->number_of_frames -1)) &&
       current_state.state.compare("Wall_Colliding_From_High_Speed_Arc") != 0 &&
       (current_state.state.compare("Wall_Colliding_From_High_Speed_Landing") != 0 || current_state.state_index == (m_current_animation_subset->number_of_frames -1)) &&
       current_state.state.compare("Standing_To_Running") != 0) {
    if (current_command.state.compare("Down") == 0 || current_command.state.compare("Looking") == 0 ) {
      current_command.state = "";
    }
    if (current_command.state.compare("Jumping") == 0) {
      if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") == 0 ||
        current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") == 0) {
        if (current_state.state_index == 0 || current_state.state_index == 1) {
          float y_velocity = m_vertical_jump_y_initial_Velocity;
          y_velocity += m_wall_jump_speed_offset;

          float x_velocity = m_vertical_jump_x_initial_Velocity;
          if (current_state.direction.compare("Left") == 0) {
            x_velocity = (x_velocity * -1);
          }
        
          m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
          
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, m_currently_adjacent_wall_tile->collision_side, m_currently_adjacent_wall_tile->colliding_tile);

          has_state_changed = true;
        }
      } else if (current_state.state.compare("Wall_Colliding") == 0) {
        float y_velocity = m_vertical_jump_y_initial_Velocity;
        float x_velocity = m_vertical_jump_x_initial_Velocity;
        if (current_state.direction.compare("Left") == 0) {
          x_velocity = (x_velocity * -1);
        }
        
        m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
          
        if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0 ) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        } else {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        }

        if (current_command.direction.compare("Right") == 0) {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
        } else {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
        }
        has_state_changed = true;
      } else {
       if (current_command.direction.compare(current_state.direction) != 0 &&
           current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") != 0 &&
           current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") != 0 &&
           current_state.state.compare("Stopping") != 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Stopping", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);      
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          has_state_changed = true;
          (*m_distance_traveled) = 0;
        } else {
          std::string direction;
          if (current_command.direction.compare("") == 0) {
            direction = m_avatar->GetState().direction;
          } else {
            direction = current_command.direction;
          }
          float y_velocity = 0;
          float x_velocity = 0;

          if (m_avatar->GetVelocity().x == 32 || m_avatar->GetVelocity().x == -32) {
            // Full Jump (Jump From Running)
            x_velocity = 28;
            y_velocity = 28;
          } else if (m_avatar->GetVelocity().x == 16 || m_avatar->GetVelocity().x == -16) {
            // Smaller Jump (Jump from Accelerating)
            x_velocity = 24;
            y_velocity = 28;
          } else { 
            // Jump from Stopping
            x_velocity = 18;
            y_velocity = 28;
          }

          if (direction.compare("Left") == 0) {
            x_velocity = (x_velocity * -1);
          }

          m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Takeoff", direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          has_state_changed = true;
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        }
      }
    } else if (current_command.state.compare("") == 0) {
      if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") != 0 &&
        current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") != 0) {
        if (current_state.state.compare("Standing_To_Running") != 0 && (current_state.state.compare("Running") == 0 && current_state.state_index != 0)) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Stopping", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        } else {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Standing", "Standing", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        }
        Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        has_state_changed = true;
      }
    } else if (current_state.state.compare("Wall_Colliding") == 0) {
      if (current_command.state.compare("Running") == 0) {
        if (current_command.direction.compare(current_state.direction) != 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Standing_To_Running", current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          has_state_changed = true;
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          (*m_distance_traveled) = 0;
        }
      } else {
        if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0 ) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_" + current_command.state, current_command.state, current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        } else {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_" + current_command.state, current_command.state, m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        }
        has_state_changed = true;
        Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
      }
    } else if (current_command.state.compare("Running") == 0) {
      if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") != 0 &&
        current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") != 0) {
        if (current_state.state.compare("Standing_To_Running") != 0 && (current_state.state.compare("Running") == 0 && current_state.state_index != 0)) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Stopping", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        } else {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Standing", "Standing", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
        }
        Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        has_state_changed = true;
        (*m_is_moving_continuously) = false;
      }
    } else {
      if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") != 0 &&
        current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") != 0) {
        if (current_command.direction.compare(current_state.direction) != 0) {
          if (current_state.state.compare("Standing_To_Running") != 0 &&
              current_state.state.compare("Stopping") != 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Stopping", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          } else {
            if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0 ) {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_" + current_command.state, current_command.state, current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
            } else {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_" + current_command.state, current_command.state, m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
            }
            has_state_changed = true;
            Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          }
        }
      }
    }
  }
  if (!has_state_changed) {
    int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset->number_of_frames - 1)) {
      if (m_current_animation_subset->is_repeatable) {
        state_index = 0;
      } else {
        if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding_From_Mid_Speed_Arc", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
          D3DXVECTOR3 velocity = m_avatar->GetVelocity();
          velocity.y = 4;
          velocity.x = ((velocity.x/2) * -1);
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Arc") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding_From_Mid_Speed_Landing", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
          D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding_From_High_Speed_Arc", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
          D3DXVECTOR3 velocity = m_avatar->GetVelocity();
          velocity.y = 8;
          velocity.x = ((velocity.x/4) * -1);
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state.compare("Wall_Colliding_From_High_Speed_Arc") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding_From_High_Speed_Landing", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
          D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state.compare("Standing_To_Running") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Running", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          has_state_changed = true;
        } else if (current_state.state.compare("Slow_Running") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Running", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          has_state_changed = true;
        } else {
          std::string error;
          error = "No handling for the non-repeating animation: " + current_command.state;
          throw Exceptions::init_error(error);
        }
      }
    }

    if (!has_state_changed) {
      if (current_state == last_state) {
        Avatar_Helper::SetAvatarStateAnimationFrame(m_avatar, state_index, m_current_animation_subset);
        if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") == 0 ||
            current_state.state.compare("Wall_Colliding_From_Mid_Speed_Landing") == 0 ||
            current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") == 0 ||
            current_state.state.compare("Wall_Colliding_From_High_Speed_Landing") == 0 ||
            current_state.state.compare("Standing_To_Running") == 0) {
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        } else {
          if (m_avatar->GetVelocity().x > 0) {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
          } else if (m_avatar->GetVelocity().x < 0) {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
          } else if (current_state.direction.compare("Right") == 0) {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
          } else if  (current_state.direction.compare("Left") == 0) {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
          } else {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
          }
        }
      }
    
      if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Arc") == 0 ||
          current_state.state.compare("Wall_Colliding_From_High_Speed_Arc") == 0) {
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
      } else if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") != 0 ||
                 current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") != 0) {
        D3DXVECTOR3 position = *m_avatar->GetPosition();
        D3DXVECTOR3 velocity = m_avatar->GetVelocity();
        position += velocity;
        m_avatar->SetPosition(position);
        if (velocity.x < 0) {
          (*m_distance_traveled) += (velocity.x * -1);
        } else {
          (*m_distance_traveled) += velocity.x;
        }
      }
  
      // Detect if the avatar is intersecting with a wall
      std::vector<Avatar_Helper::Tile_Collision> *out_colliding_wall_tiles = new std::vector<Avatar_Helper::Tile_Collision>();
      if (Avatar_Helper::IsAvatarWallColliding(m_avatar, out_colliding_wall_tiles, &m_wall_tiles)) {
        m_currently_adjacent_wall_tile = &(*(out_colliding_wall_tiles->begin()));
        if (current_state.state.compare("Wall_Colliding") != 0 && last_state.state.compare("Wall_Colliding") != 0) {
          if  (current_state.state.compare("Stopping") != 0 &&
               last_state.state.compare("Stopping") != 0 &&
               current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") != 0 &&
               last_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") != 0 &&
               current_state.state.compare("Wall_Colliding_From_Mid_Speed_Arc") != 0 &&
               last_state.state.compare("Wall_Colliding_From_Mid_Speed_Arc") != 0 &&
               current_state.state.compare("Wall_Colliding_From_Mid_Speed_Landing") != 0 &&
               last_state.state.compare("Wall_Colliding_From_Mid_Speed_Landing") != 0 &&
               current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") != 0 &&
               last_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") != 0 &&
               current_state.state.compare("Wall_Colliding_From_High_Speed_Arc") != 0 &&
               last_state.state.compare("Wall_Colliding_From_High_Speed_Arc") != 0 &&
               current_state.state.compare("Wall_Colliding_From_High_Speed_Landing") != 0 &&
               last_state.state.compare("Wall_Colliding_From_High_Speed_Landing") != 0) {
            if (out_colliding_wall_tiles->begin()->collision_side.compare(current_state.direction) != 0) {
              if (current_state.state.compare("Standing_To_Running") == 0) {
                Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              } else {
                if (m_avatar->GetVelocity().x == 32 || m_avatar->GetVelocity().x == -32) {
                 Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding_From_Mid_Speed_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
                } else if ((*m_distance_traveled) > 64) {
                 Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Stopping", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
                  m_avatar->SetVelocity(D3DXVECTOR3(0, 0 , 0));
                } else {
                 Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
                  m_avatar->SetVelocity(D3DXVECTOR3(0, 0 , 0));
                }
              }
              has_state_changed = true;
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
              (*m_distance_traveled) = 0;
            }
          } else if (current_state.state.compare("Stopping") != 0 && last_state.state.compare("Stopping") == 0) {
            if (out_colliding_wall_tiles->begin()->collision_side.compare(current_state.direction) != 0) {
             Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
              has_state_changed = true;
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
            }
          }
        }

        // Move back avatar
        if ((*out_colliding_wall_tiles->begin()).collision_side.compare("Right") == 0) {
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Right", out_colliding_wall_tiles->begin()->colliding_tile);
        } else  if ((*out_colliding_wall_tiles->begin()).collision_side.compare("Left") == 0) {
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Left", out_colliding_wall_tiles->begin()->colliding_tile);
        }
      }
      delete out_colliding_wall_tiles;
      
      // Detect if the avatar is overbalancing from walking
      std::vector<Tile_Bitmap*> *adjacent_tiles = new std::vector<Tile_Bitmap*>();
      if (!Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tiles, &m_floor_tiles) && 
           current_state.state.compare("Wall_Colliding_From_Mid_Speed_Takeoff") != 0 &&
           current_state.state.compare("Wall_Colliding_From_Mid_Speed_Arc") != 0 &&
           current_state.state.compare("Wall_Colliding_From_Mid_Speed_Landing") != 0 &&
           current_state.state.compare("Wall_Colliding_From_High_Speed_Takeoff") != 0 &&
           current_state.state.compare("Wall_Colliding_From_High_Speed_Arc") != 0 &&
           current_state.state.compare("Wall_Colliding_From_High_Speed_Landing") != 0) {
            // Detect if the avatar is overbalancing from walking
        if (adjacent_tiles->empty()) {
          std::vector<Tile_Bitmap*> *adjacent_tile = new std::vector<Tile_Bitmap*>();
          int offset = 0;
          bool try_opposite_direction = false;
          while (!Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tile, &m_floor_tiles)) {
            if (m_avatar->GetState().direction.compare("Right")) {
              offset += 8;
            } else {
              offset -= 8;
            }

            if (try_opposite_direction) {
              offset = offset * -1;
            }

            D3DXVECTOR3 position = *m_avatar->GetPosition();
            position.x += offset;
            m_avatar->SetPosition(position);

            if (offset > 64 || offset < -64) {
              if (!try_opposite_direction) {
                offset = 0;
                try_opposite_direction = true;
              } else {
                std::string error;
                error = "No Adjacent Tile Found!";
                throw Exceptions::init_error(error);
              }
            }
          }
          delete adjacent_tiles;
          adjacent_tiles = adjacent_tile;
        }
        
        if (current_state.state.compare("Stopping") == 0) {
          if (current_state.direction.compare("Right") == 0) {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Right", *(adjacent_tiles->begin()));
            Avatar_Component::Avatar_Collision_Block avatar = Avatar_Helper::GetNamedCollisionBlock("Avatar", current_state.avatar_collision_blocks);
            D3DXVECTOR3 position = *m_avatar->GetPosition();
            position.x -= avatar.size.x;
            m_avatar->SetPosition(position);
          } else  if (current_state.direction.compare("Left") == 0) {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,"Left", *(adjacent_tiles->begin()));
            Avatar_Component::Avatar_Collision_Block avatar = Avatar_Helper::GetNamedCollisionBlock("Avatar", current_state.avatar_collision_blocks);
            D3DXVECTOR3 position = *m_avatar->GetPosition();
            position.x += avatar.size.x;
            m_avatar->SetPosition(position);
          }
        } else {
          if (m_avatar->GetState().direction.compare(m_avatar->GetLastRenderedState().direction) == 0) {
            if (current_state.direction.compare("Right") == 0) {
              float y_velocity = -4;
              float x_velocity = 16;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
            } else {  // Left
              float y_velocity = -4;
              float x_velocity = -16;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
            }

            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Arc_Fall", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          } else {
            if (m_avatar->GetLastRenderedState().direction.compare("Right") == 0) {
              float y_velocity = -4;
              float x_velocity = 16;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
            } else {  // Left
              float y_velocity = -4;
              float x_velocity = -16;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity ,y_velocity, 0));
            }

           Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Arc_Fall", m_avatar->GetLastRenderedState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          }

          has_state_changed = true;
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        }

      }
      delete adjacent_tiles;
      
      std::vector<Avatar_Helper::Tile_Collision> *out_colliding_floor_tiles = new std::vector<Avatar_Helper::Tile_Collision>();
      bool is_floor_colliding = Avatar_Helper::IsAvatarFloorColliding(m_avatar, out_colliding_floor_tiles, &m_floor_tiles);
      if (is_floor_colliding) {
        if (current_state.state.compare("Wall_Colliding_From_Mid_Speed_Arc") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding_From_Mid_Speed_Landing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Top", out_colliding_floor_tiles->begin()->colliding_tile);
          
          has_state_changed = true;
          m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
          (*m_distance_traveled) = 0;
        } else if (current_state.state.compare("Wall_Colliding_From_High_Speed_Arc") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Wall_Colliding_From_High_Speed_Landing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Top", out_colliding_floor_tiles->begin()->colliding_tile);
          
          has_state_changed = true;
          m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
          (*m_distance_traveled) = 0;
        } else {
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Top", out_colliding_floor_tiles->begin()->colliding_tile);
        }
      }
      delete adjacent_tiles;
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
