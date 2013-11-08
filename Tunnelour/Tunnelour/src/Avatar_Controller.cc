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
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_has_avatar_been_generated = false;
  m_animation_tick = false;
  m_world_settings = 0;
}

//------------------------------------------------------------------------------
Avatar_Controller::~Avatar_Controller() {
  m_model->IgnoreType(this, "Bitmap_Component");
  m_floor_tiles.clear();
  m_wall_tiles.clear();
  m_avatar = 0;
  m_game_settings = 0;
  m_level = 0;
  m_has_avatar_been_generated = false;
  m_animation_tick = false;
  m_world_settings = 0;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  InitTimer();
  Avatar_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_game_settings = mutator.GetGameSettings();
    m_world_settings = mutator.GetWorldSettings();
    m_level = mutator.GetLevel();
    LoadTilesets(m_game_settings->GetTilesetPath());
    GenerateAvatarTile();
    m_model->Add(m_avatar);
    m_floor_tiles.merge(mutator.GetFloorTiles());
    m_wall_tiles.merge(mutator.GetWallTiles());
    m_model->ObserveType(this, "Bitmap_Component");
  } else {
    return false;
  }
  m_has_been_initialised = true;
  return true;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Run() {
  if (!m_has_been_initialised) { return false; }

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
void Avatar_Controller::HandleEventRemove(Tunnelour::Component * const component){
  m_wall_tiles.remove(static_cast<Tile_Bitmap*>(component));
  m_wall_tiles.remove(static_cast<Tile_Bitmap*>(component));
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Avatar_Controller::GenerateAvatar() {
}

void Avatar_Controller::GenerateAvatarTile() {
  m_avatar = new Avatar_Component();
  m_avatar->GetTexture()->transparency = 1.0f;
  Avatar_Component::Avatar_State initial_state;
  initial_state.direction = "Right";
  m_avatar->SetState(initial_state);
  m_avatar->SetPosition(D3DXVECTOR3(m_level->GetCurrentLevel().avatar_top_left_x, 
                                    m_level->GetCurrentLevel().avatar_top_left_y,
                                    -1));  // Middleground Z Space is -1
  ChangeAvatarState("Standing", initial_state.direction);
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunAvatarState() {
  std::string current_state = m_avatar->GetState().parent_state;

  // Is the avatar currently in a standing state
  if (current_state.compare("Charlie_Standing") == 0) {
    RunStandingState();
  } else if (current_state.compare("Charlie_Walking") == 0) {
    RunWalkingState();
  } else if (current_state.compare("Charlie_Falling") == 0) {
    RunFallingState();
  }

  m_animation_tick = false;
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunStandingState() {
  m_avatar->SetAngle(static_cast<float>(0.0));  // in radians
  m_avatar->SetVelocity(D3DXVECTOR3(0, 0, 0));

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

  if (current_command.state.compare("") != 0) {
    ChangeAvatarState(current_command.state, current_command.direction);
    if (current_command.state.compare("Walking") == 0) {
      RunWalkingState();
    }
  } else {
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

    UpdateAvatarState(state_index);
  }
  
  if (!IsAvatarFloorAdjacent()) {
    ChangeAvatarState("Falling_To_Standing", m_avatar->GetState().direction);
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunWalkingState() {
  m_avatar->SetAngle(static_cast<float>(0.0));  // in radians
  m_avatar->SetVelocity(D3DXVECTOR3(8, 0 , 0));

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();
  if (current_command.state.compare("") != 0) {
    // There is a command
    // Is the command different from the current state?
    if (current_state.state.compare(current_command.state) != 0 ||
        current_state.direction != current_command.direction) {
      // New State
      ChangeAvatarState(current_command.state, current_command.direction);
      AlignAvatarOnAvatarCollisionBlock();
    } else {
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

      UpdateAvatarState(state_index);
      AlignAvatarOnAvatarCollisionBlock();

      D3DXVECTOR3 position = *m_avatar->GetPosition();
      if (current_state.direction.compare("Right") == 0) {
        position += m_avatar->GetVelocity();
      } else {  // Left
        position -= m_avatar->GetVelocity();
      }
      m_avatar->SetPosition(position);
    }

    Avatar_Component::Avatar_State cleared_state;
    cleared_state.state = "";
    cleared_state.direction = "";
    cleared_state.state_index = 0;
    m_avatar->SetCommand(cleared_state);
  } else {
    // No Command, Change to standing
    ChangeAvatarState("Standing", m_avatar->GetState().direction);
    AlignAvatarOnAvatarCollisionBlock();
  }
  
  // Detect if the avatar is intersecting with a wall
  std::list<Bitmap_Component*> *out_colliding_floor_tiles = new std::list<Bitmap_Component*>();
  if (IsAvatarWallColliding(out_colliding_floor_tiles, &CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks)))) {
    ChangeAvatarState("Standing", m_avatar->GetState().direction);
    AlignAvatarOnAvatarCollisionBlock();
    // Move back avatar
    if (m_avatar->GetState().direction.compare("Left") == 0) {
      float tile_right_position = (*out_colliding_floor_tiles->begin())->GetBottomRightPostion().x;
      float avatar_collision_block_left_position = (&CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks)))->GetTopLeftPostion().x;
      D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
      float x_offset = tile_right_position + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks).size.x / 2);
      avatar_position.x = x_offset;
      m_avatar->SetPosition(avatar_position);
    }
    if (m_avatar->GetState().direction.compare("Right") == 0) {
      D3DXVECTOR3 tile_position = (*out_colliding_floor_tiles->begin())->GetTopLeftPostion();
      D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
      float foot_x_offset = GetLowestCollisionBlock().offset_from_avatar_centre.x + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks).size.x / 2);
      avatar_position.x = tile_position.x + (foot_x_offset * -1);
      m_avatar->SetPosition(avatar_position);
    }
  }
  
  // Detect if the avatar is overbalancing from walking
  if (!IsAvatarFloorAdjacent()) {
    ChangeAvatarState("Walking_To_Falling", m_avatar->GetState().direction);
    AlignAvatarOnAvatarCollisionBlock();
  }
  
}

