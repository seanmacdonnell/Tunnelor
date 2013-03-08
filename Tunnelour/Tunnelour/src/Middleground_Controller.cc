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
  m_bitmap = NULL;
  std::srand(static_cast<unsigned int>(std::time(0)));
  m_game_settings = 0;
  m_has_init_middleground_been_generated = false;
  m_has_init_tunnel_been_generated = false;
  m_tunnel_x_size = 64;
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
  m_model->Apply(&m_mutator);
  if (!m_mutator.FoundGameSettings())  {
    throw Tunnelour::Exceptions::run_error("Can't find a game settings component!");
  } else {
    m_game_settings = m_mutator.GetGameSettings();
  }

  // Load the Tileset Data
  Load_Tilset_Metadata();

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
    int number_of_1x1_y_tiles = 0;

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
              number_of_1x1_y_tiles = div_y_result.rem;
            }
          }
        }
      }
    }

    int number_of_y_tiles = number_of_32x32_y_tiles + number_of_16x16_y_tiles + number_of_8x8_y_tiles + number_of_4x4_y_tiles + number_of_2x2_y_tiles + number_of_1x1_y_tiles;

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
      } else if (number_of_1x1_y_tiles != 0) {
        base_tile_size = 1;
        number_of_1x1_y_tiles--;
      }

      int resised_tile_size = static_cast<int>(base_tile_size * m_game_settings->GetTileMultiplicationFactor());

      //Calculate number of x tiles
      int number_of_x_tiles = 0;
      std::div_t div_x_result = div(m_game_settings->GetResolution().x, resised_tile_size);
      if (div_x_result.rem != 0) {
        number_of_x_tiles = div_x_result.quot + 1; //Add Another Tile if it doesne't fit
      } else {
        number_of_x_tiles = div_x_result.quot;
      }

      std::vector<Tunnelour::Tile_Bitmap*> tile_line;
      for (int x = 0; x < number_of_x_tiles ; x++) {
         tile = Create_Tile(base_tile_size, resised_tile_size, false);
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

    m_has_init_tunnel_been_generated = true;
  }
}

//------------------------------------------------------------------------------
void Middleground_Controller::Tile_Middleground() {
  // Generate an initial Random Tile Middleground Spanning the Screen
  if (!m_has_init_middleground_been_generated) {
    std::vector<Tunnelour::Tile_Bitmap*> middleground_tiles;
    middleground_tiles = GenerateMiddlegroundTiles();

    // Check for Collisions with the Tunnel Tiles
    std::vector<Collision> collisions;
    while (DoTheseTilesetsHaveCollisions(&middleground_tiles, &m_tunnel_tiles, &collisions)) {
      // For each collision create new tiles and add them to the collection
      for (std::vector<Collision>::iterator collision = collisions.begin(); collision != collisions.end() ; collision++) {
        // If Collision Resize the tile out of the collision zone
        Tunnelour::Tile_Bitmap* boundary = GetResizedBoundarySize(collision->a_tile, collision->b_tile, collision->a_tile_collision_point);

        // Generate Replacement tiles within that boundary
        std::vector<Tunnelour::Tile_Bitmap*> new_tiles = GenerateBoundayFittingTiles(boundary, collision->b_tile, collision->a_tile_collision_point);

        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator new_tile = new_tiles.begin(); new_tile != new_tiles.end(); new_tile++) {
          middleground_tiles.push_back(*new_tile);
        }
      }

      // Remove the tiles - one loop
      for (std::vector<Collision>::iterator collision = collisions.begin(); collision != collisions.end() ; collision++) {
        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = middleground_tiles.begin(); tile != middleground_tiles.end() ;) {
          if (collision->a_tile == *tile) {
            delete *tile;
            tile = middleground_tiles.erase(tile);
          } else {
            tile++;
          }
        }
      }
    }

    // Add tiles to Model
    for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = middleground_tiles.begin(); tile != middleground_tiles.end(); ++tile) {
      m_model->Add(*tile);
    }

    m_has_init_middleground_been_generated = true;
  }
}

