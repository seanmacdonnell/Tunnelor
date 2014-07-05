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

#include "Camera_Controller.h"
#include <math.h>       /* sin */
#include "Bitmap_Helper.h"
#include <limits>       // std::numeric_limits

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Camera_Controller::Camera_Controller() : Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_camera = 0;
  is_shaking = false;
  m_adjacent_floor_tile = 0;
  m_distance_travelled = 0;
  m_leash_length = 96;
  m_camera_stationary = false;
  m_stationary_avatar_position.x = INT_MAX;
  m_stationary_avatar_position.y = INT_MAX;
  m_offset_x_divisor = 16;
  m_offset_y_divisor = 32;
}

//------------------------------------------------------------------------------
Camera_Controller::~Camera_Controller() {
  m_model->IgnoreType(this, "Bitmap_Component");
  m_avatar->Ignore(this);
  m_avatar = 0;
  m_game_settings = 0;
  m_camera = 0;
  m_adjacent_floor_tile = 0;
}

//------------------------------------------------------------------------------
bool Camera_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  if (m_camera == 0) {
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
  } else {
    return false;
  } 
  return true;
}

//------------------------------------------------------------------------------
bool Camera_Controller::Run() {
  Camera_Controller_Mutator mutator;
  if (!m_has_been_initialised) { return false; }
  if (m_game_settings->IsCameraFollowing()) {
    std::vector<Bitmap_Component*> *adjacent_tiles = new std::vector<Bitmap_Component*>();
    if (IsAvatarFloorAdjacent(adjacent_tiles)) {
      m_adjacent_floor_tile = (*adjacent_tiles->begin());
    } else {
      if (m_adjacent_floor_tile != 0) {
        if (m_avatar->GetTopLeftPostion().y < m_adjacent_floor_tile->GetTopLeftPostion().y) {
          m_adjacent_floor_tile = 0;
        }
      }
    }
    delete adjacent_tiles;

    // Currently the camera is locked to the avatar.
    D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
    D3DXVECTOR3 camera_position = m_camera->GetPosition();

    // Get the avatar collision block
    Avatar_Component::Avatar_Collision_Block avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks);
    
    Avatar_Component::Avatar_State current_state = m_avatar->GetState();
    Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();
    Avatar_Component::Avatar_State last_state = m_avatar->GetLastRenderedState();


    if (current_state.state.compare("Initial") == 0 || last_state.state.compare("Initial") == 0) {
      radius = 0;
      camera_position.x = avatar_position.x;
      camera_position.y = avatar_position.y - (avatar_collision_block.size.y / 2) + 176 + 1;
      m_stationary_avatar_position.x = m_avatar->GetPosition()->x;
      m_stationary_avatar_position.y = m_avatar->GetPosition()->y;
    } else {
      int distance = HowFarHasAvatarTravelled();
      if (current_state.parent_state.compare("Charlie_Standing") == 0 && last_state.parent_state.compare("Charlie_Standing") == 0 ||
          current_state.direction.compare(last_state.direction) != 0) {
        if (distance > m_leash_length) {
          camera_position.x = avatar_position.x;
          camera_position.x = m_camera->GetLastPosition().x + CalculateSmoothSnapXOffset(camera_position.x);
          m_stationary_avatar_position.x = m_avatar->GetPosition()->x;
          m_stationary_avatar_position.y = m_avatar->GetPosition()->y;
        } else {
          camera_position.x = m_stationary_avatar_position.x;
          camera_position.x = m_camera->GetLastPosition().x + CalculateSmoothSnapXOffset(camera_position.x);
        }
      } else {
        if (distance > m_leash_length || m_avatar->GetVelocity().x == 32 || m_avatar->GetVelocity().x == -32) {
          if (avatar_position.x > m_camera->GetLastPosition().x) {
             camera_position.x = avatar_position.x - m_leash_length;
          } else {
             camera_position.x = avatar_position.x + m_leash_length;
          }
        } else {
          camera_position.x = m_stationary_avatar_position.x;
          camera_position.x = m_camera->GetLastPosition().x + CalculateSmoothSnapXOffset(camera_position.x);
        }
      }
      //camera_position.x += CalculateSmoothSnapXOffset(camera_position.x);
      //camera_position.x = m_camera->GetLastPosition().x ;

      //camera_position.x = avatar_position.x + CalculateSmoothSnapXOffset();
      // This plus 1 (+1) is to fix a bug where black bars sometimes appear on the top
      // or the bottom of the viewspace. I don't know why these bars appear and I don't
      // know why the + 1 fixes the problem. but.. OK
      if (m_adjacent_floor_tile != 0) {
        camera_position.y = m_adjacent_floor_tile->GetTopLeftPostion().y + 128 + 1;
      } else {
        camera_position.y = m_avatar->GetBottomRightPostion().y + 128 + 1;// - (avatar_collision_block.size.y / 2) + 176 + 1;
      }
      //camera_position.y += CalculateSmoothSnapYOffset(camera_position.y);
      //camera_position.y = m_camera->GetLastPosition().y + CalculateSmoothSnapYOffset(camera_position.y);
      /*
      if (m_avatar->GetState().state.compare("Up_Facing_Falling_To_Death") == 0 && m_avatar->GetState().state_index == 0) {
        radius = 30.0;
        randomAngle = rand()%360;
        is_shaking = true;
      }
   
      if (is_shaking) {
        radius *=0.9; //diminish radius each frame
        randomAngle += (150 + rand()%60);
        float offset = (sin(randomAngle) * radius , cos(randomAngle) * radius) + 128; //create offset 2d vector
        offset = ceil(offset);
        //camera_position.x += offset; //set centre of viewport
        camera_position.y -= offset; //set centre of viewport
      }

      if (radius == 0) {
        is_shaking = false;
      }
      */
    }
    m_camera->SetPosition(camera_position);
  }

  if (last_state !=  m_avatar->GetLastRenderedState()) {
    last_state = m_avatar->GetLastRenderedState();
  }

  return true;
}

