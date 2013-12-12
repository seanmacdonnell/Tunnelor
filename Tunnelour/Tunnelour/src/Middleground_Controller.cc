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
Middleground_Controller::Middleground_Controller() : Controller() {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
}

//------------------------------------------------------------------------------
Middleground_Controller::~Middleground_Controller() {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
}

//------------------------------------------------------------------------------
bool Middleground_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));

  Middleground_Controller_Mutator mutator;
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
    m_current_middleground_subset = GetCurrentMiddlegroundSubset();

    CreateLevel();

    m_has_been_initialised = true;
  } else {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Middleground_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }

  if (m_current_level_name.compare(m_level->GetCurrentLevel().level_name) != 0) {
    DestroyLevel();
    CreateLevel();
  } else {
    // If Camera is over an area with no tiles
    D3DXVECTOR2 game_resolution = m_game_settings->GetResolution();
    D3DXVECTOR3 camera_position = m_camera->GetPosition();

    float camera_top = (camera_position.y + (game_resolution.y / 2));
    float camera_bottom = (camera_position.y - (game_resolution.y / 2));
    float camera_left = (camera_position.x - (game_resolution.x / 2));
    float camera_right = (camera_position.x + (game_resolution.x / 2));

    float middleground_top = (*m_top_edge_tiles.begin())->GetTopLeftPostion().y;
    float middleground_left = (*m_left_edge_tiles.begin())->GetTopLeftPostion().x;
    float middleground_bottom = (*m_bottom_edge_tiles.begin())->GetBottomRightPostion().y;
    float middleground_right = (*m_right_edge_tiles.begin())->GetBottomRightPostion().x;
  
    if ((camera_right + game_resolution.x) > middleground_right) {
      while ((camera_right + game_resolution.x) > middleground_right) {
        TileRight(camera_right, middleground_right);
        middleground_right = (*m_right_edge_tiles.begin())->GetBottomRightPostion().x;
      }
    }
  
    if (camera_left - (game_resolution.x) < middleground_left) {
      while ((camera_left - game_resolution.x) < middleground_left) {
        TileLeft(camera_left, middleground_left);
        middleground_left = (*m_left_edge_tiles.begin())->GetTopLeftPostion().x;
      }
    }

    if (camera_top + (game_resolution.y) > middleground_top) {
      while ((camera_top + game_resolution.y) > middleground_top) {
        TileUp(camera_top, middleground_top);
        middleground_top = (*m_top_edge_tiles.begin())->GetTopLeftPostion().y;
      }
    }

    if (camera_bottom - (game_resolution.y) < middleground_bottom) {
      while ((camera_bottom - game_resolution.y) < middleground_bottom) {
        TileDown(camera_bottom, middleground_bottom);
        middleground_bottom = (*m_bottom_edge_tiles.begin())->GetBottomRightPostion().y;
      }
    }
  
    if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
      SwitchTileset();
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
void Middleground_Controller::CreateLevel() {
  m_current_level_name = m_level->GetCurrentLevel().level_name;
  std::vector<Tile_Bitmap*> tiles = GenerateTunnelFromMetadata(m_level->GetCurrentLevel());

  // Add tiles to Model
  for (std::vector<Tile_Bitmap*>::iterator tile = tiles.begin(); tile != tiles.end(); ++tile) {
    if ((*tile)->GetTexture()->transparency != 0.0f) {
      m_model->Add(*tile);
      m_middleground_tiles.push_back(*tile);
    }
  }
}

//------------------------------------------------------------------------------
void Middleground_Controller::DestroyLevel() {
  // Add tiles to Model
  for (std::vector<Tile_Bitmap*>::iterator tile = m_middleground_tiles.begin(); tile != m_middleground_tiles.end(); ++tile) {
    m_model->Remove(*tile);
  }
  m_middleground_tiles.clear();
  m_top_edge_tiles.clear();
  m_bottom_edge_tiles.clear();
  m_left_edge_tiles.clear();
  m_right_edge_tiles.clear();
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Middleground_Controller::GenerateTunnelFromMetadata(Level_Component::Level_Metadata level_metadata) {
  LoadTilesetMetadata();

  std::vector<Tile_Bitmap*> tiles;
  std::vector<std::vector<Tile_Bitmap*>> middleground_lines;
  
  std::vector<std::vector<Level_Component::Tile_Metadata>>::iterator line;
  for (line = level_metadata.level.begin(); line != level_metadata.level.end(); line++) {
    std::vector<Level_Component::Tile_Metadata>::iterator tile;
    std::vector<Tile_Bitmap*> middleground_line;
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
      float position_z = -1;
      // X position is the sum of all the tile sizes before it.
      std::vector<Level_Component::Tile_Metadata>::iterator x_sum_iterator;
      for (x_sum_iterator = (*line).begin(); x_sum_iterator != tile; x_sum_iterator++) {
        position_x += (*x_sum_iterator).size;
      }
      position_x += ((*tile).size / 2);
      // I'm setting the position here so I can use DoTheseTilesXCollide
      new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));

      // Check the line above for any X colliding tiles
      if (!middleground_lines.empty()) {
        std::vector<Tile_Bitmap*>::iterator y_sum_iterator;
        for (y_sum_iterator = middleground_lines.back().begin(); y_sum_iterator != middleground_lines.back().end(); y_sum_iterator++) {
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

      if ((*tile).type.compare("Middleground") == 0) {
        new_tile->GetTexture()->transparency = 1.0f;
        if (!middleground_line.empty()) {
          if (middleground_line.back()->GetTexture()->transparency == 0.0f) {
            new_tile->SetIsWall(true);
            m_tunnel_edge_tiles.push_back(new_tile);
          }
          if (!middleground_lines.empty()) {
            std::vector<Tile_Bitmap*>::iterator above_tile;
            for (above_tile = middleground_lines.back().begin(); above_tile != middleground_lines.back().end(); above_tile++) {
              if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile)) {
                if ((*above_tile)->GetTexture()->transparency == 0.0f) {
                  new_tile->SetIsFloor(true);
                  m_tunnel_edge_tiles.push_back(new_tile);
                }
              }
            }
          }
        }
      } else {
        new_tile->GetTexture()->transparency = 0.0f; //TUNNEL
        if (middleground_line.back()->GetTexture()->transparency != 0.0f) {
          middleground_line.back()->SetIsWall(true);
          m_tunnel_edge_tiles.push_back(middleground_line.back());
        }
        if (!middleground_lines.empty()) {
          std::vector<Tile_Bitmap*>::iterator above_tile;
          for (above_tile = middleground_lines.back().begin(); above_tile != middleground_lines.back().end(); above_tile++) {
            if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile)) {
              if ((*above_tile)->GetTexture()->transparency != 0.0f) {
                (*above_tile)->SetIsRoof(true);
                m_tunnel_edge_tiles.push_back((*above_tile));
              }
            }
          }
        }
      }

      ResetTileTexture(new_tile);

      middleground_line.push_back(new_tile);
      tiles.push_back(new_tile);
    }
    middleground_lines.push_back(middleground_line);
  }

  return tiles;
}

