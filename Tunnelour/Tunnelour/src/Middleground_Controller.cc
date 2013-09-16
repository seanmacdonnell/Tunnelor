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
  m_game_settings = 0;
  m_has_been_initalised = false;
  m_tunnel_x_size = 0;
  m_camera = 0;
  m_tileset_filename = L"";
  m_is_debug_mode = false;
  m_camera_top = 0;
  m_camera_bottom = 0;
  m_camera_left = 0;
  m_camera_right = 0;
}

//------------------------------------------------------------------------------
Middleground_Controller::~Middleground_Controller() {
  m_game_settings = 0;
  m_has_been_initalised = false;
  m_tunnel_x_size = 0;
  m_camera = 0;
  m_tileset_filename = L"";
  m_is_debug_mode = false;
  m_camera_top = 0;
  m_camera_bottom = 0;
  m_camera_left = 0;
  m_camera_right = 0;
}

//------------------------------------------------------------------------------
void Middleground_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));
  m_tunnel_x_size = (128+64);
}

//------------------------------------------------------------------------------
void Middleground_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initalised) {
    Middleground_Controller_Mutator mutator;
    m_model->Apply(&mutator);
    if (mutator.WasSuccessful()) {
      m_game_settings = mutator.GetGameSettings();
      m_camera = mutator.GetCamera();
      m_is_debug_mode = m_game_settings->IsDebugMode();
      if (m_is_debug_mode) {
        m_tileset_filename = L"Debug_Tileset_0_4.txt";
      } else {
        m_tileset_filename = L"Dirt_Tileset_5.txt";
      }

      Load_Tilset_Metadata();

      Tile_Tunnel();

      Tile_Middleground();

      m_has_been_initalised = true;
    }
  } else {
    // If Camera is over an area with no tiles
    m_camera_top = static_cast<int>(m_camera->GetPosition().y + (m_game_settings->GetResolution().y / 2));
    m_camera_bottom = static_cast<int>(m_camera->GetPosition().y - (m_game_settings->GetResolution().y / 2));
    m_camera_left = static_cast<int>(m_camera->GetPosition().x - (m_game_settings->GetResolution().x / 2));
    m_camera_right = static_cast<int>(m_camera->GetPosition().x + (m_game_settings->GetResolution().x / 2));
      
    if (m_camera_top < m_middleground_top) {
        //throw Exceptions::unfinished_error("Up Generation not finished!");
    }

    if (m_camera_bottom > m_middleground_bottom) {
      //throw Exceptions::unfinished_error("Down Generation not finished!");
    }
      
    if (m_camera_right > m_middleground_right) {
      //Extend_Tunnel_Right();
    }

    if (m_camera_left < m_middleground_left) {
      Extend_Tunnel_Left();
    }

    if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
      Switch_Tileset();
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
  // Get game settings component from the model with the Mutator.
  Middleground_Controller_Mutator mutator;

  // Load the Tileset Data
  Load_Tilset_Metadata();

  int tunnel_start_x = static_cast<int>((m_game_settings->GetResolution().x/2) * -1);
  int tunnel_start_y = 128;
  m_middleground_left = tunnel_start_x;
  m_middleground_top = m_middleground_top;
  int current_x = tunnel_start_x;
  int current_y = tunnel_start_y;
  Tile_Bitmap* tile;

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

    //Calculate number of x tiles
    int number_of_x_tiles = 0;
    std::div_t div_x_result = div(static_cast<int>(m_game_settings->GetResolution().x), base_tile_size);
    if (div_x_result.rem != 0) {
      number_of_x_tiles = div_x_result.quot + 1; //Add Another Tile if it doesne't fit
    } else {
      number_of_x_tiles = div_x_result.quot;
    }

    std::vector<Tile_Bitmap*> tile_line;
    for (int x = 0; x < number_of_x_tiles ; x++) {
        tile = Create_Tile(base_tile_size, false);
        if (y == 0) {
          tile->Set_Is_Top_Edge(true);
        } else if (y == (number_of_y_tiles - 1)) {
          tile->Set_Is_Bottom_Edge(true);
        }

        tile->SetPosition(D3DXVECTOR3(current_x + (tile->GetSize().x/2),
                                          current_y - (tile->GetSize().y/2),
                                          -1)); // Middleground Z Space is -1
        tile->GetTexture()->transparency = 0.0f; //TUNNEL
        current_x += static_cast<int>(tile->GetSize().x);
        m_tunnel_tiles.push_back(tile);
        if (x == 0) {
          tile->Set_Is_Left_Edge(true);
          m_left_edge_tunnel_tiles.push_back(tile);
          m_middleground_left = static_cast<int>(tile->GetPosition().x - (tile->GetSize().x / 2));
        }
        if (x == (number_of_x_tiles - 1)) {
          tile->Set_Is_Right_Edge(true);
          m_right_edge_tunnel_tiles.push_back(tile);
          m_middleground_right = static_cast<int>(tile->GetPosition().x + (tile->GetSize().x / 2));
        }
    }
    current_y -= static_cast<int>(tile->GetSize().y);
    current_x = static_cast<int>(tunnel_start_x);
  }

  // Add tiles to the model
  for (std::vector<Tile_Bitmap*>::iterator tile = m_tunnel_tiles.begin(); tile != m_tunnel_tiles.end(); tile++) {
    m_model->Add(*tile);
  }
}

