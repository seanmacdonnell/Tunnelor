//  Copyright 2014 Sean MacDonnell
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

#include "File_Level_Tile_Controller.h"
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
File_Level_Tile_Controller::File_Level_Tile_Controller() : Level_Tile_Controller() {
  m_game_settings = 0;
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
  m_blocksize_x = 0;
  m_blocksize_y = 0;
}

//------------------------------------------------------------------------------
File_Level_Tile_Controller::~File_Level_Tile_Controller() {
  m_game_settings = 0;
  m_camera->Ignore(this);
  m_camera = 0;
  m_level = 0;
  m_tileset_filename = "";
  m_is_debug_mode = false;
  m_debug_metadata_file_path = "";
  m_dirt_metadata_file_path = "";
}

//------------------------------------------------------------------------------
bool File_Level_Tile_Controller::Init(Component_Composite * const model) {
  Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));

  Level_Tile_Controller_Mutator mutator;
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
      m_current_tileset = GetNamedTileset("Blue_Cave");
    }
    m_current_background_subset = GetCurrentBackgroundSubset();
    m_current_middleground_subset = GetCurrentMiddlegroundSubset();

    m_has_been_initialised = true;
  } else {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool File_Level_Tile_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }

  // If Camera is over an area with no tiles
  //D3DXVECTOR2 game_resolution = m_game_settings->GetResolution();
  //D3DXVECTOR3 camera_position = m_camera->GetPosition();
  /*
  if (!m_right_edge_tiles.empty()) {
    float camera_right = (camera_position.x + (game_resolution.x / 2));
    float middleground_right = (*m_right_edge_tiles.begin())->GetBottomRightPostion().x;
    if ((camera_right + game_resolution.x) > middleground_right) {
      while ((camera_right + game_resolution.x) > middleground_right) {
        TileRight(camera_right, middleground_right);
        middleground_right = (*m_right_edge_tiles.begin())->GetBottomRightPostion().x;
      }
    }
  }
  
  if (!m_left_edge_tiles.empty()) {
    float camera_left = (camera_position.x - (game_resolution.x / 2));
    float middleground_left = (*m_left_edge_tiles.begin())->GetTopLeftPostion().x;
    if (camera_left - (game_resolution.x) < middleground_left) {
      while ((camera_left - game_resolution.x) < middleground_left) {
        TileLeft(camera_left, middleground_left);
        middleground_left = (*m_left_edge_tiles.begin())->GetTopLeftPostion().x;
      }
    }
  }

  if (!m_top_edge_tiles.empty()) {
    float camera_top = (camera_position.y + (game_resolution.y / 2));
    float middleground_top = (*m_top_edge_tiles.begin())->GetTopLeftPostion().y;
    if (camera_top + (game_resolution.y) > middleground_top) {
      while ((camera_top + game_resolution.y) > middleground_top) {
        TileUp(camera_top, middleground_top);
        middleground_top = (*m_top_edge_tiles.begin())->GetTopLeftPostion().y;
      }
    }
  }

  if (!m_bottom_edge_tiles.empty()) {
    float camera_bottom = (camera_position.y - (game_resolution.y / 2));
    float middleground_bottom = (*m_bottom_edge_tiles.begin())->GetBottomRightPostion().y;
    if (camera_bottom - (game_resolution.y) < middleground_bottom) {
      while ((camera_bottom - game_resolution.y) < middleground_bottom) {
        TileDown(camera_bottom, middleground_bottom);
        middleground_bottom = (*m_bottom_edge_tiles.begin())->GetBottomRightPostion().y;
      }
    }
  }
  */
  if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
    SwitchTileset();
  }

  return true;
}

//------------------------------------------------------------------------------
void File_Level_Tile_Controller::CreateLevel() {
  m_current_level_name = m_level->GetCurrentLevel().level_name;
  m_created_tiles = GenerateTunnelFromMetadata(m_level->GetCurrentLevel());
  SwitchTileset();
}

//------------------------------------------------------------------------------
void File_Level_Tile_Controller::AddLevelToModel() {
  for (std::vector<Tile_Bitmap*>::iterator tile = m_created_tiles.begin(); tile != m_created_tiles.end(); ++tile) {
    m_model->Add(*tile);
  }
}

//------------------------------------------------------------------------------
void File_Level_Tile_Controller::ShowLevel() {
  for (std::vector<Tile_Bitmap*>::iterator tile = m_created_tiles.begin(); tile != m_created_tiles.end(); ++tile) {
    (*tile)->GetTexture()->transparency = 1.0f;      
  }
}

//------------------------------------------------------------------------------
void File_Level_Tile_Controller::HideLevel() {
  for (std::vector<Tile_Bitmap*>::iterator tile = m_created_tiles.begin(); tile != m_created_tiles.end(); ++tile) {
    (*tile)->GetTexture()->transparency = 0.0f;      
  }
}