//------------------------------------------------------------------------------
bool Middleground_Controller::DoTheseTilesetsHaveCollisions(std::vector<Tunnelour::Tile_Bitmap*> *a_tileset, std::vector<std::vector<Tunnelour::Tile_Bitmap*>> *b_tileset, std::vector<Collision> *output_collisions) {
  bool do_these_tilesets_have_collisions = false;
    for (std::vector<Tunnelour::Tile_Bitmap*>::iterator a_tile = a_tileset->begin(); a_tile != a_tileset->end(); a_tile++) {
      for (std::vector<std::vector<Tunnelour::Tile_Bitmap*>>::iterator tunnel_line = m_tunnel_tiles.begin(); tunnel_line != m_tunnel_tiles.end(); tunnel_line++) {
        for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tunnel_tile = tunnel_line->begin(); tunnel_tile != tunnel_line->end(); tunnel_tile++) {
          Collision collision;
          if (DoTheseTilesCollide(*a_tile, *tunnel_tile, collision.a_tile_collision_point)) {
            collision.a_tile = *a_tile;
            collision.b_tile = *tunnel_tile;
            output_collisions->push_back(collision);
            do_these_tilesets_have_collisions = true;
          }
        }
      }
    }

  return do_these_tilesets_have_collisions;
}

//------------------------------------------------------------------------------
std::vector<Tunnelour::Tile_Bitmap*> Middleground_Controller::GenerateMiddlegroundTiles() {
  std::vector<Tunnelour::Tile_Bitmap*> middleground_tiles;

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
      tile = Create_Tile(base_tile_size, resised_tile_size, false);
      tile->GetTexture()->transparency = 1.0f;
      tile->SetPosition(new D3DXVECTOR3(current_x + (tile->GetSize()->x/2),
                                          current_y - (tile->GetSize()->y/2),
                                          -3)); // Middleground Z Space is -1
      current_x += static_cast<int>(tile->GetSize()->x);
        
      middleground_tiles.push_back(tile);
    }
    current_y -= static_cast<int>(tile->GetSize()->y);
    current_x = static_cast<int>(top_left_window_x);
  }

  //D3DXVECTOR2 collision;
 // DoTheseTilesCollide(middleground_tiles[0], middleground_tiles[1], collision);

  return middleground_tiles;
}

//------------------------------------------------------------------------------
Tunnelour::Tile_Bitmap* Middleground_Controller::GetResizedBoundarySize(Tunnelour::Tile_Bitmap* tile, Tunnelour::Tile_Bitmap* tunnel_tile, D3DXVECTOR2 tile_collision_point) {
  float tile_left, tile_right, tile_top, tile_bottom;
  tile_left = tile->GetPosition()->x - static_cast<float>(tile->GetSize()->x / 2);
  tile_right = tile->GetPosition()->x + static_cast<float>(tile->GetSize()->x / 2);
  tile_top = tile->GetPosition()->y + static_cast<float>(tile->GetSize()->x / 2);
  tile_bottom = tile->GetPosition()->y - static_cast<float>(tile->GetSize()->x / 2);

  float tunnel_tile_left, tunnel_tile_right, tunnel_tile_top, tunnel_tile_bottom;
  tunnel_tile_left = tunnel_tile->GetPosition()->x - static_cast<float>(tunnel_tile->GetSize()->x / 2);
  tunnel_tile_right = tunnel_tile->GetPosition()->x + static_cast<float>(tunnel_tile->GetSize()->x / 2);
  tunnel_tile_top = tunnel_tile->GetPosition()->y + static_cast<float>(tunnel_tile->GetSize()->x / 2);
  tunnel_tile_bottom = tunnel_tile->GetPosition()->y - static_cast<float>(tunnel_tile->GetSize()->x / 2);

  //Create a new tile border with the original tile and the horizontal intersect
  float tile_boundary_left, tile_boundary_right, tile_boundary_top, tile_boundary_bottom;
  tile_boundary_left = tile_left;
  tile_boundary_right = tile_right;
  tile_boundary_top = tile_top;
  tile_boundary_bottom = tile_bottom;

  if (tile_boundary_top == tile_collision_point.y) {
    // Resize Top Down
    tile_boundary_top = tunnel_tile_bottom;
  } else if (tile_boundary_bottom ==  tile_collision_point.y) {
    // Resize Bottom Up
    tile_boundary_bottom = tunnel_tile_top;
  }

  Tunnelour::Tile_Bitmap* boundary_tile = new Tunnelour::Tile_Bitmap();
  boundary_tile->SetSize(tile_boundary_right - tile_boundary_left, tile_boundary_top - tile_boundary_bottom);

  D3DXVECTOR3 centre = D3DXVECTOR3(0, 0, 0);
  centre.x += tile_boundary_left;
  centre.y += tile_boundary_top;
  centre.x += tile_boundary_right;
  centre.y += tile_boundary_top;
  centre.x += tile_boundary_right;
  centre.y += tile_boundary_bottom;
  centre.x += tile_boundary_left;
  centre.y += tile_boundary_bottom;
  centre.x = centre.x / 4;
  centre.y = centre.y / 4;
  boundary_tile->SetPosition(&centre);

  return boundary_tile;
}

