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

#include "Charlie_Running_Controller.h"
#include "Avatar_Helper.h"
#include "Exceptions.h"

using std::string;
using std::vector;

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Charlie_Running_Controller::Charlie_Running_Controller():
  // Constant Variables
  m_avatar_z_position(-2),
  m_vertical_jump_y_initial_velocity(22),
  m_vertical_jump_x_initial_velocity(4),
  m_gap_jump_y_initial_velocity(32),
  m_gap_jump_x_initial_velocity(32),
  m_takeoff_gap_jump_y_initial_velocity(18),
  m_takeoff_gap_jump_x_initial_velocity(18),
  m_running_x_velocity(32),
  m_takeoff_running_x_velocity(32),
  m_takeoff_running_x_distance(128),
  m_stopping_from_running_distance(64),
  m_overbalancing_x_velocity(16),
  m_overbalancing_y_velocity(-16),
  m_wall_jump_y_initial_velocity(16),
  m_wall_jump_x_initial_velocity(4),
  m_wall_jump_boost_distance_threshold(128),
  m_wall_colliding_mid_y_initial_velocity(16),
  m_wall_colliding_mid_x_initial_velocity(4),
  m_wall_colliding_mid_distance(256),
  m_wall_colliding_high_y_initial_velocity(8),
  m_wall_colliding_high_x_initial_velocity(4),
  m_wall_colliding_high_distance(512),
  m_safe_falling_limit(-1280) {
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

  // Look into removing this, the velocity doesen't need to be set every time
  // this state controller is set.
  if (current_state.state == "Running" && current_state == last_state) {
    if (current_state.direction == "Right") {
      m_avatar->SetVelocity(D3DXVECTOR3(m_takeoff_running_x_velocity, 0, 0));
    } else {  // Left
      m_avatar->SetVelocity(D3DXVECTOR3(m_takeoff_running_x_velocity * -1, 0, 0));
    }
    if (((*m_distance_traveled) > m_takeoff_running_x_distance) ||
          (*m_is_moving_continuously)) {
      (*m_is_moving_continuously) = true;
      if (current_state.direction == "Right") {
        m_avatar->SetVelocity(D3DXVECTOR3(m_running_x_velocity, 0, 0));
      } else {  // Left
        m_avatar->SetVelocity(D3DXVECTOR3(m_running_x_velocity * -1, 0, 0));
      }
    }
  } else if (current_state.state == "Stopping" ||
             current_state.state == "False_Start")  {
    if (current_state.direction == "Right") {
      float new_x_velocity = m_avatar->GetVelocity().x - ceil(m_avatar->GetVelocity().x/2);
      m_avatar->SetVelocity(D3DXVECTOR3(new_x_velocity, 0, 0));
    } else {  // Left
      float new_x_velocity = m_avatar->GetVelocity().x + ceil((m_avatar->GetVelocity().x * -1)/2);
      m_avatar->SetVelocity(D3DXVECTOR3(new_x_velocity, 0, 0));
    }
  } else if (current_state.state == "Wall_Colliding")  {
     m_avatar->SetVelocity(D3DXVECTOR3(0, 0 , 0));
  } else if (current_state.state == "Standing_To_Running") {
    m_avatar->SetVelocity(D3DXVECTOR3(0, 0 , 0));
  }

  if (((current_state.state != current_command.state) || current_state.direction != current_command.direction) &&
       // The states listed below can be canceled with a different
       // direction or command.
       (current_state.state != "Stopping" || current_state.state_index == (m_current_animation_subset->number_of_frames -1)) &&
       (current_state.state != "False_Start" || current_state.state_index == (m_current_animation_subset->number_of_frames -1)) &&
        current_state.state != "Wall_Colliding_From_Mid_Speed_Arc"  &&
        current_state.state != "Wall_Colliding_From_Mid_Speed_Falling"  &&
       (current_state.state != "Wall_Colliding_From_Mid_Speed_Landing" || current_state.state_index == (m_current_animation_subset->number_of_frames -1)) &&
        current_state.state != "Wall_Colliding_From_High_Speed_Arc" &&
        current_state.state != "Wall_Colliding_From_High_Speed_Falling"  &&
       (current_state.state != "Wall_Colliding_From_High_Speed_Landing" || current_state.state_index == (m_current_animation_subset->number_of_frames -1)) &&
        current_state.state != "Standing_To_Running") {
    // Ignore these commands
    if (current_command.state == "Down" ||
        current_command.state == "Looking" ) {
      current_command.state = "";
    }

    if (current_command.state == "Jumping") {
      if (current_state.state == "Wall_Colliding_From_Mid_Speed_Takeoff" ||
          current_state.state == "Wall_Colliding_From_High_Speed_Takeoff" ){
        if (current_state.state_index == 0 || current_state.state_index == 1) {
            float y_velocity = m_wall_jump_y_initial_velocity;
            if (last_state.parent_state == "Charlie_Jumping") {
                y_velocity += m_avatar->GetVelocity().y;
            } else {
                y_velocity += m_vertical_jump_y_initial_velocity;
            }

            float x_velocity = m_vertical_jump_x_initial_velocity;
            if (current_state.direction == "Left") {
              x_velocity = (x_velocity * -1);
            }

            m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));

            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Wall_Jump_Takeoff",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                  m_adjacent_wall->collision_side,
                                                  m_adjacent_wall->colliding_tile);

            has_state_changed = true;
          }
      } else if (current_state.state == "Wall_Colliding") {
        float y_velocity = m_vertical_jump_y_initial_velocity;
        float x_velocity = m_vertical_jump_x_initial_velocity;
        if (current_state.direction == "Left") {
          x_velocity = (x_velocity * -1);
        }

        m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));

        if (current_command.direction == "Right" ||
              current_command.direction == "Left" ) {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Wall_Jump_Takeoff",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
        } else {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Wall_Jump_Takeoff",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
        }

        if (current_command.direction == "Right") {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
        } else {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
        }
        has_state_changed = true;
      } else {
       if (current_command.direction != current_state.direction &&
           current_state.state != "Wall_Colliding_From_Mid_Speed_Takeoff" &&
           current_state.state != "Wall_Colliding_From_High_Speed_Takeoff" &&
           current_state.state != "Stopping") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Stopping",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          has_state_changed = true;
          (*m_distance_traveled) = 0;
        } else {
          string direction;
          if (current_command.direction == "") {
            direction = m_avatar->GetState().direction;
          } else {
            direction = current_command.direction;
          }
          float y_velocity = 0;
          float x_velocity = 0;

          if (m_avatar->GetVelocity().x == m_running_x_velocity ||
                m_avatar->GetVelocity().x == m_running_x_velocity * -1) {
            // Full Jump (Jump From Running)
            x_velocity = m_gap_jump_y_initial_velocity;
            y_velocity = m_gap_jump_x_initial_velocity;
          } else if (m_avatar->GetVelocity().x == m_takeoff_running_x_velocity ||
                       m_avatar->GetVelocity().x == m_takeoff_running_x_velocity * -1) {
            // Smaller Jump (Jump from Accelerating)
            x_velocity = m_takeoff_gap_jump_x_initial_velocity;
            y_velocity = m_takeoff_gap_jump_y_initial_velocity;
          } else {
            // Jump from Stopping
            x_velocity = m_takeoff_gap_jump_x_initial_velocity;
            y_velocity = m_takeoff_gap_jump_y_initial_velocity;
          }

          if (direction == "Left") {
            x_velocity = (x_velocity * -1);
          }

          m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));

          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Gap_Jump_Takeoff",
                                        direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          has_state_changed = true;
        }
      }
    } else if (current_command.state == "") {
      if (current_state.state != "Wall_Colliding_From_Mid_Speed_Takeoff" &&
          current_state.state != "Wall_Colliding_From_High_Speed_Takeoff") {
        if (current_state.state == "Standing_To_Running" ||
           (current_state.state == "Running" && current_state.state_index == 0)) {
           Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "False_Start",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
        } else if (current_state.state == "False_Start" ||
                   current_state.state == "Wall_Colliding_From_Mid_Speed_Landing" ||
                   current_state.state == "Stopping" ) {
           Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Standing",
                                        "Standing",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
       } else {
           Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Stopping",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
       }
        Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        has_state_changed = true;
      }
    } else if (current_state.state == "Wall_Colliding") {
      if (current_command.state == "Running") {
        if (current_command.direction != current_state.direction) {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Standing_To_Running",
                                        current_command.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          has_state_changed = true;
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          (*m_distance_traveled) = 0;
        }
      } else {
        if (current_command.direction == "Right" ||
              current_command.direction == "Left") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_" + current_command.state,
                                        current_command.state,
                                        current_command.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
        } else {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_" + current_command.state,
                                        current_command.state,
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
        }
        has_state_changed = true;
        Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
      }
    } else if (current_command.state == "Running") {
      if (current_state.state != "Wall_Colliding_From_Mid_Speed_Takeoff" &&
            current_state.state != "Wall_Colliding_From_High_Speed_Takeoff") {
        if (current_state.state != "Standing_To_Running" &&
             (current_state.state == "Running" && current_state.state_index)) {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Stopping",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
        } else {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Standing",
                                        "Standing",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
        }
        Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        (*m_is_moving_continuously) = false;
        has_state_changed = true;
      }
    } else {
      if (current_state.state != "Wall_Colliding_From_Mid_Speed_Takeoff" &&
            current_state.state != "Wall_Colliding_From_High_Speed_Takeoff") {
        if (current_command.direction != current_state.direction) {
          if (current_state.state != "Standing_To_Running" &&
                current_state.state != "Stopping") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Running",
                                          "Stopping",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          } else {
            if (current_command.direction == "Right" ||
                  current_command.direction == "Left" ) {
              Avatar_Helper::SetAvatarState(m_avatar,
                                            m_game_settings->GetTilesetPath(),
                                            m_animation_metadata,
                                            "Charlie_" + current_command.state,
                                            current_command.state,
                                            current_command.direction,
                                            m_current_metadata_file_path,
                                            m_current_metadata,
                                            m_current_animation_subset);
              m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
            } else {
              Avatar_Helper::SetAvatarState(m_avatar,
                                            m_game_settings->GetTilesetPath(),
                                            m_animation_metadata,
                                            "Charlie_" + current_command.state,
                                            current_command.state,
                                            m_avatar->GetState().direction,
                                            m_current_metadata_file_path,
                                            m_current_metadata,
                                            m_current_animation_subset);
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
    unsigned int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset->number_of_frames - 1)) {
      if (m_current_animation_subset->is_repeatable) {
        state_index = 0;
      } else {
        if (current_state.state == "Wall_Colliding_From_Mid_Speed_Takeoff") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Wall_Colliding_From_Mid_Speed_Arc",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Align_New_Avatar_State();
          D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);
          velocity.y = m_wall_colliding_mid_y_initial_velocity;
          velocity.x = m_wall_colliding_mid_x_initial_velocity;
          if (current_state.direction == "Right") {
             velocity.x *= -1;
          }
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state == "Wall_Colliding_From_Mid_Speed_Arc") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Wall_Colliding_From_Mid_Speed_Falling",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Align_New_Avatar_State();
          has_state_changed = true;
        } else if (current_state.state == "Wall_Colliding_From_Mid_Speed_Falling") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Wall_Colliding_From_Mid_Speed_Landing",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Align_New_Avatar_State();
          D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state == "Wall_Colliding_From_High_Speed_Takeoff") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Wall_Colliding_From_High_Speed_Arc",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Align_New_Avatar_State();
          D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);
          velocity.y = m_wall_colliding_high_y_initial_velocity;
          velocity.x = m_wall_colliding_high_x_initial_velocity;
          if (current_state.direction == "Right") {
             velocity.x *= -1;
          }
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state == "Wall_Colliding_From_High_Speed_Arc") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Wall_Colliding_From_High_Speed_Falling",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Align_New_Avatar_State();
          D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);
          velocity.y = m_wall_colliding_high_y_initial_velocity;
          velocity.x = m_wall_colliding_high_x_initial_velocity;
          if (current_state.direction == "Right") {
             velocity.x *= -1;
          }
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state == "Wall_Colliding_From_High_Speed_Falling") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Wall_Colliding_From_High_Speed_Landing",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Align_New_Avatar_State();
          D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);
          m_avatar->SetVelocity(velocity);
          has_state_changed = true;
        } else if (current_state.state == "Standing_To_Running") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Running",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          has_state_changed = true;
        } else if (current_state.state == "Slow_Running") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Running",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          has_state_changed = true;
        } else {
          string error;
          error = "No handling for the non-repeating animation: ";
          error += current_command.state;
          throw Exceptions::init_error(error);
        }
      }
    }

    if (!has_state_changed) {
      if (current_state == last_state) {
        Avatar_Helper::SetAvatarStateAnimationFrame(m_avatar,
                                                    state_index,
                                                    m_current_animation_subset);
        if (current_state.state == "Wall_Colliding_From_Mid_Speed_Takeoff" ||
            current_state.state == "Wall_Colliding_From_Mid_Speed_Landing" ||
            current_state.state == "Wall_Colliding_From_High_Speed_Takeoff" ||
            current_state.state == "Wall_Colliding_From_High_Speed_Landing" ||
            current_state.state == "Standing_To_Running") {
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        } else {
          Align_New_Avatar_State();
        }
      }

      if (current_state.state == "Wall_Colliding_From_Mid_Speed_Arc" ||
          current_state.state == "Wall_Colliding_From_Mid_Speed_Falling" ||
          current_state.state == "Wall_Colliding_From_High_Speed_Arc" ||
          current_state.state == "Wall_Colliding_From_High_Speed_Falling") {
        D3DXVECTOR3 position = *m_avatar->GetPosition();
        position = *m_avatar->GetPosition();

        D3DXVECTOR3 new_velocity = m_avatar->GetVelocity();
        new_velocity.y += m_world_settings->GetGravityInPixPerFrame();
        if (new_velocity.y < m_world_settings->GetMaxVelocityInPixPerFrame()) {
          new_velocity.y = m_world_settings->GetMaxVelocityInPixPerFrame();
        }

        m_avatar->SetVelocity(new_velocity);
        position += new_velocity;
        (*m_y_fallen) += m_avatar->GetVelocity().y;
        m_avatar->SetPosition(position);
      } else if (current_state.state != "Wall_Colliding_From_Mid_Speed_Takeoff" ||
                 current_state.state != "Wall_Colliding_From_High_Speed_Takeoff" ||
                 current_state.state != "Wall_Colliding_From_Mid_Speed_Landing" ||
                 current_state.state != "Wall_Colliding_From_High_Speed_Landing") {
        D3DXVECTOR3 position = *m_avatar->GetPosition();
        D3DXVECTOR3 velocity = m_avatar->GetVelocity();
        position += velocity;
        m_avatar->SetPosition(position);
        if (velocity.x < 0) {
          (*m_distance_traveled) += (velocity.x * -1.0f);
        } else {
          (*m_distance_traveled) += velocity.x;
        }
      }

      // Detect if the avatar is intersecting with a wall
      vector<Avatar_Helper::Tile_Collision> *out_colliding_wall_tiles = new vector<Avatar_Helper::Tile_Collision>();
      if (Avatar_Helper::IsAvatarWallColliding(m_avatar, out_colliding_wall_tiles, &m_wall_tiles)) {
        if (m_adjacent_wall == 0) {
          m_adjacent_wall = new Avatar_Helper::Tile_Collision();  
        } 
        *m_adjacent_wall = *(out_colliding_wall_tiles->begin());
          
        if (current_state.state != "Wall_Colliding" && last_state.state != "Wall_Colliding") {
          if  (current_state.state != "Stopping" &&
               last_state.state != "Stopping" &&
               current_state.state != "Wall_Colliding_From_Mid_Speed_Takeoff" &&
               last_state.state != "Wall_Colliding_From_Mid_Speed_Takeoff" &&
               current_state.state != "Wall_Colliding_From_Mid_Speed_Arc" &&
               last_state.state != "Wall_Colliding_From_Mid_Speed_Arc" &&
               current_state.state != "Wall_Colliding_From_Mid_Speed_Falling" &&
               last_state.state != "Wall_Colliding_From_Mid_Speed_Falling" &&
               current_state.state != "Wall_Colliding_From_Mid_Speed_Landing" &&
               last_state.state != "Wall_Colliding_From_Mid_Speed_Landing" &&
               current_state.state != "Wall_Colliding_From_High_Speed_Takeoff" &&
               last_state.state != "Wall_Colliding_From_High_Speed_Takeoff" &&
               current_state.state != "Wall_Colliding_From_High_Speed_Arc" &&
               last_state.state != "Wall_Colliding_From_High_Speed_Arc" &&
               current_state.state != "Wall_Colliding_From_High_Speed_Falling" &&
               last_state.state != "Wall_Colliding_From_High_Speed_Falling" &&
               current_state.state != "Wall_Colliding_From_High_Speed_Landing" &&
               last_state.state != "Wall_Colliding_From_High_Speed_Landing") {
            // Check to see if we need to move the avatar back to adjacent
            std::vector<Tile_Bitmap*> *adjacent_tile = new std::vector<Tile_Bitmap*>();
            Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tile, &m_floor_tiles);
            if (out_colliding_wall_tiles->begin()->collision_side != current_state.direction) {
              if (current_state.state == "Standing_To_Running") {
                Avatar_Helper::SetAvatarState(m_avatar,
                                              m_game_settings->GetTilesetPath(),
                                              m_animation_metadata,
                                              "Charlie_Running",
                                              "Wall_Colliding",
                                              m_avatar->GetState().direction,
                                              m_current_metadata_file_path,
                                              m_current_metadata,
                                              m_current_animation_subset);
                m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
              } else {
                 if ((*m_distance_traveled) <= m_stopping_from_running_distance) {
                   Avatar_Helper::SetAvatarState(m_avatar,
                                                 m_game_settings->GetTilesetPath(),
                                                 m_animation_metadata,
                                                 "Charlie_Running",
                                                 "Stopping",
                                                 m_avatar->GetState().direction,
                                                 m_current_metadata_file_path,
                                                 m_current_metadata,
                                                 m_current_animation_subset);
                   m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
                 } else if (((*m_distance_traveled) <= m_wall_colliding_mid_distance ||
                               last_state.state == "Gap_Jump_Landing") && 
                              (last_state.state != "Wall_Colliding_From_Mid_Speed_Landing")) { 
                   Avatar_Helper::SetAvatarState(m_avatar,
                                                 m_game_settings->GetTilesetPath(),
                                                 m_animation_metadata,
                                                 "Charlie_Running",
                                                 "Wall_Colliding_From_Mid_Speed_Takeoff",
                                                 m_avatar->GetState().direction,
                                                 m_current_metadata_file_path,
                                                 m_current_metadata,
                                                 m_current_animation_subset);
                    m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
                  } else if ((m_avatar->GetVelocity().x == m_running_x_velocity ||
                           m_avatar->GetVelocity().x == m_running_x_velocity * -1) && 
                          (last_state.state != "Wall_Colliding_From_High_Speed_Landing")) { 
                     Avatar_Helper::SetAvatarState(m_avatar,
                                                   m_game_settings->GetTilesetPath(),
                                                   m_animation_metadata,
                                                   "Charlie_Running",
                                                   "Wall_Colliding_From_High_Speed_Takeoff",
                                                   m_avatar->GetState().direction,
                                                   m_current_metadata_file_path,
                                                   m_current_metadata,
                                                   m_current_animation_subset);
                     m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
                   } else {
                     Avatar_Helper::SetAvatarState(m_avatar,
                                                   m_game_settings->GetTilesetPath(),
                                                   m_animation_metadata,
                                                   "Charlie_Running",
                                                   "Wall_Colliding",
                                                   m_avatar->GetState().direction,
                                                   m_current_metadata_file_path,
                                                   m_current_metadata,
                                                   m_current_animation_subset);
                     m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
                   }
              }
              Align_New_Avatar_State(); 
              if (!adjacent_tile->empty()) {
                Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                      "Top",
                                                      *(adjacent_tile->begin()));
              }
              (*m_distance_traveled) = 0;
              has_state_changed = true;
            }
          } else if (current_state.state != "Stopping" && last_state.state == "Stopping") {
            if (out_colliding_wall_tiles->begin()->collision_side != current_state.direction) {
            // Check to see if we need to move the avatar back to adjacent
            std::vector<Tile_Bitmap*> *adjacent_tile = new std::vector<Tile_Bitmap*>();
            Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tile, &m_floor_tiles);
             Avatar_Helper::SetAvatarState(m_avatar,
                                           m_game_settings->GetTilesetPath(),
                                           m_animation_metadata,
                                           "Charlie_Running",
                                           "Wall_Colliding",
                                           m_avatar->GetState().direction,
                                           m_current_metadata_file_path,
                                           m_current_metadata,
                                           m_current_animation_subset);
              Align_New_Avatar_State(); 
              if (!adjacent_tile->empty()) {
                Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                      "Top",
                                                      *(adjacent_tile->begin()));
              }
              has_state_changed = true;
            }
          }
        }

        // Move back avatar
        if ((*out_colliding_wall_tiles->begin()).collision_side == "Right") {
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                "Right",
                                                out_colliding_wall_tiles->begin()->colliding_tile);
        } else  if ((*out_colliding_wall_tiles->begin()).collision_side == "Left") {
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                "Left",
                                                out_colliding_wall_tiles->begin()->colliding_tile);
        }
      }
      delete out_colliding_wall_tiles;

      // Detect if the avatar is overbalancing from running 
      vector<Tile_Bitmap*> *adjacent_tiles = new vector<Tile_Bitmap*>();
      if (!Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tiles, &m_floor_tiles) &&
           current_state.state != "Wall_Colliding_From_Mid_Speed_Takeoff" &&
           current_state.state != "Wall_Colliding_From_Mid_Speed_Arc" &&
           current_state.state != "Wall_Colliding_From_Mid_Speed_Falling" &&
           current_state.state != "Wall_Colliding_From_Mid_Speed_Landing" &&
           current_state.state != "Wall_Colliding_From_High_Speed_Takeoff" &&
           current_state.state != "Wall_Colliding_From_High_Speed_Arc" &&
           current_state.state != "Wall_Colliding_From_High_Speed_Falling" &&
           current_state.state != "Wall_Colliding_From_High_Speed_Landing") {
        if (adjacent_tiles->empty()) {
          // This algorithm puts the avatar BACK on the tile it overbalanced from
          vector<Tile_Bitmap*> *adjacent_tile = new vector<Tile_Bitmap*>();
          int offset = 0;
          bool try_opposite_direction = false;
          while (!Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tile, &m_floor_tiles)) {
            if (m_avatar->GetState().direction == "Right") {
              offset -= 8;
            } else {
              offset += 8;
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
                string error;
                error = "No Adjacent Tile Found!";
                throw Exceptions::init_error(error);
              }
            }
          }
          delete adjacent_tiles;
          adjacent_tiles = adjacent_tile;
        }

        if (current_state.state == "Stopping") {
          if (current_state.direction == "Right") {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                  "Right",
                                                 *(adjacent_tiles->begin()));
            Avatar_Component::Avatar_Collision_Block avatar = Avatar_Helper::GetNamedCollisionBlock("Avatar", current_state.avatar_collision_blocks);
            D3DXVECTOR3 position = *m_avatar->GetPosition();
            position.x -= avatar.size.x;
            m_avatar->SetPosition(position);
          } else  if (current_state.direction == "Left") {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Left", *(adjacent_tiles->begin()));
            Avatar_Component::Avatar_Collision_Block avatar = Avatar_Helper::GetNamedCollisionBlock("Avatar", current_state.avatar_collision_blocks);
            D3DXVECTOR3 position = *m_avatar->GetPosition();
            position.x += avatar.size.x;
            m_avatar->SetPosition(position);
          }
        } else {
          if (m_avatar->GetState().direction == m_avatar->GetLastRenderedState().direction) {
            if (current_state.direction == "Right") {
              float y_velocity = m_overbalancing_y_velocity;
              float x_velocity = m_overbalancing_x_velocity;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
            } else {  // Left
              float y_velocity = m_overbalancing_y_velocity;
              float x_velocity = m_overbalancing_x_velocity * -1;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
            }

            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Gap_Jump_Arc_Fall",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          } else {
            if (current_state.direction == "Right") {
              float y_velocity = m_overbalancing_y_velocity;
              float x_velocity = m_overbalancing_x_velocity;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
            } else {  // Left
              float y_velocity = m_overbalancing_y_velocity;
              float x_velocity = m_overbalancing_x_velocity * -1;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
            }

            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Gap_Jump_Arc_Fall",
                                          m_avatar->GetLastRenderedState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          }

          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          has_state_changed = true;
        }
      }
      if (adjacent_tiles != 0) {
        adjacent_tiles->clear();
        delete adjacent_tiles;
        adjacent_tiles = 0;
      }
      
      vector<Avatar_Helper::Tile_Collision> *out_colliding_floor_tiles = new vector<Avatar_Helper::Tile_Collision>();
      bool is_floor_colliding = Avatar_Helper::IsAvatarFloorColliding(m_avatar, out_colliding_floor_tiles, &m_floor_tiles);
      if (is_floor_colliding) {
        if (current_state.state == "Wall_Colliding_From_Mid_Speed_Arc" ||
            current_state.state == "Wall_Colliding_From_Mid_Speed_Falling") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Wall_Colliding_From_Mid_Speed_Landing",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                "Top",
                                                out_colliding_floor_tiles->begin()->colliding_tile);
          m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
          (*m_distance_traveled) = 0;
          has_state_changed = true;
        } else if (current_state.state == "Wall_Colliding_From_High_Speed_Arc" ||
                   current_state.state == "Wall_Colliding_From_High_Speed_Falling") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Running",
                                        "Wall_Colliding_From_High_Speed_Landing",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                "Top",
                                                out_colliding_floor_tiles->begin()->colliding_tile);

          m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
          (*m_distance_traveled) = 0;
          has_state_changed = true;
        }
      }
      if (adjacent_tiles != 0) {
        adjacent_tiles->clear();
        delete adjacent_tiles;
        adjacent_tiles = 0;
      }
    }
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Charlie_Running_Controller::Align_New_Avatar_State() {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  if (m_avatar->GetVelocity().x > 0) {
    Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
  } else if (m_avatar->GetVelocity().x < 0) {
    Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
  } else if (current_state.direction == "Right") {
    Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
  } else if  (current_state.direction == "Left") {
    Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
  } else {
    Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
  }
}

}  // namespace Tunnelour