//---------------------------------------------------------------------------
void Middleground_Controller::LoadTilesetMetadata() {
  Tileset_Helper::Tileset_Metadata debug_tileset_metadata;
  m_debug_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Debug_Tileset_0_5.txt");
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
  m_current_middleground_subset = GetCurrentMiddlegroundSubset();
}

//---------------------------------------------------------------------------
Tileset_Helper::Tileset_Metadata Middleground_Controller::GetNamedTileset(std::string name) {
  Tileset_Helper::Tileset_Metadata found_tileset_metadata;

  std::vector<Tileset_Helper::Tileset_Metadata>::iterator tileset_metadata;
  for (tileset_metadata = m_tilesets.begin(); tileset_metadata != m_tilesets.end(); tileset_metadata++) {
    if (tileset_metadata->name.compare(name) == 0) {
      found_tileset_metadata = (*tileset_metadata);
    }
  }

  return found_tileset_metadata;
}

//---------------------------------------------------------------------------
Tileset_Helper::Subset Middleground_Controller::GetCurrentMiddlegroundSubset() {
  Tileset_Helper::Subset found_subset;

  std::vector<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = m_current_tileset.tilesets.begin(); tileset != m_current_tileset.tilesets.end(); tileset++) {
    if (tileset->type.compare("Middleground") == 0) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}

