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

#include "Procedural_Level_Tile_Controller.h"
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
Procedural_Level_Tile_Controller::Procedural_Level_Tile_Controller() : Level_Tile_Controller() {
  m_floor_offset_x = 0;
  m_floor_offset_y = 0;
  m_roof_level = 0;
  m_floor_level = 0;
  m_last_roof_level = 0;
  m_last_floor_level = 0;
  m_last_last_roof_level = 0;
  m_last_last_floor_level = 0;
  m_tile_start_point_x = 0;
  m_tile_start_point_y = 0;
  m_level_count = 0;
  m_min_tunnel_size = 0;
  m_max_tunnel_size = 0;
}

//------------------------------------------------------------------------------
Procedural_Level_Tile_Controller::~Procedural_Level_Tile_Controller() {
  m_floor_offset_x = 0;
  m_floor_offset_y = 0;
  m_level_tiles.clear();
  m_roof_level = 1;
  m_floor_level = -1;
  m_last_roof_level = 1;
  m_last_floor_level = -1;
  m_last_last_roof_level = 1;
  m_last_last_floor_level = -1;
  m_tile_start_point_x = 0;
  m_tile_start_point_y = 0;
  m_level_count = 0;
  m_min_tunnel_size = 0;
  m_max_tunnel_size = 0;
}

//------------------------------------------------------------------------------
bool Procedural_Level_Tile_Controller::Init(Component_Composite * const model) {
  bool result = Level_Tile_Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));
  m_roof_level = 2;
  m_floor_level = -2;
  m_last_roof_level = 2;
  m_last_floor_level = -2;
  m_level_count = 2;
  m_min_tunnel_size = 3;
  m_max_tunnel_size = 5;
  return result;
}

