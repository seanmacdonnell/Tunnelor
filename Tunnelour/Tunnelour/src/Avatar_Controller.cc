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
      Place_Avatar_Tile(&mutator);
      m_model->Add(m_avatar);
      m_has_avatar_been_generated = true;   
    }
  } else {
    Avatar_Component::Avatar_State current_state = m_avatar->GetState();
    Avatar_Component::Avatar_State current_command = m_avatar->GetCommand();

    Animation_Tileset_Metadata *new_state_metadata = 0;
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
    } else {
      // Standing;
      new_state_metadata = &m_standing_metadata;
      new_state.direction = current_state.direction;
      new_state.state = "Standing";
    }

    Animation_Subset new_animation_subset;
    for (std::list<Animation_Subset>::iterator tileset = new_state_metadata->subsets.begin(); tileset != new_state_metadata->subsets.end(); tileset++) {
      if (tileset->name.compare(new_state.state) == 0) {
        new_animation_subset = *tileset;
      }
    }

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


      for (std::list<Frame_Metadata>::iterator frames = new_animation_subset.frames.begin(); frames != new_animation_subset.frames.end(); frames++) {
        if (frames->id == 1) {
          Avatar_Component::Avatar_Foot_State left;
          left.size_x = frames->left_foot_size_x;
          left.size_y = frames->left_foot_size_y;
          left.top_left_x = frames->left_foot_top_left_x;
          left.top_left_y = frames->left_foot_top_left_y;
          left.state = frames->left_foot_state;

          Avatar_Component::Avatar_Foot_State right;
          right.size_x = frames->right_foot_size_x;
          right.size_y = frames->right_foot_size_y;
          right.top_left_x = frames->right_foot_top_left_x;
          right.top_left_y = frames->right_foot_top_left_y;
          right.state = frames->right_foot_state;

          new_state.left_foot = left;
          new_state.right_foot = right;
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
          } 
        } else {
          if (new_state.state.compare("Running") == 0) {
            position.x -= 16;
          } else if (new_state.state.compare("Walking") == 0) {
            position.x -= 8;
          } 
        }

        for (std::list<Frame_Metadata>::iterator frames = new_animation_subset.frames.begin(); frames != new_animation_subset.frames.end(); frames++) {
          if (frames->id == (state_index + 1)) {
            Avatar_Component::Avatar_Foot_State left;
            left.size_x = frames->left_foot_size_x;
            left.size_y = frames->left_foot_size_y;
            left.top_left_x = frames->left_foot_top_left_x;
            left.top_left_y = frames->left_foot_top_left_y;
            left.state = frames->left_foot_state;

            Avatar_Component::Avatar_Foot_State right;
            right.size_x = frames->right_foot_size_x;
            right.size_y = frames->right_foot_size_y;
            right.top_left_x = frames->right_foot_top_left_x;
            right.top_left_y = frames->right_foot_top_left_y;
            right.state = frames->right_foot_state;

            new_state.left_foot = left;
            new_state.right_foot = right;
          }
        }

        m_avatar->SetPosition(position);
        m_animation_tick = false;
        m_avatar->SetState(new_state);
        Place_Avatar_Tile(&mutator);
      }
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
  Animation_Subset standing_animation_subset;
  for (std::list<Animation_Subset>::iterator tileset = m_standing_metadata.subsets.begin(); tileset != m_standing_metadata.subsets.end(); tileset++) {
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

  for (std::list<Frame_Metadata>::iterator frames = standing_animation_subset.frames.begin(); frames != standing_animation_subset.frames.end(); frames++) {
    if (frames->id == 1) {
      Avatar_Component::Avatar_Foot_State left;
      left.size_x = frames->left_foot_size_x;
      left.size_y = frames->left_foot_size_y;
      left.top_left_x = frames->left_foot_top_left_x;
      left.top_left_y = frames->left_foot_top_left_y;
      left.state = frames->left_foot_state;

      Avatar_Component::Avatar_Foot_State right;
      right.size_x = frames->right_foot_size_x;
      right.size_y = frames->right_foot_size_y;
      right.top_left_x = frames->right_foot_top_left_x;
      right.top_left_y = frames->right_foot_top_left_y;
      right.state = frames->right_foot_state;

      state.left_foot = left;
      state.right_foot = right;
    }
  }


  m_avatar->SetState(state);
  m_avatar->Init();
}