//------------------------------------------------------------------------------
std::vector<Tunnelour::Tile_Bitmap*> Middleground_Controller::GenerateBoundayFittingTiles(Tunnelour::Tile_Bitmap* tile, Tunnelour::Tile_Bitmap* tunnel_tile, D3DXVECTOR2 tile_collision_point) {
  std::vector<Tunnelour::Tile_Bitmap*> tiles;

  float tile_left, tile_right, tile_top, tile_bottom;
  tile_left = tile->GetPosition()->x - static_cast<float>(tile->GetSize()->x / 2);
  tile_right = tile->GetPosition()->x + static_cast<float>(tile->GetSize()->x / 2);
  tile_top = tile->GetPosition()->y + static_cast<float>(tile->GetSize()->y / 2);
  tile_bottom = tile->GetPosition()->y - static_cast<float>(tile->GetSize()->y / 2);

  float tunnel_tile_top = tunnel_tile->GetPosition()->y + static_cast<float>(tunnel_tile->GetSize()->y / 2);
  float tunnel_tile_bottom = tunnel_tile->GetPosition()->y - static_cast<float>(tunnel_tile->GetSize()->y / 2);

  // Calculate the tile which will fit in this boundary
  int number_of_32x32_y_tiles = 0, number_of_32x32_x_tiles = 0;
  int number_of_16x16_y_tiles = 0, number_of_16x16_x_tiles = 0;
  int number_of_8x8_y_tiles = 0, number_of_8x8_x_tiles = 0;
  int number_of_4x4_y_tiles = 0, number_of_4x4_x_tiles = 0;
  int number_of_2x2_y_tiles = 0, number_of_2x2_x_tiles = 0;
  int number_of_1x1_y_tiles = 1, number_of_1x1_x_tiles = 0;

  int horizontal_boundary_size =  tile_top - tile_bottom - (1 * m_game_settings->GetTileMultiplicationFactor());
  std:div_t div_y_result = div(horizontal_boundary_size, 32 * m_game_settings->GetTileMultiplicationFactor());
  number_of_32x32_y_tiles = div_y_result.quot;
  if (div_y_result.rem != 0) {
    div_y_result =  div(div_y_result.rem, 16 * m_game_settings->GetTileMultiplicationFactor());
    number_of_16x16_y_tiles = div_y_result.quot;
    if (div_y_result.rem != 0) {
      div_y_result =  div(div_y_result.rem, 8 * m_game_settings->GetTileMultiplicationFactor());
      number_of_8x8_y_tiles = div_y_result.quot;
      if (div_y_result.rem != 0) {
        div_y_result =  div(div_y_result.rem, 4 * m_game_settings->GetTileMultiplicationFactor());
        number_of_4x4_y_tiles = div_y_result.quot;
        if (div_y_result.rem != 0) {
          div_y_result =  div(div_y_result.rem, 2 * m_game_settings->GetTileMultiplicationFactor());
          number_of_2x2_y_tiles = div_y_result.quot;
          if  (div_y_result.rem != 0) {
            div_y_result =  div(div_y_result.rem, 1 * m_game_settings->GetTileMultiplicationFactor());
            number_of_1x1_y_tiles = div_y_result.quot + 1;
          }
        }
      }
    }
  }

  int sub_number_of_y_tiles = number_of_32x32_y_tiles + number_of_16x16_y_tiles + number_of_8x8_y_tiles + number_of_4x4_y_tiles + number_of_2x2_y_tiles + number_of_1x1_y_tiles;
  int sub_number_of_x_tiles = 0;
  int sub_current_y = tile_top;
  int sub_current_x = tile_left;

  bool big_tiles_to_small = false;
  int border_is_first = false;
  if (tile_bottom == tunnel_tile_top) {
    big_tiles_to_small = true;
  } else if (tile_top ==  tunnel_tile_bottom) {
    big_tiles_to_small = false;
    border_is_first = true;
  }

  for (int y = 0; y < sub_number_of_y_tiles ; y++) {
    int border = false;
    int base_tile_size = 0;
    if (big_tiles_to_small) {
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
      } else if (number_of_1x1_y_tiles != 0) {
        base_tile_size = 1;
        if (number_of_1x1_y_tiles == 1) {
          border = true;
        }
        number_of_1x1_y_tiles--;
      }
    } else {
      if (number_of_1x1_y_tiles != 0) {
        base_tile_size = 1;
        if (border_is_first) {
          border = true;
          border_is_first = false;
        }
        number_of_1x1_y_tiles--;
      } else if (number_of_2x2_y_tiles != 0) {
        base_tile_size = 2;
        number_of_2x2_y_tiles--;
      } else if (number_of_4x4_y_tiles != 0) {
        base_tile_size = 4;
        number_of_4x4_y_tiles--;
      } else if (number_of_8x8_y_tiles != 0) {
        base_tile_size = 8;
        number_of_8x8_y_tiles--;
      } else if (number_of_16x16_y_tiles != 0) {
        base_tile_size = 16;
        number_of_16x16_y_tiles--;
      } else if (number_of_32x32_y_tiles != 0) {
        base_tile_size = 32;
        number_of_32x32_y_tiles--;
      }
    }

    div_t div_y_result = div((int)tile->GetSize()->x, base_tile_size * m_game_settings->GetTileMultiplicationFactor());

    sub_number_of_x_tiles = div_y_result.quot;
    if  (div_y_result.rem != 0) {
      throw Tunnelour::Exceptions::run_error("Border isn't divisable by horizontally two!");
    }

    int resised_tile_size = static_cast<int>(base_tile_size * m_game_settings->GetTileMultiplicationFactor());


    Tunnelour::Tile_Bitmap* sub_tile;
    for (int x = 0; x < sub_number_of_x_tiles ; x++) {
      sub_tile = Create_Tile(base_tile_size, resised_tile_size, border);
      sub_tile->SetPosition(new D3DXVECTOR3(sub_current_x + (sub_tile->GetSize()->x/2),
                                            sub_current_y - (sub_tile->GetSize()->y/2),
                                            -2)); // Middleground Z Space is -1
      sub_tile->GetTexture()->transparency = 1.0f;
      sub_current_x += static_cast<int>(sub_tile->GetSize()->x);
      tiles.push_back(sub_tile);
    }
    if (sub_number_of_x_tiles != 0) {
      sub_current_y -= static_cast<int>(sub_tile->GetSize()->y);
      sub_current_x = static_cast<int>(tile_left);
    }
  }

  return tiles;
}

