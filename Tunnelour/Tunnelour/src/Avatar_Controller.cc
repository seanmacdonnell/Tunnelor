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
    // Get game settings component from the model with the Mutator.
    Avatar_Controller_Mutator mutator;
    m_model->Apply(&mutator);
    if (!mutator.FoundGameSettings())  {
      throw Tunnelour::Exceptions::run_error("Can't find a game settings component!");
    } else {
      m_game_settings = mutator.GetGameSettings();
    }

    // Load the Tileset Data
    Generate_Avatar_Tile();
    Place_Avatar_Tile(&mutator);
    m_model->Add(m_avatar);
    m_has_avatar_been_generated = true;
  }

  if (m_avatar->GetNextCommand() != "") {
    if (m_avatar->GetNextCommand().compare("DIK_RIGHT") == 0) {
      if (m_avatar->GetState().compare("Walking")) {
        // Start the waking animation from the begining
        Load_Tilset_Metadata(L"Charlie_Walking_Animation_Tileset_1_0.txt");
        Animation_Subset walkinging_animation_subset;
        for (std::list<Animation_Subset>::iterator tileset = m_metadata.subsets.begin(); tileset != m_metadata.subsets.end(); tileset++) {
          if (tileset->name.compare("Walking") == 0) {
            walkinging_animation_subset = *tileset;
          }
        }

        std::wstring texture_path = m_game_settings->GetTilesetPath();
        texture_path += String_Helper::StringToWString(m_metadata.filename);

        m_avatar->GetTexture()->texture_path = texture_path;
        m_avatar->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_metadata.size_x),
                                                           static_cast<float>(m_metadata.size_y));
        m_avatar->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(walkinging_animation_subset.tile_size_x),
                                                        static_cast<float>(walkinging_animation_subset.tile_size_y));
        m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(walkinging_animation_subset.top_left_x),
                                                                static_cast<float>(walkinging_animation_subset.top_left_y));
        m_avatar->SetSize(new D3DXVECTOR2(walkinging_animation_subset.tile_size_x, walkinging_animation_subset.tile_size_y));
        
        m_avatar->SetState(walkinging_animation_subset.name);
        m_avatar->SetStateIndex(0);
        D3DXVECTOR3 position = m_avatar->GetPosition(); 
        position.x += 1;
        m_avatar->SetPosition(position);
        m_avatar->SetScale(new D3DXVECTOR3(1.0f, 1.0f, 1.0f));
        m_avatar->SetLastCommand(m_avatar->GetNextCommand());
        m_avatar->SetNextCommand("");
        m_avatar->GetTexture()->texture = 0;
        m_avatar->GetFrame()->vertex_buffer = 0;
        m_avatar->SetDirection("RIGHT");
        m_avatar->Init();
      } else {
        Update_Timer();
        if (m_animation_tick) {
          // Increment the walking animation
          Animation_Subset walkinging_animation_subset;
          for (std::list<Animation_Subset>::iterator tileset = m_metadata.subsets.begin(); tileset != m_metadata.subsets.end(); tileset++) {
            if (tileset->name.compare("Walking") == 0) {
              walkinging_animation_subset = *tileset;
            }
          }

          unsigned int state_index = m_avatar->GetStateIndex();
          state_index++;
          if (state_index > (walkinging_animation_subset.number_of_tiles - 1)) { state_index = 0; }
          m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(walkinging_animation_subset.top_left_x + (state_index * walkinging_animation_subset.tile_size_x)),
                                                                  static_cast<float>(walkinging_animation_subset.top_left_y));
          D3DXVECTOR3 position = m_avatar->GetPosition(); 
          position.x += 6;
          m_avatar->SetPosition(position);
          m_avatar->SetLastCommand(m_avatar->GetNextCommand());
          m_avatar->SetNextCommand("");
          m_avatar->GetTexture()->texture = 0;
          m_avatar->GetFrame()->vertex_buffer = 0;
          m_avatar->SetDirection("RIGHT");
          m_avatar->Init();
          m_avatar->SetStateIndex(state_index);
          m_animation_tick = false;
        }
      }
    }
    if (m_avatar->GetNextCommand().compare("DIK_LEFT") == 0) {
      if (m_avatar->GetState().compare("Walking")) {
        // Start the waking animation from the begining
        Load_Tilset_Metadata(L"Charlie_Walking_Animation_Tileset_1_0.txt");
        Animation_Subset walkinging_animation_subset;
        for (std::list<Animation_Subset>::iterator tileset = m_metadata.subsets.begin(); tileset != m_metadata.subsets.end(); tileset++) {
          if (tileset->name.compare("Walking") == 0) {
            walkinging_animation_subset = *tileset;
          }
        }

        std::wstring texture_path = m_game_settings->GetTilesetPath();
        texture_path += String_Helper::StringToWString(m_metadata.filename);

        m_avatar->GetTexture()->texture_path = texture_path;
        m_avatar->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_metadata.size_x),
                                                           static_cast<float>(m_metadata.size_y));
        m_avatar->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(walkinging_animation_subset.tile_size_x),
                                                        static_cast<float>(walkinging_animation_subset.tile_size_y));
        m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(walkinging_animation_subset.top_left_x),
                                                                static_cast<float>(walkinging_animation_subset.top_left_y));
        m_avatar->SetSize(new D3DXVECTOR2(walkinging_animation_subset.tile_size_x, walkinging_animation_subset.tile_size_y));
        
        m_avatar->SetState(walkinging_animation_subset.name);
        m_avatar->SetStateIndex(0);
        D3DXVECTOR3 position = m_avatar->GetPosition(); 
        position.x -= 1;
        m_avatar->SetPosition(position);
        m_avatar->SetDirection("LEFT");
        m_avatar->SetScale(new D3DXVECTOR3(1.0f, 1.0f, 1.0f));
        m_avatar->SetLastCommand(m_avatar->GetNextCommand());
        m_avatar->SetNextCommand("");
        m_avatar->GetTexture()->texture = 0;
        m_avatar->GetFrame()->vertex_buffer = 0;
        m_avatar->Init();
      } else {
        Update_Timer();
        if (m_animation_tick) {
          // Increment the walking animation
          Animation_Subset walkinging_animation_subset;
          for (std::list<Animation_Subset>::iterator tileset = m_metadata.subsets.begin(); tileset != m_metadata.subsets.end(); tileset++) {
            if (tileset->name.compare("Walking") == 0) {
              walkinging_animation_subset = *tileset;
            }
          }

          unsigned int state_index = m_avatar->GetStateIndex();
          state_index++;
          if (state_index > (walkinging_animation_subset.number_of_tiles - 1)) { state_index = 0; }
          m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(walkinging_animation_subset.top_left_x + (state_index * walkinging_animation_subset.tile_size_x)),
                                                                  static_cast<float>(walkinging_animation_subset.top_left_y));
          D3DXVECTOR3 position = m_avatar->GetPosition(); 
          position.x -= 6;
          m_avatar->SetDirection("LEFT");
          m_avatar->SetPosition(position);
          m_avatar->SetLastCommand(m_avatar->GetNextCommand());
          m_avatar->SetNextCommand("");
          m_avatar->GetTexture()->texture = 0;
          m_avatar->GetFrame()->vertex_buffer = 0;
          m_avatar->Init();
          m_avatar->SetStateIndex(state_index);
          m_animation_tick = false;
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
void Avatar_Controller::Generate_Avatar_Tile() {
  Load_Tilset_Metadata(L"Charlie_Standing_Animation_Tileset_0_3.txt");
  Animation_Subset standing_animation_subset;
  for (std::list<Animation_Subset>::iterator tileset = m_metadata.subsets.begin(); tileset != m_metadata.subsets.end(); tileset++) {
    if (tileset->name.compare("Standing") == 0) {
      standing_animation_subset = *tileset;
    }
  }

  m_avatar = new Tunnelour::Avatar_Component();
  m_avatar->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_metadata.filename);
  m_avatar->GetTexture()->texture_path = texture_path;
  m_avatar->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_metadata.size_x),
                                                     static_cast<float>(m_metadata.size_y));
  m_avatar->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(standing_animation_subset.tile_size_x),
                                                  static_cast<float>(standing_animation_subset.tile_size_y));
  m_avatar->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(standing_animation_subset.top_left_x),
                                                          static_cast<float>(standing_animation_subset.top_left_y));
  m_avatar->SetSize(new D3DXVECTOR2(standing_animation_subset.tile_size_x, standing_animation_subset.tile_size_y));

  m_avatar->SetState(standing_animation_subset.name);
  m_avatar->SetStateIndex(1);
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
    int tile_top = tile->GetPosition().y + static_cast<float>(tile->GetSize()->y / 2);
    int avatar_bottom = m_avatar->GetPosition().y - static_cast<float>(m_avatar->GetSize()->y / 2);
    if (tile_top != avatar_bottom) {
      // if not
      // Move the avatar down so he is on the floor
      avatar_bottom = tile_top + static_cast<float>(m_avatar->GetSize()->y / 2);
      m_avatar->SetPosition(D3DXVECTOR3(0, avatar_bottom, -2)); // Middleground Z Space is -1
    }
  }
}

