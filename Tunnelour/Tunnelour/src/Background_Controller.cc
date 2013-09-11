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
#include "Exceptions.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Background_Controller::Background_Controller() : Controller() {
  m_tilesets.clear();
  m_background_tiles.clear();
  m_game_settings = 0;
  m_camera = 0;
  m_has_init_background_been_generated = false;
  m_background_top = 0;
  m_background_bottom = 0;
  m_background_left = 0;
  m_background_right = 0;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
  m_is_debug_mode = false;
}

//------------------------------------------------------------------------------
Background_Controller::~Background_Controller() {
  m_tilesets.clear();
  m_background_tiles.clear();
  m_game_settings = 0;
  m_camera = 0;
  m_has_init_background_been_generated = false;
  m_background_top = 0;
  m_background_bottom = 0;
  m_background_left = 0;
  m_background_right = 0;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
  m_is_debug_mode = false;
}

//------------------------------------------------------------------------------
void Background_Controller::Init(Tunnelour::Component_Composite * const model) {
    Tunnelour::Controller::Init(model);
    std::srand(static_cast<unsigned int>(std::time(0)));
}

//------------------------------------------------------------------------------
void Background_Controller::Run() {
  // Generate an initial Random Tile Background
  if (!m_has_init_background_been_generated) {
    // Get game settings component from the model with the Mutator.
    Tunnelour::Background_Controller_Mutator mutator;
    m_model->Apply(&mutator);
    if (mutator.WasSuccessful()) {
      m_game_settings = mutator.GetGameSettings();
      m_camera = mutator.GetCamera();

      LoadTilesetMetadata();

      m_is_debug_mode = m_game_settings->IsDebugMode(); 
      if (m_is_debug_mode) {
        m_current_tileset = GetNamedTileset("Debug");
      } else {
        m_current_tileset = GetNamedTileset("Dirt");
      }
      m_current_background_subset = GetCurrentBackgroundSubset();

      TileCurrentWindow();
    }
  } else {
    float camera_top = 0;
    float camera_bottom = 0;
    float camera_left = 0;
    float camera_right = 0;
    camera_top = (m_camera->GetPosition().y + (m_game_settings->GetResolution().y / 2));
    camera_bottom = (m_camera->GetPosition().y - (m_game_settings->GetResolution().y / 2));
    camera_left = (m_camera->GetPosition().x - (m_game_settings->GetResolution().x / 2));
    camera_right = (m_camera->GetPosition().x + (m_game_settings->GetResolution().x / 2));

    if (camera_top > m_background_top) {
      TileUp(camera_top);
    }

    if (camera_bottom < m_background_bottom) {
      TileDown(camera_bottom);
    }

    if (camera_right > m_background_right) {
      TileRight(camera_right);
    }

    if (camera_left < m_background_left) {
      TileLeft(camera_left);
    }

    if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
      SwitchTileset();
    }
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
Tunnelour::Tile_Bitmap* Background_Controller::CreateTile(float base_tile_size) {
  Tileset_Helper::Line middleground_line;
  std::list<Tileset_Helper::Line>::iterator line;
  for (line = m_current_background_subset.lines.begin(); line != m_current_background_subset.lines.end(); line++) {
    if (line->tile_size_x == base_tile_size && line->tile_size_y == base_tile_size) {
      middleground_line = *line;
    }
  }

  Tunnelour::Tile_Bitmap* tile = new Tunnelour::Tile_Bitmap();
  tile->SetPosition(D3DXVECTOR3(0, 0, 0));
  tile->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_current_tileset.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_current_tileset.size_x),
                                                 static_cast<float>(m_current_tileset.size_y));
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

  tile->SetSize(D3DXVECTOR2(static_cast<float>(base_tile_size),
                            static_cast<float>(base_tile_size)));
  return tile;
}

//---------------------------------------------------------------------------
void Background_Controller::LoadTilesetMetadata() {
  Tileset_Helper::Tileset_Metadata m_debug_tileset_metadata;
  m_debug_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Debug_Tileset_0_4.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_debug_metadata_file_path, &m_debug_tileset_metadata);
  m_tilesets.push_back(m_debug_tileset_metadata);

  Tileset_Helper::Tileset_Metadata m_dirt_tileset_metadata;
  m_dirt_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Dirt_Tileset_5.txt");
  Tileset_Helper::Load_Tileset_Metadata_Into_Struct(m_dirt_metadata_file_path, &m_dirt_tileset_metadata);
  m_tilesets.push_back(m_dirt_tileset_metadata);
}