//------------------------------------------------------------------------------
void Middleground_Controller::Load_Tilset_Metadata() {
  FILE * pFile;
  int lSize;

  std::wstring wtileset_path = m_game_settings->GetTilesetPath();
  //m_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Debug_Tileset_0_3.txt");
  m_metadata_file_path = String_Helper::WStringToString(wtileset_path + L"Dirt_Tileset_4_3.txt");

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
Tunnelour::Tile_Bitmap* Middleground_Controller::Create_Tile(int base_tile_size, int resised_tile_size, bool isborder) {
  Tileset middleground_tileset;
  std::list<Tileset>::iterator tileset;

  for (tileset = m_metadata.tilesets.begin(); tileset != m_metadata.tilesets.end(); tileset++) {
    if (isborder) {
      if (tileset->type.compare("Border") == 0) {
        middleground_tileset = *tileset;
      }
    } else {
      if (tileset->type.compare("Middleground") == 0) {
        middleground_tileset = *tileset;
      }
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

  tile->SetSize(new D3DXVECTOR2(resised_tile_size, resised_tile_size));

  return tile;
}

//------------------------------------------------------------------------------
bool Middleground_Controller::DoTheseTilesCollide(Tunnelour::Tile_Bitmap* TileA, Tunnelour::Tile_Bitmap* TileB, D3DXVECTOR2 &tile_a_output_colision) {
  // At least one vertex in TileA is contained in the TileB.
  float a_tile_left, a_tile_right, a_tile_top, a_tile_bottom;
  a_tile_left = TileA->GetPosition()->x - static_cast<float>(TileA->GetSize()->x / 2);
  a_tile_right = TileA->GetPosition()->x + static_cast<float>(TileA->GetSize()->x / 2);
  a_tile_top = TileA->GetPosition()->y + static_cast<float>(TileA->GetSize()->x / 2);
  a_tile_bottom = TileA->GetPosition()->y - static_cast<float>(TileA->GetSize()->x / 2);

  float b_tile_left, b_tile_right, b_tile_top, b_tile_bottom;
  b_tile_left = TileB->GetPosition()->x - static_cast<float>(TileB->GetSize()->x / 2);
  b_tile_right = TileB->GetPosition()->x + static_cast<float>(TileB->GetSize()->x / 2);
  b_tile_top = TileB->GetPosition()->y + static_cast<float>(TileB->GetSize()->x / 2);
  b_tile_bottom = TileB->GetPosition()->y - static_cast<float>(TileB->GetSize()->x / 2);

  if ((a_tile_left == b_tile_left) || (a_tile_left > b_tile_left && a_tile_left < b_tile_right)) {
    // Horrizontal Collision
    if (a_tile_top < b_tile_top && a_tile_top > b_tile_bottom) {
      // Vertical Collision!
      tile_a_output_colision = D3DXVECTOR2(a_tile_left, a_tile_top);
      return true;
    }
    if (a_tile_bottom < b_tile_top && a_tile_bottom > b_tile_bottom) {
      // Vertical Collision!
      tile_a_output_colision = D3DXVECTOR2(a_tile_left, a_tile_bottom);
      return true;
    }
  }

  if ((a_tile_right == b_tile_right) || (a_tile_right > b_tile_left && a_tile_right < b_tile_right)) {
    // Horrizontal Collision
    if (a_tile_top > b_tile_top && a_tile_top < b_tile_bottom) {
      // Vertical Collision!
      tile_a_output_colision = D3DXVECTOR2(a_tile_left, a_tile_top);
      return true;
    }
    if (a_tile_bottom > b_tile_top && a_tile_bottom < b_tile_bottom) {
      // Vertical Collision!
      tile_a_output_colision = D3DXVECTOR2(a_tile_left, a_tile_bottom);
      return true;
    }
  }

  // At least one vertex in TileA is contained within TileB.
  // Any edge of TileA intersects any edge of TileB.

  if (TileA->GetSize()->x == 4) {
    int pause = 1;
  }

  return false;
}

//------------------------------------------------------------------------------
bool Middleground_Controller::WhereDoThesePointsIntersect( D3DXVECTOR2 p1, D3DXVECTOR2 p2, D3DXVECTOR2 p3, D3DXVECTOR2 p4, D3DXVECTOR2 &output) {
  //p1-p2 is the first edge. 
  //p3-p4 is the second edge.
  float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
  float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

  float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

  if (d == 0) return NULL;

  float pre = (x1*y2 - y1*x2), pos = (x3*y4 - y3*x4);
  float x = ( pre * (x3 - x4) - (x1 - x2) * pos ) / d;
  float y = ( pre * (y3 - y4) - (y1 - y2) * pos ) / d;

  if ( x <= min(x1, x2) || x >= max(x1, x2) || x <= min(x3, x4) || x >= max(x3, x4) ) return false;
  if ( y <= min(y1, y2) || y >= max(y1, y2) || y <= min(y3, y4) || y >= max(y3, y4) ) return false;

  // Return the point of intersection
  output.x = x;
  output.y = y;
  return true;; 
}

} // Tunnelour