//------------------------------------------------------------------------------
Tile_Bitmap* Middleground_Controller::CreateTile(float base_tile_size) {
  Tileset_Helper::Line middleground_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  for (line = m_current_middleground_subset.lines.begin(); line != m_current_middleground_subset.lines.end(); line++) {
    if (line->tile_size_x == base_tile_size) {
      if (line->tile_size_y == base_tile_size) {
        middleground_line = *line;
      }
    }
  }

  Tile_Bitmap* tile = new Tile_Bitmap();
  tile->SetPosition(D3DXVECTOR3(0, 0, -1));
  tile->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_current_tileset.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(m_current_tileset.size_x,
                                                 m_current_tileset.size_y);
  tile->GetTexture()->tile_size = D3DXVECTOR2(middleground_line.tile_size_x,
                                              middleground_line.tile_size_y);
  tile->SetIsCollidable(true);

  int random_line_tile = rand() % middleground_line.number_of_tiles;

  float random_tile_x = random_line_tile * middleground_line.tile_size_x;
  random_tile_x += middleground_line.top_left_x;

  float random_tile_y = middleground_line.top_left_y;

  tile->GetTexture()->top_left_position = D3DXVECTOR2(random_tile_x, random_tile_y);

  tile->SetSize(D3DXVECTOR2(base_tile_size, base_tile_size));

  return tile;
}

//---------------------------------------------------------------------------
Tileset_Helper::Line Middleground_Controller::GetCurrentSizedLine(float size) {
  Tileset_Helper::Line middleground_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  for (line = m_current_middleground_subset.lines.begin(); line != m_current_middleground_subset.lines.end(); line++) {
    if (line->tile_size_x == size) {
      if (line->tile_size_y == size) {
        middleground_line = *line;
      }
    }
  }
  return middleground_line;
}

//---------------------------------------------------------------------------
void Middleground_Controller::ResetTileTexture(Tile_Bitmap *out_tile) {
  Tileset_Helper::Line middleground_line = GetCurrentSizedLine(out_tile->GetSize().x);
  int random_variable = 0;
  if (m_is_debug_mode) {
    if (out_tile->IsEdge()) {
      random_variable = 1;
    } else {
      random_variable = 0;
    }   
  } else {
    random_variable = rand() % middleground_line.number_of_tiles;
  }

  float left = random_variable * middleground_line.tile_size_x + middleground_line.top_left_x;
  float top = middleground_line.top_left_y;
  out_tile->GetTexture()->top_left_position = D3DXVECTOR2(left, top);
  out_tile->GetTexture()->texture = 0;
  out_tile->GetFrame()->vertex_buffer = 0;
  out_tile->Init();
}

//---------------------------------------------------------------------------
void Middleground_Controller::SwitchTileset() {
  // Debug Mode has been activated or Deactivated
  m_is_debug_mode = m_game_settings->IsDebugMode();
  if (m_is_debug_mode) {
    m_current_tileset = GetNamedTileset("Debug");
  } else {
    m_current_tileset = GetNamedTileset("Dirt");
  }

  m_current_middleground_subset = GetCurrentMiddlegroundSubset();
  std::vector<Tile_Bitmap*>::iterator tile;
  for (tile = m_middleground_tiles.begin(); tile != m_middleground_tiles.end(); ++tile) {
    Tileset_Helper::Line middleground_line = GetCurrentSizedLine((*tile)->GetSize().x);
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
void Middleground_Controller::TileUp(float camera_top, float middleground_top) {
  std::vector<Tile_Bitmap*> new_top_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_top_edge_tiles.begin(); edge_tile != m_top_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x;
    position.y = (*edge_tile)->GetPosition()->y + tile->GetSize().y;
    position.z = -1.0; // Middleground Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
    m_model->Add(tile);
    m_middleground_tiles.push_back(tile);
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
void Middleground_Controller::TileDown(float camera_bottom, float middleground_bottom) {
  std::vector<Tile_Bitmap*> new_bottom_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_bottom_edge_tiles.begin(); edge_tile != m_bottom_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x;
    position.y = (*edge_tile)->GetPosition()->y - tile->GetSize().y;
    position.z = -1.0; // Middleground Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
    m_model->Add(tile);
    m_middleground_tiles.push_back(tile);
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
void Middleground_Controller::TileRight(float camera_right, float middleground_right) {
  std::vector<Tile_Bitmap*> new_right_edge_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_right_edge_tiles.begin(); edge_tile != m_right_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x + tile->GetSize().x;
    position.y = (*edge_tile)->GetPosition()->y;
    position.z = -1.0; // Middleground Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
    m_model->Add(tile);
    m_middleground_tiles.push_back(tile);
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
void Middleground_Controller::TileLeft(float camera_left, float middleground_left) {
  std::vector<Tile_Bitmap*> new_left_edge_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_left_edge_tiles.begin(); edge_tile != m_left_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x - tile->GetSize().x;
    position.y = (*edge_tile)->GetPosition()->y;
    position.z = -1.0; // Middleground Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
    m_model->Add(tile);
    m_middleground_tiles.push_back(tile);
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