//------------------------------------------------------------------------------
bool Procedural_Level_Tile_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }

  D3DXVECTOR2 game_resolution = m_game_settings->GetResolution();
  D3DXVECTOR3 camera_position = m_camera->GetPosition();
  std::vector<Tile_Bitmap*> new_line;
  float camera_right = (camera_position.x + 256);
  if (!m_background_tiles.empty()) {
    while (camera_right > m_level_right) {
      float adjusted_roof_level;
      float adjusted_floor_level;
      float adjusted_last_roof_level;
      float adjusted_last_floor_level;

      RandomTheFloor();
      RandomTheRoof();
      EnsureMinTunnel();
      EnsureMaxTunnel();
      EnsureAPath();
      EnsureAClimbablePath();

      Tile_Bitmap* new_tile = 0;
      float tile_size = 128; 
      adjusted_roof_level = m_roof_level;
      adjusted_floor_level = m_floor_level;
      if (m_roof_level < 0) {
        adjusted_roof_level -= 1; 
      }
      if (m_floor_level < 0) {
        adjusted_floor_level -= 1; 
      }
      // Add Roof Tile
      new_tile = CreateMiddlegroundTile(tile_size);
      float position_y = m_tile_start_point_y + (tile_size * adjusted_roof_level);
      float position_x = m_level_right + tile_size;
      float position_z = 0;
      new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
      new_tile->SetIsRoof(true);
      new_line.push_back(new_tile);

      // Add Background (Tunnel) Tiles
      for (float i = (adjusted_floor_level + 1); i <= (adjusted_roof_level - 1); i++) {
        new_tile = CreateBackgroundTile(tile_size);
        position_y = m_tile_start_point_y + (tile_size * i);
        position_x = m_level_right + tile_size;
        position_z = 0;
   
        new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
        new_line.push_back(new_tile);
      }

      // Add Floor Tile
      new_tile = CreateMiddlegroundTile(tile_size);
      position_y = m_tile_start_point_y + (tile_size * adjusted_floor_level);
      position_x = m_level_right + tile_size;
      position_z = 0;
      new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
      new_tile->SetIsFloor(true);
      new_line.push_back(new_tile);

      // Sets the Right Wall property on the line before
      if (m_roof_level > m_last_roof_level) {
        float adjusted_last_roof_level = m_last_roof_level;
        if (m_last_roof_level < 0) {
          adjusted_last_roof_level -= 1; 
        }

        if (!m_level_tiles.empty()) {
          for (std::vector<Tile_Bitmap*>::iterator i = (m_level_tiles.back()).begin(); i != (m_level_tiles.back()).end(); ++i ) { 
            if (!(*i)->IsTopLeftWallEnd() && !(*i)->IsLeftFloorEnd()) {
              (*i)->SetIsLeftWall(true);
              ResetMiddlegroundTileTexture(*i);
            }
            if ((*i)->IsRoof()) {
              break;
            } 
          } 

          for (float i = adjusted_last_roof_level; i <= adjusted_roof_level; i++) {
            new_tile = CreateMiddlegroundTile(tile_size);
            position_y = m_tile_start_point_y + (tile_size * i);
            position_x = m_level_right;
            position_z = 0;
            new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
            if (i == adjusted_roof_level) {
              new_tile->SetIsTopLeftWallEnd(true);
              new_tile->SetIsLeftRoofEnd(true);
            } else {
              new_tile->SetIsLeftWall(true);
            }

            new_line.insert(new_line.begin(), new_tile);
            ResetMiddlegroundTileTexture(new_tile);
          }
        }
      }

      if (m_roof_level < m_last_roof_level) {
        float adjusted_last_roof_level = m_last_roof_level;
        if (m_last_roof_level < 0) {
          adjusted_last_roof_level -= 1; 
        }

        if (!new_line.empty()) {
          (*new_line.begin())->SetIsRightWall(true);
        }
     
        for (float i = (adjusted_roof_level + 1); i <= adjusted_last_roof_level; i++) {
          new_tile = CreateMiddlegroundTile(tile_size);
          position_y = m_tile_start_point_y + (tile_size * i);
          position_x = m_level_right + tile_size;
          position_z = 0;
          new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
          if (i == adjusted_last_roof_level) {
            new_tile->SetIsTopRightWallEnd(true);
            new_tile->SetIsRightRoofEnd(true);
          } else {
            new_tile->SetIsRightWall(true);
          }
          new_line.insert(new_line.begin(), new_tile);
        }
      }

      if (m_floor_level > m_last_floor_level) {
        float adjusted_last_floor_level = m_last_floor_level;
        if (m_last_floor_level < 0) {
          adjusted_last_floor_level -= 1; 
        }

        if (!new_line.empty()) {
          (new_line.back())->SetIsRightWall(true);
        }
     
        for (float i = adjusted_last_floor_level; i <= (adjusted_floor_level - 1); i++) {
          new_tile = CreateMiddlegroundTile(tile_size);
          position_y = m_tile_start_point_y + (tile_size * i);
          position_x = m_level_right + tile_size;
          position_z = 0;
          new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));

          if (i == adjusted_last_floor_level) {
            new_tile->SetIsBotRightWallEnd(true);
            new_tile->SetIsRightFloorEnd(true);
          } else {
            new_tile->SetIsRightWall(true);
          }

          new_line.push_back(new_tile);
        }
      }

      if (m_floor_level < m_last_floor_level) {
        float adjusted_last_floor_level = m_last_floor_level;
        if (m_last_floor_level < 0) {
          adjusted_last_floor_level -= 1; 
        }

        if (!m_level_tiles.empty()) {
          for (std::vector<Tile_Bitmap*>::reverse_iterator i = (m_level_tiles.back()).rbegin(); i != (m_level_tiles.back()).rend(); ++i ) { 
            if (!(*i)->IsBotRightWallEnd() && !(*i)->IsRightFloorEnd()) {
              (*i)->SetIsLeftWall(true);
            } else {
              (*i)->SetIsBotLeftWallEnd(true);
              (*i)->SetIsLeftFloorEnd(true);
            }
            ResetMiddlegroundTileTexture(*i);
            if ((*i)->IsFloor()) {
              break;
            } 
          } 

          for (float i = adjusted_floor_level; i <= adjusted_last_floor_level; i++) {
            new_tile = CreateMiddlegroundTile(tile_size);
            position_y = m_tile_start_point_y + (tile_size * i);
            position_x = m_level_right;
            position_z = 0;
            new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
            if (i == adjusted_floor_level) {
              new_tile->SetIsBotLeftWallEnd(true);
              new_tile->SetIsLeftFloorEnd(true);
            } else {
              new_tile->SetIsLeftWall(true);
            }

            m_model->Add(new_tile);
            ResetMiddlegroundTileTexture(new_tile);
            (m_level_tiles.back()).push_back(new_tile);
          }
        }
      }

      // Add Line to Level Tiles
      if (!new_line.empty()) {
        AddTilesToModel(new_line);
      }

      m_level_tiles.push_back(new_line);
      m_level_right += tile_size;
      m_last_roof_level = m_roof_level;
      m_last_floor_level = m_floor_level;
    }
  }

  if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
    SwitchTileset();
  }

  return true;
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::CreateLevel() {
  m_current_level_name = m_level->GetCurrentLevel().level_name;
  m_created_tiles = GenerateTunnel(m_level->GetCurrentLevel());
  SwitchTileset();
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::HandleEvent(Tunnelour::Component * const component) {
  if (component->GetType().compare("Camera_Component") == 0) {
    Run();
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddTilesToModel(std::vector<Tile_Bitmap*> tiles) {
  for (std::vector<Tile_Bitmap*>::iterator tile = tiles.begin(); tile != tiles.end(); ++tile) {
    m_model->Add(*tile);
    if ((*tile)->GetTexture()->transparency != 0.0f) {
      ResetMiddlegroundTileTexture(*tile);
    } else {
      ResetBackgroundTileTexture(*tile);
      (*tile)->GetTexture()->transparency = 1.0f;
    }
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::RemoveTilesFromModel(int count) {
  for (int i = 0; i != count; i++) {
    Tile_Bitmap* tile = *(m_floor_tiles.begin());
    m_model->Remove(tile);
    m_floor_tiles.erase(m_floor_tiles.begin());

    tile = *(m_roof_tiles.begin());
    m_model->Remove(tile);
    m_roof_tiles.erase(m_roof_tiles.begin());

    tile = *(m_background_tiles.begin());
    m_model->Remove(tile);
    m_background_tiles.erase(m_background_tiles.begin());
  }
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Procedural_Level_Tile_Controller::GenerateTunnel(Level_Component::Level_Metadata level_metadata) {
  LoadTilesetMetadata();

  std::vector<Tile_Bitmap*> tiles;

  // Find Avatar
  m_tile_start_point_x = level_metadata.start_avatar_top_left_x;
  m_tile_start_point_y = level_metadata.start_avatar_top_left_y;
  float tile_size = 128; 
  // Place background tile under avatar
  Tile_Bitmap* new_tile = 0;

  // Create Tunnel 
  for (int tile = -10; tile < 10; tile++) {
    new_tile = CreateBackgroundTile(tile_size);
    float position_z = 0;
    new_tile->SetPosition(D3DXVECTOR3(m_tile_start_point_x + (tile_size * tile), m_tile_start_point_y, position_z));
    tiles.push_back(new_tile);
    m_background_tiles.push_back(new_tile);
    m_back_offset_x = m_tile_start_point_x + (tile_size * tile);
    m_back_offset_y = m_tile_start_point_y;
    m_level_right = m_back_offset_x;
  }
  /*
  for (int tile = -10; tile < 10; tile++) {
    new_tile = CreateBackgroundTile(tile_size);
    float position_z = 0;
    new_tile->SetPosition(D3DXVECTOR3(m_tile_start_point_x + (tile_size * tile), m_tile_start_point_y, position_z));
    tiles.push_back(new_tile);
    m_background_tiles.push_back(new_tile);
  }
  for (int tile = -10; tile < 10; tile++) {
    new_tile = CreateBackgroundTile(tile_size);
    float position_z = 0;
    new_tile->SetPosition(D3DXVECTOR3(m_tile_start_point_x + (tile_size * tile), m_tile_start_point_y+tile_size, position_z));
    tiles.push_back(new_tile);
    m_background_tiles.push_back(new_tile);
  }
*/
  // Create Floor
  for (int tile = -10; tile < 10; tile++) {
    new_tile = CreateMiddlegroundTile(tile_size);
    float position_z = 0;
    new_tile->SetPosition(D3DXVECTOR3(m_tile_start_point_x + (tile_size * tile), m_tile_start_point_y - (tile_size * 3), position_z));
    new_tile->SetIsFloor(true);
    tiles.push_back(new_tile);
    m_middleground_tiles.push_back(new_tile);
    m_floor_tiles.push_back(new_tile);
    m_floor_offset_x = m_tile_start_point_x + (tile_size * tile);
    m_floor_offset_y = m_tile_start_point_y - tile_size;
  }

  // Create Roof 
  for (int tile = -10; tile < 10; tile++) {
    new_tile = CreateMiddlegroundTile(tile_size);
    float position_z = 0;
    new_tile->SetPosition(D3DXVECTOR3(m_tile_start_point_x + (tile_size * tile), m_tile_start_point_y+(tile_size), position_z));
    new_tile->SetIsRoof(true);
    tiles.push_back(new_tile);
    m_middleground_tiles.push_back(new_tile);
    m_roof_tiles.push_back(new_tile);
    m_roof_offset_x = m_tile_start_point_x + (tile_size * tile);
    m_roof_offset_y = m_tile_start_point_y + (tile_size);
  }

  for (std::vector<Tile_Bitmap*>::iterator tile = tiles.begin(); tile != tiles.end(); ++tile) {
    if ((*tile)->GetTexture()->transparency != 0.0f) {
      ResetMiddlegroundTileTexture(*tile);
    } else {
      ResetBackgroundTileTexture(*tile);
    }
  }

  return tiles;
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::RandomTheRoof() {
//  float roof_coin = rand() % 2;
//  float roof_rand = rand() % 2;
  float roof_rand = 1;
  float roof_coin = 1;

  if (roof_coin == 1) {
    roof_rand *= -1; 
  }

  m_roof_level += roof_rand;
  
  if (m_roof_level == 0) {
    if (roof_coin == 1) { 
      m_roof_level = 1; 
    } else {
      m_roof_level = -1;
    }
  }
  
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::RandomTheFloor() {
  //float floor_rand = rand() % 2;
   float floor_coin = rand() % 2;
  //float floor_coin = 0;
  float floor_rand = 1;

  if (floor_coin == 1) {
    floor_rand *= -1; 
  }

  m_floor_level += floor_rand;
  
  if (m_floor_level == 0) {
    if (floor_coin == 1) { 
      m_floor_level = -1; 
    } else {
      m_floor_level = 1;
    }
  }
  
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::EnsureMinTunnel() {
  float distance_deductor = 0;
  if (m_roof_level > 0) {
    distance_deductor--; 
  }
  if (m_floor_level < 0) {
    distance_deductor--; 
  }
  float tunnel_size = (m_roof_level - m_floor_level) - distance_deductor;

  if (tunnel_size < m_min_tunnel_size) {
    m_roof_level += (m_min_tunnel_size - tunnel_size);
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::EnsureMaxTunnel() {
  float distance_deductor = 0;
  if (m_roof_level > 0) {
    distance_deductor--; 
  }
  if (m_floor_level < 0) {
    distance_deductor--; 
  }
  float tunnel_size = (m_roof_level - m_floor_level) - distance_deductor;

  if (tunnel_size > m_max_tunnel_size) {
    m_roof_level -= (tunnel_size - m_max_tunnel_size);
  }
}

//------------------------------------------------------------------------------
// Dealing with the case that the tunnel is one square large
// and the next tile is also one square large
// but they don't line up
//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::EnsureAPath() {
  float last_distance_deductor = 0;
  if (m_last_roof_level > 0) {
    last_distance_deductor--; 
  }
  if (m_last_floor_level < 0) {
    last_distance_deductor--; 
  }
  float last_tunnel_size = (m_last_roof_level - m_last_floor_level) - last_distance_deductor;

  float distance_deductor = 0;
  if (m_roof_level > 0) {
    distance_deductor--; 
  }
  if (m_floor_level < 0) {
    distance_deductor--; 
  }
  float tunnel_size = (m_roof_level - m_floor_level) - distance_deductor;

  float roof_lowerer = 1;
  if (m_roof_level - roof_lowerer == 0) {
   roof_lowerer++; 
  }
  float lowered_roof = m_roof_level - roof_lowerer;
  if (lowered_roof <= m_last_floor_level) {
    m_roof_level = m_last_roof_level;
  }

  float floor_raiser = 1;
  if (m_floor_level + floor_raiser == 0) {
   floor_raiser++; 
  }
  float raised_floor = m_floor_level + floor_raiser;
  if (raised_floor >= m_last_roof_level) {
    m_floor_level = m_last_floor_level;
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::EnsureAClimbablePath() {
  float distance_deductor = 0;
  if (m_last_floor_level > 0) {
    distance_deductor--; 
  }
  if (m_floor_level < 0) {
    distance_deductor--; 
  }
  float climb_size = (m_floor_level - m_last_floor_level) - distance_deductor;

  if (climb_size > 2) {
    m_floor_level -= (climb_size - 2);
  }
}
} // Tunnelour