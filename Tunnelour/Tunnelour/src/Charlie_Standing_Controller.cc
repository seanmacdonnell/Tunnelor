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

#include "Charlie_Standing_Controller.h"
#include "Avatar_Helper.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Charlie_Standing_Controller::Charlie_Standing_Controller():
  // Constant Variables
  m_avatar_z_position(-2),
  m_vertical_jump_y_initial_velocity(22),
  m_vertical_jump_x_initial_velocity(4),
  m_gap_jump_y_initial_velocity(24),
  m_gap_jump_x_initial_velocity(24),
  m_takeoff_gap_jump_y_initial_velocity(18),
  m_takeoff_gap_jump_x_initial_velocity(18),
  m_running_x_velocity(32),
  m_takeoff_running_x_velocity(16),
  m_takeoff_running_x_distance(128),
  m_stopping_from_running_distance(64),
  m_overbalancing_x_velocity(16),
  m_overbalancing_y_velocity(-4),
  m_wall_jump_y_initial_velocity(16),
  m_wall_jump_x_initial_velocity(4),
  m_wall_jump_boost_distance_threshold(128),
  m_wall_colliding_y_initial_velocity(4),
  m_wall_colliding_x_initial_velocity(16),
  m_safe_falling_limit(-1280) {
}

//------------------------------------------------------------------------------
Charlie_Standing_Controller::~Charlie_Standing_Controller() {
}

//------------------------------------------------------------------------------
bool Charlie_Standing_Controller::Init(Component_Composite *const model) {
  Avatar_State_Controller::Init(model);
  m_has_been_initialised = true;
  return true;
}

//------------------------------------------------------------------------------
void Charlie_Standing_Controller::Run_Avatar_State() {
  // This is depreciated
  //  m_avatar->SetAngle(static_cast<float>(0.0));  // in radians
  // Investigate whether this needs to be set every time the state runs
  m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

  bool has_state_changed = false;

  if (current_command.state == "") {
    current_command.state = "Standing";
  }

  // If there is a command (Left, Right, Run, Etc.)
  // or the direction is different
  if (current_command.state != "" &&
        current_state.state != current_command.state) {
    std::vector<Tile_Bitmap*> *adjacent_tiles = new std::vector<Tile_Bitmap*>();
    Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tiles, &m_floor_tiles);
    if (current_command.state == "Jumping") {
      float y_velocity = m_vertical_jump_y_initial_velocity;
      float x_velocity = m_vertical_jump_x_initial_velocity;
      if (current_command.direction == "Right" ||
            current_command.direction == "Left") {
        Avatar_Helper::SetAvatarState(m_avatar,
                                      m_game_settings->GetTilesetPath(),
                                      m_animation_metadata,
                                      "Charlie_Jumping",
                                      "Vertical_Jump_Takeoff",
                                      current_command.direction,
                                      m_current_metadata_file_path,
                                      m_current_metadata,
                                      m_current_animation_subset);
        if (current_command.direction == "Left") {
          x_velocity *= -1;
        }
        m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
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
        if (current_state.direction == "Left") {
          x_velocity *= -1;
        }
        m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
      }
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
      has_state_changed = true;
    } else if (current_command.state == "Looking") {
      Avatar_Helper::SetAvatarState(m_avatar,
                                    m_game_settings->GetTilesetPath(),
                                    m_animation_metadata,
                                    "Charlie_Standing",
                                    "Looking",
                                    m_avatar->GetState().direction,
                                    m_current_metadata_file_path,
                                    m_current_metadata,
                                    m_current_animation_subset);
      has_state_changed = true;
    } else if (current_command.state == "Standing") {
      Avatar_Helper::SetAvatarState(m_avatar,
                                    m_game_settings->GetTilesetPath(),
                                    m_animation_metadata,
                                    "Charlie_Standing",
                                    "Standing",
                                    m_avatar->GetState().direction,
                                    m_current_metadata_file_path,
                                    m_current_metadata,
                                    m_current_animation_subset);
      has_state_changed = true;
    }
    Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
    if (!adjacent_tiles->empty()) {
      Avatar_Helper::MoveAvatarTileAdjacent(m_avatar,
                                            "Top",
                                            *(adjacent_tiles->begin()));
    }
  } else {  // No command, run the standing animation,
    unsigned int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset->number_of_frames - 1)) {
      if (m_current_animation_subset->is_repeatable) {
        state_index = 0;
      } else {
        if (current_state.state == "Initial") {
          Avatar_Helper::SetAvatarState(m_avatar,
                                        m_game_settings->GetTilesetPath(),
                                        m_animation_metadata,
                                        "Charlie_Standing",
                                        "Standing",
                                        m_avatar->GetState().direction,
                                        m_current_metadata_file_path,
                                        m_current_metadata,
                                        m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        } else {
          std::string error;
          error = "No handling for this non-repeating animation: ";
          error += current_command.state;
          throw Exceptions::init_error(error);
        }
      }
    }

    if (m_avatar->GetState() == m_avatar->GetLastRenderedState()) {
      Avatar_Helper::SetAvatarStateAnimationFrame(m_avatar,
                                                  state_index,
                                                  m_current_animation_subset);
    }
  }

  if (!HasAvatarStateChanged()) {
    // Detect if the avatar is intersecting with a wall
    std::vector<Avatar_Helper::Tile_Collision> *out_colliding_wall_tiles = new std::vector<Avatar_Helper::Tile_Collision>();
    if (Avatar_Helper::IsAvatarWallColliding(m_avatar, out_colliding_wall_tiles, &m_wall_tiles)) {
      m_adjacent_wall = &(*(out_colliding_wall_tiles->begin()));
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

    // If the avatar is not adjacent to the floor, she should fall
    std::vector<Tile_Bitmap*> *adjacent_tiles = new std::vector<Tile_Bitmap*>();
    if (!Avatar_Helper::IsAvatarFloorAdjacent(m_avatar,
                                              adjacent_tiles,
                                              &m_floor_tiles)) {
      if (m_avatar->GetState().direction == m_avatar->GetLastRenderedState().direction) {
        float y_velocity = m_overbalancing_y_velocity;
        float x_velocity = m_overbalancing_x_velocity;
        if (current_state.direction == "Left") {
          x_velocity *= -1;
        }
        m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));

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
        float y_velocity = m_overbalancing_y_velocity;
        float x_velocity = m_overbalancing_x_velocity;
        if (m_avatar->GetLastRenderedState().direction == "Left") {
          x_velocity *= -1;
        }
        m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));

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
    delete adjacent_tiles;
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------

}  // namespace Tunnelour
