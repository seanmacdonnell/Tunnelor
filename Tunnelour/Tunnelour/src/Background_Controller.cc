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

#include <math.h>
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
void Background_Controller::Init(Component_Composite * const model) {
    Controller::Init(model);
    std::srand(static_cast<unsigned int>(std::time(0)));
}

//------------------------------------------------------------------------------
void Background_Controller::Run() {
  // Generate an initial Random Tile Background
  if (!m_has_init_background_been_generated) {
    // Get game settings component from the model with the Mutator.
    Background_Controller_Mutator mutator;
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

    D3DXVECTOR2 game_resolution = m_game_settings->GetResolution();
    D3DXVECTOR3 camera_position = m_camera->GetPosition();

    camera_top = (camera_position.y + (game_resolution.y / 2));
    camera_bottom = (camera_position.y - (game_resolution.y / 2));
    camera_left = (camera_position.x - (game_resolution.x / 2));
    camera_right = (camera_position.x + (game_resolution.x / 2));

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
Tile_Bitmap* Background_Controller::CreateTile(float base_tile_size,
                                                          bool is_top_edge,
                                                          bool is_bottom_edge,
                                                          bool is_right_edge,
                                                          bool is_left_edge) {
  Tileset_Helper::Line middleground_line;
  std::list<Tileset_Helper::Line>::iterator line;
  for (line = m_current_background_subset.lines.begin(); line != m_current_background_subset.lines.end(); line++) {
    if (line->tile_size_x == base_tile_size) {
      if (line->tile_size_y == base_tile_size) {
        middleground_line = *line;
      }
    }
  }

  Tile_Bitmap* tile = new Tile_Bitmap();
  tile->SetPosition(D3DXVECTOR3(0, 0, 0));
  tile->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_current_tileset.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(m_current_tileset.size_x,
                                                 m_current_tileset.size_y);
  tile->GetTexture()->tile_size = D3DXVECTOR2(middleground_line.tile_size_x,
                                              middleground_line.tile_size_y);

  tile->Set_Is_Top_Edge(is_top_edge);
  tile->Set_Is_Bottom_Edge(is_bottom_edge);
  tile->Set_Is_Right_Edge(is_right_edge);
  tile->Set_Is_Left_Edge(is_left_edge);

  unsigned int random_line_tile = 0;
  if (m_is_debug_mode) {
    if (tile->IsEdge()) {
      random_line_tile = 1;
    } else {
      random_line_tile = 0;
    }
  } else {
    random_line_tile = rand() % middleground_line.number_of_tiles;
  }

  float random_tile_x = (random_line_tile * middleground_line.tile_size_x);
  random_tile_x += middleground_line.top_left_x;

  float random_tile_y = middleground_line.top_left_y;

  tile->GetTexture()->top_left_position = D3DXVECTOR2(random_tile_x, random_tile_y);

  tile->SetSize(D3DXVECTOR2(base_tile_size, base_tile_size));

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
  float top_left_window_y = (m_game_settings->GetResolution().y / 2);
  m_background_left = top_left_window_x;
  m_background_top = top_left_window_y;
  float current_x = top_left_window_x;
  float current_y = top_left_window_y;
  float base_tile_size = 128.0;

  unsigned int number_of_y_tiles = 0;
  float y_rem = m_game_settings->GetResolution().y / base_tile_size;
  number_of_y_tiles = static_cast<unsigned int>(std::ceil(y_rem));
  number_of_y_tiles -= 1;  // Because 0 is the first tile

  unsigned int number_of_x_tiles = 0;
  float x_rem = m_game_settings->GetResolution().x / base_tile_size;
  number_of_x_tiles = static_cast<unsigned int>(std::ceil(x_rem));
  number_of_x_tiles -= 1;  // Because 0 is the first tile

  for (unsigned int y = 0; y <= number_of_y_tiles ; y++) {
    std::vector<Tile_Bitmap*> tile_line;
    Tile_Bitmap* new_tile;
    for (unsigned int x = 0; x <= number_of_x_tiles ; x++) {
      bool is_top_edge = false;
      bool is_bottom_edge = false;
      bool is_right_edge = false;
      bool is_left_edge = false;
      if (y == 0) {
        is_top_edge = true;
      } else if (y == number_of_y_tiles) {
        is_bottom_edge = true;
      }
      if (x == 0) {
        is_left_edge = true;
      } else if (x == number_of_x_tiles) {
        is_right_edge = true;
      }

      new_tile = CreateTile(base_tile_size,
                            is_top_edge,
                            is_bottom_edge,
                            is_right_edge,
                            is_left_edge);
      float position_x = current_x + (new_tile->GetSize().x / 2);
      float position_y = current_y - (new_tile->GetSize().y / 2);
      new_tile->SetPosition(D3DXVECTOR3(position_x,
                                        position_y,
                                        0)); // Background Z Space is 0

      if (new_tile->Is_Top_Edge()) {
        m_top_edge_tiles.push_back(new_tile);
      } else if (new_tile->Is_Bottom_Edge()) {
        m_bottom_edge_tiles.push_back(new_tile);
      }

      if (new_tile->Is_Right_Edge()) {
        m_right_edge_tiles.push_back(new_tile);
      } else if (new_tile->Is_Left_Edge()) {
        m_left_edge_tiles.push_back(new_tile);
      }

      current_x += static_cast<int>(new_tile->GetSize().x);
      tile_line.push_back(new_tile);
      m_background_tiles.push_back(new_tile);
    }
    current_y -= new_tile->GetSize().y;
    current_x = top_left_window_x;
  }

  // Add tiles to the model
  std::vector<Tile_Bitmap*>::iterator tile_it;
  for (tile_it = m_background_tiles.begin(); tile_it != m_background_tiles.end(); ++tile_it) {
     // this is the last tile in the background model
    if ((*tile_it)->GetBottomRightPostion().x > m_background_right) {
      m_background_right = (*tile_it)->GetBottomRightPostion().x;
    }
    if ((*tile_it)->GetBottomRightPostion().x < m_background_bottom) {
      m_background_bottom = (*tile_it)->GetBottomRightPostion().y;
    }
    m_model->Add(*tile_it);
  }

  m_has_init_background_been_generated = true;
}

//---------------------------------------------------------------------------
void Background_Controller::TileUp(float camera_top) {
  std::string error = "Background Up Generation not finished!";
  throw Exceptions::unfinished_error(error);
}

//---------------------------------------------------------------------------
void Background_Controller::TileDown(float camera_bottom) {
  unsigned int number_of_x_tiles = 0;
  float rem = (m_background_right - m_background_left) / 128;
  number_of_x_tiles = static_cast<unsigned int>(std::ceil(rem));

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_bottom_edge_tiles.begin(); edge_tile != m_bottom_edge_tiles.end(); edge_tile++) {
    (*edge_tile)->Set_Is_Bottom_Edge(false);
    ResetTileTexture((*edge_tile));
  }
  m_bottom_edge_tiles.clear();

  for (unsigned int x = 0; x < number_of_x_tiles; x++) {
    Tile_Bitmap* tile = CreateTile(128,
                                              false,
                                              true,
                                              false,
                                              false);
    D3DXVECTOR3 position;
    position.x = m_background_left + (x * 128) + (128 / 2);
    position.y = m_background_bottom - (tile->GetSize().y / 2);
    position.z = 0.0; // Background Z Space is 0
    tile->SetPosition(position);
    tile->Set_Is_Bottom_Edge(true);
    m_model->Add(tile);
    m_background_tiles.push_back(tile);
    m_bottom_edge_tiles.push_back(tile);
  }

  m_background_bottom -= 128;
}

//---------------------------------------------------------------------------
void Background_Controller::TileRight(float camera_right) {
  unsigned int number_of_y_tiles = 0;
  float rem = (m_background_top - m_background_bottom) / 128;
  number_of_y_tiles = static_cast<unsigned int>(std::ceil(rem));

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_right_edge_tiles.begin(); edge_tile != m_right_edge_tiles.end(); edge_tile++) {
    (*edge_tile)->Set_Is_Right_Edge(false);
    ResetTileTexture((*edge_tile));
  }
  m_right_edge_tiles.clear();

  while (camera_right > m_background_right) {
    for (unsigned int x = 0; x < number_of_y_tiles; x++) {
    Tile_Bitmap* tile = CreateTile(128,
                                              false,
                                              false,
                                              true,
                                              false);
      D3DXVECTOR3 position;
      position.x = m_background_right + (tile->GetSize().y / 2);
      position.y = m_background_top - (x * 128) - (128 / 2);
      position.z = 0.0;
      tile->SetPosition(position);
      tile->Set_Is_Right_Edge(true);
      m_model->Add(tile);
      m_background_tiles.push_back(tile);
      m_right_edge_tiles.push_back(tile);
    }
    m_background_right += 128;
  }
}