//------------------------------------------------------------------------------
void Middleground_Controller::Tile_Middleground() {
  // Generate an initial Random Tile Middleground Spanning the Screen
  std::vector<Tile_Bitmap*> middleground_tiles;

  for (std::vector<Tile_Bitmap*>::iterator tunnel_tile = m_tunnel_tiles.begin(); tunnel_tile != m_tunnel_tiles.end(); tunnel_tile++) {
    std::vector<Tile_Bitmap*> tiles;
    if ((*tunnel_tile)->Is_Top_Edge()) {
      // produce tiles upwards of the same size until out of camera view
      tiles = GenerateTilesUpwards(*tunnel_tile);
    } else if ((*tunnel_tile)->Is_Bottom_Edge()) {
      // produce tiles upwards of the same size until out of camera view
      tiles = GenerateTilesDownwards(*tunnel_tile);
    } 

    for (std::vector<Tile_Bitmap*>::iterator middleground_tile = tiles.begin(); middleground_tile != tiles.end(); ++middleground_tile) {
      if ((*tunnel_tile)->Is_Left_Edge()) {
        (*middleground_tile)->Set_Is_Left_Edge(true);
        m_left_edge_middleground_tiles.push_back(*middleground_tile);
      } else if  ((*tunnel_tile)->Is_Right_Edge()) {
        (*middleground_tile)->Set_Is_Right_Edge(true);
        m_right_edge_middleground_tiles.push_back(*middleground_tile);
      }
      middleground_tiles.push_back(*middleground_tile);
    }
  }

  // Add tiles to Model
  for (std::vector<Tile_Bitmap*>::iterator tile = middleground_tiles.begin(); tile != middleground_tiles.end(); ++tile) {
    m_model->Add(*tile);
    m_middleground_tiles.push_back(*tile);
  }
}

