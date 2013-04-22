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

#include "Middleground_Controller.h"
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
Middleground_Controller::Middleground_Controller() : Controller() {
  std::srand(static_cast<unsigned int>(std::time(0)));
  m_game_settings = 0;
  m_has_init_middleground_been_generated = false;
  m_has_init_tunnel_been_generated = false;
  m_tunnel_x_size = (128+64);
  m_camera = 0;
  m_tileset_filename = L"";
}

//------------------------------------------------------------------------------
Middleground_Controller::~Middleground_Controller() {
}

//------------------------------------------------------------------------------
void Middleground_Controller::Init(Tunnelour::Component_Composite * const model) {
    Tunnelour::Controller::Init(model);
}

//------------------------------------------------------------------------------
void Middleground_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  Tunnelour::Middleground_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.FoundGameSettings() && mutator.FoundCamera() )  {
    if (!m_has_init_tunnel_been_generated || !m_has_init_middleground_been_generated) {
      m_game_settings = mutator.GetGameSettings();
      m_camera = mutator.GetCamera();
      
      if (m_game_settings->IsDebugMode()) {
        m_tileset_filename = L"Debug_Tileset_0_4.txt";
      } else {
        m_tileset_filename = L"Dirt_Tileset_5.txt";
      }
      Load_Tilset_Metadata();

      Tile_Tunnel();

      Tile_Middleground();
    } else {
    // If Camera is over an area with no tiles
    int m_camera_top, m_camera_bottom, m_camera_left, m_camera_right;
    m_camera_top = m_camera->GetPosition().y + (m_game_settings->GetResolution().y / 2);
    m_camera_bottom = m_camera->GetPosition().y - (m_game_settings->GetResolution().y / 2);
    m_camera_left = m_camera->GetPosition().x - (m_game_settings->GetResolution().x / 2);
    m_camera_right = m_camera->GetPosition().x + (m_game_settings->GetResolution().x / 2);
    /*
    if (m_camera_top > m_middleground_top) {
       throw Tunnelour::Exceptions::unfinished_error("Up Generation not finished!");
    }

    if (m_camera_bottom < m_middleground_bottom) {
      throw Tunnelour::Exceptions::unfinished_error("Up Generation not finished!");
    }
    */
    if (m_camera_right > m_middleground_right) {
      //Fill right
      std::vector<Tunnelour::Tile_Bitmap*> new_tunnel_tiles;
      while (m_camera_right > m_middleground_right) {
        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tunnel_edge_tile = m_right_edge_tiles.begin(); tunnel_edge_tile !=  m_right_edge_tiles.end(); tunnel_edge_tile++) {
          int current_tile_right = (*tunnel_edge_tile)->GetPosition().x  + ((*tunnel_edge_tile)->GetSize()->x / 2);
          while (m_camera_right > current_tile_right) {
            Tunnelour::Tile_Bitmap* tile = Create_Tile((*tunnel_edge_tile)->GetSize()->x);
            D3DXVECTOR3 position = D3DXVECTOR3((*tunnel_edge_tile)->GetPosition().x + tile->GetSize()->x,
                                               (*tunnel_edge_tile)->GetPosition().y,
                                               (*tunnel_edge_tile)->GetPosition().z);
            tile->GetTexture()->transparency = 0.0f; //TUNNEL
            tile->SetPosition(position);
            m_model->Add(tile);
            m_tunnel_tiles.push_back(tile);
            new_tunnel_tiles.push_back(tile);
            *tunnel_edge_tile = tile;
            current_tile_right += (tile->GetSize()->x / 2);
            if (tunnel_edge_tile == m_right_edge_tiles.begin()) {
              tile->Set_Is_Top_Edge(true);
            } else if ((tunnel_edge_tile + 1) == m_right_edge_tiles.end()) {
              tile->Set_Is_Bottom_Edge(true);
            }
          }
        }
        Tunnelour::Tile_Bitmap* smallest_tile = 0;
        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tunnel_edge_tile = m_right_edge_tiles.begin(); tunnel_edge_tile !=  m_right_edge_tiles.end(); tunnel_edge_tile++) {
          // store the smallest number as the new middleground right.
          if (smallest_tile == 0) {
            smallest_tile = *tunnel_edge_tile;
          } else if (smallest_tile->GetSize()->x > (*tunnel_edge_tile)->GetSize()->x) {
            smallest_tile = (*tunnel_edge_tile);
          }
        }
        m_middleground_right += smallest_tile->GetSize()->x;

        std::vector<Tunnelour::Tile_Bitmap*> new_middleground_tiles;

        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tunnel_tile = new_tunnel_tiles.begin(); tunnel_tile != new_tunnel_tiles.end(); tunnel_tile++) {
          std::vector<Tunnelour::Tile_Bitmap*> tiles;
          if ((*tunnel_tile)->Is_Top_Edge()) {
            // produce tiles upwards of the same size until out of camera view
            tiles = GenerateTilesUpwards(*tunnel_tile);
          } else if ((*tunnel_tile)->Is_Bottom_Edge()) {
            // produce tiles upwards of the same size until out of camera view
            tiles = GenerateTilesDownwards(*tunnel_tile);
          } 

          for (std::vector<Tunnelour::Tile_Bitmap*>::iterator middleground_tile = tiles.begin(); middleground_tile != tiles.end(); ++middleground_tile) {
            new_middleground_tiles.push_back(*middleground_tile);
          }
        }

        // Add tiles to Model
        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = new_middleground_tiles.begin(); tile != new_middleground_tiles.end(); ++tile) {
          m_model->Add(*tile);
          m_middleground_tiles.push_back(*tile);
        }
      }
    }

    if (m_camera_left < m_middleground_left) {
      //Fill left
      std::vector<Tunnelour::Tile_Bitmap*> new_tunnel_tiles;
      while (m_camera_left < m_middleground_left) {
        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tunnel_edge_tile = m_left_edge_tiles.begin(); tunnel_edge_tile !=  m_left_edge_tiles.end(); tunnel_edge_tile++) {
          int current_tile_left = (*tunnel_edge_tile)->GetPosition().x  - ((*tunnel_edge_tile)->GetSize()->x / 2);
          while (m_camera_left < current_tile_left) {
            Tunnelour::Tile_Bitmap* tile = Create_Tile((*tunnel_edge_tile)->GetSize()->x);
            D3DXVECTOR3 position = D3DXVECTOR3((*tunnel_edge_tile)->GetPosition().x - tile->GetSize()->x,
                                               (*tunnel_edge_tile)->GetPosition().y,
                                               (*tunnel_edge_tile)->GetPosition().z);
            tile->GetTexture()->transparency = 0.0f; //TUNNEL
            tile->SetPosition(position);
            m_model->Add(tile);
            m_tunnel_tiles.push_back(tile);
            new_tunnel_tiles.push_back(tile);
            *tunnel_edge_tile = tile;
            current_tile_left -= (tile->GetSize()->x / 2);
            if (tunnel_edge_tile == m_left_edge_tiles.begin()) {
              tile->Set_Is_Top_Edge(true);
            } else if ((tunnel_edge_tile + 1) == m_left_edge_tiles.end()) {
              tile->Set_Is_Bottom_Edge(true);
            }
          }
        }
        Tunnelour::Tile_Bitmap* smallest_tile = 0;
        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tunnel_edge_tile = m_left_edge_tiles.begin(); tunnel_edge_tile !=  m_left_edge_tiles.end(); tunnel_edge_tile++) {
          // store the smallest number as the new middleground right.
          if (smallest_tile == 0) {
            smallest_tile = *tunnel_edge_tile;
          } else if (smallest_tile->GetSize()->x > (*tunnel_edge_tile)->GetSize()->x) {
            smallest_tile = (*tunnel_edge_tile);
          }
        }
        m_middleground_left -= smallest_tile->GetSize()->x;

        std::vector<Tunnelour::Tile_Bitmap*> new_middleground_tiles;

        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tunnel_tile = new_tunnel_tiles.begin(); tunnel_tile != new_tunnel_tiles.end(); tunnel_tile++) {
          std::vector<Tunnelour::Tile_Bitmap*> tiles;
          if ((*tunnel_tile)->Is_Top_Edge()) {
            // produce tiles upwards of the same size until out of camera view
            tiles = GenerateTilesUpwards(*tunnel_tile);
          } else if ((*tunnel_tile)->Is_Bottom_Edge()) {
            // produce tiles upwards of the same size until out of camera view
            tiles = GenerateTilesDownwards(*tunnel_tile);
          } 

          for (std::vector<Tunnelour::Tile_Bitmap*>::iterator middleground_tile = tiles.begin(); middleground_tile != tiles.end(); ++middleground_tile) {
            new_middleground_tiles.push_back(*middleground_tile);
          }
        }

        // Add tiles to Model
        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = new_middleground_tiles.begin(); tile != new_middleground_tiles.end(); ++tile) {
          m_model->Add(*tile);
          m_middleground_tiles.push_back(*tile);
        }
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
void Middleground_Controller::Tile_Tunnel() {
  if (!m_has_init_tunnel_been_generated) {
    // Get game settings component from the model with the Mutator.
    Tunnelour::Middleground_Controller_Mutator mutator;

    m_model->Apply(&mutator);
    if (!mutator.FoundGameSettings())  {
      throw Tunnelour::Exceptions::run_error("Can't find a game settings component!");
    } else {
      m_game_settings = mutator.GetGameSettings();
    }

    // Load the Tileset Data
    Load_Tilset_Metadata();

    int tunnel_start_x = static_cast<int>((m_game_settings->GetResolution().x/2) * -1);
    int tunnel_start_y = 128;
    m_middleground_left = tunnel_start_x;
    m_middleground_top = m_middleground_top;
    int current_x = tunnel_start_x;
    int current_y = tunnel_start_y;
    Tunnelour::Tile_Bitmap* tile;

    int number_of_128x128_y_tiles = 0;
    int number_of_64x64_y_tiles = 0;
    int number_of_32x32_y_tiles = 0;
    int number_of_16x16_y_tiles = 0;
    int number_of_8x8_y_tiles = 0;
    int number_of_4x4_y_tiles = 0;
    int number_of_2x2_y_tiles = 0;
    int number_of_1x1_y_tiles = 0;

    div_t div_y_result =  div(m_tunnel_x_size, 128);
    number_of_128x128_y_tiles = div_y_result.quot;
    if (div_y_result.rem != 0) {
      div_y_result =  div(div_y_result.rem, 64);
      number_of_64x64_y_tiles = div_y_result.quot;
      if (div_y_result.rem != 0) {
        div_y_result =  div(div_y_result.rem, 32);
        number_of_32x32_y_tiles = div_y_result.quot;
        if (div_y_result.rem != 0) {
          div_y_result =  div(div_y_result.rem, 32);
          number_of_16x16_y_tiles = div_y_result.quot;
          if (div_y_result.rem != 0) {
            div_y_result =  div(div_y_result.rem, 16);
            number_of_16x16_y_tiles = div_y_result.quot;
            if (div_y_result.rem != 0) {
              div_y_result =  div(div_y_result.rem, 8);
              number_of_8x8_y_tiles = div_y_result.quot;
              if (div_y_result.rem != 0) {
                div_y_result =  div(div_y_result.rem, 4);
                number_of_4x4_y_tiles = div_y_result.quot;
                if (div_y_result.rem != 0) {
                  div_y_result =  div(div_y_result.rem, 2);
                  number_of_2x2_y_tiles = div_y_result.quot;
                  if  (div_y_result.rem != 0) {
                    number_of_1x1_y_tiles = div_y_result.rem;
                  }
                }
              }
            }
          }
        }
      }
    }

    int number_of_y_tiles = number_of_128x128_y_tiles + 
                            number_of_64x64_y_tiles +
                            number_of_32x32_y_tiles +
                            number_of_16x16_y_tiles +
                            number_of_8x8_y_tiles +
                            number_of_4x4_y_tiles +
                            number_of_2x2_y_tiles +
                            number_of_1x1_y_tiles;

    for (int y = 0; y < number_of_y_tiles ; y++) {
      int base_tile_size = 0;
      if (number_of_128x128_y_tiles != 0) {
        base_tile_size = 128;
        number_of_128x128_y_tiles--;
      } else if (number_of_64x64_y_tiles != 0) {
        base_tile_size = 64;
        number_of_64x64_y_tiles--;
      } else if (number_of_32x32_y_tiles != 0) {
        base_tile_size = 32;
        number_of_32x32_y_tiles--;
      } else if (number_of_16x16_y_tiles != 0) {
        base_tile_size = 16;
        number_of_16x16_y_tiles--;
      } else if (number_of_8x8_y_tiles != 0) {
        base_tile_size = 8;
        number_of_8x8_y_tiles--;
      } else if (number_of_4x4_y_tiles != 0) {
        base_tile_size = 4;
        number_of_4x4_y_tiles--;
      } else if (number_of_2x2_y_tiles != 0) {
        base_tile_size = 2;
        number_of_2x2_y_tiles--;
      } else if (number_of_1x1_y_tiles != 0) {
        base_tile_size = 1;
        number_of_1x1_y_tiles--;
      }

      int resised_tile_size = static_cast<int>(base_tile_size * m_game_settings->GetTileMultiplicationFactor());

      //Calculate number of x tiles
      int number_of_x_tiles = 0;
      std::div_t div_x_result = div(static_cast<int>(m_game_settings->GetResolution().x), resised_tile_size);
      if (div_x_result.rem != 0) {
        number_of_x_tiles = div_x_result.quot + 1; //Add Another Tile if it doesne't fit
      } else {
        number_of_x_tiles = div_x_result.quot;
      }

      std::vector<Tunnelour::Tile_Bitmap*> tile_line;
      for (int x = 0; x < number_of_x_tiles ; x++) {
         tile = Create_Tile(base_tile_size);
         if (y == 0) {
           tile->Set_Is_Top_Edge(true);
         } else if (y == (number_of_y_tiles - 1)) {
           tile->Set_Is_Bottom_Edge(true);
         }
         tile->SetPosition(D3DXVECTOR3(current_x + (tile->GetSize()->x/2),
                                           current_y - (tile->GetSize()->y/2),
                                           -1)); // Middleground Z Space is -1
         tile->GetTexture()->transparency = 0.0f; //TUNNEL
         current_x += static_cast<int>(tile->GetSize()->x);
         m_tunnel_tiles.push_back(tile);
         if (x == 0) {
           m_left_edge_tiles.push_back(tile);
           m_middleground_left = tile->GetPosition().x - (tile->GetSize()->x / 2);
         }
         if (x == (number_of_x_tiles - 1)) {
           m_right_edge_tiles.push_back(tile);
           m_middleground_right = tile->GetPosition().x + (tile->GetSize()->x / 2);
         }
      }
      current_y -= static_cast<int>(tile->GetSize()->y);
      current_x = static_cast<int>(tunnel_start_x);
    }

    // Add tiles to the model
    for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = m_tunnel_tiles.begin(); tile != m_tunnel_tiles.end(); tile++) {
      m_model->Add(*tile);
    }

    m_has_init_tunnel_been_generated = true;
  }
}