//---------------------------------------------------------------------------
void Background_Controller::TileLeft(float camera_left) {
  unsigned int number_of_y_tiles = 0;
  float rem = (m_background_top - m_background_bottom) / 128;
  number_of_y_tiles = static_cast<unsigned int>(std::ceil(rem));

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_left_edge_tiles.begin(); edge_tile != m_left_edge_tiles.end(); edge_tile++) {
    (*edge_tile)->Set_Is_Left_Edge(false);
    ResetTileTexture((*edge_tile));
  }
  m_left_edge_tiles.clear();

  while (camera_left < m_background_left) {
    for (unsigned int x = 0; x < number_of_y_tiles; x++) {
    Tile_Bitmap* tile = CreateTile(128,
                                              false,
                                              false,
                                              false,
                                              true);
      D3DXVECTOR3 position;
      position.x = m_background_left - (tile->GetSize().y / 2);
      position.y = m_background_top - (x * 128) - (128 / 2);
      position.z = 0;
      tile->SetPosition(position);
      tile->Set_Is_Left_Edge(true);
      m_model->Add(tile);
      m_background_tiles.push_back(tile);
      m_left_edge_tiles.push_back(tile);
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
  std::vector<Tile_Bitmap*>::iterator tile;
  for (tile = m_background_tiles.begin(); tile != m_background_tiles.end(); ++tile) {
    Tileset_Helper::Line middleground_line = GetCurrentSizedLine((*tile)->GetSize());
    std::wstring texture_path = m_game_settings->GetTilesetPath();
    texture_path += String_Helper::StringToWString(m_current_tileset.filename);
    (*tile)->GetTexture()->texture_path = texture_path;
    D3DXVECTOR2 size = D3DXVECTOR2(m_current_tileset.size_x,
                                   m_current_tileset.size_y);
    (*tile)->GetTexture()->texture_size = size;
    ResetTileTexture((*tile));
  }
}

//---------------------------------------------------------------------------
Tileset_Helper::Line Background_Controller::GetCurrentSizedLine(D3DXVECTOR2 size) {
  Tileset_Helper::Line middleground_line;
  std::list<Tileset_Helper::Line>::iterator line;
  for (line = m_current_background_subset.lines.begin(); line != m_current_background_subset.lines.end(); line++) {
    if (line->tile_size_x == size.x) {
      if (line->tile_size_y == size.y) {
        middleground_line = *line;
      }
    }
  }
  return middleground_line;
}

//---------------------------------------------------------------------------
void Background_Controller::ResetTileTexture(Tile_Bitmap *out_tile) {
  Tileset_Helper::Line middleground_line = GetCurrentSizedLine(out_tile->GetSize());
  int random_variable = 0;
  if (m_is_debug_mode) {
    random_variable = 0;
  } else {
    int random_variable = rand() % middleground_line.number_of_tiles;
  }

  float left = random_variable * middleground_line.tile_size_x + middleground_line.top_left_x;
  float top = middleground_line.top_left_y;
  out_tile->GetTexture()->top_left_position = D3DXVECTOR2(left, top);
  out_tile->GetTexture()->texture = 0;
  out_tile->GetFrame()->vertex_buffer = 0;
  out_tile->Init();
}

}  // namespace Tunnelour