//------------------------------------------------------------------------------
void Avatar_Controller::Load_Tilset_Metadata(std::wstring metadata_file) {
  FILE * pFile;
  int lSize;

  std::wstring wtileset_path = m_game_settings->GetTilesetPath();
  m_metadata_file_path = String_Helper::WStringToString(wtileset_path + metadata_file);

  // Open Font File as a text file
  if (fopen_s(&pFile, m_metadata_file_path.c_str(), "r") != 0) {
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
      m_metadata.name = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_Type") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.type = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_FileName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.filename = token;
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.top_left_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_TopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.top_left_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.size_x = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_SizeY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.size_y = atoi(token);
    }
  }

  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Tileset_NumOfSubSets") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      m_metadata.number_of_subsets = atoi(token);
    }
  }

  if (m_metadata.number_of_subsets != 0) {
    for (int i = 0; i <  m_metadata.number_of_subsets; i++) {
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

      m_metadata.subsets.push_back(temp_subset);
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

	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

 if (m_frameTime >= 62.5) {
   m_startTime = currentTime;
   m_frameTime = m_frameTime - 62.5;
   m_animation_tick = true;
 }
}

//------------------------------------------------------------------------------
bool Avatar_Controller::DoTheseTilesXCollide(Tunnelour::Bitmap_Component* TileA, Tunnelour::Bitmap_Component* TileB) {
  // At least one vertex in TileA is contained in the TileB.
  float a_tile_left, a_tile_right, a_tile_top, a_tile_bottom;
  a_tile_left = TileA->GetPosition().x - static_cast<float>(TileA->GetSize()->x / 2);
  a_tile_right = TileA->GetPosition().x + static_cast<float>(TileA->GetSize()->x / 2);

  float b_tile_left, b_tile_right, b_tile_top, b_tile_bottom;
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