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
      m_avatar->SetPosition(D3DXVECTOR3(0, 0, -2)); // Middleground Z Space is -1
      m_model->Add(m_avatar);
      m_has_avatar_been_generated = true;   
    }
  } else {
    Avatar_Component::Avatar_State current_state = m_avatar->GetState();
    Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

    Tileset_Helper::Animation_Tileset_Metadata *new_state_metadata = 0;
    Avatar_Component::Avatar_State new_state;
    // Change State
    if (current_command.state.compare("Walking") == 0) {
      new_state_metadata = &m_walking_metadata;
      new_state.direction = current_command.direction;
      new_state.state = current_command.state;
    } else if (current_command.state.compare("Running") == 0) {
      new_state_metadata = &m_running_metadata;
      new_state.direction = current_command.direction;
      new_state.state = current_command.state;
    } else if (current_command.state.compare("Falling") == 0) {
      new_state_metadata = &m_standing_metadata;
      new_state.direction = current_state.direction;
      new_state.state = current_command.state;
    } else {
      // Standing;
      new_state_metadata = &m_standing_metadata;
      new_state.direction = current_state.direction;
      new_state.state = "Standing";
    }

    Tileset_Helper::Animation_Subset new_animation_subset;
    for (std::list<Tileset_Helper::Animation_Subset>::iterator tileset = new_state_metadata->subsets.begin(); tileset != new_state_metadata->subsets.end(); tileset++) {
      if (tileset->name.compare(new_state.state) == 0) {
        new_animation_subset = *tileset;
      }
    }

    if (current_state.state.compare("Falling") != 0) {
      // If State does not equal Command
      if (current_state.state.compare(current_command.state) != 0) {
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
      } else {
        // Continue Same State if its time
        // Increment Index
        Update_Timer();
        if (m_animation_tick) {
          // Increment the animation
          // Change State
          unsigned int state_index = current_state.state_index;
          state_index++;
          if (state_index > (new_animation_subset.number_of_frames - 1)) { state_index = 0; }
          new_state.state_index = state_index;
          m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(new_animation_subset.top_left_x + (state_index * new_animation_subset.tile_size_x)),
                                                                  static_cast<float>(new_animation_subset.top_left_y));
          D3DXVECTOR3 position = m_avatar->GetPosition();
          if (new_state.direction.compare("Right") == 0) {
            if (new_state.state.compare("Running") == 0) {
              position.x += 16;
            } else if (new_state.state.compare("Walking") == 0) {
              position.x += 8;
            } else if (new_state.state.compare("Falling") == 0) {
              position.y -= 16;
            } 
          } else {
            if (new_state.state.compare("Running") == 0) {
              position.x -= 16;
            } else if (new_state.state.compare("Walking") == 0) {
              position.x -= 8;
            } else if (new_state.state.compare("Falling") == 0) {
              position.y -= 16;
            } 
          }

          for (std::list<Tileset_Helper::Frame_Metadata>::iterator frames = new_animation_subset.frames.begin(); frames != new_animation_subset.frames.end(); frames++) {
            if (frames->id == (state_index + 1)) {
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

          m_avatar->SetPosition(position);
          m_animation_tick = false;
          m_avatar->SetState(new_state);
          Is_Avatar_Falling(&mutator);
        }
      }
    } else {
      // Current state is falling, uncontrolable.
      D3DXVECTOR3 position = m_avatar->GetPosition();
      position.y -= 16;
      m_avatar->SetPosition(position);
    }
    Avatar_Component::Avatar_State cleared_state;
    cleared_state.state = "";
    cleared_state.direction = "";
    cleared_state.state_index = 0;
    m_avatar->SetCommand(cleared_state);

    m_avatar->GetTexture()->texture = 0;
    m_avatar->GetFrame()->vertex_buffer = 0;
    m_avatar->GetFrame()->index_buffer = 0;
    m_avatar->Init();
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Avatar_Controller::Generate_Avatar_Tile() {
  Tileset_Helper::Animation_Subset standing_animation_subset;
  for (std::list<Tileset_Helper::Animation_Subset>::iterator tileset = m_standing_metadata.subsets.begin(); tileset != m_standing_metadata.subsets.end(); tileset++) {
    if (tileset->name.compare("Standing") == 0) {
      standing_animation_subset = *tileset;
    }
  }

  m_avatar = new Tunnelour::Avatar_Component();
  m_avatar->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_standing_metadata.filename);
  m_avatar->GetTexture()->texture_path = texture_path;
  m_avatar->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_standing_metadata.size_x),
                                                     static_cast<float>(m_standing_metadata.size_y));
  m_avatar->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(standing_animation_subset.tile_size_x),
                                                  static_cast<float>(standing_animation_subset.tile_size_y));
  m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(standing_animation_subset.top_left_x),
                                                          static_cast<float>(standing_animation_subset.top_left_y));
  m_avatar->SetSize(new D3DXVECTOR2(static_cast<float>(standing_animation_subset.tile_size_x), static_cast<float>(standing_animation_subset.tile_size_y)));

  Avatar_Component::Avatar_State state;
  state.state = standing_animation_subset.name;
  state.direction = "Right";
  state.state_index = 0;

  for (std::list<Tileset_Helper::Frame_Metadata>::iterator frames = standing_animation_subset.frames.begin(); frames != standing_animation_subset.frames.end(); frames++) {
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
        animation_frame_centre.x = ((m_avatar->GetState().state_index + 1) * 128) - (128 / 2);
        // and 128/2 for x
        animation_frame_centre.y = (128 / 2) * -1;
        animation_frame_centre.z = -2;

        // store the distance from x and y to the centre of the animation frame
        new_collision_block.avatar_centre_offset_centre_x = collision_block_tilesheet_centre.x - animation_frame_centre.x;
        new_collision_block.avatar_centre_offset_centre_y = collision_block_tilesheet_centre.y - animation_frame_centre.y;

        state.collision_blocks.push_back(new_collision_block);
      }
    }
  }


  m_avatar->SetState(state);
  m_avatar->Init();
}