//---------------------------------------------------------------------------
Tileset_Helper::Tileset_Metadata Background_Controller::GetNamedTileset(std::string name) {
  Tileset_Helper::Tileset_Metadata found_tileset_metadata;

  std::list<Tileset_Helper::Tileset_Metadata>::iterator tileset_metadata;
  for (tileset_metadata = m_tilesets.begin(); tileset_metadata != m_tilesets.end(); tileset_metadata++) {
    if (tileset_metadata->name.compare(name) == 0) {
      found_tileset_metadata = (*tileset_metadata);
    }
  }

  return found_tileset_metadata;
}

//---------------------------------------------------------------------------
Tileset_Helper::Subset Background_Controller::GetCurrentBackgroundSubset() {
  Tileset_Helper::Subset found_subset;

  std::list<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = m_current_tileset.tilesets.begin(); tileset != m_current_tileset.tilesets.end(); tileset++) {
    if (tileset->type.compare("Background") == 0) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}

//---------------------------------------------------------------------------
void Background_Controller::TileCurrentWindow() {
  float top_left_window_x = (m_game_settings->GetResolution().x / 2) * -1;
  float top_left_window_y = m_game_settings->GetResolution().y / 2;
  m_background_left = top_left_window_x;
  m_background_top = top_left_window_y;
  float current_x = top_left_window_x;
  float current_y = top_left_window_y;
  Tunnelour::Tile_Bitmap* tile;

  float base_tile_size = 128;

  int number_of_y_tiles = 0;
  std::div_t div_y_result =  div(static_cast<int>(m_game_settings->GetResolution().y), base_tile_size);
  if (div_y_result.rem != 0) {
    number_of_y_tiles = div_y_result.quot + 1;
  } else {
    number_of_y_tiles = div_y_result.quot;
  }

  int number_of_x_tiles = 0;
  std::div_t div_x_result = div(static_cast<int>(m_game_settings->GetResolution().x), base_tile_size);
  if (div_x_result.rem != 0) {
    number_of_x_tiles = div_x_result.quot + 1;
  } else {
    number_of_x_tiles = div_x_result.quot;
  }

  for (int y = 0; y < number_of_y_tiles ; y++) {
    std::vector<Tunnelour::Tile_Bitmap*> tile_line;
    for (int x = 0; x < number_of_x_tiles ; x++) {
      tile = CreateTile(base_tile_size);
      tile->SetPosition(D3DXVECTOR3(current_x + (tile->GetSize().x/2),
                                    current_y - (tile->GetSize().y/2),
                                    0)); // Background Z Space is 0
      current_x += static_cast<int>(tile->GetSize().x);
      tile_line.push_back(tile);
      m_background_tiles.push_back(tile);
    }
    current_y -= static_cast<int>(tile->GetSize().y);
    current_x = static_cast<int>(top_left_window_x);
  }

  // Add tiles to the model
  for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = m_background_tiles.begin(); tile != m_background_tiles.end(); ++tile) {
    if (tile + 1 == m_background_tiles.end()) {
      //this is the last tile in the background model
      m_background_bottom = static_cast<int>((*tile)->GetPosition().y + ((*tile)->GetSize().y /2));
      m_background_right = static_cast<int>((*tile)->GetPosition().x + ((*tile)->GetSize().x /2));
    }
    m_model->Add(*tile);
  }

  m_has_init_background_been_generated = true;
}

//---------------------------------------------------------------------------
void Background_Controller::TileUp(float camera_top) {
  std::string error = "Background Up Generation not finished!";
  throw Tunnelour::Exceptions::unfinished_error(error);
}