//---------------------------------------------------------------------------
Avatar_Component::Avatar_Collision_Block Camera_Controller::GetNamedCollisionBlock(std::string id, std::vector<Avatar_Component::Avatar_Collision_Block> avatar_collision_blocks) {
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
bool Camera_Controller::IsAvatarFloorAdjacent(std::vector<Bitmap_Component*> *adjacent_tiles) {
  adjacent_tiles->clear();
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot (Lowest collision Block).
  if (!m_floor_tiles.empty()) {
    // Find the lowest contact block
    // Get the lowest block most right/left block
    Avatar_Component::Avatar_Collision_Block avatar_avatar_collision_block;
    if (m_avatar->GetState().state.compare("Stopping") == 0) {
      avatar_avatar_collision_block = GetNamedCollisionBlock("Right_Foot", m_avatar->GetState().avatar_collision_blocks);
    } else {
      avatar_avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().avatar_collision_blocks);
    }
    Bitmap_Component *avatar_avatar_collision_block_bitmap;
    avatar_avatar_collision_block_bitmap = CollisionBlockToBitmapComponent(avatar_avatar_collision_block, (*m_avatar->GetPosition()));

    // Create a list of floor tiles which are adjacent with the collision block
    std::vector<Tile_Bitmap*>::iterator floor_tile;
    for (floor_tile = m_floor_tiles.begin(); floor_tile != m_floor_tiles.end(); floor_tile++) {
      if (m_game_settings->IsDebugMode()) {
        (*floor_tile)->GetTexture()->transparency = 1.0f;
      }
      if (Bitmap_Helper::DoTheseTilesXCollide(*floor_tile, avatar_avatar_collision_block_bitmap)) {
        float floor_top = (*floor_tile)->GetTopLeftPostion().y;
        float avatar_bottom = avatar_avatar_collision_block_bitmap->GetBottomRightPostion().y;
        if (floor_top == avatar_bottom) {
          adjacent_tiles->push_back(*floor_tile);
          if (m_game_settings->IsDebugMode()) {
            (*floor_tile)->GetTexture()->transparency = 0.5f;
          }
        }
      }
    }
    delete avatar_avatar_collision_block_bitmap;

    if (adjacent_tiles->empty()) {
      return false;
    }
  } else {
    return false;
  }

  // Adjacent tiles!
  return true;
}

//------------------------------------------------------------------------------
Bitmap_Component* Camera_Controller::CollisionBlockToBitmapComponent(Avatar_Component::Avatar_Collision_Block avatar_collision_block, D3DXVECTOR3 position) {
  Bitmap_Component* collision_bitmap = new Bitmap_Component();

  D3DXVECTOR3 collision_bitmap_position;
  collision_bitmap_position.x = position.x + avatar_collision_block.offset_from_avatar_centre.x;
  collision_bitmap_position.y = position.y + avatar_collision_block.offset_from_avatar_centre.y;
  collision_bitmap_position.z = position.z;

  collision_bitmap->SetPosition(collision_bitmap_position);

  collision_bitmap->SetSize(avatar_collision_block.size.x, avatar_collision_block.size.y);

  return collision_bitmap;
}

//------------------------------------------------------------------------------
int Camera_Controller::HowFarHasAvatarTravelled() {
  D3DXVECTOR2 point_1;
  point_1.x = m_avatar->GetPosition()->x;
  point_1.y = 0;
  D3DXVECTOR2 point_2;
  point_2.x = m_stationary_avatar_position.x;
  point_2.y = 0;

  double x = point_1.x - point_2.x;
  double y = point_1.y - point_2.y;
  double dist;

  dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
  dist = sqrt(dist);                  //sqrt is function in math.h

  return static_cast<int>(dist);
}

