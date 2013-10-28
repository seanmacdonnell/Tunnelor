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
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
}

//------------------------------------------------------------------------------
Middleground_Controller::~Middleground_Controller() {
  m_game_settings = 0;
  m_camera = 0;
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

  if (camera_right > middleground_right) {
    while ((camera_right + (game_resolution.x / 2)) > middleground_right) {
      TileRight(camera_right, middleground_right);
      middleground_right = (*m_right_edge_tiles.begin())->GetBottomRightPostion().x;
    }
  }
  
  if (camera_left < middleground_left) {
    while ((camera_left - (game_resolution.x / 2)) < middleground_left) {
      TileLeft(camera_left, middleground_left);
      middleground_left = (*m_left_edge_tiles.begin())->GetTopLeftPostion().x;
    }
  }

  if (camera_top > middleground_top) {
    while ((camera_top + (game_resolution.y / 2)) > middleground_top) {
      TileUp(camera_top, middleground_top);
      middleground_top = (*m_top_edge_tiles.begin())->GetTopLeftPostion().y;
    }
  }

  if (camera_bottom < middleground_bottom) {
    while ((camera_bottom - (game_resolution.y / 2)) < middleground_bottom) {
      TileDown(camera_bottom, middleground_bottom);
      middleground_bottom = (*m_bottom_edge_tiles.begin())->GetBottomRightPostion().y;
    }
  }
  
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
void Middleground_Controller::CreateLevel() {
  LoadLevelMetadata();
  std::vector<Tile_Bitmap*> tiles = GenerateTunnelFromMetadata((*m_level_metadata.begin()));

  // Add tiles to Model
  for (std::vector<Tile_Bitmap*>::iterator tile = tiles.begin(); tile != tiles.end(); ++tile) {
    m_model->Add(*tile);
    m_middleground_tiles.push_back(*tile);
  }
}

//---------------------------------------------------------------------------
void Middleground_Controller::LoadLevelMetadata() {
  Level_Metadata level_0_metadata;
  std::string level_0_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_0.txt");
  level_0_metadata = LoadLevelMetadataIntoStruct(level_0_metadata_file_path);
  std::string level_0_csv_file_path = String_Helper::WStringToString(m_game_settings->GetLevelPath() + L"Level_0.csv");
  LoadLevelCSVIntoStruct(level_0_csv_file_path, &level_0_metadata);
  m_level_metadata.push_back(level_0_metadata);
}

//------------------------------------------------------------------------------
Middleground_Controller::Level_Metadata Middleground_Controller::LoadLevelMetadataIntoStruct(std::string metadata_path) {
  Level_Metadata level_metadata;

  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_path.c_str(), "r") != 0) {
    std::string error = "Open Tileset Metadata Failed! " +  metadata_path;
    throw Tunnelour::Exceptions::init_error(error);
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
    if (strcmp(token, "Level_Name") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.level_name = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Name");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_Blurb") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.blurb = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_Blurb");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_CSV_Filename") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.filename = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_CSV_Filename");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TilesetName") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tileset_name = token;
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Level_TilesetName");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TunnelStartTopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tunnel_top_left_x = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Tileset_TopLeftX");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_TunnelStartTopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.tunnel_top_left_y = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Tileset_TopLeftX");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_AvatarStartTopLeftX") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.avatar_top_left_x = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Tileset_TopLeftX");
    }
  }
  fgets(line, 225, pFile);
  if (line != NULL) {
    token = strtok_s(line, " ", &next_token);
    if (strcmp(token, "Level_AvatarStartTopLeftY") == 0)   {
      token = strtok_s(NULL, " =\"", &next_token);
      level_metadata.avatar_top_left_y = static_cast<float>(atof(token));
    } else {
      throw Tunnelour::Exceptions::init_error("Parse Metadata Failed! Expected: Tileset_TopLeftX");
    }
  }
  fclose(pFile);

  return level_metadata;
}