//------------------------------------------------------------------------------
void Middleground_Controller::Tile_Middleground() {
  // Generate an initial Random Tile Middleground Spanning the Screen
  if (!m_has_init_middleground_been_generated) {
    std::vector<Tunnelour::Tile_Bitmap*> middleground_tiles;

    for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tunnel_tile = m_tunnel_tiles.begin(); tunnel_tile != m_tunnel_tiles.end(); tunnel_tile++) {
      std::vector<Tunnelour::Tile_Bitmap*> tiles;
      if ((*tunnel_tile)->Is_Top_Edge()) {
        // produce tiles upwards of the same size until out of camera view
        tiles = GenerateTilesUpwards(*tunnel_tile);
      } else if ((*tunnel_tile)->Is_Bottom_Edge()) {
        // produce tiles upwards of the same size until out of camera view
        tiles = GenerateTilesDownwards(*tunnel_tile);
      } 

      for (std::vector<Tunnelour::Tile_Bitmap*>::iterator middleground_tile = tiles.begin(); middleground_tile != tiles.end(); ++middleground_tile) {
        middleground_tiles.push_back(*middleground_tile);
      }
    }

    // Add tiles to Model
    for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = middleground_tiles.begin(); tile != middleground_tiles.end(); ++tile) {
      m_model->Add(*tile);
      m_middleground_tiles.push_back(*tile);
    }

    m_has_init_middleground_been_generated = true;
  }
}