//------------------------------------------------------------------------------
int Camera_Controller::CalculateSmoothSnapXOffset(float camera_position_x) {
  int offset = 0;

  D3DXVECTOR2 point_1;
  point_1.x = camera_position_x;
  point_1.y = 0;
  D3DXVECTOR2 point_2;
  point_2.x = m_camera->GetLastPosition().x;
  point_2.y = 0;

  double x = point_1.x - point_2.x;
  double y = point_1.y - point_2.y;
  double dist = 0;

  dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
  dist = sqrt(dist);                  //sqrt is function in math.h

  int multiplier = 1;
  if (m_avatar->GetState().parent_state.compare("Charlie_Standing") != 0) {
    multiplier = 1;
  }

  if (dist > 512) {
    offset = 16 * multiplier;
  } else if (dist > 256) {
    offset = 8 * multiplier;
  } else if (dist > 128) {
    offset = 4 * multiplier;
  } else if (dist > 64) {
    offset = 2 * multiplier;
  } else if (dist > 32) {
    offset = 1 * multiplier;
  } else if (dist > 1) {
    offset = 1;
  } else {
    offset = dist;
  }

  if (camera_position_x < m_camera->GetLastPosition().x) {
    offset = offset*-1;
  }

  return offset;
}

//------------------------------------------------------------------------------
int Camera_Controller::CalculateSmoothSnapYOffset(float camera_position_y) {
  int offset = 0;

  D3DXVECTOR2 point_1;
  point_1.y = camera_position_y;
  point_1.x = 0;
  D3DXVECTOR2 point_2;
  point_2.x = 0;
  point_2.y = m_camera->GetLastPosition().y;

  double x = point_1.x - point_2.x;
  double y = point_1.y - point_2.y;
  double dist = 0;

  dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
  dist = sqrt(dist);                  //sqrt is function in math.h

  if (dist > 512) {
    offset = dist;
  } else if (dist > 256) {
    offset = 6;
  } else if (dist > 128) {
    offset = 4;
  } else if (dist > 64) {
    offset = 2;
  } else if (dist > 32) {
    offset = 1;
  } else {
    offset = dist;
  }

  if (camera_position_y < m_camera->GetLastPosition().y) {
    offset = offset * -1;
  }

  return offset;
}

/*
At the moment this code is making a camera effect like an earthquake, so I am keeping it for later.
//------------------------------------------------------------------------------
int Camera_Controller::CalculateSmoothSnapXOffset(float camera_position_x) {
  int offset = 0;

  D3DXVECTOR2 point_1;
  point_1.x = camera_position_x;
  point_1.y = 0;
  D3DXVECTOR2 point_2;
  point_2.x = m_camera->GetLastPosition().x;
  point_2.y = 0;

  double x = point_1.x - point_2.x;
  double y = point_1.y - point_2.y;
  double dist = 0;

  dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
  dist = sqrt(dist);                  //sqrt is function in math.h

  if (dist > 32) {
    offset = dist/m_offset_x_divisor;
    m_offset_x_divisor =- 16;
  } else {
    offset = dist;
    m_offset_x_divisor = 32;
  }

  if (m_offset_x_divisor == 1) {
    m_offset_x_divisor = 32;
  }

  if (camera_position_x > m_camera->GetLastPosition().x) {
    offset = offset*-1;
  }

  return offset;
}

//------------------------------------------------------------------------------
int Camera_Controller::CalculateSmoothSnapYOffset(float camera_position_y) {
  int offset = 0;

  D3DXVECTOR2 point_1;
  point_1.y = camera_position_y;
  point_1.x = 0;
  D3DXVECTOR2 point_2;
  point_2.x = 0;
  point_2.y = m_camera->GetLastPosition().y;

  double x = point_1.x - point_2.x;
  double y = point_1.y - point_2.y;
  double dist = 0;

  dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
  dist = sqrt(dist);                  //sqrt is function in math.h

  if (dist > 32) {
    offset = dist/m_offset_y_divisor;
    m_offset_y_divisor =- 16;
  } else {
    offset = dist;
    m_offset_y_divisor = 32;
  }

  if (m_offset_y_divisor == 1) {
    m_offset_y_divisor = 32;
  }

  if (camera_position_y > m_camera->GetLastPosition().y) {
    offset = offset * -1;
  }

  return offset;
}
*/
//------------------------------------------------------------------------------
int Camera_Controller::WhatsTheDistanceBetweenThesAvatarAndTheCamera() {
  D3DXVECTOR2 point_1;
  point_1.x = m_avatar->GetPosition()->x;
  point_1.y = m_avatar->GetPosition()->y;
  D3DXVECTOR2 point_2;
  point_2.x = m_camera->GetLastPosition().x;
  point_2.y = m_camera->GetLastPosition().y;

  double x = point_1.x - point_2.x;
  double y = point_1.y - point_2.y;
  double dist;

  dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
  dist = sqrt(dist);                  //sqrt is function in math.h

  return static_cast<int>(dist);
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
}  // Tunnelour
