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

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Avatar_Controller::Avatar_Controller() : Controller() {
  m_avatar = 0;
  m_game_settings = 0;
  m_has_avatar_been_generated = false;
  m_animation_tick = false;
}

//------------------------------------------------------------------------------
Avatar_Controller::~Avatar_Controller() {
}

//------------------------------------------------------------------------------
void Avatar_Controller::Init(Tunnelour::Component_Composite * const model) {
    Tunnelour::Controller::Init(model);
    Init_Timer();

}

//------------------------------------------------------------------------------
void Avatar_Controller::Run() {
  Avatar_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (!m_has_avatar_been_generated) {
    if (mutator.FoundGameSettings() && mutator.FoundBorderTiles())  {
      m_game_settings = mutator.GetGameSettings();
      Load_Tilesets(m_game_settings->GetTilesetPath());
      Generate_Avatar_Tile();
      m_model->Add(m_avatar);
      m_has_avatar_been_generated = true;   
      if (Is_Avatar_Falling(&mutator)) {
        ChangeAvatarState("Falling", m_avatar->GetState().direction);
      }
    }
  } else {
    // Is the avatar currenly not contacting the ground
    Update_Timer();
    if (m_animation_tick) {
      Avatar_Component::Avatar_State current_state = m_avatar->GetState();
      Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

      if (Is_Avatar_Falling(&mutator)) {
        // Is the avatar currently in a falling state
        if (current_state.state.compare("Falling") == 0) {
          // Contunue the falling animation
          unsigned int state_index = current_state.state_index;
          state_index++;
          if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
            if (m_current_animation_subset.is_repeatable) {
              state_index = 0; 
            } else {
              throw Tunnelour::Exceptions::init_error("No handling for non-repeating animtions yet");
            }
          }

          UpdateAvatarState(state_index);

          D3DXVECTOR3 position = m_avatar->GetPosition();
          position.y -= 4;

          m_avatar->SetPosition(position);
        } else {
          // start the falling animation
          ChangeAvatarState("Falling", m_avatar->GetState().direction);
        }
      } else if (current_command.state.compare("") != 0) { // There is a command
        //  Is the command different from the current state?
        if (current_state.state.compare(current_command.state) != 0 || current_state.direction != current_command.direction) {
          // New State
          ChangeAvatarState(current_command.state, current_command.direction);
        } else {
          // Continue State
          // Contunue the falling animation
          unsigned int state_index = current_state.state_index;
          state_index++;
          if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
            if (m_current_animation_subset.is_repeatable) {
              state_index = 0; 
            } else {
              throw Tunnelour::Exceptions::init_error("No handling for non-repeating animtions yet");
            }
          }

          UpdateAvatarState(state_index);

          D3DXVECTOR3 position = m_avatar->GetPosition();
          if (current_state.direction.compare("Right") == 0) {
            if (current_state.state.compare("Running") == 0) {
              position.x += 16;
            } else if (current_state.state.compare("Walking") == 0) {
              position.x += 8;
            }
          } else { // Left
            if (current_state.state.compare("Running") == 0) {
              position.x -= 16;
            } else if (current_state.state.compare("Walking") == 0) {
              position.x -= 8;
            }
          }

          m_avatar->SetPosition(position);
        }
      } else {
        // Is the avatar currently in a standing state
        if (current_state.state.compare("Standing") == 0) {
          // Contunue the falling animation
          unsigned int state_index = current_state.state_index;
          state_index++;
          if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
            if (m_current_animation_subset.is_repeatable) {
              state_index = 0; 
            } else {
              throw Tunnelour::Exceptions::init_error("No handling for non-repeating animtions yet");
            }
          }

          UpdateAvatarState(state_index);
        } else {
          // start the standing animation
          ChangeAvatarState("Standing", m_avatar->GetState().direction);
        }
      }
      m_animation_tick = false;
    }

    Avatar_Component::Avatar_State cleared_state;
    cleared_state.state = "";
    cleared_state.direction = "";
    cleared_state.state_index = 0;
    m_avatar->SetCommand(cleared_state);
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Avatar_Controller::Generate_Avatar_Tile() {
  m_avatar = new Tunnelour::Avatar_Component();
  m_avatar->GetTexture()->transparency = 1.0f;
  Avatar_Component::Avatar_State initial_state;
  initial_state.direction = "Right";
  m_avatar->SetState(initial_state);
  m_avatar->SetPosition(D3DXVECTOR3(0, 0, -2)); // Middleground Z Space is -1
  ChangeAvatarState("Standing", initial_state.direction);
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Is_Avatar_Falling(Avatar_Controller_Mutator *mutator) {
  // Going to deal only with gravity only right now
  // Also only dealing with the lowest foot, if they have the same Y I do not do any special calculations.
  // If there are no border tiles, the avatar is in freefall
  if (mutator->FoundBorderTiles()) {
    // Go through the contact blocks and find the lowest contact block
    Avatar_Component::Collision_Block* lowest_collision_block = 0;
    std::list<Avatar_Component::Collision_Block> collision_blocks = m_avatar->GetState().collision_blocks;
    for (std::list<Avatar_Component::Collision_Block>::iterator collision_block = collision_blocks.begin(); collision_block != collision_blocks.end(); collision_block++) {
      if (lowest_collision_block == 0) {
        lowest_collision_block = &(*(collision_block));
      }
      if ((collision_block->avatar_centre_offset_centre_y - collision_block->size_y) < (lowest_collision_block->avatar_centre_offset_centre_y - lowest_collision_block->size_y)) {
        lowest_collision_block = &(*(collision_block));
      }
    }

    // Make a frame for the lowest avatar collision block
    Frame_Component lowest_collision_block_frame;
    D3DXVECTOR3 lowest_collision_block_frame_centre_position;
    lowest_collision_block_frame_centre_position.x = m_avatar->GetPosition().x + lowest_collision_block->avatar_centre_offset_centre_x;
    lowest_collision_block_frame_centre_position.y = m_avatar->GetPosition().y + lowest_collision_block->avatar_centre_offset_centre_y;
    lowest_collision_block_frame_centre_position.z = -2;

    lowest_collision_block_frame.SetPosition(lowest_collision_block_frame_centre_position);
    lowest_collision_block_frame.SetSize(lowest_collision_block->size_x, lowest_collision_block->size_y);

    // Cull the border tiles which are not within the x range of the lowest avatar collision block
    std::list<Tunnelour::Bitmap_Component*> colliding_border_tiles;
    std::list<Tunnelour::Bitmap_Component*> border_tiles = mutator->GetBorderTiles();
    for (std::list<Tunnelour::Bitmap_Component*>::iterator it = border_tiles.begin(); it != border_tiles.end(); it++) {
      if (DoTheseTilesXCollide(*it, &lowest_collision_block_frame)) {
        if (DoTheseTilesYCollide(*it, &lowest_collision_block_frame)) {
          colliding_border_tiles.push_back(*it);
        }
      }
    }

    // If that block is not intersecting with the ground put the avatar into a falling state
    if (colliding_border_tiles.empty()) {
      // Avatar is falling
      return true;
    } else {
      // There is a Y collision, which means the avatar is either tangent on a tile, or is intersecting with one
      bool is_tangent = false;
      int tile_top;
      for (std::list<Tunnelour::Bitmap_Component*>::iterator it = colliding_border_tiles.begin(); it != colliding_border_tiles.end(); it++) {
         tile_top = static_cast<int>((*it)->GetPosition().y + (*it)->GetSize()->y / 2);
         int collision_block_bottom = static_cast<int>(lowest_collision_block_frame_centre_position.y - (lowest_collision_block->size_y /2));

         if (tile_top == collision_block_bottom) {
           is_tangent = true;
         }
      }

      if (!is_tangent) {
        // Avatar is falling
        return true;
      } else {
        // Move the avatar to the top of the tile
        D3DXVECTOR3 avatar_position = m_avatar->GetPosition();
        avatar_position.y = tile_top - (lowest_collision_block->avatar_centre_offset_centre_y - (lowest_collision_block->size_y / 2));
        m_avatar->SetPosition(avatar_position);
        return false;
      }
    }
  } else {
    // Avatar is falling
    return true;
  }

  return false;
}

void Avatar_Controller::Load_Tilesets(std::wstring wtileset_path) {
  // Running
  m_running_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Running_Animation_Tileset_1_1.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_running_metadata_file_path, m_running_metadata);

  // Walking
  m_walking_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Walking_Animation_Tileset_1_5.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_walking_metadata_file_path, m_walking_metadata);

  // Standing
  m_standing_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Standing_Animation_Tileset_1_1.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_standing_metadata_file_path, m_standing_metadata);
 
  // Falling
  m_falling_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Falling_Animation_Tileset_0_1.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_falling_metadata_file_path, m_falling_metadata);
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Init_Timer() {
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if(m_frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

//------------------------------------------------------------------------------
void Avatar_Controller::Update_Timer() {
 //avatar is 16 fps one frame every 62.5 milliseond
 // Avatar 24 fps is 1000/24 = 41.66666666666667
  int frames_per_millisecond = static_cast<int>(1000/m_current_animation_fps);

	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

 if (m_frameTime >= frames_per_millisecond) {
   m_startTime = currentTime;
   m_frameTime = static_cast<float>(m_frameTime - frames_per_millisecond);
   m_animation_tick = true;
 }
}

//------------------------------------------------------------------------------
bool Avatar_Controller::DoTheseTilesXCollide(Tunnelour::Frame_Component* TileA, Tunnelour::Frame_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_left, a_tile_right;
  a_tile_left = TileA->GetPosition().x - static_cast<float>(TileA->GetSize()->x / 2);
  a_tile_right = TileA->GetPosition().x + static_cast<float>(TileA->GetSize()->x / 2);

  float b_tile_left, b_tile_right;
  b_tile_left = TileB->GetPosition().x - static_cast<float>(TileB->GetSize()->x / 2);
  b_tile_right = TileB->GetPosition().x + static_cast<float>(TileB->GetSize()->x / 2);

  if (b_tile_left == a_tile_left  || b_tile_left == a_tile_right) {
    return true;
  }
    
  if (b_tile_right == a_tile_left  || b_tile_right == a_tile_right) {
    return true;
  }

  if (b_tile_left > a_tile_left && b_tile_left < a_tile_right) {
    return true;
  }

  if (b_tile_right > a_tile_left && b_tile_right < a_tile_right) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::DoTheseTilesYCollide(Tunnelour::Frame_Component* TileA, Tunnelour::Frame_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_top, a_tile_bottom;
  a_tile_top = TileA->GetPosition().y + static_cast<float>(TileA->GetSize()->y / 2);
  a_tile_bottom = TileA->GetPosition().y - static_cast<float>(TileA->GetSize()->y / 2);

  float b_tile_top, b_tile_bottom;
  b_tile_top = TileB->GetPosition().y + static_cast<float>(TileB->GetSize()->y / 2);
  b_tile_bottom = TileB->GetPosition().y - static_cast<float>(TileB->GetSize()->y / 2);

  bool stop = false;
  if (a_tile_bottom > b_tile_top) {
    stop = true;
  }

  if (b_tile_top == a_tile_top  || b_tile_top == a_tile_bottom) {
    return true;
  }
    
  if (b_tile_bottom == a_tile_top  || b_tile_bottom == a_tile_bottom) {
    return true;
  }

  if (b_tile_top < a_tile_top && b_tile_top > a_tile_bottom) {
    return true;
  }

  if (b_tile_bottom < a_tile_top && b_tile_bottom > a_tile_bottom) {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
void Avatar_Controller::ChangeAvatarState(std::string new_state_name, std::string direction) {
  Tileset_Helper::Animation_Tileset_Metadata *new_state_metadata = 0;
  Avatar_Component::Avatar_State new_state;

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  // Change State
  if (new_state_name.compare("Walking") == 0) {
    new_state_metadata = &m_walking_metadata;
    new_state.direction = direction;
    new_state.state = new_state_name;
  } else if (new_state_name.compare("Running") == 0) {
    new_state_metadata = &m_running_metadata;
    new_state.direction = direction;
    new_state.state = new_state_name;
  } else if (new_state_name.compare("Falling") == 0) {
    new_state_metadata = &m_falling_metadata;
    new_state.direction = direction;
    new_state.state = new_state_name;
  } else {
    // Standing;
    new_state_metadata = &m_standing_metadata;
    new_state.direction = direction;
    new_state.state = new_state_name;
  }

  Tileset_Helper::Animation_Subset new_animation_subset;
  for (std::list<Tileset_Helper::Animation_Subset>::iterator tileset = new_state_metadata->subsets.begin(); tileset != new_state_metadata->subsets.end(); tileset++) {
    if (tileset->name.compare(new_state.state) == 0) {
      new_animation_subset = *tileset;
    }
  }

  new_state.state_index = 0;
  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(new_state_metadata->filename);

  m_avatar->GetTexture()->texture_path = texture_path;
  m_avatar->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(new_state_metadata->size_x),
                                                      static_cast<float>(new_state_metadata->size_y));
  m_avatar->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(new_animation_subset.tile_size_x),
                                                  static_cast<float>(new_animation_subset.tile_size_y));
  m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(new_animation_subset.top_left_x),
                                                          static_cast<float>(new_animation_subset.top_left_y));
  m_avatar->SetSize(new D3DXVECTOR2(static_cast<float>(new_animation_subset.tile_size_x), static_cast<float>(new_animation_subset.tile_size_y)));

  for (std::list<Tileset_Helper::Frame_Metadata>::iterator frames = new_animation_subset.frames.begin(); frames != new_animation_subset.frames.end(); frames++) {
    if (frames->id == 1) {
      for (std::list<Tileset_Helper::Collision_Block>::iterator collision_block = frames->collision_blocks.begin(); collision_block != frames->collision_blocks.end(); collision_block++) {
        Avatar_Component::Collision_Block new_collision_block;
        new_collision_block.id = collision_block->id;
        new_collision_block.is_contacting = collision_block->is_contacting;
        new_collision_block.size_x = collision_block->size_x;
        new_collision_block.size_y = collision_block->size_y;

        //Need to change the coordinate information from the "animation sheet" to the coordinate system in the game world.
        //Create a frame for the avatar foot
        Frame_Component avatar_contact_foot;
        D3DXVECTOR3 collision_block_tilesheet_centre;
        collision_block_tilesheet_centre.x = collision_block->top_left_x + (collision_block->size_x /2);
        collision_block_tilesheet_centre.y = collision_block->top_left_y - (collision_block->size_y /2);

        // account for different positions in the frame and the avatar
        // Work out the centre of the avatar frame (128x128 block) int the tileset
        // frame is 128x128 so get the frame # and times it by 128/2 for y
        D3DXVECTOR3 animation_frame_centre;
        animation_frame_centre.x = ((current_state.state_index + 1) * 128) - (128 / 2);
        // and 128/2 for x
        animation_frame_centre.y = (128 / 2) * -1;
        animation_frame_centre.z = -2;

        // store the distance from x and y to the centre of the animation frame
        new_collision_block.avatar_centre_offset_centre_x = collision_block_tilesheet_centre.x - animation_frame_centre.x;
        new_collision_block.avatar_centre_offset_centre_y = collision_block_tilesheet_centre.y - animation_frame_centre.y;

        new_state.collision_blocks.push_back(new_collision_block);
      }
    }
  }

  m_avatar->SetState(new_state);
  m_current_animation_fps = new_animation_subset.frames_per_second;

  m_avatar->GetTexture()->texture = 0;
  m_avatar->GetFrame()->vertex_buffer = 0;
  m_avatar->GetFrame()->index_buffer = 0;
  m_avatar->Init();

  m_current_metadata_file_path = new_state_metadata->filename;
  m_current_metadata = (*new_state_metadata);
  m_current_animation_subset = new_animation_subset;
}

//------------------------------------------------------------------------------
void Avatar_Controller::UpdateAvatarState(int new_state_index) {
  Avatar_Component::Avatar_State incremented_state;

  if (new_state_index > (m_current_animation_subset.number_of_frames - 1)) {
    if (m_current_animation_subset.is_repeatable) {
      new_state_index = 0; 
    } else {
      throw Tunnelour::Exceptions::init_error("No handling for non-repeating animtions yet");
    }
  }

  incremented_state.state = m_avatar->GetState().state;
  incremented_state.direction = m_avatar->GetState().direction;
  incremented_state.state_index = new_state_index;
  m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(m_current_animation_subset.top_left_x + (new_state_index * m_current_animation_subset.tile_size_x)),
                                                          static_cast<float>(m_current_animation_subset.top_left_y));

  bool stop = false;
  if (new_state_index != 0) {
    bool stop = true;
  }

  for (std::list<Tileset_Helper::Frame_Metadata>::iterator frames = m_current_animation_subset.frames.begin(); frames != m_current_animation_subset.frames.end(); frames++) {
    if (frames->id == (new_state_index + 1)) {
      for (std::list<Tileset_Helper::Collision_Block>::iterator collision_block = frames->collision_blocks.begin(); collision_block != frames->collision_blocks.end(); collision_block++) {
        Avatar_Component::Collision_Block new_collision_block;
        new_collision_block.id = collision_block->id;
        new_collision_block.is_contacting = collision_block->is_contacting;
        new_collision_block.size_x = collision_block->size_x;
        new_collision_block.size_y = collision_block->size_y;

        //Need to change the coordinate information from the "animation sheet" to the coordinate system in the game world.
        //Create a frame for the avatar foot
        Frame_Component avatar_contact_foot;
        D3DXVECTOR3 collision_block_tilesheet_centre;
        collision_block_tilesheet_centre.x = collision_block->top_left_x + (collision_block->size_x /2);
        collision_block_tilesheet_centre.y = collision_block->top_left_y - (collision_block->size_y /2);

        // account for different positions in the frame and the avatar
        // Work out the centre of the avatar frame (128x128 block) int the tileset
        // frame is 128x128 so get the frame # and times it by 128/2 for y
        D3DXVECTOR3 animation_frame_centre;
        animation_frame_centre.x = ((new_state_index + 1) * 128) - (128 / 2);
        // and 128/2 for x
        animation_frame_centre.y = (128 / 2) * -1;
        animation_frame_centre.z = -2;

        // store the distance from x and y to the centre of the animation frame
        new_collision_block.avatar_centre_offset_centre_x = collision_block_tilesheet_centre.x - animation_frame_centre.x;
        new_collision_block.avatar_centre_offset_centre_y = collision_block_tilesheet_centre.y - animation_frame_centre.y;

        incremented_state.collision_blocks.push_back(new_collision_block);
      }
    }
  }

  m_avatar->SetState(incremented_state);

  m_avatar->GetTexture()->texture = 0;
  m_avatar->GetFrame()->vertex_buffer = 0;
  m_avatar->GetFrame()->index_buffer = 0;
  m_avatar->Init();
}

} // Tunnelour