//------------------------------------------------------------------------------
std::vector<Tunnelour::Tile_Bitmap*> Middleground_Controller::GenerateTilesUpwards(Tunnelour::Tile_Bitmap* from_tile) {
  std::vector<Tunnelour::Tile_Bitmap*> new_tiles;

  int y_boundary_top = m_camera->GetPosition().y + (m_game_settings->GetResolution().y / 2);
  int y_current = (from_tile->GetPosition().y + (from_tile->GetSize()->y / 2));
  Tunnelour::Tile_Bitmap* tile = 0;
  while (y_current < y_boundary_top) {
    tile = Create_Tile(static_cast<int>(from_tile->GetSize()->y));
    D3DXVECTOR3 position = from_tile->GetPosition();
    position.y = y_current + (tile->GetSize()->y / 2);
    tile->SetPosition(position);
    y_current = (tile->GetPosition().y + (tile->GetSize()->y / 2));
    new_tiles.push_back(tile);
  }
  m_middleground_top = tile->GetPosition().x;

  return new_tiles;
}

//------------------------------------------------------------------------------
std::vector<Tunnelour::Tile_Bitmap*> Middleground_Controller::GenerateTilesDownwards(Tunnelour::Tile_Bitmap* from_tile) {
  std::vector<Tunnelour::Tile_Bitmap*> new_tiles;

  int y_boundary_top = m_camera->GetPosition().y - (m_game_settings->GetResolution().y / 2);
  int y_current = (from_tile->GetPosition().y - (from_tile->GetSize()->y / 2));
  bool is_platform = true;
  Tunnelour::Tile_Bitmap* tile = 0;
  while (y_current > y_boundary_top) {
    tile = Create_Tile(static_cast<int>(from_tile->GetSize()->y));
    D3DXVECTOR3 position = from_tile->GetPosition();
    position.y = y_current - (tile->GetSize()->y / 2);
    tile->SetPosition(position);
    tile->Set_Is_Platform(is_platform);
    y_current = (tile->GetPosition().y - (tile->GetSize()->y / 2));
    new_tiles.push_back(tile);
    is_platform = false;
  }
  m_middleground_bottom = tile->GetPosition().x;

  return new_tiles;
}