//------------------------------------------------------------------------------
void Middleground_Controller::LoadLevelCSVIntoStruct(std::string metadata_path, Level_Metadata *out_metadata) {
  if (out_metadata == 0) { throw Tunnelour::Exceptions::run_error("Need an initialized strut"); }
  if (out_metadata->filename.compare("") == 0) { throw Tunnelour::Exceptions::run_error("Need a struct with a filename"); }
  Level_Metadata level_metadata;

  FILE * pFile;
  int lSize;

  // Open Font File as a text file
  if (fopen_s(&pFile, metadata_path.c_str(), "r") != 0) {
    std::string error = "Open Tileset Metadata Failed! " +  metadata_path;
    throw Tunnelour::Exceptions::init_error(error);
  }

  // obtain file size:
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);

  char * token;
  char * next_token;

  //This is string::max_size
  unsigned int max_size = 10000;
  char line[10000];
  while (fgets(line, max_size, pFile) != NULL) {
    std::vector<Tile_Metadata> line_metadata;
    std::vector<std::string> split_line = String_Helper::Split(line, ',');
    std::vector<std::string>::iterator line_tile;
    for (line_tile = split_line.begin(); line_tile != split_line.end(); line_tile++) {
      Tile_Metadata tile_metadata;
      std::vector<std::string> quote_stripper = String_Helper::Split((*line_tile), '\'');
      std::vector<std::string> split_tile = String_Helper::Split(quote_stripper[0], ';');
      std::vector<std::string>::iterator line_tile_data;
      for (line_tile_data = split_tile.begin(); line_tile_data != split_tile.end(); line_tile_data++) {
        std::vector<std::string> split_tile_data = String_Helper::Split((*line_tile_data), ' ');
        if (split_tile_data.begin()->compare("Size") == 0) {
          tile_metadata.size = static_cast<float>(atof(split_tile_data[1].c_str()));
        } else if (split_tile_data.begin()->compare("Type") == 0) {
          tile_metadata.type = split_tile_data[1];
        }
      }
      line_metadata.push_back(tile_metadata);
    }
    out_metadata->level.push_back(line_metadata);
  }

  fclose (pFile);
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Middleground_Controller::GenerateTunnelFromMetadata(Level_Metadata level_metadata) {
  LoadTilesetMetadata();

  std::vector<Tile_Bitmap*> tiles;
  std::vector<std::vector<Tile_Bitmap*>> middleground_lines;
  
  std::vector<std::vector<Tile_Metadata>>::iterator line;
  for (line = level_metadata.level.begin(); line != level_metadata.level.end(); line++) {
    std::vector<Tile_Metadata>::iterator tile;
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
      std::vector<Tile_Metadata>::iterator x_sum_iterator;
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
            position_y = (*y_sum_iterator)->GetPosition().y;
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
  m_current_middleground_subset = GetCurrentMiddlegroundSubset();
}

//---------------------------------------------------------------------------
Tileset_Helper::Tileset_Metadata Middleground_Controller::GetNamedTileset(std::string name) {
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
Tileset_Helper::Subset Middleground_Controller::GetCurrentMiddlegroundSubset() {
  Tileset_Helper::Subset found_subset;

  std::list<Tileset_Helper::Subset>::iterator tileset;
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
  std::list<Tileset_Helper::Line>::iterator line;
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
  std::list<Tileset_Helper::Line>::iterator line;
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
Middleground_Controller::Level_Metadata Middleground_Controller::GetNamedLevel(std::string name) {
  Level_Metadata found_level_metadata;

  std::list<Level_Metadata>::iterator level_metadata;
  for (level_metadata = m_level_metadata.begin(); level_metadata != m_level_metadata.end(); level_metadata++) {
    if (level_metadata->level_name.compare(name) == 0) {
      found_level_metadata = (*level_metadata);
    }
  }

  return found_level_metadata;
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
    position.x = (*edge_tile)->GetPosition().x;
    position.y = (*edge_tile)->GetPosition().y + tile->GetSize().y;
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
    position.x = (*edge_tile)->GetPosition().x;
    position.y = (*edge_tile)->GetPosition().y - tile->GetSize().y;
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
    position.x = (*edge_tile)->GetPosition().x + tile->GetSize().x;
    position.y = (*edge_tile)->GetPosition().y;
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
    position.x = (*edge_tile)->GetPosition().x - tile->GetSize().x;
    position.y = (*edge_tile)->GetPosition().y;
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