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

#include "Camera_Controller.h"

#include <math.h>
#include <limits>

#include "Bitmap_Helper.h"
#include "Avatar_Helper.h"
#include "Tile_Bitmap.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Camera_Controller::Camera_Controller() : Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_camera = 0;
  m_is_shaking = false;
  m_adjacent_floor_tile = 0;
  m_distance_travelled = 0;
  m_leash_length = 256;
  m_stationary_avatar_position.x = 0;
  m_stationary_avatar_position.y = 0;
  m_max_x_look_distance = 600;
  m_max_y_look_distance = 300;
  m_input = 0;
}

//------------------------------------------------------------------------------
Camera_Controller::~Camera_Controller() {
  if (m_model != 0) {
    m_model->IgnoreType(this, "Bitmap_Component");
    m_model = 0;
  }
  if (m_avatar != 0) {
    m_avatar->Ignore(this);
    m_avatar = 0;
  }
  m_game_settings = 0;
  m_camera = 0;
  m_is_shaking = false;
  m_floor_tiles.clear();
  m_adjacent_floor_tile = 0;
  m_distance_travelled = 0;
  m_leash_length = 0;
  m_stationary_avatar_position.x = 0;
  m_stationary_avatar_position.y = 0;
  m_max_x_look_distance = 0;
  m_max_y_look_distance = 0;
  m_input = 0;
}

//------------------------------------------------------------------------------
bool Camera_Controller::Init(Component_Composite * const model) {
  bool result = false;
  Controller::Init(model);
  if (m_camera == 0 && m_model != 0) {
    m_camera = new Camera_Component();
    m_model->Add(m_camera);
    m_camera->Init();
  }
  Camera_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_model->ObserveType(this, "Bitmap_Component");
    m_avatar = mutator.GetAvatarComponent();
    m_avatar->Observe(this);
    m_game_settings = mutator.GetGameSettings();
    m_has_been_initialised = true;
    m_floor_tiles = mutator.GetFloorTiles();
    m_input = mutator.GetInputComponent();
    result = true;
  } else {
    result = false;
  }
  return result;
}