//------------------------------------------------------------------------------
Tunnelour::Tile_Bitmap* Middleground_Controller::Create_Tile(int base_tile_size) {
  Tileset middleground_tileset;
  std::list<Tileset>::iterator tileset;

  for (tileset = m_metadata.tilesets.begin(); tileset != m_metadata.tilesets.end(); tileset++) {
    if (tileset->type.compare("Middleground") == 0) {
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

  int random_variable = rand() % middleground_line.number_of_tiles;

  tile->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(random_variable*(middleground_line.tile_size_x) + static_cast<float>(middleground_line.top_left_x)),
                                                      static_cast<float>(middleground_line.top_left_y));

  tile->SetSize(new D3DXVECTOR2(static_cast<float>(base_tile_size), static_cast<float>(base_tile_size)));

  return tile;
}

//------------------------------------------------------------------------------
void Middleground_Controller::Load_Tilset_Metadata() {
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
    for (int subset_num = 0; subset_num <  m_metadata.number_of_subsets; subset_num++) {
      Tileset temp_tileset;

      fgets(line, 225, pFile);
      if (line != NULL) {
        token = strtok_s(line, " ", &next_token);
        if (strcmp(token, "SubSet_Name") == 0)   {
          token = strtok_s(NULL, " =\"", &next_token);
          temp_tileset.name = token;
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
        for (int line_num = 0; line_num < temp_tileset.number_of_lines; line_num++) {
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

} // Tunnelour