//------------------------------------------------------------------------------
void File_Level_Tile_Controller::DestroyLevel() {
  // Add tiles to Model
  for (std::vector<Tile_Bitmap*>::iterator tile = m_created_tiles.begin(); tile != m_created_tiles.end(); ++tile) {
    m_model->Remove(*tile);
  }
  m_created_tiles.clear();
  m_middleground_tiles.clear();
  m_background_tiles.clear();
  m_top_edge_tiles.clear();
  m_bottom_edge_tiles.clear();
  m_left_edge_tiles.clear();
  m_right_edge_tiles.clear();
  m_exit_tiles.clear();
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> File_Level_Tile_Controller::GetExitTiles() {
  return m_exit_tiles;
}

//------------------------------------------------------------------------------
void File_Level_Tile_Controller::HandleEvent(Tunnelour::Component * const component) {
  if (component->GetType().compare("Camera_Component") == 0) {
    Run();
  }
}

//------------------------------------------------------------------------------
// protected:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private:
//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> File_Level_Tile_Controller::GenerateTunnelFromMetadata(Level_Component::Level_Metadata level_metadata) {
  LoadTilesetMetadata();

  std::vector<Tile_Bitmap*> tiles;
  std::vector<std::vector<Tile_Bitmap*>> tile_lines;
  
  std::vector<std::vector<Level_Component::Tile_Metadata>>::iterator line;
  unsigned int offset_y = 0;
  for (line = level_metadata.level.begin(); line != level_metadata.level.end(); line++) {
    std::vector<Level_Component::Tile_Metadata>::iterator tile;
    std::vector<Tile_Bitmap*> tile_line;
    float current_block_x = 0;
    float current_block_y = 0;
    float offset_x = 0.0f;
    for (tile = (*line).begin(); tile != (*line).end(); tile++) {
      Tile_Bitmap* new_tile = 0;
      float position_x = 0;
      float position_y = 0;
      float position_z = 0;
      if ((*tile).type.compare("Middleground") == 0) {
        new_tile = CreateMiddlegroundTile((*tile).size);
        position_z = -1;
      } else {
        new_tile = CreateBackgroundTile((*tile).size);
        position_z = 0;
      }
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

      position_x += ((*tile).size / 2) + offset_x + current_block_x;
      position_y -= ((*tile).size / 2) + offset_y + current_block_y;

      if ((*tile).size == 128) {
        offset_x += (*tile).size;
      } else {
        current_block_x += (*tile).size;
        if (current_block_x == 128) {
          current_block_x = 0;
          current_block_y += (*tile).size;
          if (current_block_y == 128) {
            current_block_y = 0;
            offset_x += 128;
          }
        }
      }
      // I'm setting the position here so I can use DoTheseTilesXCollide
      new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));

      if ((*tile).type.compare("Middleground") == 0) {
        m_middleground_tiles.push_back(new_tile);
        new_tile->GetTexture()->transparency = 0.0f;
        if (!tile_line.empty()) {
          if (new_tile->GetSize().x == 128) {
            if (tile_line.back()->GetPosition()->z == 0) {
              new_tile->SetIsRightWall(true);
              m_tunnel_edge_tiles.push_back(new_tile);
              std::vector<Tile_Bitmap*>::iterator above_tile;
              for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                  if ((*above_tile)->GetPosition()->z != 0) {
                    if (!(*above_tile)->IsRightWall()) {
                      (*above_tile)->SetIsTopRightWallEnd(true);
                      if ((*std::prev(above_tile))->IsRoof()) {
                        (*above_tile)->SetIsRightRoofEnd(true);
                      }
                      if (std::next(above_tile) != tile_lines.back().end()) {
                        if ((*std::next(above_tile))->IsRoof()) {
                          (*above_tile)->SetIsRightRoofEnd(true);
                        }
                      }
                    }
                  }
                }
              }
            }
            if (!tile_lines.empty()) {
              std::vector<Tile_Bitmap*>::iterator above_tile;
              for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                  if ((*above_tile)->GetPosition()->z == 0) {
                    new_tile->SetIsFloor(true);
                    m_tunnel_edge_tiles.push_back(new_tile);
                    if (!tile_line.back()->IsFloor()) {
                      tile_line.back()->SetIsLeftFloorEnd(true);
                      std::vector<Tile_Bitmap*>::iterator above_tile;
                      for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                        if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), tile_line.back()) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), tile_line.back())) {
                          if ((*above_tile)->GetPosition()->z != 0) {
                            if ((*above_tile)->IsLeftWall()) {
                              tile_line.back()->SetIsBotLeftWallEnd(true);
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
              if (!new_tile->IsFloor() && tile_line.back()->IsFloor()) {
                new_tile->SetIsRightFloorEnd(true);
                std::vector<Tile_Bitmap*>::iterator above_tile;
                for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                  if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                    if ((*above_tile)->GetPosition()->z != 0) {
                      if ((*above_tile)->IsRightWall()) {
                        new_tile->SetIsBotRightWallEnd(true);
                      }
                    }
                  }
                }
              }
            }
          } else {
            if (m_blocksize_y == 0 && m_blocksize_x == 0) {
              m_back_block = tile_line.back();
            }
            if (m_blocksize_y == 0) {
              if (tile_line.back()->GetPosition()->z == 0) {
                new_tile->SetIsRightWall(true);
                m_tunnel_edge_tiles.push_back(new_tile);
              }
              if (!tile_lines.empty()) {
                std::vector<Tile_Bitmap*>::iterator above_tile;
                for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                  if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                    if ((*above_tile)->GetPosition()->z == 0) {
                      new_tile->SetIsFloor(true);
                      m_tunnel_edge_tiles.push_back(new_tile);
                    }
                  }
                }
              }
              m_block_tile_line.push_back(new_tile);
            } else {
              if (m_blocksize_x == 0) {
                if (m_back_block->GetPosition()->z == 0) {
                  new_tile->SetIsRightWall(true);
                  m_tunnel_edge_tiles.push_back(new_tile);
                }
                if (!m_block_tile_lines.empty()) {
                  std::vector<Tile_Bitmap*>::iterator above_tile;
                  for (above_tile = m_block_tile_lines.back().begin(); above_tile != m_block_tile_lines.back().end(); above_tile++) {
                    if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                      if ((*above_tile)->GetPosition()->z == 0) {
                        new_tile->SetIsFloor(true);
                        m_tunnel_edge_tiles.push_back(new_tile);
                      }
                    }
                  }
                }
                m_block_tile_line.push_back(new_tile);
              } else {
                if (m_block_tile_line.back()->GetPosition()->z == 0) {
                  new_tile->SetIsRightWall(true);
                  m_tunnel_edge_tiles.push_back(new_tile);
                }
                if (!m_block_tile_lines.empty()) {
                  std::vector<Tile_Bitmap*>::iterator above_tile;
                  for (above_tile = m_block_tile_lines.back().begin(); above_tile != m_block_tile_lines.back().end(); above_tile++) {
                    if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                      if ((*above_tile)->GetPosition()->z == 0) {
                        new_tile->SetIsFloor(true);
                        m_tunnel_edge_tiles.push_back(new_tile);
                      }
                    }
                  }
                }
                m_block_tile_line.push_back(new_tile);
              }
            }
            
            m_blocksize_x += new_tile->GetSize().x;

            if (m_blocksize_x == 128 && m_blocksize_y + new_tile->GetSize().y == 128) {
              m_blocksize_x = 0;
              m_blocksize_y = 0;
              m_block_tile_lines.clear();
              m_block_tile_line.clear();
              m_back_block = 0;
            } else if (m_blocksize_x == 128) {
              m_blocksize_x = 0;
              m_blocksize_y += new_tile->GetSize().y;
              m_block_tile_lines.push_back(m_block_tile_line);
              m_block_tile_line.clear();
            }
          }
        }
      } else { // TUNNEL
        m_background_tiles.push_back(new_tile);
        new_tile->GetTexture()->transparency = 0.0f; //TUNNEL
        if (new_tile->GetSize().x == 128) {
          if (tile_line.back()->GetPosition()->z != 0) {
            tile_line.back()->SetIsLeftWall(true);
            m_tunnel_edge_tiles.push_back(tile_line.back());
            if (!tile_lines.empty()) {
              std::vector<Tile_Bitmap*>::iterator above_tile;
              for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), tile_line.back()) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), tile_line.back())) {
                  if ((*above_tile)->GetPosition()->z != 0) {
                    if (!(*above_tile)->IsLeftWall()) {
                      (*above_tile)->SetIsTopLeftWallEnd(true);
                    }
                  }
                }
              }
            }
          }
          if (!tile_lines.empty()) {
            std::vector<Tile_Bitmap*>::iterator above_tile;
            for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
              if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                if ((*above_tile)->GetPosition()->z != 0) {
                  (*above_tile)->SetIsRoof(true);
                  m_tunnel_edge_tiles.push_back((*above_tile));
                  if ((*std::prev(above_tile))->GetPosition()->z != 0 && !(*std::prev(above_tile))->IsRoof() ) {
                    (*std::prev(above_tile))->SetIsLeftRoofEnd(true);
                  }
                  if (std::next(above_tile) != tile_lines.back().end()) {
                    if ((*std::next(above_tile))->GetPosition()->z != 0 && !(*std::next(above_tile))->IsRoof() ) {
                      (*std::next(above_tile))->SetIsRightRoofEnd(true);
                    }
                  }
                }
              }
            }
          }
        } else {
            if (m_blocksize_y == 0 && m_blocksize_x == 0) {
              m_back_block = tile_line.back();
            }
            if (m_blocksize_y == 0) {
              if (tile_line.back()->GetPosition()->z != 0) {
                tile_line.back()->SetIsLeftWall(true);
                m_tunnel_edge_tiles.push_back(tile_line.back());
                if (!tile_line.empty()) {
                  std::vector<Tile_Bitmap*>::iterator above_tile;
                  for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                    if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), tile_line.back()) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), tile_line.back())) {
                      if ((*above_tile)->GetPosition()->z != 0) {
                        if (!(*above_tile)->IsLeftWall()) {
                          (*above_tile)->SetIsTopLeftWallEnd(true);
                        }
                      }
                    }
                  }
                }
              }
              if (!tile_lines.empty()) {
                std::vector<Tile_Bitmap*>::iterator above_tile;
                for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                  if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                    if ((*above_tile)->GetPosition()->z != 0) {
                      (*above_tile)->SetIsRoof(true);
                      m_tunnel_edge_tiles.push_back((*above_tile));
                    }
                  }
                }
              }
              m_block_tile_line.push_back(new_tile);
            } else {
              if (m_blocksize_x == 0) {
                if (m_back_block->GetPosition()->z != 0) {
                  m_back_block->SetIsLeftWall(true);
                  m_tunnel_edge_tiles.push_back(tile_line.back());
                  std::vector<Tile_Bitmap*>::iterator above_tile;
                  for (above_tile = m_block_tile_lines.back().begin(); above_tile != m_block_tile_lines.back().end(); above_tile++) {
                    if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                      if ((*above_tile)->GetPosition()->z != 0) {
                        if (!(*above_tile)->IsLeftWall()) {
                          (*above_tile)->SetIsTopLeftWallEnd(true);
                        }
                      }
                    }
                  }
                }
                if (!tile_lines.empty()) {
                  std::vector<Tile_Bitmap*>::iterator above_tile;
                  for (above_tile = tile_lines.back().begin(); above_tile != tile_lines.back().end(); above_tile++) {
                    if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                      if ((*above_tile)->GetPosition()->z != 0) {
                        (*above_tile)->SetIsRoof(true);
                        m_tunnel_edge_tiles.push_back((*above_tile));
                      }
                    }
                  }
                }
                m_block_tile_line.push_back(new_tile);
              } else {
                if (m_back_block->GetPosition()->z != 0) {
                  m_back_block->SetIsLeftWall(true);
                  m_tunnel_edge_tiles.push_back(tile_line.back());
                  std::vector<Tile_Bitmap*>::iterator above_tile;
                  for (above_tile = m_block_tile_lines.back().begin(); above_tile != m_block_tile_lines.back().end(); above_tile++) {
                    if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                      if ((*above_tile)->GetPosition()->z != 0) {
                        if (!(*above_tile)->IsLeftWall()) {
                          (*above_tile)->SetIsTopLeftWallEnd(true);
                        }
                      }
                    }
                  }
                }
                if (!tile_lines.empty()) {
                  std::vector<Tile_Bitmap*>::iterator above_tile;
                  for (above_tile = m_block_tile_lines.back().begin(); above_tile != m_block_tile_lines.back().end(); above_tile++) {
                    if (Bitmap_Helper::DoTheseTilesXCollide((*above_tile), new_tile) && Bitmap_Helper::AreTheseTilesYAdjacent((*above_tile), new_tile)) {
                      if ((*above_tile)->GetPosition()->z != 0) {
                        (*above_tile)->SetIsRoof(true);
                        m_tunnel_edge_tiles.push_back((*above_tile));
                      }
                    }
                  }
                }
                m_block_tile_line.push_back(new_tile);
              }
            }
            
            m_blocksize_x += new_tile->GetSize().x;
            
            if (m_blocksize_x == 128 && m_blocksize_y + new_tile->GetSize().y == 128) {
              m_blocksize_x = 0;
              m_blocksize_y = 0;
              m_block_tile_lines.clear();
              m_block_tile_line.clear();
              m_back_block = 0;
            } else if (m_blocksize_x == 128) {
              m_blocksize_x = 0;
              m_blocksize_y += new_tile->GetSize().y;
              m_block_tile_lines.push_back(m_block_tile_line);
              m_block_tile_line.clear();
            }
        }
        if ((*tile).type.compare("Exit") == 0) {
          if (tile_line.back()->IsLeftWall()) {
            new_tile->SetIsLeftExit(true);
          } else {
            new_tile->SetIsRightExit(true);
          }
          m_exit_tiles.push_back(new_tile);
        }
      }

      if ((*tile).type.compare("Middleground") == 0) {
        ResetMiddlegroundTileTexture(new_tile);
      } else {
        ResetBackgroundTileTexture(new_tile);
      }
      
      tile_line.push_back(new_tile);
      tiles.push_back(new_tile);
    }
    tile_lines.push_back(tile_line);
    offset_y += 128;
  }

  return tiles;
}