//------------------------------------------------------------------------------
bool Avatar_Controller::Is_Avatar_Falling(Avatar_Controller_Mutator *mutator) {
  // if avatar is currently falling, no problems.
  if (m_avatar->GetState().state.compare("Falling")) { return true; }

  // Going to deal only with gravity only right now
  // Also only dealing with the lowest foot, if they have the same Y I do not do any special calculations.
  // If there are no border tiles, the avatar is in freefall
  if (mutator->FoundBorderTiles()) {
    // Go through the contact blocks and find the lowest contact block
    Avatar_Component::Collision_Block* lowest_collision_block = 0;
    for (std::list<Avatar_Component::Collision_Block>::iterator collision_block = m_avatar->GetState().collision_blocks.begin(); collision_block != m_avatar->GetState().collision_blocks.end(); collision_block++) {
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
    for (std::list<Tunnelour::Bitmap_Component*>::iterator it = mutator->GetBorderTiles().begin(); it != mutator->GetBorderTiles().end(); it++) {
      if (DoTheseTilesXCollide(*it, &lowest_collision_block_frame)) {
        if (DoTheseTilesYCollide(*it, &lowest_collision_block_frame)) {
          colliding_border_tiles.push_back(*it);
        }
      }
    }

    // If that block is not intersecting with the ground put the avatar into a falling state
    if (colliding_border_tiles.empty()) {
      // Avatar is falling.
      Avatar_Component::Avatar_State falling_state;
      falling_state.direction = m_avatar->GetState().direction;
      falling_state.state = "Falling";
      falling_state.state_index = 0;
      m_avatar->SetState(falling_state);
      return true;
    } else {
      // Check to see if the lowest bottom line is tangent with any tiles which are colliding

      bool is_tangent = false;
      for (std::list<Tunnelour::Bitmap_Component*>::iterator it = colliding_border_tiles.begin(); it != colliding_border_tiles.end(); it++) {
         int tile_top = static_cast<int>((*it)->GetPosition().y + (*it)->GetSize()->y / 2);
         int collision_block_bottom = static_cast<int>(lowest_collision_block_frame_centre_position.y - (lowest_collision_block->size_y /2));

         if (tile_top == collision_block_bottom) {
           is_tangent = true;
         }
      }

      if (!is_tangent) {
        // Avatar is falling.
        Avatar_Component::Avatar_State falling_state;
        falling_state.direction = m_avatar->GetState().direction;
        falling_state.state = "Falling";
        falling_state.state_index = 0;
        m_avatar->SetState(falling_state);
        return true;
      }
    }
  } else {
    // Avatar is falling.
    Avatar_Component::Avatar_State falling_state;
    falling_state.direction = m_avatar->GetState().direction;
    falling_state.state = "Falling";
    falling_state.state_index = 0;
    m_avatar->SetState(falling_state);
    return true;
  }


  /*



  if (mutator->FoundBorderTiles()) {
    Avatar_Component::Avatar_State current_state = m_avatar->GetState();
    Avatar_Component::Avatar_Foot_State lowest_foot;
    int lowest_foot_bottom_left_y = 0;
    // Find the lowest foot which is contacting the floor
    if (current_state.left_foot.is_contacting || current_state.right_foot.is_contacting) {
      if (current_state.left_foot.is_contacting) {
        lowest_foot = current_state.left_foot;
        lowest_foot_bottom_left_y = current_state.left_foot.top_left_y - current_state.left_foot.size_y;
        if (current_state.right_foot.is_contacting) {
          if ((current_state.left_foot.top_left_y - current_state.left_foot.size_y) < current_state.right_foot.top_left_y - current_state.right_foot.size_y) {
            // Right foot is lower!
            lowest_foot = current_state.right_foot;
            lowest_foot_bottom_left_y = current_state.right_foot.top_left_y - current_state.right_foot.size_y;
          }
        }
      } else if (current_state.right_foot.is_contacting) {
        lowest_foot = current_state.right_foot;
        lowest_foot_bottom_left_y = current_state.right_foot.top_left_y - current_state.right_foot.size_y;
      } 
    } else {
      // No foot on floor!
      // Change State to falling
      Avatar_Component::Avatar_State falling_state;
      falling_state.direction = current_state.direction;
      falling_state.state = "Falling";
      falling_state.state_index = 0;
      m_avatar->SetState(falling_state);
      return;
    }

    // Cull the border tiles which are not within the x range of the avatar square
    std::list<Tunnelour::Bitmap_Component*> horizontal_collision_border_tiles;
    std::list<Tunnelour::Bitmap_Component*> border_tiles = mutator->GetBorderTiles();
    std::list<Tunnelour::Bitmap_Component*>::iterator it;

    //Create a frame for the avatar foot
    Frame_Component avatar_contact_foot;
    D3DXVECTOR3 lowest_foot_centre_position;
    lowest_foot_centre_position.x = lowest_foot.top_left_x + (lowest_foot.size_x /2);
    lowest_foot_centre_position.y = lowest_foot.top_left_y - (lowest_foot.size_y /2);
    lowest_foot_centre_position.z = -2;

    // account for different positions in the frame and the avatar

    // Work out the centre of the frame
    // frame is 128x128 so get the frame # and times it by 128/2 for y
    D3DXVECTOR3 animation_frame_centre;
    animation_frame_centre.x = ((current_state.state_index + 1) * 128) - (128 / 2);
    // and 128/2 for x
    animation_frame_centre.y = (128 / 2) * -1;
    animation_frame_centre.z = -2;

    // store the distance from x and y to the centre of the animation frame
    D3DXVECTOR3 animation_frame_offset;
    animation_frame_offset.x = lowest_foot_centre_position.x - animation_frame_centre.x;
    animation_frame_offset.y = lowest_foot_centre_position.y - animation_frame_centre.y;
    animation_frame_offset.z = -2;

    // Untranslate
    D3DXVECTOR3 avatar_lowest_foot_centre_position;
    avatar_lowest_foot_centre_position = animation_frame_offset;
    avatar_lowest_foot_centre_position.x = avatar_lowest_foot_centre_position.x + m_avatar->GetPosition().x;
    avatar_lowest_foot_centre_position.y = avatar_lowest_foot_centre_position.y - m_avatar->GetPosition().y;
    avatar_lowest_foot_centre_position.z = m_avatar->GetPosition().z;
     
    avatar_contact_foot.SetPosition(avatar_lowest_foot_centre_position);
    avatar_contact_foot.SetSize(lowest_foot.size_x, lowest_foot.size_y);

    for (it = border_tiles.begin(); it != border_tiles.end(); it++) {
      if (DoTheseTilesXCollide(*it, &avatar_contact_foot)) {
        horizontal_collision_border_tiles.push_back(*it);
      }
    }

    if (horizontal_collision_border_tiles.empty()) {
      // No foot on floor!
      // Change State to falling
      // Move Avatar down
      Tileset_Helper::Animation_Subset standing_animation_subset;
      for (std::list<Tileset_Helper::Animation_Subset>::iterator tileset = m_standing_metadata.subsets.begin(); tileset != m_standing_metadata.subsets.end(); tileset++) {
        if (tileset->name.compare("Standing") == 0) {
          standing_animation_subset = *tileset;
        }
      }

      Avatar_Component::Avatar_State new_state;
      new_state.state_index = 0;
      std::wstring texture_path = m_game_settings->GetTilesetPath();
      texture_path += String_Helper::StringToWString(m_standing_metadata.filename);
      m_avatar->GetTexture()->texture_path = texture_path;
      m_avatar->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_standing_metadata.size_x),
                                                          static_cast<float>(m_standing_metadata.size_y));
      m_avatar->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(standing_animation_subset.tile_size_x),
                                                      static_cast<float>(standing_animation_subset.tile_size_y));
      m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(standing_animation_subset.top_left_x),
                                                              static_cast<float>(standing_animation_subset.top_left_y));
      m_avatar->SetSize(new D3DXVECTOR2(static_cast<float>(standing_animation_subset.tile_size_x), static_cast<float>(standing_animation_subset.tile_size_y)));

      for (std::list<Tileset_Helper::Frame_Metadata>::iterator frames = standing_animation_subset.frames.begin(); frames != standing_animation_subset.frames.end(); frames++) {
        if (frames->id == 1) {
          for (std::list<Tileset_Helper::Collision_Block>::iterator collision_block = frames->collision_blocks.begin(); collision_block != frames->collision_blocks.end(); collision_block++) {
            if (collision_block->id.compare("Left_Foot") == 0) {
              Avatar_Component::Avatar_Foot_State left;
              left.size_x = collision_block->size_x;
              left.size_y = collision_block->size_y;
              left.top_left_x = collision_block->top_left_x;
              left.top_left_y = collision_block->top_left_y;
              left.is_contacting = collision_block->is_contacting;
              new_state.left_foot = left;
            }
            if (collision_block->id.compare("Right_Foot") == 0) {
              Avatar_Component::Avatar_Foot_State left;
              left.size_x = collision_block->size_x;
              left.size_y = collision_block->size_y;
              left.top_left_x = collision_block->top_left_x;
              left.top_left_y = collision_block->top_left_y;
              left.is_contacting = collision_block->is_contacting;
              new_state.left_foot = left;
            }
          }
        }
      }
      new_state.state = "Falling";
      new_state.direction = m_avatar->GetState().direction;
      m_avatar->SetState(new_state);
      m_current_animation_fps = standing_animation_subset.frames_per_second;
      return;
    }

    // Check to see if the avatar is on a border
    Tunnelour::Bitmap_Component* tile = horizontal_collision_border_tiles.front();
    int tile_top = static_cast<int>(tile->GetPosition().y + tile->GetSize()->y / 2);
    int avatar_bottom = static_cast<int>(m_avatar->GetPosition().y - (m_avatar->GetSize()->y / 2));
    int avatar_foot_bottom  = static_cast<int>((m_avatar->GetPosition().y + animation_frame_offset.y) - (lowest_foot.size_y /2));

    int foot_to_tile_bottom_offset;
    foot_to_tile_bottom_offset =  avatar_foot_bottom - avatar_bottom;
    int avatar_y;

    avatar_y = tile_top - avatar_foot_bottom;

    if (tile_top != avatar_foot_bottom) {
      // if not
      // Move the frame down that the avatars feet touch the floor
      // Determine the distance between the foot and the bottom of the tile
      // Move the frame down that amount of pixels
      // Move the avatar tile down so the frame touches the floor
      m_avatar->SetPosition(D3DXVECTOR3(0, static_cast<float>(avatar_y), -2)); // Middleground Z Space is -1
    }
  }
  */
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

  return true;
}

bool Avatar_Controller::DoTheseTilesYCollide(Tunnelour::Frame_Component* TileA, Tunnelour::Frame_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  float a_tile_top, a_tile_bottom;
  a_tile_top = TileA->GetPosition().y + static_cast<float>(TileA->GetSize()->y / 2);
  a_tile_bottom = TileA->GetPosition().y - static_cast<float>(TileA->GetSize()->y / 2);

  float b_tile_top, b_tile_bottom;
  b_tile_top = TileA->GetPosition().y + static_cast<float>(TileA->GetSize()->y / 2);
  b_tile_bottom = TileA->GetPosition().y - static_cast<float>(TileA->GetSize()->y / 2);

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

} // Tunnelour