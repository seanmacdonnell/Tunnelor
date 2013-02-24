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
#include "Exceptions.h"
#include "Middleground_Controller_Mutator.h"
#include "String_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Middleground_Controller::Middleground_Controller() : Controller() {
  m_bitmap = NULL;
  std::srand(static_cast<unsigned int>(std::time(0)));
  m_game_settings = 0;
  m_has_init_middleground_been_generated = false;
  m_has_init_tunnel_been_generated = false;
  m_tunnel_x_size = 48;
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
  Tile_Tunnel();
  Tile_Middleground();
  m_is_finished = true;
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
    int tunnel_start_y = 64;
    int current_x = tunnel_start_x;
    int current_y = tunnel_start_y;
    Tunnelour::Tile_Bitmap* tile;

    int number_of_32x32_y_tiles = 0;
    int number_of_16x16_y_tiles = 0;
    int number_of_8x8_y_tiles = 0;
    int number_of_4x4_y_tiles = 0;
    int number_of_2x2_y_tiles = 0;

    std:div_t div_y_result =  div(m_tunnel_x_size, 32);
    number_of_32x32_y_tiles = div_y_result.quot;
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
              number_of_2x2_y_tiles += 1;
              m_tunnel_x_size += 1;
            }
          }
        }
      }
    }

    int number_of_y_tiles = number_of_32x32_y_tiles + number_of_16x16_y_tiles + number_of_8x8_y_tiles + number_of_4x4_y_tiles + number_of_2x2_y_tiles;

    for (int y = 0; y < number_of_y_tiles ; y++) {
      int base_tile_size = 0;
      if (number_of_32x32_y_tiles != 0) {
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
      }

      int resised_tile_size = static_cast<int>(base_tile_size * m_game_settings->GetTileMultiplicationFactor());

      //Calculate number of x tiles
      int number_of_x_tiles = 0;
      std::div_t div_x_result = div(m_game_settings->GetResolution().x, resised_tile_size);
      if (div_x_result.rem != 0) {
        number_of_x_tiles = div_x_result.quot + 1;
      } else {
        number_of_x_tiles = div_x_result.quot;
      }

      std::vector<Tunnelour::Tile_Bitmap*> tile_line;
      for (int x = 0; x < number_of_x_tiles ; x++) {
         tile = Create_Tile(base_tile_size, resised_tile_size);
         tile->SetPosition(new D3DXVECTOR3(current_x + (tile->GetSize()->x/2),
                                           current_y - (tile->GetSize()->y/2),
                                           -1)); // Middleground Z Space is -1
         tile->GetTexture()->transparency = 0.0f;
         current_x += static_cast<int>(tile->GetSize()->x);
         tile_line.push_back(tile);
      }
      current_y -= static_cast<int>(tile->GetSize()->y);
      current_x = static_cast<int>(tunnel_start_x);
      m_tunnel_tiles.push_back(tile_line);
    }

    // Add tiles to the model
    for (std::vector<std::vector<Tunnelour::Tile_Bitmap*>>::iterator row = m_tunnel_tiles.begin(); row != m_tunnel_tiles.end(); ++row) {
      for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = row->begin() ; tile != row->end(); ++tile) {
        m_model->Add(*tile);
      }
    }
  }

  m_has_init_tunnel_been_generated = true;
}

//------------------------------------------------------------------------------
void Middleground_Controller::Tile_Middleground() {
  // Generate an initial Random Tile Middleground
  if (!m_has_init_middleground_been_generated) {
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

    int top_left_window_x = static_cast<int>((m_game_settings->GetResolution().x/2) * -1);
    int top_left_window_y = static_cast<int>(m_game_settings->GetResolution().y/2);
    int current_x = top_left_window_x;
    int current_y = top_left_window_y;
    Tunnelour::Tile_Bitmap* tile;

    int base_tile_size = 32;
    int resised_tile_size = static_cast<int>(base_tile_size * m_game_settings->GetTileMultiplicationFactor());

    int number_of_y_tiles = 0;
    std::div_t div_y_result =  div(m_game_settings->GetResolution().y, resised_tile_size);
    if (div_y_result.rem != 0) {
      number_of_y_tiles = div_y_result.quot + 1;
    } else {
      number_of_y_tiles = div_y_result.quot;
    }

    //Calculate number of x tiles
    int number_of_x_tiles = 0;
    std::div_t div_x_result = div(m_game_settings->GetResolution().x, resised_tile_size);
    if (div_x_result.rem != 0) {
      number_of_x_tiles = div_x_result.quot + 1;
    } else {
      number_of_x_tiles = div_x_result.quot;
    }

    for (int y = 0; y < number_of_y_tiles ; y++) {
      std::vector<Tunnelour::Tile_Bitmap*> tile_line;
      for (int x = 0; x < number_of_x_tiles ; x++) {
         tile = Create_Tile(base_tile_size, resised_tile_size);
         tile->SetPosition(new D3DXVECTOR3(current_x + (tile->GetSize()->x/2),
                                           current_y - (tile->GetSize()->y/2),
                                           -1)); // Middleground Z Space is -1
         current_x += static_cast<int>(tile->GetSize()->x);

//         bool result = DoTheseTilesCollide(tile, tile);
//         if (result) {
//            throw Tunnelour::Exceptions::run_error("Collision!");
//         }
         tile_line.push_back(tile);
      }
      current_y -= static_cast<int>(tile->GetSize()->y);
      current_x = static_cast<int>(top_left_window_x);
      m_middleground_tiles.push_back(tile_line);
    }

    // Add tiles to the model
    for (std::vector<std::vector<Tunnelour::Tile_Bitmap*>>::iterator row = m_middleground_tiles.begin(); row != m_middleground_tiles.end(); ++row) {
      for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = row->begin() ; tile != row->end(); ++tile) {
        m_model->Add(*tile);
      }
    }

    m_has_init_middleground_been_generated = true;
  }
}

//------------------------------------------------------------------------------
void Middleground_Controller::Load_Tilset_Metadata() {
  FILE * pFile;
  int lSize;

  std::wstring wtileset_path = m_game_settings->GetTilesetPath();
  m_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Debug_Tileset_0_0.txt");
  //m_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Dirt_Tileset_4_0.txt");

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
Tunnelour::Tile_Bitmap* Middleground_Controller::Create_Tile(int base_tile_size, int resised_tile_size) {
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
  tile->SetPosition(new D3DXVECTOR3(0, 0, 0));
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

  //tile->SetSize(new D3DXVECTOR2(middleground_line.tile_size_x * 4, middleground_line.tile_size_x * 4));
  tile->SetSize(new D3DXVECTOR2(resised_tile_size, resised_tile_size));
  return tile;
}

//------------------------------------------------------------------------------
bool Middleground_Controller::DoTheseTilesCollide(Tunnelour::Tile_Bitmap* TileA, Tunnelour::Tile_Bitmap* TileB) {
    // At least one vertex in TileA is contained in the TileB.
    // Check Horrizontally (x) first.
    // Remember that verticies are stored clockwise. So 0 is top left and 1 is top right.

    if (TileA->GetFrame()->vertices[0].position.x >= TileB->GetFrame()->vertices[0].position.x &&
        TileA->GetFrame()->vertices[1].position.x <= TileB->GetFrame()->vertices[1].position.x) {
          //Collision!
          return true;
    }


    // At least one vertex in TileA is contained within TileB.
    // Any edge of TileA intersects any edge of TileB.

    return false;
}

}  // namespace Tunnelour