void Middleground_Controller::Extend_Tunnel_Right() {
  std::vector<Tile_Bitmap*> new_tunnel_tiles;
  while (m_camera_right > m_middleground_right) {
    for (std::vector<Tile_Bitmap*>::iterator tunnel_edge_tile = m_right_edge_tunnel_tiles.begin(); tunnel_edge_tile !=  m_right_edge_tunnel_tiles.end(); tunnel_edge_tile++) {
      int current_tile_right = static_cast<int>((*tunnel_edge_tile)->GetPosition().x  + ((*tunnel_edge_tile)->GetSize().x / 2));
      while (m_camera_right > current_tile_right) {
        Tile_Bitmap* tile = Create_Tile(static_cast<int>((*tunnel_edge_tile)->GetSize().x), false);
        D3DXVECTOR3 position = D3DXVECTOR3((*tunnel_edge_tile)->GetPosition().x + tile->GetSize().x,
                                            (*tunnel_edge_tile)->GetPosition().y,
                                            (*tunnel_edge_tile)->GetPosition().z);
        tile->GetTexture()->transparency = 0.0f; //TUNNEL
        tile->SetPosition(position);
        m_model->Add(tile);
        m_tunnel_tiles.push_back(tile);
        new_tunnel_tiles.push_back(tile);
        *tunnel_edge_tile = tile;
        current_tile_right += static_cast<int>(tile->GetSize().x / 2);
        if (tunnel_edge_tile == m_right_edge_tunnel_tiles.begin()) {
          tile->Set_Is_Top_Edge(true);
        } else if ((tunnel_edge_tile + 1) == m_right_edge_tunnel_tiles.end()) {
          tile->Set_Is_Bottom_Edge(true);
        }
      }
    }
    Tile_Bitmap* smallest_tile = 0;
    for (std::vector<Tile_Bitmap*>::iterator tunnel_edge_tile = m_right_edge_tunnel_tiles.begin(); tunnel_edge_tile !=  m_right_edge_tunnel_tiles.end(); tunnel_edge_tile++) {
      // store the smallest number as the new middleground right.
      if (smallest_tile == 0) {
        smallest_tile = *tunnel_edge_tile;
      } else if (smallest_tile->GetSize().x > (*tunnel_edge_tile)->GetSize().x) {
        smallest_tile = (*tunnel_edge_tile);
      }
    }
    m_middleground_right += static_cast<int>(smallest_tile->GetSize().x);

    std::vector<Tile_Bitmap*> new_middleground_tiles;

    for (std::vector<Tile_Bitmap*>::iterator tunnel_tile = new_tunnel_tiles.begin(); tunnel_tile != new_tunnel_tiles.end(); tunnel_tile++) {
      std::vector<Tile_Bitmap*> tiles;
      if ((*tunnel_tile)->Is_Top_Edge()) {
        // produce tiles upwards of the same size until out of camera view
        tiles = GenerateTilesUpwards(*tunnel_tile);
      } else if ((*tunnel_tile)->Is_Bottom_Edge()) {
        // produce tiles upwards of the same size until out of camera view
        tiles = GenerateTilesDownwards(*tunnel_tile);
      } 

      for (std::vector<Tile_Bitmap*>::iterator middleground_tile = tiles.begin(); middleground_tile != tiles.end(); ++middleground_tile) {
        new_middleground_tiles.push_back(*middleground_tile);
      }
    }

    // Add tiles to Model
    for (std::vector<Tile_Bitmap*>::iterator tile = new_middleground_tiles.begin(); tile != new_middleground_tiles.end(); ++tile) {
      m_model->Add(*tile);
      m_middleground_tiles.push_back(*tile);
    }
  }
}