//------------------------------------------------------------------------------
void Avatar_Controller::RunFallingState() {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  std::list<Bitmap_Component*> out_colliding_floor_tiles;
  Bitmap_Component* out_collision_block = new Bitmap_Component();
  if (current_state.state.compare("Falling_To_Standing") == 0) {
    bool is_colliding = IsAvatarFloorColliding(&out_colliding_floor_tiles,
                                               out_collision_block);
    if (is_colliding) {
      // Avatar has landed, move the avatar to the top of the tile
      bool is_tangent = false;
      float tile_top = 0;

      std::list<Bitmap_Component*>::iterator it;
      it = out_colliding_floor_tiles.begin();
      for (; it != out_colliding_floor_tiles.end(); it++) {
        tile_top = (*it)->GetTopLeftPostion().y;
        float block_bottom = out_collision_block->GetBottomRightPostion().y;
        if (tile_top == block_bottom) {
          is_tangent = true;
        }
      }

      D3DXVECTOR3 avatar_position = *m_avatar->GetPosition();
      float foot_bottom = out_collision_block->GetBottomRightPostion().y;
      float foot_bottom_centre_offset = foot_bottom - avatar_position.y;

      avatar_position.y = tile_top - foot_bottom_centre_offset;
      m_avatar->SetPosition(avatar_position);

      ChangeAvatarState("Standing", m_avatar->GetState().direction);
    } else {
      D3DXVECTOR3 position = *m_avatar->GetPosition();
      position.y -= 32;
      m_avatar->SetPosition(position);
    }
  } else {
    bool is_colliding = IsAvatarFloorColliding(&out_colliding_floor_tiles,
                                               out_collision_block);
    std::list<Bitmap_Component*> out_colliding_wall_tiles;
    bool is_wall_colliding = IsAvatarWallColliding(&out_colliding_wall_tiles, &CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks)));
    if (is_wall_colliding && (current_state.state.compare("Walking_To_Falling") != 0)) {
      if (m_avatar->GetState().direction.compare("Right") == 0) {
        ChangeAvatarState("Falling_Flip", "Left");
        MoveAvatarWallAdjacent("Right");
        D3DXVECTOR3 new_velocity = D3DXVECTOR3(2, 0 , 0);
        m_avatar->SetVelocity(new_velocity);
      } else {
        ChangeAvatarState("Falling_Flip", "Right");
        MoveAvatarWallAdjacent("Left");
        D3DXVECTOR3 new_velocity = D3DXVECTOR3(2, 0 , 0);
        m_avatar->SetVelocity(new_velocity);
      }
    } else {
      D3DXVECTOR3 position = *m_avatar->GetPosition();

      unsigned int state_index = current_state.state_index;
      state_index++;
      if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
        if (m_current_animation_subset.is_repeatable) {
          state_index = 0;
        } else {
          if (current_state.state.compare("Walking_To_Falling") == 0) {
            ChangeAvatarState("Falling_Flip", m_avatar->GetState().direction);
            state_index = 0;
            current_state = m_avatar->GetState();
            if (m_avatar->GetState().direction.compare("Right") == 0) {
              MoveAvatarWallAdjacent("Left");
            } else if (m_avatar->GetState().direction.compare("Left") == 0) {
              MoveAvatarWallAdjacent("Right");
            }
          } else if (current_state.state.compare("Falling_Flip") == 0) {
            ChangeAvatarState("Falling", m_avatar->GetState().direction);
            state_index = 0;
            current_state = m_avatar->GetState();
          } else if (current_state.state.compare("Falling_To_Death") == 0) {
            ChangeAvatarState("Death", m_avatar->GetState().direction);
            state_index = 0;
            current_state = m_avatar->GetState();
          } else {
            std::string error;
            error = "No handling for the non-repeating animation: " + current_state.state;
            throw Exceptions::init_error(error);
          }
        }
      } else {
        UpdateAvatarState(state_index);
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
        AlignAvatarOnAvatarCollisionBlock();
      } else {
        position = *m_avatar->GetPosition();
        D3DXVECTOR3 new_velocity = D3DXVECTOR3(0, 0 , 0);
        new_velocity.y = m_avatar->GetVelocity().y + m_world_settings->GetGravity();
        if (new_velocity.y > 128) { new_velocity.y = 128; }
        new_velocity.x = m_avatar->GetVelocity().x;

        m_avatar->SetVelocity(new_velocity);
        if (m_avatar->GetState().direction.compare("Right") == 0) {
          position.x = position.x + m_avatar->GetVelocity().x;
        } else if  (m_avatar->GetState().direction.compare("Left") == 0) {
          position.x = position.x - m_avatar->GetVelocity().x;
        }
        position.y = position.y - m_avatar->GetVelocity().y;
        m_avatar->SetPosition(position);
      }
      out_colliding_floor_tiles.clear();
      is_colliding = IsAvatarFloorColliding(&out_colliding_floor_tiles, out_collision_block);
      if (is_colliding && (m_avatar->GetState().state.compare("Walking_To_Falling") != 0)) {
        ChangeAvatarState("Falling_To_Death", m_avatar->GetState().direction);
        MoveAvatarFloorAdjacent();
      } 
    }
  }
}