//------------------------------------------------------------------------------
bool Camera_Controller::Run() {
  bool result = false;;
  Camera_Controller_Mutator mutator;
  if (m_has_been_initialised) {
    Avatar_Component::Avatar_State current_state = m_avatar->GetState();
    Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();
    Avatar_Component::Avatar_State last_state = m_avatar->GetLastRenderedState();
    if (m_game_settings->IsCameraFollowing()) {
      std::vector<Tile_Bitmap*> *adjacent_tiles = new std::vector<Tile_Bitmap*>();
      if (Avatar_Helper::IsAvatarFloorAdjacent(m_avatar, adjacent_tiles, &m_floor_tiles)) {
        m_adjacent_floor_tile = (*adjacent_tiles->begin());
      } else {
        if (m_adjacent_floor_tile != 0) {
          if (m_avatar->GetTopLeftPostion().y < m_adjacent_floor_tile->GetTopLeftPostion().y) {
            m_adjacent_floor_tile = 0;
          }
        }
      }
      delete adjacent_tiles;

      D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
      D3DXVECTOR3 camera_position = m_camera->GetPosition();

      Avatar_Component::Avatar_Collision_Block avatar_collision_block = Avatar_Helper::GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks);


      if (current_state.state.compare("Looking") == 0) {
        if (last_state.state.compare("Looking") != 0) {
          camera_position.x = avatar_position.x;
          camera_position.y = m_adjacent_floor_tile->GetTopLeftPostion().y + 128 + 1;
        }
        if (m_input->GetCurrentKeyInput().IsRight) {
          camera_position.x = avatar_position.x + m_max_x_look_distance;
        } else if (m_input->GetCurrentKeyInput().IsLeft) {
          camera_position.x = avatar_position.x - m_max_x_look_distance;
        } else {
          camera_position.x = avatar_position.x;
        }

        if (m_input->GetCurrentKeyInput().IsDown) {
          camera_position.y = avatar_position.y - m_max_y_look_distance;
        } else if (m_input->GetCurrentKeyInput().IsUp) {
          camera_position.y = avatar_position.y + m_max_y_look_distance;
        } else {
          camera_position.y = m_adjacent_floor_tile->GetTopLeftPostion().y + 128 + 1;;
        }

        if (m_input->GetCurrentKeyInput().IsRight || m_input->GetCurrentKeyInput().IsLeft) {
          camera_position.x = m_camera->GetLastPosition().x + CalculateSmoothSnapXOffset(camera_position.x);
        }

        if (m_input->GetCurrentKeyInput().IsUp || m_input->GetCurrentKeyInput().IsDown) {
          camera_position.y = m_camera->GetLastPosition().y + CalculateSmoothSnapYOffset(camera_position.y);
        }
      } else {
        if (current_state.state.compare("Initial") == 0 || last_state.state.compare("Initial") == 0) {
          m_radius = 0;
          camera_position.x = avatar_position.x;
          // This plus 1 (+1) is to fix a bug where black bars sometimes appear on the top
          // or the bottom of the viewspace. I don't know why these bars appear and I don't
          // know why the + 1 fixes the problem. but.. OK
          camera_position.y = avatar_position.y - (avatar_collision_block.size.y / 2) + 128 + 1;
          m_stationary_avatar_position.x = m_avatar->GetPosition()->x;
          m_stationary_avatar_position.y = m_avatar->GetPosition()->y;
        } else {
          float distance = HowFarHasAvatarTravelled();
          if (current_state.parent_state.compare("Charlie_Standing") == 0 && last_state.parent_state.compare("Charlie_Standing") == 0 || current_state.direction.compare(last_state.direction) != 0) {
            if (distance > m_leash_length) {
              camera_position.x = avatar_position.x;
              m_stationary_avatar_position.x = m_avatar->GetPosition()->x;
              m_stationary_avatar_position.y = m_avatar->GetPosition()->y;
            } else {
              camera_position.x = m_stationary_avatar_position.x;
            }
          } else {
            if (distance > m_leash_length) {
              if (current_state.direction.compare("Right") == 0) {
                camera_position.x = avatar_position.x + m_leash_length;
              } else {
                camera_position.x = avatar_position.x - m_leash_length;
              }
            } else {
              camera_position.x = m_stationary_avatar_position.x;
            }
          }

          camera_position.x = m_camera->GetLastPosition().x + CalculateSmoothSnapXOffset(camera_position.x);

          // This plus 1 (+1) is to fix a bug where black bars sometimes appear on the top
          // or the bottom of the viewspace. I don't know why these bars appear and I don't
          // know why the + 1 fixes the problem. but.. OK
          if (m_adjacent_floor_tile != 0) {
            camera_position.y = m_adjacent_floor_tile->GetTopLeftPostion().y + 128 + 1;
          } else {
            camera_position.y = m_avatar->GetBottomRightPostion().y + 128 + 1;
          }

          if ((m_avatar->GetState().state.compare("Up_Facing_Falling_To_Death") == 0 && m_avatar->GetState().state_index == 0) ||
             (m_avatar->GetState().state.compare("Down_Facing_Falling_To_Death") == 0 && m_avatar->GetState().state_index == 0)) {
            m_radius = 30.0;
            m_randomAngle = static_cast<float>(rand()%360);
            m_is_shaking = true;
          }

          if (m_is_shaking) {
            m_radius *= 0.9f;  // Diminish m_radius each frame
            // This plus 1 (+1) is to fix a bug where black bars sometimes appear on the top
            // or the bottom of the viewspace. I don't know why these bars appear and I don't
            // know why the + 1 fixes the problem. but.. OK
            m_randomAngle += (150 + rand()%60);
            float offset = (sin(m_randomAngle) * m_radius , cos(m_randomAngle) * m_radius) + 128 + 1;
            offset = ceil(offset);
            camera_position.y -= static_cast<float>(offset);
          }

          if (m_radius == 0) {
            m_is_shaking = false;
          }
        }
      }
      m_camera->SetPosition(camera_position);
    }
    result = true;
  } else {
    result = false;
  }
  return result;
}

//------------------------------------------------------------------------------
void Camera_Controller::HandleEvent(Tunnelour::Component * const component) {
  if (component->GetType().compare("Avatar_Component") == 0) {
    Run();
  }
}

//------------------------------------------------------------------------------
void Camera_Controller::HandleEventAdd(Tunnelour::Component * const component) {
  Tile_Bitmap *tile = 0;
  tile = static_cast<Tile_Bitmap*>(component);
  if (tile->IsFloor()) {
    m_floor_tiles.push_back(tile);
  }
}