void Middleground_Controller::Extend_Tunnel_Left() {
  std::vector<Tile_Bitmap*> new_tunnel_tiles;
  while (m_camera_left < m_middleground_left) {
    for (std::vector<Tile_Bitmap*>::iterator tunnel_edge_tile = m_left_edge_tunnel_tiles.begin(); tunnel_edge_tile !=  m_left_edge_tunnel_tiles.end(); tunnel_edge_tile++) {
      int current_tile_left = static_cast<int>((*tunnel_edge_tile)->GetPosition().x  - ((*tunnel_edge_tile)->GetSize().x / 2));
      while (m_camera_left < current_tile_left) {
        Tile_Bitmap* tile = Create_Tile(static_cast<int>((*tunnel_edge_tile)->GetSize().x), false);
        D3DXVECTOR3 position = D3DXVECTOR3((*tunnel_edge_tile)->GetPosition().x - tile->GetSize().x,
                                            (*tunnel_edge_tile)->GetPosition().y,
                                            (*tunnel_edge_tile)->GetPosition().z);
        tile->GetTexture()->transparency = 0.0f; //TUNNEL
        tile->SetPosition(position);
        m_model->Add(tile);
        m_tunnel_tiles.push_back(tile);
        new_tunnel_tiles.push_back(tile);
        *tunnel_edge_tile = tile;
        current_tile_left -= static_cast<int>(tile->GetSize().x / 2);
        if (tunnel_edge_tile == m_left_edge_tunnel_tiles.begin()) {
          tile->Set_Is_Top_Edge(true);
        } else if ((tunnel_edge_tile + 1) == m_left_edge_tunnel_tiles.end()) {
          tile->Set_Is_Bottom_Edge(true);
        }

        tile->Set_Is_Left_Edge(true);
      }
    }
    Tile_Bitmap* smallest_tile = 0;
    for (std::vector<Tile_Bitmap*>::iterator tunnel_edge_tile = m_left_edge_tunnel_tiles.begin(); tunnel_edge_tile !=  m_left_edge_tunnel_tiles.end(); tunnel_edge_tile++) {
      // store the smallest number as the new middleground right.
      if (smallest_tile == 0) {
        smallest_tile = *tunnel_edge_tile;
      } else if (smallest_tile->GetSize().x > (*tunnel_edge_tile)->GetSize().x) {
        smallest_tile = (*tunnel_edge_tile);
      }
    }
    m_middleground_left -= static_cast<int>(smallest_tile->GetSize().x);

    std::vector<Tile_Bitmap*> new_middleground_tiles;

    for (std::vector<Tile_Bitmap*>::iterator tunnel_tile = new_tunnel_tiles.begin(); tunnel_tile != new_tunnel_tiles.end(); tunnel_tile++) {
      std::vector<Tile_Bitmap*> tiles;
      if ((*tunnel_tile)->Is_Top_Edge()) {
        // produce tiles upwards of the same size until out of camera view
        tiles = GenerateTilesUpwards(*tunnel_tile);
      } else if ((*tunnel_tile)->Is_Bottom_Edge()) {
        // produce tiles upwards of the same size until out of camera view
        tiles = GenerateTilesDownwards(*tunnel_tile);
      } 

      for (std::vector<Tile_Bitmap*>::iterator middleground_tile = tiles.begin(); middleground_tile != tiles.end(); ++middleground_tile) {
        new_middleground_tiles.push_back(*middleground_tile);
      }
    }

    for (std::vector<Tile_Bitmap*>::iterator old_edge_tile = m_left_edge_middleground_tiles.begin(); old_edge_tile != m_left_edge_middleground_tiles.end(); old_edge_tile++) {
      (*old_edge_tile)->Set_Is_Left_Edge(false);

      if (m_is_debug_mode) {
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
          if (line->tile_size_x == (*old_edge_tile)->GetSize().x && line->tile_size_y == (*old_edge_tile)->GetSize().y) {
            middleground_line = *line;
          }
        }

        int random_variable = 0;
        if ((*old_edge_tile)->Is_Platform() || (*old_edge_tile)->Is_Bottom_Edge() || (*old_edge_tile)->Is_Left_Edge() || (*old_edge_tile)->Is_Right_Edge() || (*old_edge_tile)->Is_Top_Edge()) {
          random_variable = 1;
        }
        (*old_edge_tile)->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(random_variable*(middleground_line.tile_size_x) + static_cast<float>(middleground_line.top_left_x)),
                                                            static_cast<float>(middleground_line.top_left_y));
        (*old_edge_tile)->GetTexture()->texture = 0;
        (*old_edge_tile)->GetFrame()->vertex_buffer = 0;
        (*old_edge_tile)->Init();
      }
    }
    m_left_edge_middleground_tiles.clear();

    // Add tiles to Model
    for (std::vector<Tile_Bitmap*>::iterator tile = new_middleground_tiles.begin(); tile != new_middleground_tiles.end(); ++tile) {
      (*tile)->Set_Is_Left_Edge(true);
      m_model->Add(*tile);
      m_middleground_tiles.push_back(*tile);
      m_left_edge_middleground_tiles.push_back(*tile);

      if (m_is_debug_mode) {
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
          if (line->tile_size_x == (*tile)->GetSize().x && line->tile_size_y == (*tile)->GetSize().y) {
            middleground_line = *line;
          }
        }

        int random_variable = 0;
        if ((*tile)->Is_Platform() || (*tile)->Is_Bottom_Edge() || (*tile)->Is_Left_Edge() || (*tile)->Is_Right_Edge() || (*tile)->Is_Top_Edge()) {
          random_variable = 1;
        }
        (*tile)->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(random_variable*(middleground_line.tile_size_x) + static_cast<float>(middleground_line.top_left_x)),
                                                            static_cast<float>(middleground_line.top_left_y));
        (*tile)->GetTexture()->texture = 0;
        (*tile)->GetFrame()->vertex_buffer = 0;
        (*tile)->Init();
      }
    }
  }
}

