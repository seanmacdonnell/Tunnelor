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
  m_has_avatar_been_generated = false;
  m_animation_tick = false;

  m_angle = 0.7; //IN RADIANS
  m_init_velocity = 16;
  // compute velocities in x,y
  m_current_x_velocity = m_init_velocity*cos(m_angle);
  m_current_y_velocity = m_init_velocity*sin(m_angle);
  m_gravity = 32;
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
    }
  } else {
    // Is the avatar currenly not contacting the ground
    Update_Timer();
    if (m_animation_tick) {
      Run_Avatar_State(&mutator);
    }
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Avatar_Controller::Generate_Avatar() {
}

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
void Avatar_Controller::Run_Avatar_State(Avatar_Controller_Mutator *mutator) {
  std::string current_state = m_avatar->GetState().state;

  // Is the avatar currently in a standing state
  if (current_state.compare("Standing") == 0) {
    Run_Standing_State(mutator);
  } else if (current_state.compare("Walking") == 0) {
    Run_Walking_State(mutator);
  } else if (current_state.compare("Falling") == 0 || current_state.compare("Walking_To_Falling") == 0 || current_state.compare("Falling_To_Standing") == 0 ) {
    Run_Falling_State(mutator);
  }

  m_animation_tick = false;
}

//------------------------------------------------------------------------------
void Avatar_Controller::Run_Standing_State(Avatar_Controller_Mutator *mutator) {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

  if (current_command.state.compare("") != 0) {
    ChangeAvatarState(current_command.state, current_command.direction);
    Avatar_Component::Avatar_State cleared_state;
    cleared_state.state = "";
    cleared_state.direction = "";
    cleared_state.state_index = 0;
    m_avatar->SetCommand(cleared_state);
  } else {
    unsigned int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
      if (m_current_animation_subset.is_repeatable) {
        state_index = 0; 
      } else {
        throw Tunnelour::Exceptions::init_error("No handling for non-repeating standing animtions yet");
      }
    }

    UpdateAvatarState(state_index);
  }

  // Detect if the avatar is overbalancing from walking
  if (!Is_Avatar_Platform_Adjacent(mutator)) {
    ChangeAvatarState("Falling_To_Standing", m_avatar->GetState().direction);
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::Run_Walking_State(Avatar_Controller_Mutator *mutator) {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();
  Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();
  if (current_command.state.compare("") != 0) { 
    // There is a command
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
          throw Tunnelour::Exceptions::init_error("No handling for non-repeating walking animtions yet");
        }
      }

      UpdateAvatarState(state_index);

      D3DXVECTOR3 position = m_avatar->GetPosition();
      if (current_state.direction.compare("Right") == 0) {
        position =Align_Avatar_On_Right_Foot(mutator);
        //position.x += 8;
      } else { // Left
        //position.x -= 8;
        position = Align_Avatar_On_Right_Foot(mutator);
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
  }

  // Detect if the avatar is overbalancing from walking
  if (!Is_Avatar_Platform_Adjacent(mutator)) {
    ChangeAvatarState("Walking_To_Falling", m_avatar->GetState().direction);
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::Run_Falling_State(Avatar_Controller_Mutator *mutator) {
  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  // If the avatar is currently intersecting with a tile, move him to the top of the tile
  std::list<Tunnelour::Bitmap_Component*> out_colliding_border_tiles;
  Tunnelour::Bitmap_Component* out_collision_block = new Tunnelour::Bitmap_Component();
  if (current_state.state.compare("Falling_To_Standing") == 0) {
    if (Is_Avatar_Platform_Colliding(mutator, &out_colliding_border_tiles, out_collision_block)) {
      //Move to the top of the tile
      bool is_tangent = false;
      int tile_top;
      for (std::list<Tunnelour::Bitmap_Component*>::iterator it = out_colliding_border_tiles.begin(); it != out_colliding_border_tiles.end(); it++) {
        tile_top = static_cast<int>((*it)->GetPosition().y + (*it)->GetSize()->y / 2);
        int collision_block_bottom = static_cast<int>(out_collision_block->GetPosition().y - (out_collision_block->GetSize()->y /2));

        if (tile_top == collision_block_bottom) {
          is_tangent = true;
        }
      }

      // Move the avatar to the top of the tile
      D3DXVECTOR3 avatar_position = m_avatar->GetPosition();
      int foot_bottom = out_collision_block->GetPosition().y - (out_collision_block->GetSize()->y / 2);
      int foot_bottom_centre_offset = foot_bottom - avatar_position.y;

      avatar_position.y = static_cast<float>(tile_top - foot_bottom_centre_offset);
      m_avatar->SetPosition(avatar_position);

      ChangeAvatarState("Standing", m_avatar->GetState().direction);
    } else {
      D3DXVECTOR3 position = m_avatar->GetPosition();
      position.y -= 32;
      m_avatar->SetPosition(position);
    }
  } else {
    unsigned int state_index = current_state.state_index;
    state_index++;
    if (state_index > (m_current_animation_subset.number_of_frames - 1)) {
      if (m_current_animation_subset.is_repeatable) {
        state_index = 0; 
      } else {
        if (current_state.state.compare("Walking_To_Falling") == 0) {
          ChangeAvatarState("Falling", m_avatar->GetState().direction);
          state_index = 0;
        } else {
          throw Tunnelour::Exceptions::init_error("No handling for non-repeating falling animtions yet");
        }
      }
    }

    UpdateAvatarState(state_index);

    D3DXVECTOR3 position = m_avatar->GetPosition();
  
    if (current_state.state.compare("Walking_To_Falling") == 0) {
      if (state_index < 6) {
        position = Align_Avatar_On_Right_Foot(mutator);
      } else {
        position = CalculateArc();
      }
    } else {
      position = CalculateArc();
    }

    m_avatar->SetPosition(position);
  }
}



//------------------------------------------------------------------------------
bool Avatar_Controller::Is_Avatar_Platform_Adjacent(Avatar_Controller_Mutator *mutator) {
  // Going to deal only with gravity only right now
  // Also only dealing with the lowest foot, if they have the same Y I do not do any special calculations.

  // If the lowest collision block is adjecent to a tile
    // Do nothing
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
    Bitmap_Component lowest_collision_block_frame;
    D3DXVECTOR3 lowest_collision_block_frame_centre_position;
    lowest_collision_block_frame_centre_position.x = static_cast<float>(m_avatar->GetPosition().x + lowest_collision_block->avatar_centre_offset_centre_x);
    lowest_collision_block_frame_centre_position.y = static_cast<float>(m_avatar->GetPosition().y + lowest_collision_block->avatar_centre_offset_centre_y);
    lowest_collision_block_frame_centre_position.z = static_cast<float>(-2);

    lowest_collision_block_frame.SetPosition(lowest_collision_block_frame_centre_position);
    lowest_collision_block_frame.SetSize(static_cast<float>(lowest_collision_block->size_x), static_cast<float>(lowest_collision_block->size_y));

    // Cull the border tiles which are not within the x range of the lowest avatar collision block
    std::list<Tunnelour::Bitmap_Component*> colliding_border_tiles;
    std::list<Tunnelour::Bitmap_Component*> border_tiles = mutator->GetBorderTiles();
    for (std::list<Tunnelour::Bitmap_Component*>::iterator it = border_tiles.begin(); it != border_tiles.end(); it++) {
      if (Bitmap_Helper::DoTheseTilesXCollide(*it, &lowest_collision_block_frame)) {
        if (Bitmap_Helper::AreTheseTilesYAdjacent(*it, &lowest_collision_block_frame)) {
          colliding_border_tiles.push_back(*it);
        }
      }
    }

    // If that block is not intersecting with the ground put the avatar into a falling state
    if (!colliding_border_tiles.empty()) {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Is_Avatar_Platform_Colliding(Avatar_Controller_Mutator *mutator, std::list<Tunnelour::Bitmap_Component*> *out_colliding_border_tiles, Tunnelour::Bitmap_Component *out_collision_block) {
  // Going to deal only with gravity only right now
  // Also only dealing with the lowest foot, if they have the same Y I do not do any special calculations.

  // If the lowest collision block is adjecent to a tile
    // Do nothing
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
    D3DXVECTOR3 lowest_collision_block_frame_centre_position;
    lowest_collision_block_frame_centre_position.x = static_cast<float>(m_avatar->GetPosition().x + lowest_collision_block->avatar_centre_offset_centre_x);
    lowest_collision_block_frame_centre_position.y = static_cast<float>(m_avatar->GetPosition().y + lowest_collision_block->avatar_centre_offset_centre_y);
    lowest_collision_block_frame_centre_position.z = static_cast<float>(-2);

    out_collision_block->SetPosition(lowest_collision_block_frame_centre_position);
    out_collision_block->SetSize(static_cast<float>(lowest_collision_block->size_x), static_cast<float>(lowest_collision_block->size_y));

    // Cull the border tiles which are not within the x range of the lowest avatar collision block
    std::list<Tunnelour::Bitmap_Component*> border_tiles = mutator->GetBorderTiles();
    for (std::list<Tunnelour::Bitmap_Component*>::iterator it = border_tiles.begin(); it != border_tiles.end(); it++) {
      if (Bitmap_Helper::DoTheseTilesXCollide(*it, out_collision_block)) {
        if (Bitmap_Helper::DoTheseTilesYCollide(*it, out_collision_block)) {
          out_colliding_border_tiles->push_back(*it);
        }
      }
    }

    // If that block is not intersecting with the ground put the avatar into a falling state
    if (!out_colliding_border_tiles->empty()) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
void Avatar_Controller::Load_Tilesets(std::wstring wtileset_path) {
  // Running
  m_running_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Running_Animation_Tileset_1_1.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_running_metadata_file_path, m_running_metadata);
  m_animation_metadata.push_back(m_running_metadata);

  // Walking
  m_walking_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Walking_Animation_Tileset_1_5.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_walking_metadata_file_path, m_walking_metadata);
  m_animation_metadata.push_back(m_walking_metadata);

  // Standing
  m_standing_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Standing_Animation_Tileset_1_1.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_standing_metadata_file_path, m_standing_metadata);
  m_animation_metadata.push_back(m_standing_metadata);
 
  // Falling
  m_falling_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Falling_Animation_Tileset_1_0.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_falling_metadata_file_path, m_falling_metadata);
  m_animation_metadata.push_back(m_falling_metadata);
}

//------------------------------------------------------------------------------
D3DXVECTOR3 Avatar_Controller::Align_Avatar_On_Right_Foot(Avatar_Controller_Mutator *mutator) {
  D3DXVECTOR3 new_avatar_position;

  // Go through the contact blocks and find the lowest contact block
  Avatar_Component::Collision_Block* current_right_foot_collision_block = 0;
  std::list<Avatar_Component::Collision_Block> current_collision_blocks = m_avatar->GetState().collision_blocks;
  for (std::list<Avatar_Component::Collision_Block>::iterator collision_block = current_collision_blocks.begin(); collision_block != current_collision_blocks.end(); collision_block++) {
    if (collision_block->id.compare("Right_Foot") == 0) {
      current_right_foot_collision_block = &(*(collision_block));
    }
  }

  Avatar_Component::Collision_Block* last_right_foot_collision_block = 0;
  std::list<Avatar_Component::Collision_Block> last_collision_blocks = m_avatar->GetLastState().collision_blocks;
  for (std::list<Avatar_Component::Collision_Block>::iterator collision_block = last_collision_blocks.begin(); collision_block != last_collision_blocks.end(); collision_block++) {
    if (collision_block->id.compare("Right_Foot") == 0) {
      last_right_foot_collision_block = &(*(collision_block));
    }
  }

  D3DXVECTOR3 right_foot_offset;
  right_foot_offset.x = last_right_foot_collision_block->avatar_centre_offset_centre_x - current_right_foot_collision_block->avatar_centre_offset_centre_x;
  right_foot_offset.y = 0;//last_right_foot_collision_block->avatar_centre_offset_centre_y - current_right_foot_collision_block->avatar_centre_offset_centre_y;
  right_foot_offset.z = 0;
  

  // TODO: This experement was a failure, just move the avatar 6 pixels untill I fix
  //       the walking animation to be more natural.
  right_foot_offset.x = 8;

  if (m_avatar->GetState().direction.compare("Right") == 0 && right_foot_offset.x < 0) {
    right_foot_offset.x = right_foot_offset.x * -1;
  }

  if (m_avatar->GetState().direction.compare("Left") == 0 && right_foot_offset.x > 0) {
    right_foot_offset.x = right_foot_offset.x * -1;
  }
  
  /*
  if (m_avatar->GetState().direction.compare("Right") == 0 && right_foot_offset.x < 8) {
    right_foot_offset.x = 4;
  }

  if (m_avatar->GetState().direction.compare("Left") == 0 && right_foot_offset.x > -8) {
    right_foot_offset.x = -4;
  }
  */

  D3DXVECTOR3 current_avatar_position = m_avatar->GetPosition();
  //new_avatar_position.x = static_cast<float>(m_avatar->GetPosition().x + right_foot_collision_block->avatar_centre_offset_centre_x);
  //new_avatar_position.y = static_cast<float>(m_avatar->GetPosition().y + right_foot_collision_block->avatar_centre_offset_centre_y);
  //new_avatar_position.z = static_cast<float>(-2);

  new_avatar_position = current_avatar_position + right_foot_offset;

  return new_avatar_position;
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
void Avatar_Controller::ChangeAvatarState(std::string new_state_name, std::string direction) {
  Tileset_Helper::Animation_Tileset_Metadata *new_state_metadata = 0;
  Avatar_Component::Avatar_State new_state;

  Avatar_Component::Avatar_State current_state = m_avatar->GetState();

  Tileset_Helper::Animation_Subset new_animation_subset;
  for (std::list<Tileset_Helper::Animation_Tileset_Metadata>::iterator metadata = m_animation_metadata.begin(); metadata != m_animation_metadata.end(); metadata++) {
    for (std::list<Tileset_Helper::Animation_Subset>::iterator tileset = (*metadata).subsets.begin(); tileset != (*metadata).subsets.end(); tileset++) {
      if (tileset->name.compare(new_state_name) == 0) {
        new_animation_subset = *tileset;
        new_state_metadata = &(*metadata);
        new_state.direction = direction;
        new_state.state = new_state_name;
      }
    }
  }

  if (new_state_metadata == 0) {
    throw Tunnelour::Exceptions::init_error("Animation not found in metadata list!" + new_state_name);  
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
                                                          static_cast<float>((new_animation_subset.top_left_y) * -1));
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
        collision_block_tilesheet_centre.x = static_cast<float>(collision_block->top_left_x + (collision_block->size_x /2));
        collision_block_tilesheet_centre.y = static_cast<float>(collision_block->top_left_y - (collision_block->size_y /2));

        // account for different positions in the frame and the avatar
        // Work out the centre of the avatar frame (128x128 block) int the tileset
        // frame is 128x128 so get the frame # and times it by 128/2 for y
        D3DXVECTOR3 animation_frame_centre;
        animation_frame_centre.x = static_cast<float>(((new_state.state_index + 1) * 128) - (128 / 2));
        // and 128/2 for x
        animation_frame_centre.y = static_cast<float>(((new_animation_subset.top_left_y) - (128 / 2)));
        animation_frame_centre.z = static_cast<float>(-2);

        // store the distance from x and y to the centre of the animation frame
        new_collision_block.avatar_centre_offset_centre_x = static_cast<int>(collision_block_tilesheet_centre.x - animation_frame_centre.x);
        new_collision_block.avatar_centre_offset_centre_y = static_cast<int>(collision_block_tilesheet_centre.y - animation_frame_centre.y);

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

  incremented_state.state = m_avatar->GetState().state;
  incremented_state.direction = m_avatar->GetState().direction;
  incremented_state.state_index = new_state_index;
  m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(m_current_animation_subset.top_left_x + (new_state_index * m_current_animation_subset.tile_size_x)),
                                                          static_cast<float>(m_current_animation_subset.top_left_y * -1));

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
        collision_block_tilesheet_centre.x = static_cast<float>(collision_block->top_left_x + (collision_block->size_x /2));
        collision_block_tilesheet_centre.y = static_cast<float>(collision_block->top_left_y - (collision_block->size_y /2));

        // account for different positions in the frame and the avatar
        // Work out the centre of the avatar frame (128x128 block) int the tileset
        // frame is 128x128 so get the frame # and times it by 128/2 for y
        D3DXVECTOR3 animation_frame_centre;
        animation_frame_centre.x = static_cast<float>(((new_state_index + 1) * 128) - (128 / 2));
        // and 128/2 for x
        animation_frame_centre.y = static_cast<float>(((m_current_animation_subset.top_left_y) - (128 / 2)));
        animation_frame_centre.z = static_cast<float>(-2);

        // store the distance from x and y to the centre of the animation frame
        new_collision_block.avatar_centre_offset_centre_x = static_cast<int>(collision_block_tilesheet_centre.x - animation_frame_centre.x);
        new_collision_block.avatar_centre_offset_centre_y = static_cast<int>(collision_block_tilesheet_centre.y - animation_frame_centre.y);

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


//------------------------------------------------------------------------------
D3DXVECTOR3 Avatar_Controller::CalculateArc() {
  D3DXVECTOR3 new_position = m_avatar->GetPosition();

  // update position
  new_position.x = new_position.x + static_cast<int>(m_current_x_velocity);
  new_position.y = new_position.y - static_cast<int>(m_current_y_velocity);

  // update velocity
  m_current_y_velocity = m_current_y_velocity + m_gravity;

  return new_position;
}

} // Tunnelour