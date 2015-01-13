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

#include "Charlie_Climbing_Controller.h"

#include "Avatar_Helper.h"
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Charlie_Climbing_Controller::Charlie_Climbing_Controller() {
  m_avatar_z_position = -2;  // Middleground Z Space is -1
  m_vertical_jump_y_initial_velocity = 22;
  m_vertical_jump_x_initial_velocity = 4;
  m_wall_jump_y_initial_velocity = 16;
}

//------------------------------------------------------------------------------
Charlie_Climbing_Controller::~Charlie_Climbing_Controller() {
}

//------------------------------------------------------------------------------
bool Charlie_Climbing_Controller::Init(Component_Composite *const model) {
  Avatar_State_Controller::Init(model);
  m_has_been_initialised = true;
  return true;
}

//------------------------------------------------------------------------------
void Charlie_Climbing_Controller::Run_Avatar_State() {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();
  Avatar_Component::Avatar_State last_state = m_avatar->GetLastRenderedState();

  bool has_state_changed = false;

  m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));

  // If there is a command (Left, Right, Run, Etc.)
  // The avatar will only respond to commands if they are in the Hanging state
  if (current_command.state.compare("") != 0 && current_state.state.compare("Hanging") == 0) {
    if (current_state.direction.compare(current_command.direction) == 0) {
      // Go up the ledge
      Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Climbing_1", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
      Avatar_Helper::AlignAvatarOnLastHand(m_avatar);
      has_state_changed = true;
    } else if (current_command.state.compare("Down") == 0) {
      // Fall off the ledge
      Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Wall_Jump_Fall_Arc", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
      if (m_avatar->GetState().direction.compare("Right") == 0) {
        Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockRightTop(m_avatar);
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Left", *m_currently_grabbed_tile);
      } else {
        Avatar_Helper::AlignAvatarOnLastAvatarCollisionBlockLeftTop(m_avatar);
        Avatar_Helper::MoveAvatarTileAdjacent(m_avatar, "Right", *m_currently_grabbed_tile);
      }
      has_state_changed = true;
    }
  } else {
    // Continue the current state
    unsigned int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset->number_of_frames - 1)) {
      // State has finished!
      if (m_current_animation_subset->is_repeatable) {
        // Repeat state
        state_index = 0;
      } else {
        // Find the next state
        if (current_state.state.compare("Ascending_To_Grabbing") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Grabbing_To_Hanging", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastHand(m_avatar);
          has_state_changed = true;
        } else if (current_state.state.compare("Grabbing_To_Hanging") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Hanging", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastHand(m_avatar);
          has_state_changed = true;
        } else if (current_state.state.compare("Descending_To_Grabbing") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Grabbing_To_Hanging", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastHand(m_avatar);
          has_state_changed = true;
        } else if (current_state.state.compare("Climbing_1") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Climbing_2", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastHand(m_avatar);
          has_state_changed = true;
        } else if (current_state.state.compare("Climbing_2") == 0) {
          Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Climbing", "Climbing_To_Standing", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
          Avatar_Helper::AlignAvatarOnLastHand(m_avatar);
          has_state_changed = true;
        } else if (current_state.state.compare("Climbing_To_Standing") == 0) {
          m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));
          if (current_command.state.compare("Jumping") == 0) {
            D3DXVECTOR3 position = *m_avatar->GetPosition();
            position += m_avatar->GetVelocity();
            m_avatar->SetPosition(position);
            if (current_command.direction.compare("Right") == 0) {
              float y_velocity = 24;
              float x_velocity = 24;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
            } else {  // Left
              float y_velocity = 24;
              float x_velocity = -24;
              m_avatar->SetVelocity(D3DXVECTOR3(x_velocity, y_velocity, 0));
            }
            if (current_command.direction.compare("Right") == 0 || current_command.direction.compare("Left") == 0) {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Takeoff", current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            } else {
              Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Jumping", "Gap_Jump_Takeoff", m_avatar->GetState().direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            }
            has_state_changed = true;
            Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          } else if (current_command.state.compare("Running") == 0) {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Running", "Standing_To_Running", current_command.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          } else {
            Avatar_Helper::SetAvatarState(m_avatar, m_game_settings->GetTilesetPath(), m_animation_metadata, "Charlie_Standing", "Standing", current_state.direction, m_current_metadata_file_path, m_current_metadata, m_current_animation_subset);
            Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
          }
          has_state_changed = true;
          (*m_y_fallen) = 0;
        } else {
          std::string error;
          error = "No handling for this non-repeating animation: " + current_state.state;
          throw Exceptions::init_error(error);
        }
      }
    }
    if (!has_state_changed) {
      if (current_state == last_state) {
        Avatar_Helper::SetAvatarStateAnimationFrame(m_avatar, state_index, m_current_animation_subset);
        if (current_state.state.compare("Climbing_To_Standing") == 0) {
          Avatar_Helper::AlignAvatarOnLastContactingFoot(m_avatar);
        } else {
          Avatar_Helper::AlignAvatarOnLastHand(m_avatar);
        }
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

}  // namespace Tunnelour