void Middleground_Controller::Switch_Tileset() {
  // Debug Mode has been activated or Deactivated
  m_is_debug_mode = m_game_settings->IsDebugMode();
  if (m_is_debug_mode) {
    m_tileset_filename = L"Debug_Tileset_0_4.txt";
    Load_Tilset_Metadata();
  } else {
    m_tileset_filename = L"Dirt_Tileset_5.txt";
    Load_Tilset_Metadata();
  }

  for (std::vector<Tile_Bitmap*>::iterator tile = m_middleground_tiles.begin(); tile != m_middleground_tiles.end(); ++tile) {
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
      if (line->tile_size_x == (*tile)->GetSize().x && line->tile_size_y == (*tile)->GetSize().y) {
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
      if ((*tile)->Is_Platform() || (*tile)->Is_Bottom_Edge() || (*tile)->Is_Left_Edge() || (*tile)->Is_Right_Edge() || (*tile)->Is_Top_Edge()) {
        random_variable = 1;
      }
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

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Middleground_Controller::GenerateTilesUpwards(Tile_Bitmap* from_tile) {
  std::vector<Tile_Bitmap*> new_tiles;

  int y_boundary_top = static_cast<int>(m_camera->GetPosition().y + (m_game_settings->GetResolution().y / 2));
  int y_current = static_cast<int>(from_tile->GetPosition().y + (from_tile->GetSize().y / 2));
  Tile_Bitmap* tile = 0;
  bool is_top_edge = true;
  while (y_current < y_boundary_top) {
    tile = Create_Tile(static_cast<int>(from_tile->GetSize().y), from_tile->Is_Platform());
    D3DXVECTOR3 position = from_tile->GetPosition();
    position.y = y_current + (tile->GetSize().y / 2);
    tile->SetPosition(position);
    tile->Set_Is_Top_Edge(is_top_edge);
    y_current = static_cast<int>(tile->GetPosition().y + (tile->GetSize().y / 2));
    new_tiles.push_back(tile);
  }
  m_middleground_top = static_cast<int>(tile->GetPosition().y);

  return new_tiles;
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Middleground_Controller::GenerateTilesDownwards(Tile_Bitmap* from_tile) {
  std::vector<Tile_Bitmap*> new_tiles;

  int y_boundary_top = static_cast<int>(m_camera->GetPosition().y - (m_game_settings->GetResolution().y / 2));
  int y_current = static_cast<int>(from_tile->GetPosition().y - (from_tile->GetSize().y / 2));
  Tile_Bitmap* tile = 0;
  bool is_platform = true;
  bool is_bottom_edge = true;
  while (y_current > y_boundary_top) {
    tile = Create_Tile(static_cast<int>(from_tile->GetSize().y), is_platform);
    D3DXVECTOR3 position = from_tile->GetPosition();
    position.y = y_current - (tile->GetSize().y / 2);
    tile->SetPosition(position);
    tile->Set_Is_Bottom_Edge(is_bottom_edge);
    y_current = static_cast<int>(tile->GetPosition().y - (tile->GetSize().y / 2));
    new_tiles.push_back(tile);
    is_platform = false;
    is_bottom_edge = false;
  }
  m_middleground_bottom = static_cast<int>(tile->GetPosition().y);

  return new_tiles;
}

//------------------------------------------------------------------------------
Tile_Bitmap* Middleground_Controller::Create_Tile(int base_tile_size, bool is_platform) {
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

  Tile_Bitmap* tile = new Tile_Bitmap();
  tile->SetPosition(D3DXVECTOR3(0, 0, -1));
  tile->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_metadata.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_metadata.size_x),
                                                 static_cast<float>(m_metadata.size_y));
  tile->GetTexture()->tile_size = D3DXVECTOR2(static_cast<float>(middleground_line.tile_size_x),
                                              static_cast<float>(middleground_line.tile_size_y));

  tile->Set_Is_Platform(is_platform);
  int random_variable = rand() % middleground_line.number_of_tiles;
 

  tile->GetTexture()->top_left_position = D3DXVECTOR2(static_cast<float>(random_variable*(middleground_line.tile_size_x) + static_cast<float>(middleground_line.top_left_x)),
                                                      static_cast<float>(middleground_line.top_left_y));

  tile->SetSize(D3DXVECTOR2(static_cast<float>(base_tile_size), static_cast<float>(base_tile_size)));

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
    throw Exceptions::init_error("Open Tileset Metadata Failed!");
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