//------------------------------------------------------------------------------
void Avatar_Controller::Place_Avatar_Tile(Avatar_Controller_Mutator *mutator) {
  //m_avatar->SetPosition(D3DXVECTOR3(0, 20, -2)); // Middleground Z Space is -1
  if (mutator->FoundBorderTiles()) {
    Avatar_Component::Avatar_State current_state = m_avatar->GetState();
    Avatar_Component::Avatar_Foot_State lowest_foot;
    int lowest_foot_bottom_left_y = 0;
    // Find the lowest foot which is contacting the floor
    if (current_state.left_foot.state.compare("Contact") == 0 || current_state.right_foot.state.compare("Contact") == 0) {
      if (current_state.left_foot.state.compare("Contact") == 0) {
        lowest_foot = current_state.left_foot;
        lowest_foot_bottom_left_y = current_state.left_foot.top_left_y - current_state.left_foot.size_y;
        if (current_state.right_foot.state.compare("Contact") == 0) {
          if ((current_state.left_foot.top_left_y - current_state.left_foot.size_y) < current_state.right_foot.top_left_y - current_state.right_foot.size_y) {
            // Right foot is lower!
            lowest_foot = current_state.right_foot;
            lowest_foot_bottom_left_y = current_state.right_foot.top_left_y - current_state.right_foot.size_y;
          }
        }
      } else if (current_state.right_foot.state.compare("Contact") == 0) {
        lowest_foot = current_state.right_foot;
        lowest_foot_bottom_left_y = current_state.right_foot.top_left_y - current_state.right_foot.size_y;
      } 
    } else {
      // No foot on floor!
      // Change State to falling
      // Move Avatar down
      int avatar_y = static_cast<int>(m_avatar->GetPosition().y - 2);
      m_avatar->SetPosition(D3DXVECTOR3(m_avatar->GetPosition().x, static_cast<float>(avatar_y), -2)); // Middleground Z Space is -1
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
      int avatar_y = static_cast<int>(m_avatar->GetPosition().y - 2);
      m_avatar->SetPosition(D3DXVECTOR3(m_avatar->GetPosition().x, static_cast<float>(avatar_y), -2)); // Middleground Z Space is -1
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
}

void Avatar_Controller::Load_Tilesets(std::wstring wtileset_path) {
  // Running
  m_running_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Running_Animation_Tileset_1_0.txt");
  Load_Tileset_Metadata(m_running_metadata_file_path, m_running_metadata);

  // Walking
  m_walking_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Walking_Animation_Tileset_1_4.txt");
  Load_Tileset_Metadata(m_walking_metadata_file_path, m_walking_metadata);

  // Standing
  m_standing_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Standing_Animation_Tileset_1_0.txt");
  Load_Tileset_Metadata(m_standing_metadata_file_path, m_standing_metadata);
}

//------------------------------------------------------------------------------
void Avatar_Controller::Load_Tileset_Metadata(std::string metadata_file, Avatar_Controller::Animation_Tileset_Metadata &out_metadata) {
  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_file.c_str(), "r") != 0) {
    throw Tunnelour::Exceptions::init_error("Open Tileset Metadata Failed!");
  }

  // obtain file size:
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  char * token;
  char * next_token;

  char line[255];
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_Name") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.name = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_Type") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.type = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_FileName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.filename = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.top_left_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.top_left_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.size_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.size_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_NumOfSubSets") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      out_metadata.number_of_subsets = atoi(token);
    }
  }

  if (out_metadata.number_of_subsets != 0) {
    for (int i = 0; i <  out_metadata.number_of_subsets; i++) {
      Animation_Subset temp_subset;

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Name") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.name = token;
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Type") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.type = token;
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.top_left_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.top_left_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.size_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.size_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Tile_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.tile_size_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Tile_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.tile_size_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_NumOfFrames") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.number_of_frames = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_FramesPerSecond") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.frames_per_second = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Repeating") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          if (token == "Y") {
            temp_subset.is_repeatable = true;
          } else {
            temp_subset.is_repeatable = false;
          }
        }
      }

      for (int i = 0; i <  temp_subset.number_of_frames; i++) {
        Frame_Metadata temp_frame_metadata;

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_ID") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.id = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Right_Foot_TopLeftX") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.right_foot_top_left_x = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Right_Foot_TopLeftY") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.right_foot_top_left_y = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Right_Foot_SizeX") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.right_foot_size_x = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Right_Foot_SizeY") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.right_foot_size_y = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Right_Foot_State") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.right_foot_state = token;
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Left_Foot_TopLeftX") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.left_foot_top_left_x = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Left_Foot_TopLeftY") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.left_foot_top_left_y = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Left_Foot_SizeX") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.left_foot_size_x = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Left_Foot_SizeY") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.left_foot_size_y = atoi(token);
          }
        }

        fgets(line, 225, pFile);
        if (line != NULL) {
          token = strtok_s(line, " ", &next_token);
          if (strcmp(token, "Frame_Left_Foot_State") == 0)   {
            token = strtok_s(NULL, " =\"", &next_token);
            temp_frame_metadata.left_foot_state = token;
          }
        }

        temp_subset.frames.push_back(temp_frame_metadata);
      }
    
      out_metadata.subsets.push_back(temp_subset);
    }
  }
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

  bool stop = false;
  if (TileB->GetPosition().x == 14) {
    stop = true;
      if (TileA->GetPosition().x == 32) {
        stop = false;
      }
  }

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

/* BACKUP

//------------------------------------------------------------------------------
bool Avatar_Controller::DoTheseTilesXCollide(Tunnelour::Frame_Component* TileA, Tunnelour::Frame_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.

  bool stop = false;
  if (TileB->GetPosition().x == 14) {
    stop = true;
      if (TileA->GetPosition().x == 32) {
        stop = false;
      }
  }

  float a_tile_left, a_tile_right;
  a_tile_left = TileA->GetPosition().x - static_cast<float>(TileA->GetSize()->x / 2);
  a_tile_right = TileA->GetPosition().x + static_cast<float>(TileA->GetSize()->x / 2);

  float b_tile_left, b_tile_right;
  b_tile_left = TileB->GetPosition().x - static_cast<float>(TileB->GetSize()->x / 2);
  b_tile_right = TileB->GetPosition().x + static_cast<float>(TileB->GetSize()->x / 2);

  if ((a_tile_left == b_tile_left) || (a_tile_left > b_tile_left && a_tile_left < b_tile_right)) {
    // Horrizontal Collision
    return true;
  }

  if ((a_tile_right == b_tile_right) || (a_tile_right > b_tile_left && a_tile_right < b_tile_right)) {
    // Horrizontal Collision
    return true;
  }

  return false;
}

*/

} // Tunnelour