//---------------------------------------------------------------------------
void File_Level_Tile_Controller::LoadTilesetMetadata() {
  Tileset_Helper::Tileset_Metadata debug_tileset_metadata;
  m_debug_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Debug_Tileset.txt");
  Tileset_Helper::LoadTilesetMetadataIntoStruct(m_debug_metadata_file_path, &debug_tileset_metadata);
  m_tilesets.push_back(debug_tileset_metadata);

  Tileset_Helper::Tileset_Metadata blue_cave_tileset_metadata;
  m_blue_cave_metadata_file_path = String_Helper::WStringToString(m_game_settings->GetTilesetPath() + L"Blue_Cave_Tileset_2.txt");
  Tileset_Helper::LoadTilesetMetadataIntoStruct(m_blue_cave_metadata_file_path, &blue_cave_tileset_metadata);
  m_tilesets.push_back(blue_cave_tileset_metadata);

  if (m_game_settings->IsDebugMode()) {
    m_current_tileset = GetNamedTileset("Debug");
  } else {
    m_current_tileset = GetNamedTileset("Blue_Cave");
  }
  m_current_middleground_subset = GetCurrentMiddlegroundSubset();
  m_current_background_subset = GetCurrentBackgroundSubset();
}

//---------------------------------------------------------------------------
Tileset_Helper::Tileset_Metadata File_Level_Tile_Controller::GetNamedTileset(std::string name) {
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
Tileset_Helper::Subset File_Level_Tile_Controller::GetCurrentMiddlegroundSubset() {
  Tileset_Helper::Subset found_subset;

  std::vector<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = m_current_tileset.tilesets.begin(); tileset != m_current_tileset.tilesets.end(); tileset++) {
    if (tileset->type.compare("Middleground") == 0) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}

//---------------------------------------------------------------------------
Tileset_Helper::Subset File_Level_Tile_Controller::GetCurrentMiddlegroundSubsetType(File_Level_Tile_Controller::Middleground_Tile_Type types) {
  Tileset_Helper::Subset found_subset;

  found_subset = (*m_current_tileset.tilesets.begin());
  std::vector<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = m_current_tileset.tilesets.begin(); tileset != m_current_tileset.tilesets.end(); tileset++) {    
    Middleground_Tile_Type found_types = ParseSubsetTypesFromString(tileset->type);
    if (found_types == types) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}

//---------------------------------------------------------------------------
Tileset_Helper::Subset File_Level_Tile_Controller::GetCurrentBackgroundSubsetType(File_Level_Tile_Controller::Background_Tile_Type types) {
  Tileset_Helper::Subset found_subset;

  found_subset = (*m_current_tileset.tilesets.begin());
  std::vector<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = m_current_tileset.tilesets.begin(); tileset != m_current_tileset.tilesets.end(); tileset++) {    
    Background_Tile_Type found_types = ParseSubsetBackgroundTypesFromString(tileset->type);
    if (found_types == types) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}

//---------------------------------------------------------------------------
File_Level_Tile_Controller::Middleground_Tile_Type File_Level_Tile_Controller::ParseSubsetTypesFromString(std::string type) {
  Middleground_Tile_Type found_types;
  std::size_t found;
  found = type.find(" End");
  if (found == std::string::npos) {
    found = type.find("Roof");
    if (found != std::string::npos) {
      found_types.found_roof = true;
    }
    found = type.find("Floor");
    if (found != std::string::npos) {
      found_types.found_floor = true;
    }
    found = type.find("Right Wall");
    if (found != std::string::npos) {
      found_types.found_right_wall = true;
    }
    found = type.find("Left Wall");
    if (found != std::string::npos) {
      found_types.found_left_wall = true;
    }
  } else {
    found = type.find("Right Roof End");
    if (found != std::string::npos) {
      found_types.found_roof_right_end = true;
    }
    found = type.find("Left Roof End");
    if (found != std::string::npos) {
      found_types.found_roof_left_end = true;
    }
    found = type.find("Right Floor End");
    if (found != std::string::npos) {
      found_types.found_floor_right_end = true;
    }
    found = type.find("Left Floor End");
    if (found != std::string::npos) {
      found_types.found_floor_left_end = true;
    }
    found = type.find("Right Wall Top End");
    if (found != std::string::npos) {
      found_types.found_right_wall_top_end = true;
    }
    found = type.find("Right Wall Bot End");
    if (found != std::string::npos) {
      found_types.found_right_wall_bot_end = true;
    }
    found = type.find("Left Wall Top End");
    if (found != std::string::npos) {
      found_types.found_left_wall_top_end = true;
    }
    found = type.find("Left Wall Bot End");
    if (found != std::string::npos) {
      found_types.found_right_wall_bot_end = true;
    }
  }
  return found_types;
}

//---------------------------------------------------------------------------
File_Level_Tile_Controller::Background_Tile_Type File_Level_Tile_Controller::ParseSubsetBackgroundTypesFromString(std::string type) {
  Background_Tile_Type found_types;
  std::size_t found;

  found = type.find("Right Exit");
  if (found != std::string::npos) {
    found_types.found_right_exit = true;
  }
  found = type.find("Left Exit");
  if (found != std::string::npos) {
    found_types.found_left_exit = true;
  }

  return found_types;
}

//---------------------------------------------------------------------------
Tileset_Helper::Subset File_Level_Tile_Controller::GetCurrentNamedSubset(std::string name) {
  Tileset_Helper::Subset found_subset;

  std::vector<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = m_current_tileset.tilesets.begin(); tileset != m_current_tileset.tilesets.end(); tileset++) {
    if (tileset->type.compare(name) == 0) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}

//---------------------------------------------------------------------------
Tileset_Helper::Subset File_Level_Tile_Controller::GetCurrentBackgroundSubset() {
  Tileset_Helper::Subset found_subset;

  std::vector<Tileset_Helper::Subset>::iterator tileset;
  for (tileset = m_current_tileset.tilesets.begin(); tileset != m_current_tileset.tilesets.end(); tileset++) {
    if (tileset->type.compare("Background") == 0) {
      found_subset = *tileset;
    }
  }

  return found_subset;
}

//------------------------------------------------------------------------------
Tile_Bitmap* File_Level_Tile_Controller::CreateMiddlegroundTile(float base_tile_size) {
  Tileset_Helper::Line tile_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  bool found = false;
  for (line = m_current_middleground_subset.lines.begin(); line != m_current_middleground_subset.lines.end(); line++) {
    if (line->tile_size_x == base_tile_size) {
      if (line->tile_size_y == base_tile_size) {
        tile_line = *line;
        found = true;
      }
    }
  }
  if (!found) {
    std::string error = "Unable to find size: ";
    error.append(String_Helper::To_String(base_tile_size));
    error += " in middleground texture sheet.";
    throw Tunnelour::Exceptions::init_error(error);
  }

  Tile_Bitmap* tile = new Tile_Bitmap();
  tile->SetPosition(D3DXVECTOR3(0, 0, -1));
  tile->GetTexture()->transparency = 1.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_current_tileset.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(m_current_tileset.size_x,
                                                 m_current_tileset.size_y);
  tile->GetTexture()->tile_size = D3DXVECTOR2(tile_line.tile_size_x,
                                              tile_line.tile_size_y);
  tile->SetIsCollidable(true);

  int random_line_tile = rand() % tile_line.number_of_tiles;

  float random_tile_x = random_line_tile * tile_line.tile_size_x;
  random_tile_x += tile_line.top_left_x;

  float random_tile_y = tile_line.top_left_y;

  tile->GetTexture()->top_left_position = D3DXVECTOR2(random_tile_x, random_tile_y);

  tile->SetSize(D3DXVECTOR2(base_tile_size, base_tile_size));

  return tile;
}

//------------------------------------------------------------------------------
Tile_Bitmap* File_Level_Tile_Controller::CreateBackgroundTile(float base_tile_size) {
  Tileset_Helper::Line background_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  bool found = false;
  for (line = m_current_background_subset.lines.begin(); line != m_current_background_subset.lines.end(); line++) {
    if (line->tile_size_x == base_tile_size) {
      if (line->tile_size_y == base_tile_size) {
        background_line = *line;
        found = true;
      }
    }
  }
  if (!found) {
    std::string error = "Unable to find background tile size: ";
    error += String_Helper::To_String(base_tile_size);
    error += " in texture sheet.";
    throw Tunnelour::Exceptions::init_error(error);
  }

  Tile_Bitmap* tile = new Tile_Bitmap();
  tile->SetPosition(D3DXVECTOR3(0, 0, 0));
  tile->GetTexture()->transparency = 0.0f;

  std::wstring texture_path = m_game_settings->GetTilesetPath();
  texture_path += String_Helper::StringToWString(m_current_tileset.filename);
  tile->GetTexture()->texture_path = texture_path;
  tile->GetTexture()->texture_size = D3DXVECTOR2(m_current_tileset.size_x,
                                                 m_current_tileset.size_y);
  tile->GetTexture()->tile_size = D3DXVECTOR2(background_line.tile_size_x,
                                              background_line.tile_size_y);
  tile->SetIsCollidable(false);

  int random_line_tile = 0;
  if (m_is_debug_mode) {
    random_line_tile = 0;
  } else {
    random_line_tile = rand() % background_line.number_of_tiles;
  }

  float random_tile_x = random_line_tile * background_line.tile_size_x;
  random_tile_x += background_line.top_left_x;

  float random_tile_y = background_line.top_left_y;

  tile->GetTexture()->top_left_position = D3DXVECTOR2(random_tile_x, random_tile_y);

  tile->SetSize(D3DXVECTOR2(base_tile_size, base_tile_size));

  return tile;
}

//---------------------------------------------------------------------------
Tileset_Helper::Line File_Level_Tile_Controller::GetCurrentSizedMiddlegroundLine(float size) {
  Tileset_Helper::Line tile_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  for (line = m_current_middleground_subset.lines.begin(); line != m_current_middleground_subset.lines.end(); line++) {
    if (line->tile_size_x == size) {
      if (line->tile_size_y == size) {
        tile_line = *line;
      }
    }
  }
  return tile_line;
}

//---------------------------------------------------------------------------
Tileset_Helper::Line File_Level_Tile_Controller::GetSizedNamedLine(Tileset_Helper::Subset subset, float size) {
  Tileset_Helper::Line tile_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  for (line = subset.lines.begin(); line != subset.lines.end(); line++) {
    if (line->tile_size_x == size) {
      if (line->tile_size_y == size) {
        tile_line = *line;
      }
    }
  }
  return tile_line;
}

//---------------------------------------------------------------------------
Tileset_Helper::Line File_Level_Tile_Controller::GetCurrentSizedBackgroundLine(float size) {
  Tileset_Helper::Line tile_line;
  std::vector<Tileset_Helper::Line>::iterator line;
  for (line = m_current_background_subset.lines.begin(); line != m_current_background_subset.lines.end(); line++) {
    if (line->tile_size_x == size) {
      if (line->tile_size_y == size) {
        tile_line = *line;
      }
    }
  }
  return tile_line;
}

//---------------------------------------------------------------------------
void File_Level_Tile_Controller::ResetMiddlegroundTileTexture(Tile_Bitmap *out_tile) {
  Tileset_Helper::Line tile_line = GetCurrentSizedMiddlegroundLine(out_tile->GetSize().x);
  Tileset_Helper::Subset edge_subset;
  int random_variable = 0;

  if (out_tile->IsLeftWall() || out_tile->IsRightWall() || out_tile->IsRoof() || out_tile->IsFloor()) {
    Middleground_Tile_Type middleground_tile_type;
    //std::string types;
    if (out_tile->IsLeftWall()) {
      middleground_tile_type.found_left_wall = true;
    }
    if (out_tile->IsRightWall()) {
      middleground_tile_type.found_right_wall = true;
    }
    if (out_tile->IsRoof()) {
      middleground_tile_type.found_roof = true;
    }
    if (out_tile->IsFloor()) {
      middleground_tile_type.found_floor = true;
    }
    //Middleground_Tile_Type middleground_tile_type = ParseSubsetTypesFromString(types);
    edge_subset = GetCurrentMiddlegroundSubsetType(middleground_tile_type);
    tile_line = GetSizedNamedLine(edge_subset, out_tile->GetSize().x);
    random_variable = random_variable = rand() % tile_line.number_of_tiles;
  } else if (out_tile->IsRightFloorEnd() || out_tile->IsLeftFloorEnd() ||
              out_tile->IsRightRoofEnd() || out_tile->IsLeftRoofEnd() ||
              out_tile->IsTopLeftWallEnd() || out_tile->IsBotLeftWallEnd() ||
              out_tile->IsTopRightWallEnd() || out_tile->IsBotRightWallEnd()) {
    std::string types;
    if (out_tile->IsRightFloorEnd()) {
      types += "Right Floor End";
    }
    if (out_tile->IsLeftFloorEnd()) {
      types += "Left Floor End";
    }
    if (out_tile->IsRightRoofEnd()) {
      types += "Right Roof End";
    }
    if (out_tile->IsLeftRoofEnd()) {
      types += "Left Roof End";
    }
    if (out_tile->IsTopLeftWallEnd()) {
      types += "Left Wall Top End";
    }
    if (out_tile->IsBotLeftWallEnd()) {
      types += "Left Wall Bot End";
    }
    if (out_tile->IsTopRightWallEnd()) {
      types += "Right Wall Top End";
    }
    if (out_tile->IsBotRightWallEnd()) {
      types += "Right Wall Bot End";
    }
    Middleground_Tile_Type Middleground_Tile_Type = ParseSubsetTypesFromString(types);
    edge_subset = GetCurrentMiddlegroundSubsetType(Middleground_Tile_Type);
    tile_line = GetSizedNamedLine(edge_subset, out_tile->GetSize().x);
    random_variable = random_variable = rand() % tile_line.number_of_tiles;
  } else {
    random_variable = random_variable = rand() % tile_line.number_of_tiles;;
  }     

  float left = random_variable * tile_line.tile_size_x + tile_line.top_left_x;
  left += edge_subset.top_left_x_offset + (random_variable * edge_subset.top_left_x_offset);
  float top = tile_line.top_left_y;
  top += edge_subset.top_left_y_offset;
  out_tile->GetTexture()->top_left_position = D3DXVECTOR2(left, top);
  out_tile->GetTexture()->texture = 0;
  out_tile->GetFrame()->vertex_buffer = 0;
  out_tile->Init();
}

//---------------------------------------------------------------------------
void File_Level_Tile_Controller::ResetBackgroundTileTexture(Tile_Bitmap *out_tile) {
  Tileset_Helper::Line tile_line = GetCurrentSizedBackgroundLine(out_tile->GetSize().x);
  Tileset_Helper::Subset edge_subset;
  int random_variable = 0;
  if (m_is_debug_mode) {
    if (out_tile->IsRightExit() || out_tile->IsLeftExit()) {
      random_variable = 1;
    } else {
      random_variable = 0;
    }
  } else if (out_tile->IsRightExit() || out_tile->IsLeftExit()) {
    std::string types;
    if (out_tile->IsRightExit()) {
      types += " Right Exit ";
    } 
    if (out_tile->IsLeftExit()) {
      types += " Left Exit ";
    }
    Background_Tile_Type bacground_tile_type = ParseSubsetBackgroundTypesFromString(types);
    edge_subset = GetCurrentBackgroundSubsetType(bacground_tile_type);
    tile_line = GetSizedNamedLine(edge_subset, out_tile->GetSize().x);
    random_variable = random_variable = rand() % tile_line.number_of_tiles;
  } else {
    std::string types = "";
    Background_Tile_Type bacground_tile_type = ParseSubsetBackgroundTypesFromString(types);
    edge_subset = GetCurrentBackgroundSubsetType(bacground_tile_type);
    random_variable = rand() % tile_line.number_of_tiles;
  }

  float left = random_variable * tile_line.tile_size_x + tile_line.top_left_x;
  left += edge_subset.top_left_x_offset + (random_variable * edge_subset.top_left_x_offset);
  float top = tile_line.top_left_y;
  top += edge_subset.top_left_y_offset;
  out_tile->GetTexture()->top_left_position = D3DXVECTOR2(left, top);
  out_tile->GetTexture()->texture = 0;
  out_tile->GetFrame()->vertex_buffer = 0;
  out_tile->Init();
}

//---------------------------------------------------------------------------
void File_Level_Tile_Controller::SwitchTileset() {
  // Debug Mode has been activated or Deactivated
  m_is_debug_mode = m_game_settings->IsDebugMode();
  if (m_is_debug_mode) {
    m_current_tileset = GetNamedTileset("Debug");
  } else {
    m_current_tileset = GetNamedTileset("Blue_Cave");
  }

  std::vector<Tile_Bitmap*>::iterator tile;
  
  m_current_middleground_subset = GetCurrentMiddlegroundSubset();
  for (tile = m_middleground_tiles.begin(); tile != m_middleground_tiles.end(); tile++) {
    Tileset_Helper::Line tile_line = GetCurrentSizedMiddlegroundLine((*tile)->GetSize().x);
    std::wstring texture_path = m_game_settings->GetTilesetPath();
    texture_path += String_Helper::StringToWString(m_current_tileset.filename);
    (*tile)->GetTexture()->texture_path = texture_path;
    D3DXVECTOR2 size = D3DXVECTOR2(m_current_tileset.size_x,
                                   m_current_tileset.size_y);
    (*tile)->GetTexture()->texture_size = size;
    ResetMiddlegroundTileTexture((*tile));
  }
  
  m_current_background_subset = GetCurrentBackgroundSubset();
  for (tile = m_background_tiles.begin(); tile != m_background_tiles.end(); tile++) {
    Tileset_Helper::Line background_line = GetCurrentSizedBackgroundLine((*tile)->GetSize().x);
    std::wstring texture_path = m_game_settings->GetTilesetPath();
    texture_path += String_Helper::StringToWString(m_current_tileset.filename);
    (*tile)->GetTexture()->texture_path = texture_path;
    D3DXVECTOR2 size = D3DXVECTOR2(m_current_tileset.size_x,
                                   m_current_tileset.size_y);
    (*tile)->GetTexture()->texture_size = size;
    ResetBackgroundTileTexture((*tile));
  }
}
/* DEPRECIATED
//---------------------------------------------------------------------------
void File_Level_Tile_Controller::TileUp(float camera_top, float middleground_top) {
  std::vector<Tile_Bitmap*> new_top_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_top_edge_tiles.begin(); edge_tile != m_top_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateMiddlegroundTile(128);
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
    ResetMiddlegroundTileTexture((*edge_tile));
    ResetMiddlegroundTileTexture(tile);
    m_created_tiles.push_back(tile);
  }
  m_top_edge_tiles.clear();

  m_top_edge_tiles = new_top_tiles;
}

//---------------------------------------------------------------------------
void File_Level_Tile_Controller::TileDown(float camera_bottom, float middleground_bottom) {
  std::vector<Tile_Bitmap*> new_bottom_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_bottom_edge_tiles.begin(); edge_tile != m_bottom_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateMiddlegroundTile(128);
    D3DXVECTOR3 position;
    position.x = (*edge_tile)->GetPosition()->x;
    position.y = (*edge_tile)->GetPosition()->y - tile->GetSize().y;
    position.z = -1.0; // Middleground Z Space is -1
    tile->SetPosition(position);
    tile->SetIsTopEdge((*edge_tile)->IsTopEdge());
    tile->SetIsBottomEdge((*edge_tile)->IsBottomEdge());
    tile->SetIsRightEdge((*edge_tile)->IsRightEdge());
    tile->SetIsLeftEdge((*edge_tile)->IsLeftEdge());
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
    //if (m_is_debug_mode) {
      ResetMiddlegroundTileTexture((*edge_tile));
      ResetMiddlegroundTileTexture(tile);
    //}
    m_created_tiles.push_back(tile);
    m_model->Add(tile);
  }

  m_bottom_edge_tiles.clear();
  m_bottom_edge_tiles = new_bottom_tiles;
}

//---------------------------------------------------------------------------
void File_Level_Tile_Controller::TileRight(float camera_right, float middleground_right) {
  std::vector<Tile_Bitmap*> new_right_edge_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_right_edge_tiles.begin(); edge_tile != m_right_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateMiddlegroundTile(128);
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
    //if (m_is_debug_mode) {
      ResetMiddlegroundTileTexture((*edge_tile));
      ResetMiddlegroundTileTexture(tile);
    //}
    m_created_tiles.push_back(tile);
  }
  m_right_edge_tiles.clear();

  m_right_edge_tiles = new_right_edge_tiles;
}

//---------------------------------------------------------------------------
void File_Level_Tile_Controller::TileLeft(float camera_left, float middleground_left) {
  std::vector<Tile_Bitmap*> new_left_edge_tiles;

  std::vector<Tile_Bitmap*>::iterator edge_tile;
  for (edge_tile = m_left_edge_tiles.begin(); edge_tile != m_left_edge_tiles.end(); edge_tile++) {
    Tile_Bitmap* tile = CreateMiddlegroundTile(128);
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
    //if (m_is_debug_mode) {
      ResetMiddlegroundTileTexture((*edge_tile));
      ResetMiddlegroundTileTexture(tile);
    //}
    m_created_tiles.push_back(tile);
  }
  m_left_edge_tiles.clear();

  m_left_edge_tiles = new_left_edge_tiles;
}
*/
} // Tunnelour