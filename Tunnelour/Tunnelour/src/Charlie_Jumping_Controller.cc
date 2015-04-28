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

#include "Charlie_Jumping_Controller.h"
#include "Avatar_Helper.h"
#include "Exceptions.h"
#include "String.h"

using std::string;
using std::vector;

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Charlie_Jumping_Controller::Charlie_Jumping_Controller():
  // Constant Variables
  m_avatar_z_position(-2),
  m_vertical_jump_y_initial_velocity(22),
  m_vertical_jump_x_initial_velocity(4),
  m_gap_jump_y_initial_velocity(38),
  m_gap_jump_x_initial_velocity(38),
  m_running_x_velocity(32),
  m_wall_jump_y_initial_velocity(8),
  m_wall_jump_x_initial_velocity(4),
  m_wall_jump_boost_distance_threshold(128),
  m_safe_falling_limit(-512) {
}

//------------------------------------------------------------------------------
Charlie_Jumping_Controller::~Charlie_Jumping_Controller() {
}

//------------------------------------------------------------------------------
bool Charlie_Jumping_Controller::Init(Component_Composite *const model) {
  m_model = model;
  m_has_been_initialised = true;
  return true;
}

//------------------------------------------------------------------------------
void Charlie_Jumping_Controller::Run_Avatar_State() {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State last_state = m_avatar->GetLastRenderedState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();
  bool has_state_changed = false;

  // This state does not recognize the follwing commands:
  if (current_command.state == "Down" ||
        current_command.state == "Looking") {
    current_command.state = "";
  }

  unsigned int state_index = current_state.state_index;
  state_index++;
  // If current state equals the last state this is the first run through of
  // this state since it was rendered.
  // Alternatively if current_state does not equal last state this is the
  // first run through of this state since it was changed in either this
  // state controller or a different state controller by SetAvatarState.
  if (current_state == last_state) {
    // State includes 0 as the first state.
    if (state_index > (m_current_animation_subset->number_of_frames - 1)) {
      if (m_current_animation_subset->is_repeatable) {
        if (current_state.state == "Wall_Jump_Rising") {
          if (m_avatar->GetVelocity().y <= 0) {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Wall_Jump_Fall_Arc",
                                          current_state.direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);
            has_state_changed = true;
          }
        } else if (current_state.state == "Wall_Jump_Falling") {
          if ((*m_y_fallen) < m_safe_falling_limit) {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Falling",
                                          "Down_Facing_Falling",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
            has_state_changed = true;
          }
        } else if (current_state.state == "Gap_Jump_Falling") {
          if ((*m_y_fallen) < m_safe_falling_limit) {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Falling",
                                          "Down_Facing_Falling",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
            has_state_changed = true;
          }
        } else if (current_state.state == "Vertical_Jump_Arc") {
          if ((*m_y_fallen) < m_safe_falling_limit) {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Falling",
                                          "Down_Facing_Falling",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
            has_state_changed = true;
          }
        }
        // This is set for alignment on the previous state
        state_index = 0;
      } else {
        if (current_state.state == "Wall_Jump_Takeoff") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Wall_Jump_Rise_Arc",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state == "Wall_Jump_Rise_Arc") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Wall_Jump_Rising",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state == "Wall_Jump_Fall_Arc") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Wall_Jump_Falling",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state == "Vertical_Jump_Takeoff") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Vertical_Jump_Arc",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state == "Vertical_Jump_Landing" ||
                     current_state.state == "Wall_Jump_Landing") {
          m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));

          if (current_command.state == "Jumping") {
            float y_velocity = m_vertical_jump_y_initial_velocity;
            float x_velocity = m_vertical_jump_x_initial_velocity;
            if (current_state.direction == "Left") {
              x_velocity *= -1;
            }
            m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));

            if (current_command.direction == "Right" ||
                  current_command.direction == "Left" ) {
              Avatar_Helper::SetAvatarState(m_avatar,
                                            m_game_settings->GetTilesetPath(),
                                            m_animation_metadata,
                                            "Charlie_Jumping",
                                            "Vertical_Jump_Takeoff",
                                            current_command.direction,
                                            m_current_metadata_file_path,
                                            m_current_metadata,
                                            m_current_animation_subset);
            } else {
              Avatar_Helper::SetAvatarState(m_avatar,
                                            m_game_settings->GetTilesetPath(),
                                            m_animation_metadata,
                                            "Charlie_Jumping",
                                            "Vertical_Jump_Takeoff",
                                            m_avatar->GetState().direction,
                                            m_current_metadata_file_path,
                                            m_current_metadata,
                                            m_current_animation_subset);
            }
            if (m_avatar->GetState().direction == "Right") {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            } else {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            }
            has_state_changed = true;
          } else if (current_command.state == "Running") {
              Avatar_Helper::SetAvatarState(m_avatar,
                                            m_game_settings->GetTilesetPath(),
                                            m_animation_metadata,
                                            "Charlie_Running",
                                            "Standing_To_Running",
                                            current_command.direction,
                                            m_current_metadata_file_path,
                                            m_current_metadata,
                                            m_current_animation_subset);
              if (current_command.direction == "Right") {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
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
            if (m_avatar->GetState().direction == "Right") {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            } else {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            }
          }
          has_state_changed = true;
          (*m_y_fallen) = 0;
          (*m_distance_traveled) = 0;
        } else if (current_state.state == "Gap_Jump_Takeoff") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Gap_Jump_Arc_Rise",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state == "Gap_Jump_Arc_Rise") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Gap_Jump_Arc_Fall",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state == "Gap_Jump_Arc_Fall") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Jumping",
                                        "Gap_Jump_Falling",
                                        current_state.direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlock(m_avatar);;
          has_state_changed = true;
        } else if (current_state.state == "Gap_Jump_Landing") {
          m_avatar->SetVelocity(D3DXVECTOR3(m_avatar->GetVelocity().x, 0, 0));
          if (current_command.state == "Jumping") {
            // Avatar is Jumping after a Gap Jump
            if (current_command.direction == current_state.direction) {
              float y_velocity = m_gap_jump_y_initial_velocity;
              float x_velocity = m_gap_jump_x_initial_velocity;
              if (current_command.direction == "Left") {
                x_velocity *= -1;
              }
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));

              if (current_command.direction == "Right" ||
                    current_command.direction == "Left") {
                Avatar_Helper::SetAvatarState(m_avatar,
                                              m_game_settings->GetTilesetPath(),
                                              m_animation_metadata,
                                              "Charlie_Jumping",
                                              "Gap_Jump_Takeoff",
                                              current_command.direction,
                                              m_current_metadata_file_path,
                                              m_current_metadata,
                                              m_current_animation_subset);
              } else {
                Avatar_Helper::SetAvatarState(m_avatar,
                                              m_game_settings->GetTilesetPath(),
                                              m_animation_metadata,
                                              "Charlie_Jumping",
                                              "Gap_Jump_Takeoff",
                                              m_avatar->GetState().direction,
                                              m_current_metadata_file_path,
                                              m_current_metadata,
                                              m_current_animation_subset);
              }
              if (m_avatar->GetState().direction == "Right") {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
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
              if (m_avatar->GetState().direction == "Right") {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
              has_state_changed = true;
              (*m_is_moving_continuously) = false;
            }
          } else if (current_command.state == "Running") {
            // After is running after landing from a Gap Jump
            if (current_command.direction == current_state.direction) {
              (*m_is_moving_continuously) = true;
              D3DXVECTOR3 new_velocity(m_running_x_velocity, 0, 0);
              if (current_state.direction == "Left") {
                 new_velocity.x *= -1;
              }
              m_avatar->SetVelocity(new_velocity);

              D3DXVECTOR3 position = *m_avatar->GetPosition();
              position += new_velocity;
              (*m_y_fallen) += m_avatar->GetVelocity().y;
              m_avatar->SetPosition(position);

              Avatar_Helper::SetAvatarState(m_avatar,
                                            m_game_settings->GetTilesetPath(),
                                            m_animation_metadata,
                                            "Charlie_" + current_command.state,
                                            current_command.state,
                                            current_command.direction,
                                            m_current_metadata_file_path,
                                            m_current_metadata,
                                            m_current_animation_subset);
              if (m_avatar->GetState().direction == "Right") {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
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
              if (m_avatar->GetState().direction == "Right") {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
              } else {
                Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
              }
              has_state_changed = true;
              (*m_is_moving_continuously) = false;
            }
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
            if (m_avatar->GetState().direction == "Right") {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            } else {
              Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            }
          }
          has_state_changed = true;
          (*m_y_fallen) = 0;
        } else {
          string error;
          error = "No handling for the non-repeating animation: " + current_command.state;
          throw Exceptions::init_error(error);
        }
      }
    }
  }

  if (!has_state_changed) {
    // If current state equals the last state this is the first run through of
    // this state since it was rendered.
    // Alternatively if current_state does not equal last state this is the
    // first run through of this state since it was changed in either this
    // state controller or a different state controller by SetAvatarState.
    if (current_state == last_state) {
      Avatar_Helper::SetAvatarStateAnimationFrame(m_avatar,
                                                  state_index,
                                                  m_current_animation_subset);
      // Align the new avatar state on the last
      if (current_state.state == "Wall_Jump_Takeoff" ||
          current_state.state == "Vertical_Jump_Landing" ||
          current_state.state == "Vertical_Jump_Takeoff" ||
          current_state.state == "Gap_Jump_Takeoff" ||
          current_state.state == "Gap_Jump_Landing") {
        Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
      } else if (current_state.state == "Wall_Jump_Rise_Arc" ||
                 current_state.state == "Wall_Jump_Rising" ||
                 current_state.state == "Wall_Jump_Fall_Arc" ||
                 current_state.state == "Wall_Jump_Falling" ) {
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                              m_adjacent_wall->collision_side,
                                              m_adjacent_wall->colliding_tile);
      } else {
        if (m_avatar->GetState().direction == "Right") {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
        } else {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
        }
      }
    }

    // Move the avatar!
    if (current_state.state == "Vertical_Jump_Arc" ||
        current_state.state == "Gap_Jump_Arc_Rise" ||
        current_state.state == "Gap_Jump_Arc_Fall" ||
        current_state.state == "Gap_Jump_Falling" ||
        current_state.state == "Wall_Jump_Rise_Arc" ||
        current_state.state == "Wall_Jump_Rising" ||
        current_state.state == "Wall_Jump_Fall_Arc" ||
        current_state.state == "Wall_Jump_Falling") {
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
    } else if (current_state.state == "Gap_Jump_Landing") {
      D3DXVECTOR3 position = *m_avatar->GetPosition();
      position.x += m_avatar->GetVelocity().x;
      m_avatar->SetPosition(position);
    }

    // Is the new avatar tile and position colliding with a wall tile?
    vector<Avatar_Helper::Tile_Collision> *out_colliding_ledge_tiles = new vector<Avatar_Helper::Tile_Collision>();
    bool can_avatar_grab_a_ledge = Avatar_Helper::CanAvatarGrabALedge(m_avatar, out_colliding_ledge_tiles, &m_ledge_tiles);
    vector<Avatar_Helper::Tile_Collision> *out_colliding_wall_tiles = new vector<Avatar_Helper::Tile_Collision>();
    bool is_wall_colliding = false;
    is_wall_colliding = Avatar_Helper::IsAvatarWallColliding(m_avatar,
                                                             out_colliding_wall_tiles,
                                                             &m_wall_tiles);
    if (is_wall_colliding) {
      (*m_adjacent_wall) = *(out_colliding_wall_tiles->begin());
      if (current_state.state == "Vertical_Jump_Takeoff" ||
          current_state.state == "Vertical_Jump_Arc" ||
          current_state.state == "Vertical_Jump_Landing" ||
          current_state.state == "Wall_Jump_Takeoff" ||
          current_state.state == "Wall_Jump_Rise_Arc" ||
          current_state.state == "Wall_Jump_Rising" ||
          current_state.state == "Wall_Jump_Fall_Arc" ||
          current_state.state == "Wall_Jump_Falling" ||
          current_state.state == "Wall_Jump_Landing") {
        float y_velocity = m_avatar->GetVelocity().y;
        float x_velocity = 0;
        m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                              out_colliding_wall_tiles->begin()->collision_side,
                                              out_colliding_wall_tiles->begin()->colliding_tile);
      } else if (can_avatar_grab_a_ledge) {
        Avatar_Helper::SetAvatarState(m_avatar,
                                      m_game_settings->GetTilesetPath(),
                                      m_animation_metadata,
                                      "Charlie_Climbing",
                                      "Descending_To_Grabbing",
                                      m_avatar->GetState().direction,
                                      m_current_metadata_file_path,
                                      m_current_metadata,
                                      m_current_animation_subset);
        Avatar_Helper::AlignAvatarOnLastLedgeEdge(m_avatar,
                                                *(out_colliding_ledge_tiles->begin()));
        *m_currently_grabbed_tile = out_colliding_ledge_tiles->begin()->colliding_tile;
        (*m_y_fallen) = 0;
        (*m_distance_traveled) = 0;
        has_state_changed = true;
      } else if (current_state.state == "Gap_Jump_Landing") {
        Avatar_Helper::SetAvatarState(m_avatar,
                                      m_game_settings->GetTilesetPath(),
                                      m_animation_metadata,
                                      "Charlie_Running",
                                      "Wall_Colliding_From_Mid_Speed_Takeoff",
                                      m_avatar->GetState().direction,
                                      m_current_metadata_file_path,
                                      m_current_metadata,
                                      m_current_animation_subset);
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                              m_adjacent_wall->collision_side,
                                              m_adjacent_wall->colliding_tile);
        (*m_distance_traveled) = 0;
        has_state_changed = true;
      } else {
        // This is called when the avatar is jumping
        // and the command is to jump
        // and the avatar is colliding with a wall.
        if (current_command.state == "Jumping") {
          float y_velocity = m_wall_jump_y_initial_velocity;
          float x_velocity = m_wall_jump_x_initial_velocity;
          if (m_avatar->GetVelocity().y > 0) {
              if ((*m_distance_traveled) > m_wall_jump_boost_distance_threshold) {
              if (current_state.state == "Gap_Jump_Takeoff" ||
                   (current_state.state == "Gap_Jump_Arc_Rise" &&
                     (current_state.state_index == 0 ||
                      current_state.state_index == 1))) {
                y_velocity = m_vertical_jump_y_initial_velocity + m_wall_jump_y_initial_velocity;
              } 
            }
          }

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
          has_state_changed = true;
          if (out_colliding_wall_tiles->begin()->collision_side == "Right") {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
          } else {
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
          }
          if (out_colliding_wall_tiles->begin()->collision_side == "Left") {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                  "Left",
                                                  out_colliding_wall_tiles->begin()->colliding_tile);
          } else {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                  "Right",
                                                  out_colliding_wall_tiles->begin()->colliding_tile);
          }
        } else if ((*m_y_fallen) < m_safe_falling_limit) {
          if (m_avatar->GetState().direction == "Right") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Vertical_Jump_Arc",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            has_state_changed = true;
          } else {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Vertical_Jump_Arc",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            has_state_changed = true;
          }
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(0, 0, 0);
          m_avatar->SetVelocity(new_velocity);
          if (out_colliding_wall_tiles->begin()->collision_side == "Left") {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
              "Left", out_colliding_wall_tiles->begin()->colliding_tile);
          } else {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                  "Right",
                                                  out_colliding_wall_tiles->begin()->colliding_tile);
          }
        } else {
          if (m_avatar->GetState().direction == "Right") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Falling",
                                          "Down_Falling_Wall_Impact_Right",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
            has_state_changed = true;
          } else {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Falling",
                                          "Down_Falling_Wall_Impact_Right",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
            has_state_changed = true;
          }
          D3DXVECTOR3 old_velocity = m_avatar->GetVelocity();
          D3DXVECTOR3 new_velocity = D3DXVECTOR3(0, old_velocity.y, 0);
          m_avatar->SetVelocity(new_velocity);
          if (out_colliding_wall_tiles->begin()->collision_side == "Left") {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                  "Left",
                                                  out_colliding_wall_tiles->begin()->colliding_tile);
          } else {
            Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                                  "Right",
                                                  out_colliding_wall_tiles->begin()->colliding_tile);
          }
        }
      }
    }
    delete out_colliding_wall_tiles;
    delete out_colliding_ledge_tiles;

    // Is the new avatar size and position colliding with the floor?
    if (current_state.state == "Gap_Jump_Arc_Fall" ||
        current_state.state == "Gap_Jump_Falling" ||
        current_state.state == "Vertical_Jump_Arc" ||
        current_state.state == "Wall_Jump_Fall_Arc" ||
        current_state.state == "Wall_Jump_Falling" ||
        current_state.state == "Wall_Jump_Rise_Arc" ||
        current_state.state == "Wall_Jump_Rising") {
      vector<Tile_Bitmap*> *adjacent_tiles = new vector<Tile_Bitmap*>();
      vector<Avatar_Helper::Tile_Collision> *out_colliding_floor_tiles = new vector<Avatar_Helper::Tile_Collision>();
      bool is_floor_colliding = Avatar_Helper::IsAvatarFloorColliding(m_avatar, out_colliding_floor_tiles, &m_floor_tiles);
      if (is_floor_colliding) {
        if ((*m_y_fallen) < m_safe_falling_limit) {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Falling",
                                        "Down_Facing_Falling_To_Death",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata, 
                                        m_current_animation_subset);
        } else {
          if (current_state.state == "Vertical_Jump_Arc") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Vertical_Jump_Landing",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          } else if (current_state.state == "Gap_Jump_Arc_Fall" ||
                       current_state.state == "Gap_Jump_Falling") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Gap_Jump_Landing",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          } else if (current_state.state == "Wall_Jump_Falling" ||
                       current_state.state == "Wall_Jump_Fall_Arc") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Wall_Jump_Landing",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          }
        }

        if (m_avatar->GetState().direction == "Right") {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftBottom(m_avatar);
        } else {
          Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightBottom(m_avatar);
        }
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                              "Top",
                                              out_colliding_floor_tiles->begin()->colliding_tile);
        // Clear the velocity
        m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));

        (*m_y_fallen) = 0;
        has_state_changed = true;
      } else if (Avatar_Helper::IsAvatarFloorAdjacent(m_avatar,
                                                      adjacent_tiles,
                                                      &m_floor_tiles)) {
        if ((*m_y_fallen) < m_safe_falling_limit) {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Falling",
                                        "Down_Facing_Falling_To_Death",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
        } else {
          if (current_state.state == "Vertical_Jump_Arc") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Vertical_Jump_Landing",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          } else if (current_state.state == "Gap_Jump_Arc_Fall" ||
                     current_state.state == "Gap_Jump_Falling") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Gap_Jump_Landing",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          } else if (current_state.state == "Wall_Jump_Falling" ||
                     current_state.state == "Wall_Jump_Rising" ||
                     current_state.state == "Wall_Jump_Rise_Arc" ||
                     current_state.state == "Wall_Jump_Fall_Arc") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Jumping",
                                          "Wall_Jump_Landing",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
          }
        }

        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                              "Top",
                                            *(adjacent_tiles->begin()));

        m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
        has_state_changed = true;
        (*m_y_fallen) = 0;
      }
      delete adjacent_tiles;
      delete out_colliding_floor_tiles;
    }

    // Check to see if the avatar is in range to grab a ledge
    if (current_state.state == "Vertical_Jump_Takeoff" ||
        current_state.state == "Vertical_Jump_Arc" ||
        current_state.state == "Vertical_Jump_Landing" ||
        current_state.state == "Gap_Jump_Takeoff" ||
        current_state.state == "Gap_Jump_Arc_Rise" ||
        current_state.state == "Gap_Jump_Arc_Fall" ||
        current_state.state == "Gap_Jump_Falling" ||
        current_state.state == "Wall_Jump_Takeoff" ||
        current_state.state == "Wall_Jump_Rise_Arc" ||
        current_state.state == "Wall_Jump_Fall_Arc" ||
        current_state.state == "Wall_Jump_Rising" ||
        current_state.state == "Wall_Jump_Falling") {
      vector<Avatar_Helper::Tile_Collision> *out_colliding_floor_tiles = new vector<Avatar_Helper::Tile_Collision>();
      bool can_avatar_grab_a_ledge = Avatar_Helper::CanAvatarGrabALedge(m_avatar, out_colliding_floor_tiles, &m_ledge_tiles);
      if (can_avatar_grab_a_ledge) {
          if (current_state.state == "Wall_Jump_Rise_Arc" ||
              current_state.state == "Wall_Jump_Fall_Arc" ||
              current_state.state == "Wall_Jump_Rising" ||
              current_state.state == "Wall_Jump_Falling") {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Climbing",
                                          "Ascending_To_Grabbing",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastLedgeEdge(m_avatar,
                                                    *(out_colliding_floor_tiles->begin()));
            *m_currently_grabbed_tile = out_colliding_floor_tiles->begin()->colliding_tile;
            (*m_y_fallen) = 0;
            (*m_distance_traveled) = 0;
            has_state_changed = true;
          } else {
            Avatar_Helper::SetAvatarState(m_avatar,
                                          m_game_settings->GetTilesetPath(),
                                          m_animation_metadata,
                                          "Charlie_Climbing",
                                          "Pop_Up",
                                          m_avatar->GetState().direction,
                                          m_current_metadata_file_path,
                                          m_current_metadata,
                                          m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastLedgeEdge(m_avatar,
                                                    *(out_colliding_floor_tiles->begin()));
            *m_currently_grabbed_tile = out_colliding_floor_tiles->begin()->colliding_tile;
            (*m_y_fallen) = 0;
            (*m_distance_traveled) = 0;
            has_state_changed = true;
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
