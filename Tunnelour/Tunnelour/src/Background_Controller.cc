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
#include <string>
#include "Exceptions.h"
#include "String_Helper.h"
#include "Bitmap_Helper.h"

namespace Tunnelour {

//------------------------------------------------------------------------------
// public:
//------------------------------------------------------------------------------
Background_Controller::Background_Controller() : Controller() {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
}

//------------------------------------------------------------------------------
Background_Controller::~Background_Controller() {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
}

//------------------------------------------------------------------------------
bool Background_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));

  Background_Controller_Mutator mutator;
  m_model->Apply(&mutator);
  if (mutator.WasSuccessful()) {
    m_game_settings = mutator.GetGameSettings();
    m_camera = mutator.GetCamera();
    m_level = mutator.GetLevel();
    m_is_debug_mode = m_game_settings->IsDebugMode();

    m_is_debug_mode = m_game_settings->IsDebugMode();
    if (m_is_debug_mode) {
      m_current_tileset = GetNamedTileset("Debug");
    } else {
      m_current_tileset = GetNamedTileset("Dirt");
    }
    m_current_background_subset = GetCurrentBackgroundSubset();

    CreateLevel();

    m_has_been_initialised = true;
  } else {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Background_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }
 
  if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
    SwitchTileset();
  }

  return true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Background_Controller::CreateLevel() {
  std::vector<Tile_Bitmap*> tiles = GenerateTunnelFromMetadata(m_level->GetCurrentLevel());

  // Add tiles to Model
  for (std::vector<Tile_Bitmap*>::iterator tile = tiles.begin(); tile != tiles.end(); ++tile) {
    if ((*tile)->GetTexture()->transparency != 0.0f) {
      m_model->Add(*tile);
      m_background_tiles.push_back(*tile);
    }
  }
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Background_Controller::GenerateTunnelFromMetadata(Level_Component::Level_Metadata level_metadata) {
  LoadTilesetMetadata();

  std::vector<Tile_Bitmap*> tiles;
  std::vector<std::vector<Tile_Bitmap*>> background_lines;
  
  std::vector<std::vector<Level_Component::Tile_Metadata>>::iterator line;
  for (line = level_metadata.level.begin(); line != level_metadata.level.end(); line++) {
    std::vector<Level_Component::Tile_Metadata>::iterator tile;
    std::vector<Tile_Bitmap*> background_line;
    for (tile = (*line).begin(); tile != (*line).end(); tile++) {
      Tile_Bitmap* new_tile = new_tile = CreateTile((*tile).size);
      if (tile == (*line).begin()) {
        new_tile->SetIsLeftEdge(true);
        m_left_edge_tiles.push_back(new_tile);
      }
      if (tile == (*line).end() - 1) {
        new_tile->SetIsRightEdge(true);
        m_right_edge_tiles.push_back(new_tile);
      }
      if (line == level_metadata.level.begin()) {
        new_tile->SetIsTopEdge(true);
        m_top_edge_tiles.push_back(new_tile);
      }
      if (line == level_metadata.level.end() - 1) {
        new_tile->SetIsBottomEdge(true);
        m_bottom_edge_tiles.push_back(new_tile);
      }

      float position_x = 0;
      float position_y = 0;
      float position_z = 0;
      // X position is the sum of all the tile sizes before it.
      std::vector<Level_Component::Tile_Metadata>::iterator x_sum_iterator;
      for (x_sum_iterator = (*line).begin(); x_sum_iterator != tile; x_sum_iterator++) {
        position_x += (*x_sum_iterator).size;
      }
      position_x += ((*tile).size / 2);
      // I'm setting the position here so I can use DoTheseTilesXCollide
      new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));

      // Check the line above for any X colliding tiles
      if (!background_lines.empty()) {
        std::vector<Tile_Bitmap*>::iterator y_sum_iterator;
        for (y_sum_iterator = background_lines.back().begin(); y_sum_iterator != background_lines.back().end(); y_sum_iterator++) {
          if (Bitmap_Helper::DoTheseTilesXCollide((*y_sum_iterator), new_tile) ||
            Bitmap_Helper::AreTheseTilesXAdjacent((*y_sum_iterator), new_tile)) {
            position_y = (*y_sum_iterator)->GetPosition()->y;
          }
        }
        position_y -= ((*tile).size);
      } else {
        position_y -= ((*tile).size / 2);
      }

      new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));

      if ((*tile).type.compare("Tunnel") == 0) {
        new_tile->GetTexture()->transparency = 1.0f;
        if (!background_line.empty()) {
          if (background_line.back()->GetTexture()->transparency == 0.0f) {
            m_tunnel_edge_tiles.push_back(new_tile);
          }
          if (!background_lines.empty()) {
            std::vector<Tile_Bitmap*>::iterator above_tile;
            for (above_tile = background_lines.back().begin(); above_tile != background_lines.back().end(); above_tile++) {
              if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile)) {
                if ((*above_tile)->GetTexture()->transparency == 0.0f) {
                  m_tunnel_edge_tiles.push_back(new_tile);
                }
              }
            }
          }
        }
      } else {
        new_tile->GetTexture()->transparency = 0.0f; //Middleground
        if (!background_line.empty()) {
          if (background_line.back()->GetTexture()->transparency != 0.0f) {
            m_tunnel_edge_tiles.push_back(background_line.back());
          }
          if (!background_lines.empty()) {
            std::vector<Tile_Bitmap*>::iterator above_tile;
            for (above_tile = background_lines.back().begin(); above_tile != background_lines.back().end(); above_tile++) {
              if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile)) {
                if ((*above_tile)->GetTexture()->transparency != 0.0f) {
                  m_tunnel_edge_tiles.push_back((*above_tile));
                }
              }
            }
          }
        }
      }

      ResetTileTexture(new_tile);

      background_line.push_back(new_tile);
      tiles.push_back(new_tile);
    }
    background_lines.push_back(background_line);
  }

  return tiles;
}