//---------------------------------------------------------------------------
void Background_Controller::TileDown(float camera_bottom) {
  //Fill Down
  unsigned int number_of_x_tiles = 0;
  std::div_t div_x_result = div(m_background_right - m_background_left, 128);
  if (div_x_result.rem != 0) {
    number_of_x_tiles = div_x_result.quot + 1;
  } else {
    number_of_x_tiles = div_x_result.quot;
  }

  while (camera_bottom < m_background_bottom) {
    for (unsigned int x = 0; x < number_of_x_tiles; x++) {
      Tunnelour::Tile_Bitmap* tile = CreateTile(128);
      D3DXVECTOR3 position;
      position = D3DXVECTOR3(m_background_left + (x * 128) + (128 / 2),
                             m_background_bottom - (tile->GetSize().y / 2),
                             0);
      tile->SetPosition(position);
      m_model->Add(tile);
      m_background_tiles.push_back(tile);
    }
    m_background_bottom -= 128;
  }
}

//---------------------------------------------------------------------------
void Background_Controller::TileRight(float camera_right) {
  //Fill right
  int number_of_y_tiles = 0;
  std::div_t div_y_result =  div(m_background_top - m_background_bottom, 128);
  if (div_y_result.rem != 0) {
    number_of_y_tiles = div_y_result.quot + 1;
  } else {
    number_of_y_tiles = div_y_result.quot;
  }

  while (camera_right > m_background_right) {
    for (int x = 0; x < number_of_y_tiles; x++) {
      Tunnelour::Tile_Bitmap* tile = CreateTile(128);
      D3DXVECTOR3 position = D3DXVECTOR3(static_cast<float>(m_background_right + (tile->GetSize().y / 2)),
                                         static_cast<float>(m_background_top - (x * 128) - (128 / 2)),
                                         static_cast<float>(0));
      tile->SetPosition(position);
      m_model->Add(tile);
      m_background_tiles.push_back(tile);
    }
    m_background_right += 128;
  }
}

//---------------------------------------------------------------------------
void Background_Controller::TileLeft(float camera_left) {
      //Fill left
      int number_of_y_tiles = 0;
      std::div_t div_y_result =  div(m_background_top - m_background_bottom, 128);
      if (div_y_result.rem != 0) {
        number_of_y_tiles = div_y_result.quot + 1;
      } else {
        number_of_y_tiles = div_y_result.quot;
      }

      while (camera_left < m_background_left) {
        for (int x = 0; x < number_of_y_tiles; x++) {
          Tunnelour::Tile_Bitmap* tile = CreateTile(128);
          D3DXVECTOR3 position = D3DXVECTOR3(static_cast<float>(m_background_left - (tile->GetSize().y / 2)),
                                             static_cast<float>(m_background_top - (x * 128) - (128 / 2)),
                                             static_cast<float>(0));
          tile->SetPosition(position);
          m_model->Add(tile);
          m_background_tiles.push_back(tile);
        }
        m_background_left -= 128;
      }
}

//---------------------------------------------------------------------------
void Background_Controller::SwitchTileset() {
  // Debug Mode has been activated or Deactivated
  m_is_debug_mode = m_game_settings->IsDebugMode(); 
  if (m_is_debug_mode) {
    m_current_tileset = GetNamedTileset("Debug");
  } else {
    m_current_tileset = GetNamedTileset("Dirt");
  }

  m_current_background_subset = GetCurrentBackgroundSubset();

  for (std::vector<Tunnelour::Tile_Bitmap*>::iterator tile = m_background_tiles.begin(); tile != m_background_tiles.end(); ++tile) {

    Tileset_Helper::Line middleground_line;
    std::list<Tileset_Helper::Line>::iterator line;
    for (line = m_current_background_subset.lines.begin(); line != m_current_background_subset.lines.end(); line++) {
      if (line->tile_size_x == (*tile)->GetSize().x && line->tile_size_y == (*tile)->GetSize().y) {
        middleground_line = *line;
      }
    }

    std::wstring texture_path = m_game_settings->GetTilesetPath();
    texture_path += String_Helper::StringToWString(m_current_tileset.filename);
    (*tile)->GetTexture()->texture_path = texture_path;
    (*tile)->GetTexture()->texture_size = D3DXVECTOR2(static_cast<float>(m_current_tileset.size_x),
                                                    static_cast<float>(m_current_tileset.size_y));

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
}  // namespace Tunnelour
