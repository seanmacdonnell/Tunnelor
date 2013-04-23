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

#include "Background_Controller.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include "Exceptions.h"
#include "Background_Controller_Mutator.h"
#include "String_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Background_Controller::Background_Controller() : Controller() {
  m_bitmap = NULL;
  std::srand(static_cast<unsigned int>(std::time(0)));
  m_game_settings = 0;
  m_camera = 0;
  m_has_init_background_been_generated = false;
  m_is_debug_mode = false;
  m_tileset_filename = L"";
}

//------------------------------------------------------------------------------
Background_Controller::~Background_Controller() {
}

//------------------------------------------------------------------------------
void Background_Controller::Init(Tunnelour::Component_Composite * const model) {
    Tunnelour::Controller::Init(model);
}

//------------------------------------------------------------------------------
void Background_Controller::Run() {
  // Generate an initial Random Tile Background
  if (!m_has_init_background_been_generated) {
    // Get game settings component from the model with the Mutator.
    Tunnelour::Background_Controller_Mutator mutator;
    m_model->Apply(&mutator);
    if (mutator.FoundGameSettings() && mutator.FoundCamera())  {
      m_game_settings = mutator.GetGameSettings();
      m_is_debug_mode = m_game_settings->IsDebugMode(); 
      if (m_is_debug_mode) {
        m_tileset_filename = L"Debug_Tileset_0_4.txt";
      } else {
        m_tileset_filename = L"Dirt_Tileset_5.txt";
      }

      m_camera = mutator.GetCamera();

      // Load the Tileset Data
      Load_Tilset_Metadata();

      int top_left_window_x = static_cast<int>((m_game_settings->GetResolution().x/2) * -1);
      int top_left_window_y = static_cast<int>(m_game_settings->GetResolution().y/2);
      m_background_left = top_left_window_x;
      m_background_top = top_left_window_y;
      int current_x = top_left_window_x;
      int current_y = top_left_window_y;
      Tunnelour::Tile_Bitmap* tile;

      int base_tile_size = 128;
      int resised_tile_size = static_cast<int>(base_tile_size * m_game_settings->GetTileMultiplicationFactor());

      int number_of_y_tiles = 0;
      std::div_t div_y_result =  div(static_cast<int>(m_game_settings->GetResolution().y), resised_tile_size);
      if (div_y_result.rem != 0) {
        number_of_y_tiles = div_y_result.quot + 1;
      } else {
        number_of_y_tiles = div_y_result.quot;
      }

      //Calculate number of x tiles
      int number_of_x_tiles = 0;
      std::div_t div_x_result = div(static_cast<int>(m_game_settings->GetResolution().x), resised_tile_size);
      if (div_x_result.rem != 0) {
        number_of_x_tiles = div_x_result.quot + 1;
      } else {
        number_of_x_tiles = div_x_result.quot;
      }

      for (int y = 0; y < number_of_y_tiles ; y++) {
        std::vector<Tunnelour::Tile_Bitmap*> tile_line;
        for (int x = 0; x < number_of_x_tiles ; x++) {
           tile = Create_Tile(base_tile_size);
           tile->SetPosition(D3DXVECTOR3(current_x + (tile->GetSize()->x/2),
                                             current_y - (tile->GetSize()->y/2),
                                             0)); // Background Z Space is 0
           current_x += static_cast<int>(tile->GetSize()->x);
           tile_line.push_back(tile);
           m_background_tiles.push_back(tile);
        }
        current_y -= static_cast<int>(tile->GetSize()->y);
        current_x = static_cast<int>(top_left_window_x);
      }

      // Add tiles to the model
      for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = m_background_tiles.begin(); tile != m_background_tiles.end(); ++tile) {
          if (tile + 1 == m_background_tiles.end()) {
            //this is the last tile in the background model
            m_background_bottom = (*tile)->GetPosition().y + ((*tile)->GetSize()->y /2);
            m_background_right = (*tile)->GetPosition().x + ((*tile)->GetSize()->x /2);
          }
          m_model->Add(*tile);
      }

      m_has_init_background_been_generated = true;
    }
  } else {
    // If Camera is over an area with no tiles
    int m_camera_top, m_camera_bottom, m_camera_left, m_camera_right;
    m_camera_top = m_camera->GetPosition().y + (m_game_settings->GetResolution().y / 2);
    m_camera_bottom = m_camera->GetPosition().y - (m_game_settings->GetResolution().y / 2);
    m_camera_left = m_camera->GetPosition().x - (m_game_settings->GetResolution().x / 2);
    m_camera_right = m_camera->GetPosition().x + (m_game_settings->GetResolution().x / 2);

    if (m_camera_top > m_background_top) {
     throw Tunnelour::Exceptions::unfinished_error("Up Generation not finished!");
    }

    if (m_camera_bottom < m_background_bottom) {
      //Fill Down
      unsigned int number_of_x_tiles = 0;
      std::div_t div_x_result = div(m_background_right - m_background_left, 128);
      if (div_x_result.rem != 0) {
        number_of_x_tiles = div_x_result.quot + 1;
      } else {
        number_of_x_tiles = div_x_result.quot;
      }

      while (m_camera_bottom < m_background_bottom) {
        for (int x = 0; x < number_of_x_tiles; x++) {
          Tunnelour::Tile_Bitmap* tile = Create_Tile(128);
          D3DXVECTOR3 position = D3DXVECTOR3(m_background_left + (x * 128) + (128 / 2),
                                            m_background_bottom - (tile->GetSize()->y / 2),
                                            0);
          tile->SetPosition(position);
          m_model->Add(tile);
          m_background_tiles.push_back(tile);
        }
        m_background_bottom -= 128;
      }
    }

    if (m_camera_right > m_background_right) {
      //Fill right
      int number_of_y_tiles = 0;
      std::div_t div_y_result =  div(m_background_top - m_background_bottom, 128);
      if (div_y_result.rem != 0) {
        number_of_y_tiles = div_y_result.quot + 1;
      } else {
        number_of_y_tiles = div_y_result.quot;
      }

      while (m_camera_right > m_background_right) {
        for (int x = 0; x < number_of_y_tiles; x++) {
          Tunnelour::Tile_Bitmap* tile = Create_Tile(128);
          D3DXVECTOR3 position = D3DXVECTOR3(m_background_right + (tile->GetSize()->y / 2),
                                             m_background_top - (x * 128) - (128 / 2),
                                             0);
          tile->SetPosition(position);
          m_model->Add(tile);
          m_background_tiles.push_back(tile);
        }
        m_background_right += 128;
      }
    }

    if (m_camera_left < m_background_left) {
      //Fill left
      int number_of_y_tiles = 0;
      std::div_t div_y_result =  div(m_background_top - m_background_bottom, 128);
      if (div_y_result.rem != 0) {
        number_of_y_tiles = div_y_result.quot + 1;
      } else {
        number_of_y_tiles = div_y_result.quot;
      }

      while (m_camera_left < m_background_left) {
        for (int x = 0; x < number_of_y_tiles; x++) {
          Tunnelour::Tile_Bitmap* tile = Create_Tile(128);
          D3DXVECTOR3 position = D3DXVECTOR3(m_background_left - (tile->GetSize()->y / 2),
                                             m_background_top - (x * 128) - (128 / 2),
                                             0);
          tile->SetPosition(position);
          m_model->Add(tile);
          m_background_tiles.push_back(tile);
        }
        m_background_left -= 128;
      }
    }

    if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
      // Debug Mode has been activated or Deactivated
      m_is_debug_mode = m_game_settings->IsDebugMode();
      if (m_is_debug_mode) {
        m_tileset_filename = L"Debug_Tileset_0_4.txt";
        Load_Tilset_Metadata();
      } else {
        m_tileset_filename = L"Dirt_Tileset_5.txt";
        Load_Tilset_Metadata();
      }

      for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = m_background_tiles.begin(); tile != m_background_tiles.end(); ++tile) {
        Tileset middleground_tileset;
        std::list<Tileset>::iterator tileset;

        for (tileset = m_metadata.tilesets.begin(); tileset != m_metadata.tilesets.end(); tileset++) {
          if (tileset->type.compare("Background") == 0) {
            middleground_tileset = *tileset;
          }
        }

        Line middleground_line;
        std::list<Line>::iterator line;
        for (line = middleground_tileset.lines.begin(); line != middleground_tileset.lines.end(); line++) {
          if (line->tile_size_x == (*tile)->GetSize()->x && line->tile_size_y == (*tile)->GetSize()->y) {
            middleground_line = *line;
          }
        }

        std::wstring texture_path = m_game_settings->GetTilesetPath();
        texture_path += String_Helper::StringToWString(m_metadata.filename);
        (*tile)->GetTexture()->texture_path = texture_path;
        (*tile)->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_metadata.size_x),
                                                        static_cast<float>(m_metadata.size_y));

        int random_variable = 0;
        if (m_is_debug_mode) {
          random_variable = 0;
        } else {
          int random_variable = rand() % middleground_line.number_of_tiles;
        }

        (*tile)->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(random_variable*(middleground_line.tile_size_x) + static_cast<float>(middleground_line.top_left_x)),
                                                            static_cast<float>(middleground_line.top_left_y));

          
        (*tile)->GetTexture()->texture = 0;
        (*tile)->GetFrame()->vertex_buffer = 0;
        (*tile)->Init();
      }
    }

    // Fill up tiles in that area.
  }

  
  //m_is_finished = true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Background_Controller::Load_Tilset_Metadata() {
  FILE * pFile;
  int lSize;

  std::wstring wtileset_path = m_game_settings->GetTilesetPath();
  m_metadata_file_path = String_Helper::WStringToString(wtileset_path + m_tileset_filename);

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
      Tileset temp_tileset;

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Name") == 0)   {
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Type") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.type = token;
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.top_left_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_TopLeftY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.top_left_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeX") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.size_x = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_SizeY") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.size_y = atoi(token);
        }
      }

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_NumOfLines") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.number_of_lines = atoi(token);
        }
      }

      if (temp_tileset.number_of_lines != 0) {
        for (int i = 0; i < temp_tileset.number_of_lines; i++) {
          Line temp_line;

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_Name") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.line_number = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_TopLeftX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.top_left_x = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_TopLeftY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.top_left_y = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Tile_SizeX") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.tile_size_x = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Tile_SizeY") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.tile_size_y = atoi(token);
            }
          }

          fgets(line, 225, pFile);
          if (line != NULL) {
            token = strtok_s(line, " ", &next_token);
            if (strcmp(token, "Line_NumOfTiles") == 0)   {
              token = strtok_s(NULL, " =\"", &next_token);
              temp_line.number_of_tiles = atoi(token);
            }
          }
          temp_tileset.lines.push_back(temp_line);
        }
      }

      m_metadata.tilesets.push_back(temp_tileset);
    }
  }
}