//---------------------------------------------------------------------------
void Background_Controller::LoadTilesetMetadata() {
  Tileset_Helper::Tileset_Metadata debug_tileset_metadata;
  m_debug_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Debug_Tileset_0_4.txt");
  Tileset_Helper::LoadTilesetMetadataIntoStruct(m_debug_metadata_file_path, &debug_tileset_metadata);
  m_tilesets.push_back(debug_tileset_metadata);

  Tileset_Helper::Tileset_Metadata dirt_tileset_metadata;
  m_dirt_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Dirt_Tileset_5.txt");
  Tileset_Helper::LoadTilesetMetadataIntoStruct(m_dirt_metadata_file_path, &dirt_tileset_metadata);
  m_tilesets.push_back(dirt_tileset_metadata);

  if (m_game_settings->IsDebugMode()) {
    m_current_tileset = GetNamedTileset("Debug");
  } else {
    m_current_tileset = GetNamedTileset("Dirt");
  }
  m_current_background_subset = GetCurrentBackgroundSubset();
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

//------------------------------------------------------------------------------
Tile_Bitmap* Background_Controller::CreateTile(float base_tile_size) {
  Tileset_Helper::Line background_line;
  std::list<Tileset_Helper::Line>::iterator line;
  for (line = m_current_background_subset.lines.begin(); line != m_current_background_subset.lines.end(); line++) {
    if (line->tile_size_x == base_tile_size) {
      if (line->tile_size_y == base_tile_size) {
        background_line = *line;
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
  tile->GetTexture()->tile_size = D3DXVECTOR2(background_line.tile_size_x,
                                              background_line.tile_size_y);
  tile->SetIsCollidable(true);

  int random_line_tile = rand() % background_line.number_of_tiles;

  float random_tile_x = random_line_tile * background_line.tile_size_x;
  random_tile_x += background_line.top_left_x;

  float random_tile_y = background_line.top_left_y;

  tile->GetTexture()->top_left_position = D3DXVECTOR2(random_tile_x, random_tile_y);

  tile->SetSize(D3DXVECTOR2(base_tile_size, base_tile_size));

  return tile;
}

//---------------------------------------------------------------------------
Tileset_Helper::Line Background_Controller::GetCurrentSizedLine(float size) {
  Tileset_Helper::Line background_line;
  std::list<Tileset_Helper::Line>::iterator line;
  for (line = m_current_background_subset.lines.begin(); line != m_current_background_subset.lines.end(); line++) {
    if (line->tile_size_x == size) {
      if (line->tile_size_y == size) {
        background_line = *line;
      }
    }
  }
  return background_line;
}

//---------------------------------------------------------------------------
void Background_Controller::ResetTileTexture(Tile_Bitmap *out_tile) {
  Tileset_Helper::Line background_line = GetCurrentSizedLine(out_tile->GetSize().x);
  int random_variable = 0;
  if (m_is_debug_mode) {
    if (out_tile->IsEdge()) {
      random_variable = 1;
    } else {
      random_variable = 0;
    }   
  } else {
    random_variable = rand() % background_line.number_of_tiles;
  }

  float left = random_variable * background_line.tile_size_x + background_line.top_left_x;
  float top = background_line.top_left_y;
  out_tile->GetTexture()->top_left_position = D3DXVECTOR2(left, top);
  out_tile->GetTexture()->texture = 0;
  out_tile->GetFrame()->vertex_buffer = 0;
  out_tile->Init();
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
    Tileset_Helper::Line background_line = GetCurrentSizedLine((*tile)->GetSize().x);
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
void Background_Controller::TileUp(float camera_top, float background_top) {
  std::vector<Tile_Bitmap*> new_top_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_top_edge_tiles.begin(); edge_tile != m_top_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x;
    position.y = (*edge_tile)->GetPosition()->y + tile->GetSize().y;
    position.z = 0.0; // Background Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
    m_model->Add(tile);
    m_background_tiles.push_back(tile);
    new_top_tiles.push_back(tile);

    (*edge_tile)->SetIsTopEdge(false);
    if (tile->IsBottomEdge()) {
      m_bottom_edge_tiles.push_back(tile);
    }
    if (tile->IsRightEdge()) {
      m_right_edge_tiles.push_back(tile);
    }
    if (tile->IsLeftEdge()) {
      m_left_edge_tiles.push_back(tile);
    }
    if (m_is_debug_mode) {
      ResetTileTexture((*edge_tile));
      ResetTileTexture(tile);
    }
  }
  m_top_edge_tiles.clear();

  m_top_edge_tiles = new_top_tiles;
}

//---------------------------------------------------------------------------
void Background_Controller::TileDown(float camera_bottom, float background_bottom) {
  std::vector<Tile_Bitmap*> new_bottom_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_bottom_edge_tiles.begin(); edge_tile != m_bottom_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x;
    position.y = (*edge_tile)->GetPosition()->y - tile->GetSize().y;
    position.z = 0.0; // Background Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
    m_model->Add(tile);
    m_background_tiles.push_back(tile);
    new_bottom_tiles.push_back(tile);

    (*edge_tile)->SetIsBottomEdge(false);
    if (tile->IsTopEdge()) {
      m_top_edge_tiles.push_back(tile);
    }
    if (tile->IsRightEdge()) {
      m_right_edge_tiles.push_back(tile);
    }
    if (tile->IsLeftEdge()) {
      m_left_edge_tiles.push_back(tile);
    }
    if (m_is_debug_mode) {
      ResetTileTexture((*edge_tile));
      ResetTileTexture(tile);
    }
  }
  m_bottom_edge_tiles.clear();

  m_bottom_edge_tiles = new_bottom_tiles;
}

//---------------------------------------------------------------------------
void Background_Controller::TileRight(float camera_right, float background_right) {
  std::vector<Tile_Bitmap*> new_right_edge_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_right_edge_tiles.begin(); edge_tile != m_right_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x + tile->GetSize().x;
    position.y = (*edge_tile)->GetPosition()->y;
    position.z = 0.0; // Background Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
    m_model->Add(tile);
    m_background_tiles.push_back(tile);
    new_right_edge_tiles.push_back(tile);

    (*edge_tile)->SetIsRightEdge(false);
    if (tile->IsTopEdge()) {
      m_top_edge_tiles.push_back(tile);
    }
    if (tile->IsBottomEdge()) {
      m_bottom_edge_tiles.push_back(tile);
    }
    if (tile->IsLeftEdge()) {
      m_left_edge_tiles.push_back(tile);
    }
    if (m_is_debug_mode) {
      ResetTileTexture((*edge_tile));
      ResetTileTexture(tile);
    }
  }
  m_right_edge_tiles.clear();

  m_right_edge_tiles = new_right_edge_tiles;
}

//---------------------------------------------------------------------------
void Background_Controller::TileLeft(float camera_left, float background_left) {
  std::vector<Tile_Bitmap*> new_left_edge_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_left_edge_tiles.begin(); edge_tile != m_left_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x - tile->GetSize().x;
    position.y = (*edge_tile)->GetPosition()->y;
    position.z = 0.0; // Background Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
    m_model->Add(tile);
    m_background_tiles.push_back(tile);
    new_left_edge_tiles.push_back(tile);

    (*edge_tile)->SetIsLeftEdge(false);
    if (tile->IsTopEdge()) {
      m_top_edge_tiles.push_back(tile);
    }
    if (tile->IsBottomEdge()) {
      m_bottom_edge_tiles.push_back(tile);
    }
    if (tile->IsRightEdge()) {
      m_right_edge_tiles.push_back(tile);
    }
    if (m_is_debug_mode) {
      ResetTileTexture((*edge_tile));
      ResetTileTexture(tile);
    }
  }
  m_left_edge_tiles.clear();

  m_left_edge_tiles = new_left_edge_tiles;
}

} // Tunnelour