//------------------------------------------------------------------------------
bool Avatar_Controller::IsAvatarFloorAdjacent() {
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot (Lowest collision Block).
  if (!m_floor_tiles.empty()) {
    // Find the lowest contact block
    // Get the lowest block most right/left block
    Avatar_Component::Collision_Block avatar_collision_block;
    avatar_collision_block = GetLowestMostForwardFootCollisionBlock(m_avatar->GetState().direction);

    // Make a bitmap for the lowest avatar collision block
    Bitmap_Component avatar_collision_block_bitmap;
    avatar_collision_block_bitmap = CollisionBlockToBitmapComponent(avatar_collision_block);

    // Create a list of floor tiles which are adjacent with the collision block
    std::list<Tile_Bitmap*> adjacent_tiles;
    std::list<Tile_Bitmap*>::iterator floor_tile;
    for (floor_tile = m_floor_tiles.begin(); floor_tile != m_floor_tiles.end(); floor_tile++) {
      if (Bitmap_Helper::DoTheseTilesXCollide(*floor_tile, &avatar_collision_block_bitmap)) {
        if (Bitmap_Helper::AreTheseTilesYAdjacent(*floor_tile, &avatar_collision_block_bitmap)) {
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
bool Avatar_Controller::IsAvatarFloorColliding(std::list<Bitmap_Component*> *out_colliding_floor_tiles, Bitmap_Component *out_collision_block) {
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot
  if (!m_floor_tiles.empty()) {
    // Get the lowest block
    Avatar_Component::Collision_Block avatar_collision_block;

    avatar_collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks);

    // Make a bitmap for the lowest avatar collision block
    (*out_collision_block) = CollisionBlockToBitmapComponent(avatar_collision_block);

    // Create a list of floor tiles which are colliding with the collision block
    std::list<Tile_Bitmap*>::iterator border_tile;
    for (border_tile = m_floor_tiles.begin(); border_tile != m_floor_tiles.end(); border_tile++) {
      if (Bitmap_Helper::DoTheseTilesYCollide(*border_tile, out_collision_block)) {
        if (Bitmap_Helper::DoTheseTilesXCollide(*border_tile, out_collision_block)) {
          out_colliding_floor_tiles->push_back(*border_tile);
        }
      }
    }

    // If colliding tiles is not empty
    // This means the avatar is colliding with a tile.
    if (!out_colliding_floor_tiles->empty()) {
      return true;
    }
  }

  // No colliding tiles
  return false;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::IsAvatarWallColliding(std::list<Bitmap_Component*> *out_colliding_floor_tiles, Bitmap_Component *out_collision_block) {
  // Going to deal only with gravity only
  // Also only dealing with the lowest foot
  if (!m_wall_tiles.empty()) {
    // Get the lowest block
    Avatar_Component::Collision_Block lowest_collision_block;
    lowest_collision_block = GetLowestCollisionBlock();

    // Make a bitmap for the lowest avatar collision block
    (*out_collision_block) = CollisionBlockToBitmapComponent(lowest_collision_block);

    // Create a list of floor tiles which are colliding with the collision block
    std::list<Tile_Bitmap*>::iterator border_tile;
    for (border_tile = m_wall_tiles.begin(); border_tile != m_wall_tiles.end(); border_tile++) {
      if (Bitmap_Helper::DoTheseTilesIntersect(*border_tile, out_collision_block)) {
        out_colliding_floor_tiles->push_back(*border_tile);
      }
    }

    // If colliding tiles is not empty
    // This means the avatar is colliding with a tile.
    if (!out_colliding_floor_tiles->empty()) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
void Avatar_Controller::LoadTilesets(std::wstring wtileset_path) {
  // Running
  m_running_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Running_Animation_Tileset_1_1.txt");
  Tileset_Helper::LoadAnimationTilesetMetadataIntoStruct(m_running_metadata_file_path, &m_running_metadata);
  m_animation_metadata.push_back(m_running_metadata);

  // Walking
  m_walking_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Walking_Animation_Tileset_1_6.txt");
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
}

//------------------------------------------------------------------------------
void Avatar_Controller::AlignAvatarOnRightFoot() {
  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Collision_Block current_collision_block;
  current_collision_block = GetNamedCollisionBlock("Right_Foot",
                                                   m_avatar->GetState().collision_blocks);

  Avatar_Component::Collision_Block last_collision_block;
  last_collision_block = GetNamedCollisionBlock("Right_Foot",
                                                m_avatar->GetLastState().collision_blocks);

  D3DXVECTOR3 right_foot_offset;
  right_foot_offset.x = static_cast<float>(last_collision_block.offset_from_avatar_centre.x -
                                           current_collision_block.offset_from_avatar_centre.x);
  right_foot_offset.y = static_cast<float>(last_collision_block.offset_from_avatar_centre.y -
                                           current_collision_block.offset_from_avatar_centre.y);
  right_foot_offset.z = 0;

  D3DXVECTOR3 right_foot_size_offset = D3DXVECTOR3(0, 0, 0);
  if (last_collision_block.size != current_collision_block.size) {
    right_foot_size_offset.x  = (last_collision_block.size.x /2) - (current_collision_block.size.x /2);
    right_foot_size_offset.y  = (last_collision_block.size.y /2) - (current_collision_block.size.y /2);
    right_foot_size_offset.z  = 0;
  }

  D3DXVECTOR3 current_avatar_position = *m_avatar->GetPosition();
  new_avatar_position = current_avatar_position + (right_foot_offset - right_foot_size_offset);
  m_avatar->SetPosition(new_avatar_position);
}

//------------------------------------------------------------------------------
void Avatar_Controller::AlignAvatarOnLeftFoot() {
  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Collision_Block current_collision_block;
  current_collision_block = GetNamedCollisionBlock("Left_Foot",
                                                   m_avatar->GetState().collision_blocks);

  Avatar_Component::Collision_Block last_collision_block;
  last_collision_block = GetNamedCollisionBlock("Left_Foot",
                                                m_avatar->GetLastState().collision_blocks);

  D3DXVECTOR3 right_foot_offset;
  right_foot_offset.x = static_cast<float>(last_collision_block.offset_from_avatar_centre.x -
                                           current_collision_block.offset_from_avatar_centre.x);
  right_foot_offset.y = static_cast<float>(last_collision_block.offset_from_avatar_centre.y -
                                           current_collision_block.offset_from_avatar_centre.y);
  right_foot_offset.z = 0;

  D3DXVECTOR3 right_foot_size_offset = D3DXVECTOR3(0, 0, 0);
  if (last_collision_block.size != current_collision_block.size) {
    right_foot_size_offset.x  = (last_collision_block.size.x /2) - (current_collision_block.size.x /2);
    right_foot_size_offset.y  = (last_collision_block.size.y /2) - (current_collision_block.size.y /2);
    right_foot_size_offset.z  = 0;
  }

  D3DXVECTOR3 current_avatar_position = *m_avatar->GetPosition();
  new_avatar_position = current_avatar_position + (right_foot_offset + right_foot_size_offset);
  m_avatar->SetPosition(new_avatar_position);
}

//------------------------------------------------------------------------------
void Avatar_Controller::AlignAvatarOnAvatarCollisionBlock() {
  if (m_avatar->GetLastState().state.compare("") == 0) { return; }

  D3DXVECTOR3 new_avatar_position;

  Avatar_Component::Collision_Block current_collision_block;
  current_collision_block = GetNamedCollisionBlock("Avatar",
                                                   m_avatar->GetState().collision_blocks);
  Bitmap_Component current_collision_bitmap = CollisionBlockToBitmapComponent(current_collision_block);

  Avatar_Component::Collision_Block last_collision_block;
  last_collision_block = GetNamedCollisionBlock("Avatar",
                                                m_avatar->GetLastState().collision_blocks);
  Bitmap_Component last_collision_bitmap = CollisionBlockToBitmapComponent(last_collision_block);

  if (current_collision_bitmap.GetBottomRightPostion().y != last_collision_bitmap.GetBottomRightPostion().y) {
    D3DXVECTOR3 right_foot_offset;
    right_foot_offset.x = static_cast<float>(last_collision_block.size.x -
                                             current_collision_block.size.x);
    right_foot_offset.y = static_cast<float>(last_collision_block.size.y -
                                             current_collision_block.size.y);
    right_foot_offset.z = 0;

    D3DXVECTOR3 current_avatar_position = *m_avatar->GetPosition();
    new_avatar_position.x = current_avatar_position.x;
    new_avatar_position.y = current_avatar_position.y - (right_foot_offset.y / 2);
    new_avatar_position.z = current_avatar_position.z;

    m_avatar->SetPosition(new_avatar_position);
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::MoveAvatarWallAdjacent(std::string direction) {
  std::list<Bitmap_Component*> out_colliding_wall_tiles;
  bool is_wall_colliding = IsAvatarWallColliding(&out_colliding_wall_tiles, &CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks)));
  if (is_wall_colliding) {
    D3DXVECTOR3 new_avatar_position = *m_avatar->GetPosition();
    if (direction.compare("Left") == 0) {
      D3DXVECTOR3 tile_position = (*out_colliding_wall_tiles.begin())->GetBottomRightPostion();
      Avatar_Component::Collision_Block collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks);
      float foot_x_offset = collision_block.offset_from_avatar_centre.x + (collision_block.size.x / 2);
      new_avatar_position.x = tile_position.x + foot_x_offset;
    }
    if (direction.compare("Right") == 0) {
      D3DXVECTOR3 tile_position = (*out_colliding_wall_tiles.begin())->GetTopLeftPostion();
      float foot_x_offset = GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks).offset_from_avatar_centre.x + (GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks).size.x / 2);
      new_avatar_position.x = tile_position.x - foot_x_offset;
    }
    m_avatar->SetPosition(new_avatar_position);
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::MoveAvatarFloorAdjacent() {
  std::list<Bitmap_Component*> out_colliding_wall_tiles;
  bool is_floor_colliding = IsAvatarFloorColliding(&out_colliding_wall_tiles, &CollisionBlockToBitmapComponent(GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks)));
  if (is_floor_colliding) {
    D3DXVECTOR3 new_avatar_position = *m_avatar->GetPosition();
    D3DXVECTOR3 tile_position = (*out_colliding_wall_tiles.begin())->GetTopLeftPostion();
    Avatar_Component::Collision_Block collision_block = GetNamedCollisionBlock("Avatar", m_avatar->GetState().collision_blocks);
    float foot_x_offset = collision_block.offset_from_avatar_centre.y + (collision_block.size.y / 2);
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
  int frames_per_millisecond = static_cast<int>(1000/30);///m_current_animation_fps); //m_current_animation_fps);

  INT64 currentTime;
  float timeDifference;

  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

  timeDifference = static_cast<float>(currentTime - m_startTime);

  m_frameTime = timeDifference / m_ticksPerMs;

  if (m_frameTime >= frames_per_millisecond) {
    m_startTime = currentTime;
    m_frameTime = static_cast<float>(m_frameTime - frames_per_millisecond);
    m_animation_tick = true;
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::ChangeAvatarState(std::string new_state_name, std::string direction) {
  Avatar_Component::Avatar_State new_state;
  Tileset_Helper::Animation_Tileset_Metadata new_state_metadata;
  Tileset_Helper::Animation_Subset new_animation_subset;

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  std::list<Tileset_Helper::Animation_Tileset_Metadata>::iterator metadata;
  for (metadata = m_animation_metadata.begin(); metadata != m_animation_metadata.end(); metadata++) {
    std::list<Tileset_Helper::Animation_Subset>::iterator tileset;
    for (tileset = (*metadata).subsets.begin(); tileset != (*metadata).subsets.end(); tileset++) {
      if (tileset->name.compare(new_state_name) == 0) {
        new_animation_subset = *tileset;
        new_state_metadata = (*metadata);
        new_state.direction = direction;
        new_state.state = new_state_name;
        new_state.parent_state = metadata->name;
      }
    }
  }

  if (new_state_metadata.name.compare("") == 0) {
    std::string error;
    error = "Animation not found in Metadata list!" + new_state_name;
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
  std::list<Tileset_Helper::Frame_Metadata>::iterator frame;
  for (frame = new_animation_subset.frames.begin(); frame != new_animation_subset.frames.end(); frame++) {
    if (frame->id == 1) {
      initial_frame = (*frame);
    }
  }

    // Create new collision block from the initial frame collision block
  std::list<Tileset_Helper::Collision_Block>::iterator collision_block;
  for (collision_block = initial_frame.collision_blocks.begin(); collision_block != initial_frame.collision_blocks.end(); collision_block++) {
    Avatar_Component::Collision_Block new_collision_block;
    new_collision_block = TilesetCollisionBlockToAvatarCollisionBlock((*collision_block),
                                                                      new_animation_subset.top_left_y,
                                                                      new_state.state_index);
    new_state.collision_blocks.push_back(new_collision_block);
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
void Avatar_Controller::UpdateAvatarState(int new_state_index) {
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
  std::list<Tileset_Helper::Frame_Metadata>::iterator frame;
  for (frame = m_current_animation_subset.frames.begin(); frame != m_current_animation_subset.frames.end(); frame++) {
    if (frame->id == (new_state_index + 1)) {
      new_frame = (*frame);
    }
  }

  // Create new collision block from the initial frame collision block
  std::list<Tileset_Helper::Collision_Block>::iterator collision_block;
  for (collision_block = new_frame.collision_blocks.begin(); collision_block != new_frame.collision_blocks.end(); collision_block++) {
    Avatar_Component::Collision_Block new_collision_block;
    new_collision_block = TilesetCollisionBlockToAvatarCollisionBlock((*collision_block),
                                                                      m_current_animation_subset.top_left_y,
                                                                      new_state_index);
    incremented_state.collision_blocks.push_back(new_collision_block);
  }

  m_avatar->SetState(incremented_state);

  m_avatar->GetTexture()->texture = 0;
  m_avatar->GetFrame()->vertex_buffer = 0;
  m_avatar->GetFrame()->index_buffer = 0;
  m_avatar->Init();
}

//------------------------------------------------------------------------------
Avatar_Component::Collision_Block Avatar_Controller::GetLowestCollisionBlock() {
  Avatar_Component::Collision_Block lowest_collision_block;

  std::list<Avatar_Component::Collision_Block> collision_blocks;
  collision_blocks = m_avatar->GetState().collision_blocks;

  lowest_collision_block = (*collision_blocks.begin());

  std::list<Avatar_Component::Collision_Block>::iterator collision_block;
  for (collision_block = collision_blocks.begin(); collision_block != collision_blocks.end(); collision_block++) {
    float collision_block_bottom = collision_block->offset_from_avatar_centre.y - collision_block->size.y;
    float lowest_collision_block_bottom = lowest_collision_block.offset_from_avatar_centre.y - lowest_collision_block.size.y;
    if (collision_block_bottom < lowest_collision_block_bottom) {
      lowest_collision_block = (*collision_block);
    }
  }

  return lowest_collision_block;
}

//------------------------------------------------------------------------------
Avatar_Component::Collision_Block Avatar_Controller::GetLowestMostForwardFootCollisionBlock(std::string direction) {
  Avatar_Component::Collision_Block lowest_collision_block;

  std::list<Avatar_Component::Collision_Block> collision_blocks;
  collision_blocks = m_avatar->GetState().collision_blocks;

  std::list<Avatar_Component::Collision_Block>::iterator collision_block;
  for (collision_block = collision_blocks.begin(); collision_block != collision_blocks.end(); collision_block++) {
    if ((collision_block->id.compare("Right_Foot") == 0 || collision_block->id.compare("Left_Foot") == 0) && collision_block->is_contacting) {
      if (lowest_collision_block.id.compare("") == 0) { 
        lowest_collision_block = (*collision_block); 
      } else {
        float collision_block_bottom = collision_block->offset_from_avatar_centre.y - collision_block->size.y;
        float lowest_collision_block_bottom = lowest_collision_block.offset_from_avatar_centre.y - lowest_collision_block.size.y;
        if (collision_block_bottom < lowest_collision_block_bottom) {
          lowest_collision_block = (*collision_block);
        }
        if (collision_block_bottom == lowest_collision_block_bottom) {
          if (direction.compare("Right") == 0) {
            float collision_block_right = collision_block->offset_from_avatar_centre.x + collision_block->size.x;
            float lowest_collision_block_right = lowest_collision_block.offset_from_avatar_centre.x + lowest_collision_block.size.x;
            if (collision_block_right > lowest_collision_block_right) {
              lowest_collision_block = (*collision_block);
            }
          }
          if (direction.compare("Left") == 0) {
            float collision_block_right = collision_block->offset_from_avatar_centre.x - collision_block->size.x;
            float lowest_collision_block_right = lowest_collision_block.offset_from_avatar_centre.x - lowest_collision_block.size.x;
            if (collision_block_right < lowest_collision_block_right) {
              lowest_collision_block = (*collision_block);
            }
          }
        }
      }
    }
  }

  return lowest_collision_block;
}

//------------------------------------------------------------------------------
Bitmap_Component Avatar_Controller::CollisionBlockToBitmapComponent(Avatar_Component::Collision_Block collision_block) {
  Bitmap_Component collision_bitmap;

  D3DXVECTOR3 collision_bitmap_position;
  collision_bitmap_position.x = m_avatar->GetPosition()->x + collision_block.offset_from_avatar_centre.x;
  collision_bitmap_position.y = m_avatar->GetPosition()->y + collision_block.offset_from_avatar_centre.y;
  collision_bitmap_position.z = m_avatar->GetPosition()->z;

  collision_bitmap.SetPosition(collision_bitmap_position);

  D3DXVECTOR2 size = collision_block.size;
  collision_bitmap.SetSize(size);

  return collision_bitmap;
}

//---------------------------------------------------------------------------
Avatar_Component::Collision_Block Avatar_Controller::GetNamedCollisionBlock(std::string id, std::list<Avatar_Component::Collision_Block> collision_blocks) {
  Avatar_Component::Collision_Block found_collision_block;

  Avatar_Component::Collision_Block* current_right_foot_collision_block = 0;

  std::list<Avatar_Component::Collision_Block>::iterator collision_block;
  for (collision_block = collision_blocks.begin(); collision_block != collision_blocks.end(); collision_block++) {
    if (collision_block->id.compare(id) == 0) {
      found_collision_block = (*collision_block);
    }
  }

  return found_collision_block;
}

//---------------------------------------------------------------------------
Avatar_Component::Collision_Block Avatar_Controller::TilesetCollisionBlockToAvatarCollisionBlock(Tileset_Helper::Collision_Block tileset_collision_block, 
                                                                                                 float tileset_animation_top_left_y,
                                                                                                 int state_index) {
  Avatar_Component::Collision_Block new_collision_block;

  // Create new collision block from the initial frame collision block
  new_collision_block.id = tileset_collision_block.id;
  new_collision_block.is_contacting = tileset_collision_block.is_contacting;
  new_collision_block.size.x = static_cast<float>(tileset_collision_block.size_x);
  new_collision_block.size.y = static_cast<float>(tileset_collision_block.size_y);

  // Need to change the coordinate information from the "animation sheet" to
  // the coordinate system in the game world.
  // Create a frame for the avatar foot
  Frame_Component avatar_contact_foot;
  D3DXVECTOR3 collision_block_tilesheet_centre;
  collision_block_tilesheet_centre.x = static_cast<float>(tileset_collision_block.top_left_x + (tileset_collision_block.size_x /2));
  collision_block_tilesheet_centre.y = static_cast<float>(tileset_collision_block.top_left_y - (tileset_collision_block.size_y /2));

  // Need to account for different positions in the frame and the avatar
  // Work out the center of the avatar frame (128x128 block) int the Tileset
  // frame is 128x128 so get the frame # and times it by 128/2 for y
  D3DXVECTOR3 animation_frame_centre;
  animation_frame_centre.x = static_cast<float>(((state_index + 1) * 128) - (128 / 2));
  // and 128/2 for x
  animation_frame_centre.y = static_cast<float>(((tileset_animation_top_left_y) - (128 / 2)));
  animation_frame_centre.z = static_cast<float>(-2);

  // store the distance from x and y to the center of the animation frame
  new_collision_block.offset_from_avatar_centre.x = collision_block_tilesheet_centre.x - animation_frame_centre.x;
  new_collision_block.offset_from_avatar_centre.y = collision_block_tilesheet_centre.y - animation_frame_centre.y;

  if (m_avatar->GetState().direction.compare("Left") == 0) {
    // We need to reverse the x on the collision block.
    if (new_collision_block.id.compare("Left_Foot") == 0) {
      new_collision_block.id = "Right_Foot";
      new_collision_block.offset_from_avatar_centre.x = (new_collision_block.offset_from_avatar_centre.x * -1);
    } else if (new_collision_block.id.compare("Right_Foot") == 0) {
      new_collision_block.id = "Left_Foot";
      new_collision_block.offset_from_avatar_centre.x = (new_collision_block.offset_from_avatar_centre.x * -1);
    }
  }

  return new_collision_block;
}
}  // Tunnelour