//------------------------------------------------------------------------------
Tunnelour::Tile_Bitmap* Background_Controller::Create_Tile(int base_tile_size) {
  Tileset middleground_tileset;
  std::list<Tileset>::iterator tileset;
  for (tileset = m_metadata.tilesets.begin(); tileset != m_metadata.tilesets.end(); tileset++) {
    if (tileset->type.compare("Background") == 0) {
      middleground_tileset = *tileset;
    }
  }

  Line middleground_line;
  std::list<Line>::iterator line;
  for (line = middleground_tileset.lines.begin(); line != middleground_tileset.lines.end(); line++) {
    if (line->tile_size_x == base_tile_size && line->tile_size_y == base_tile_size) {
      middleground_line = *line;
    }
  }

  Tunnelour::Tile_Bitmap* tile = new Tunnelour::Tile_Bitmap();
  tile->SetPosition(D3DXVECTOR3(0, 0, 0));
  tile->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_metadata.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_metadata.size_x),
                                                 static_cast<float>(m_metadata.size_y));
  tile->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(middleground_line.tile_size_x),
                                              static_cast<float>(middleground_line.tile_size_y));

  int random_variable = 0;
  if (m_is_debug_mode) {
    random_variable = 0;
  } else {
    int random_variable = rand() % middleground_line.number_of_tiles;
  }

  tile->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(random_variable*(middleground_line.tile_size_x) + static_cast<float>(middleground_line.top_left_x)),
                                                      static_cast<float>(middleground_line.top_left_y));

  tile->SetSize(new D3DXVECTOR2(base_tile_size, base_tile_size));
  return tile;
}

}  // namespace Tunnelour