//------------------------------------------------------------------------------
void Camera_Controller::HandleEventRemove(Tunnelour::Component * const component) {
  Tunnelour::Tile_Bitmap *target_bitmap = 0;
  target_bitmap = static_cast<Tunnelour::Tile_Bitmap*>(component);
  std::vector<Tile_Bitmap*>::iterator found_bitmap;
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
float Camera_Controller::HowFarHasAvatarTravelled() {
  D3DXVECTOR2 point_1;
  point_1.x = m_avatar->GetPosition()->x;
  point_1.y = 0;
  D3DXVECTOR2 point_2;
  point_2.x = m_stationary_avatar_position.x;
  point_2.y = 0;

  float x = point_1.x - point_2.x;
  float y = point_1.y - point_2.y;
  float dist;

  dist = pow(x, 2) + pow(y, 2);  // Calculating distance by euclidean formula
  dist = sqrt(dist);

  return dist;
}

//------------------------------------------------------------------------------
float Camera_Controller::HowFarHasAvatarTravelledLastFrame() {
  D3DXVECTOR2 point_1;
  point_1.x = m_avatar->GetPosition()->x;
  point_1.y = 0;
  D3DXVECTOR2 point_2;
  point_2.x = m_avatar->GetLastRenderedPosition().x;
  point_2.y = 0;

  float x = point_1.x - point_2.x;
  float y = point_1.y - point_2.y;
  float dist;

  dist = pow(x, 2) + pow(y, 2);  // Calculating distance by euclidean formula
  dist = sqrt(dist);

  return dist;
}

//------------------------------------------------------------------------------
float Camera_Controller::CalculateSmoothSnapXOffset(float camera_position_x) {
  float offset = 0;

  D3DXVECTOR2 point_1;
  point_1.x = camera_position_x;
  point_1.y = 0;
  D3DXVECTOR2 point_2;
  point_2.x = m_camera->GetLastPosition().x;
  point_2.y = 0;

  float x = point_1.x - point_2.x;
  float y = point_1.y - point_2.y;
  float dist = 0;

  dist = pow(x, 2) + pow(y, 2);  // Calculating distance by euclidean formula
  dist = sqrt(dist);

  float multiplier = 1.0f;

  float distance_travelled = HowFarHasAvatarTravelledLastFrame();

  if (dist > 1024) {
    offset = 8.0f * multiplier;
  } else if (dist > 512) {
    offset = 6.0f * multiplier;
  } else if (dist > 256) {
    offset = 4.0f * multiplier;
  } else if (dist > 128) {
    offset = 2.0f * multiplier;
  } else if (dist > 64) {
    offset = 1.0f * multiplier;
  } else if (dist > 32) {
    offset = 1.0f * multiplier;
  } else if (dist > 1) {
    offset = 1.0f * multiplier;
  } else {
    offset = dist;
  }

  if (offset < distance_travelled) {
    offset += distance_travelled;
  }

  if (offset > dist) {
    offset = dist;
  }

  if (camera_position_x < m_camera->GetLastPosition().x) {
    offset = offset*-1;
  }

  return offset;
}

//------------------------------------------------------------------------------
float Camera_Controller::CalculateSmoothSnapYOffset(float camera_position_y) {
  float offset = 0;

  D3DXVECTOR2 point_1;
  point_1.y = camera_position_y;
  point_1.x = 0;
  D3DXVECTOR2 point_2;
  point_2.x = 0;
  point_2.y = m_camera->GetLastPosition().y;

  float x = point_1.x - point_2.x;
  float y = point_1.y - point_2.y;
  float dist = 0;

  dist = pow(x, 2) + pow(y, 2);  // Calculating distance by euclidean formula
  dist = sqrt(dist);

  float multiplier = 1.0f;

  float distance_travelled = HowFarHasAvatarTravelledLastFrame();

  if (dist > 1024) {
    offset = 8.0f * multiplier;
  } else if (dist > 512) {
    offset = 6.0f * multiplier;
  } else if (dist > 256) {
    offset = 4.0f * multiplier;
  } else if (dist > 128) {
    offset = 2.0f * multiplier;
  } else if (dist > 64) {
    offset = 1.0f * multiplier;
  } else if (dist > 32) {
    offset = 1.0f * multiplier;
  } else if (dist > 1) {
    offset = 1.0f * multiplier;
  } else {
    offset = dist;
  }

  if (offset < distance_travelled) {
    offset += distance_travelled;
  }

  if (offset > dist) {
    offset = dist;
  }

  if (camera_position_y < m_camera->GetLastPosition().y) {
    offset = offset * -1;
  }

  return offset;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // namespace Tunnelour
