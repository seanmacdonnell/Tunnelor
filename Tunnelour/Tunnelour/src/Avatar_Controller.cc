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
  if (!m_has_avatar_been_generated) {
    Avatar_Controller_Mutator mutator;
    m_model->Apply(&mutator);
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
      m_avatar->SetState(new_state);
    } else {
      // Continue Same State if its time
      // Increment Index
      Update_Timer();
      if (m_animation_tick) {
        // Increment the animation
        // Change State
        unsigned int state_index = current_state.state_index;
        state_index++;
        if (state_index > (new_animation_subset.number_of_tiles - 1)) { state_index = 0; }
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

        m_avatar->SetPosition(position);
        m_animation_tick = false;
        m_avatar->SetState(new_state);
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
  state.state_index = 1;
  m_avatar->SetState(state);
  m_avatar->Init();
}

//------------------------------------------------------------------------------
void Avatar_Controller::Place_Avatar_Tile(Avatar_Controller_Mutator *mutator) {
  m_avatar->SetPosition(D3DXVECTOR3(0, 0, -2)); // Middleground Z Space is -1
  if (mutator->FoundBorderTiles()) {
    // Cull the border tiles which are not within the x range of the avatar square
    std::list<Tunnelour::Bitmap_Component*> horizontal_collision_border_tiles;
    std::list<Tunnelour::Bitmap_Component*> border_tiles = mutator->GetBorderTiles();
    std::list<Tunnelour::Bitmap_Component*>::iterator it;
    for (it = border_tiles.begin(); it != border_tiles.end(); it++) {
      if (DoTheseTilesXCollide(*it, m_avatar)) {
        horizontal_collision_border_tiles.push_back(*it);
      }
    }

    if (horizontal_collision_border_tiles.empty()) { return; }
    // Check to see if the avatar is on a border
    Tunnelour::Bitmap_Component* tile = horizontal_collision_border_tiles.front();
    int tile_top = static_cast<int>(tile->GetPosition().y + tile->GetSize()->y / 2);
    int avatar_bottom = static_cast<int>(m_avatar->GetPosition().y - m_avatar->GetSize()->y / 2);
    if (tile_top != avatar_bottom) {
      // if not
      // Move the avatar down so he is on the floor
      avatar_bottom = static_cast<int>(tile_top + m_avatar->GetSize()->y / 2);
      m_avatar->SetPosition(D3DXVECTOR3(0.0, static_cast<float>(avatar_bottom), -2.0)); // Middleground Z Space is -1
    }
  }
}

void Avatar_Controller::Load_Tilesets(std::wstring wtileset_path) {
  // Running
  m_running_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Running_Animation_Tileset_1_0.txt");
  Load_Tileset_Metadata(m_running_metadata_file_path, m_running_metadata);

  // Walking
  m_walking_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Walking_Animation_Tileset_1_0.txt");
  Load_Tileset_Metadata(m_walking_metadata_file_path, m_walking_metadata);

  // Standing
  m_standing_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Charlie_Standing_Animation_Tileset_0_3.txt");
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
        if (strcmp(token, "SubSet_NumOfTiles") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_subset.number_of_tiles = atoi(token);
        }
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

	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

 if (m_frameTime >= 41.6) {
   m_startTime = currentTime;
   m_frameTime = static_cast<float>(m_frameTime - 41.6);
   m_animation_tick = true;
 }
}

//------------------------------------------------------------------------------
bool Avatar_Controller::DoTheseTilesXCollide(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.
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

} // Tunnelour