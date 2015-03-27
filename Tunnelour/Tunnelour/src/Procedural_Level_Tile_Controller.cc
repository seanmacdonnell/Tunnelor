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
#include <algorithm>    // std::find
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
  m_tile_size = 0;
}

//------------------------------------------------------------------------------
Procedural_Level_Tile_Controller::~Procedural_Level_Tile_Controller() {
  m_floor_offset_x = 0;
  m_floor_offset_y = 0;
  m_level_tile_lines.clear();
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
  m_tile_size = 0;
}

//------------------------------------------------------------------------------
bool Procedural_Level_Tile_Controller::Init(Component_Composite * const model) {
  bool result = Level_Tile_Controller::Init(model);
  std::srand(static_cast<unsigned int>(std::time(0)));
  m_roof_level = 1;
  m_floor_level = -1;
  m_last_roof_level = 1;
  m_last_floor_level = -1;
  m_level_count = 2;
  m_min_tunnel_size = 1;
  m_max_tunnel_size = 6;
  m_tile_size = 128;
  return result;
}

//------------------------------------------------------------------------------
bool Procedural_Level_Tile_Controller::Run() {
  // Get game settings component from the model with the Mutator.
  if (!m_has_been_initialised) { return false; }

  D3DXVECTOR2 game_resolution = m_game_settings->GetResolution();
  D3DXVECTOR3 camera_position = m_camera->GetPosition();
  std::vector<Tile_Bitmap*> new_line;
  float camera_right = (camera_position.x + game_resolution.x);
  while (camera_right > m_level_right) {
	GenerateDefaultTunnel(new_line);

    m_level_tile_lines.push_back(new_line);

    // Add Line to Level Tiles
    if (!new_line.empty()) {
      AddTilesToModel(new_line);
      if (m_level_tile_lines.size() > 16) {
        RemoveTilesFromModel((*m_level_tile_lines.begin()));
        m_level_tile_lines.erase(m_level_tile_lines.begin());
        CloseTunnelBehindAvatar();
      }
   }

    m_level_right += m_tile_size;
    m_last_roof_level = m_roof_level;
    m_last_floor_level = m_floor_level;
  }

  if (m_is_debug_mode != m_game_settings->IsDebugMode()) {
    SwitchTileset();
  }

  return true;
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::CreateLevel() {
  m_current_level_name = m_level->GetCurrentLevel().level_name;
  m_level_tiles = GenerateTunnel(m_level->GetCurrentLevel());
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
    m_level_tiles.push_back(*tile);
    if ((*tile)->GetTexture()->transparency != 0.0f) {
      ResetMiddlegroundTileTexture(*tile);
    } else {
      ResetBackgroundTileTexture(*tile);
      (*tile)->GetTexture()->transparency = 1.0f;
    }
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::RemoveTilesFromModel(std::vector<Tile_Bitmap*> tiles) {
  for (std::vector<Tile_Bitmap*>::iterator tile = tiles.begin(); tile != tiles.end(); ++tile) {
    m_model->Remove(*tile);
    m_level_tiles.erase(std::remove(m_level_tiles.begin(), m_level_tiles.end(), (*tile)), m_level_tiles.end());
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddLevelToModel() {
  for (std::vector<Tile_Bitmap*>::iterator tile = m_level_tiles.begin(); tile != m_level_tiles.end(); ++tile) {
    m_model->Add(*tile);
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::DestroyLevel() {
  // Add tiles to Model
  for (std::vector<Tile_Bitmap*>::iterator tile = m_level_tiles.begin(); tile != m_level_tiles.end(); ++tile) {
    m_model->Remove(*tile);
    (*tile) = 0;
  }
  m_level_tiles.clear();
  m_middleground_tiles.clear();
  m_background_tiles.clear();
  m_top_edge_tiles.clear();
  m_bottom_edge_tiles.clear();
  m_left_edge_tiles.clear();
  m_right_edge_tiles.clear();
  m_exit_tiles.clear();

  m_level_tile_lines.clear();
}

//------------------------------------------------------------------------------
std::vector<Tile_Bitmap*> Procedural_Level_Tile_Controller::GenerateTunnel(Level_Component::Level_Metadata level_metadata) {
  LoadTilesetMetadata();

  std::vector<Tile_Bitmap*> tiles;

  // Find Avatar
  float level_left = level_metadata.start_avatar_top_left_x - (m_tile_size * 4);
  m_level_right = level_metadata.start_avatar_top_left_x - (m_tile_size * 4);
  m_tile_start_point_y = level_metadata.start_avatar_top_left_y;

  m_floor_level = -1;
  m_roof_level = 0;

  std::vector<Tile_Bitmap*> new_line;
  AddRoofTile(new_line);
  (new_line.back())->SetIsRoof(false);
  (new_line.back())->SetIsTopLeftWallEnd(true);
  (new_line.back())->SetIsLeftRoofEnd(true);
  AddFloorTile(new_line);
  (new_line.back())->SetIsFloor(false);
  (new_line.back())->SetIsBotLeftWallEnd(true);
  (new_line.back())->SetIsLeftFloorEnd(true);
  m_level_right += m_tile_size;

  m_level_tile_lines.push_back(new_line);
  tiles.insert(tiles.end(), new_line.begin(), new_line.end()); 


  while (m_level_right < level_metadata.start_avatar_top_left_x - m_tile_size) {
    std::vector<Tile_Bitmap*> new_line;
    AddRoofTile(new_line);
    AddBackgroundTiles(new_line);
    AddFloorTile(new_line);
    m_level_right += m_tile_size;

    m_level_tile_lines.push_back(new_line);
    tiles.insert(tiles.end(), new_line.begin(), new_line.end());
  }

  m_last_roof_level = m_roof_level;
  m_last_floor_level = m_floor_level;
  m_floor_level = -1;
  m_roof_level = 2;
  for (int i = 0; i < 10; i++) {
    std::vector<Tile_Bitmap*> new_line;
    AddRoofTile(new_line);
    AddBackgroundTiles(new_line);
    AddFloorTile(new_line);
    AddRoofLeftWalls();
    m_level_right += m_tile_size;

    m_level_tile_lines.push_back(new_line);
    tiles.insert(tiles.end(), new_line.begin(), new_line.end());
    m_last_roof_level = m_roof_level;
    m_last_floor_level = m_floor_level;
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
  float roof_rand = rand() % 4;
  float roof_coin = rand() % 2;

  if (roof_coin == 1) {
    roof_rand *= -1; 
  }

  while ((m_roof_level + roof_rand) <= m_floor_level) {
    roof_rand = rand() % 4;
    roof_coin = rand() % 2;

    if (roof_coin == 1) {
      roof_rand *= -1; 
    }
  }

  m_roof_level += roof_rand;
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::RandomTheFloor() {
  float floor_rand = rand() % 4;
  float floor_coin = rand() % 2;

  if (floor_coin == 1) {
    floor_rand *= -1; 
  }

  m_floor_level += floor_rand;
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::EnsureMinTunnel() {
  float tunnel_size = (m_roof_level -1) - m_floor_level;

  if (tunnel_size < m_min_tunnel_size) {
    m_roof_level += (m_min_tunnel_size - tunnel_size);
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::EnsureMaxTunnel() {
  float tunnel_size = ((m_roof_level -1 ) - m_floor_level);

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
  float last_tunnel_size = (m_last_roof_level - m_last_floor_level);
  float tunnel_size = (m_roof_level - m_floor_level);

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
  float climb_size = (m_floor_level - m_last_floor_level);

  while (climb_size > 2) {
    RandomTheFloor();
    RandomTheRoof();
    EnsureAPath();
    EnsureMinTunnel();
    EnsureMaxTunnel();
 
    climb_size = (m_floor_level - m_last_floor_level);
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddRoofTile(std::vector<Tile_Bitmap*> &new_line) {
  Tile_Bitmap* new_tile = 0;

  float adjusted_roof_level = m_roof_level;
  float adjusted_floor_level = m_floor_level;

  // Add Roof Tile
  new_tile = CreateMiddlegroundTile(m_tile_size);
  float position_y = m_tile_start_point_y + (m_tile_size * adjusted_roof_level);
  float position_x = m_level_right + m_tile_size;
  float position_z = 0;
  new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
  new_tile->SetIsRoof(true);
  new_line.push_back(new_tile);
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddBackgroundTiles(std::vector<Tile_Bitmap*> &new_line) {
  Tile_Bitmap* new_tile = 0;

  float adjusted_roof_level = m_roof_level;
  float adjusted_floor_level = m_floor_level;

  for (float i = (adjusted_floor_level + 1); i <= (adjusted_roof_level - 1); i++) {
    new_tile = CreateBackgroundTile(m_tile_size);
    float position_y = m_tile_start_point_y + (m_tile_size * i);
    float position_x = m_level_right + m_tile_size;
    float position_z = 0;

    new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
    new_line.push_back(new_tile);
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddFloorTile(std::vector<Tile_Bitmap*> &new_line) {
  Tile_Bitmap* new_tile = 0;

  float adjusted_roof_level = m_roof_level;
  float adjusted_floor_level = m_floor_level;

  new_tile = CreateMiddlegroundTile(m_tile_size);
  float position_y = m_tile_start_point_y + (m_tile_size * adjusted_floor_level);
  float position_x = m_level_right + m_tile_size;
  float position_z = 0;
  new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));
  new_tile->SetIsFloor(true);
  new_line.push_back(new_tile);
}

//------------------------------------------------------------------------------
// Add Left tiles ( X| ) to the previous tile line
//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddRoofLeftWalls() {
  Tile_Bitmap* new_tile = 0;

  float adjusted_roof_level = m_roof_level;
  float adjusted_floor_level = m_floor_level;
  if (m_roof_level > m_last_roof_level) {
    float adjusted_last_roof_level = m_last_roof_level;
    if (!m_level_tile_lines.empty()) {
      bool found_end = false;
      for (std::vector<Tile_Bitmap*>::iterator i = (m_level_tile_lines.back()).begin(); i != (m_level_tile_lines.back()).end(); ++i ) { 
        if ((*i)->GetPosition()->y <= (m_tile_start_point_y + (m_tile_size * adjusted_roof_level))) {
          float position_y = m_tile_start_point_y + (m_tile_size * adjusted_roof_level);
          if ((*i)->GetPosition()->y == position_y) {
            found_end = true;
            (*i)->SetIsTopLeftWallEnd(true);
            (*i)->SetIsLeftRoofEnd(true);
          }
          else {
            (*i)->SetIsLeftWall(true);
          }
          ResetMiddlegroundTileTexture(*i);
          m_model->Update((*i));
          if ((*i)->IsRoof()) {
            break;
          }
        }
      }
      if (!found_end) {
        for (float i = adjusted_last_roof_level + 1; i <= adjusted_roof_level; i++) {
          new_tile = CreateMiddlegroundTile(m_tile_size);
          float position_y = m_tile_start_point_y + (m_tile_size * i);
          float position_x = m_level_right;
          float position_z = 0;
          new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));

          ((m_level_tile_lines.back()).front())->SetIsTopLeftWallEnd(false);
          ((m_level_tile_lines.back()).front())->SetIsLeftRoofEnd(false);
          ((m_level_tile_lines.back()).front())->SetIsLeftWall(true);
          ResetMiddlegroundTileTexture(((m_level_tile_lines.back()).front()));
          m_model->Update((m_level_tile_lines.back()).front());

          new_tile->SetIsTopLeftWallEnd(true);
          new_tile->SetIsLeftRoofEnd(true);
          m_model->Add(new_tile);
          m_level_tiles.push_back(new_tile);
          ResetMiddlegroundTileTexture(new_tile);
          (m_level_tile_lines.back()).insert((m_level_tile_lines.back()).begin(), new_tile);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddRoofRightWalls(std::vector<Tile_Bitmap*> &new_line) {
  Tile_Bitmap* new_tile = 0;

  float adjusted_roof_level = m_roof_level;
  float adjusted_floor_level = m_floor_level;
  if (m_roof_level < m_last_roof_level) {
    float adjusted_last_roof_level = m_last_roof_level;

    if (!new_line.empty()) {
      (*new_line.begin())->SetIsRightWall(true);
    }
 
    for (float i = (adjusted_roof_level + 1); i <= adjusted_last_roof_level; i++) {
      new_tile = CreateMiddlegroundTile(m_tile_size);
      float position_y = m_tile_start_point_y + (m_tile_size * i);
      float position_x = m_level_right + m_tile_size;
      float position_z = 0;
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
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddFloorRightWalls(std::vector<Tile_Bitmap*> &new_line) {
  Tile_Bitmap* new_tile = 0;

  float adjusted_roof_level = m_roof_level;
  float adjusted_floor_level = m_floor_level;
  if (m_floor_level > m_last_floor_level) {
    float adjusted_last_floor_level = m_last_floor_level;
    if (!new_line.empty()) {
      (new_line.back())->SetIsRightWall(true);
    }
 
    for (float i = adjusted_last_floor_level; i <= (adjusted_floor_level - 1); i++) {
      new_tile = CreateMiddlegroundTile(m_tile_size);
      float position_y = m_tile_start_point_y + (m_tile_size * i);
      float position_x = m_level_right + m_tile_size;
      float position_z = 0;
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
}

//------------------------------------------------------------------------------
// Add Left tiles ( X| ) to the previous tile line
//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::AddFloorLeftWalls() {
  Tile_Bitmap* new_tile = 0;

  float adjusted_roof_level = m_roof_level;
  float adjusted_floor_level = m_floor_level;
  if (m_floor_level < m_last_floor_level) {
    float adjusted_last_floor_level = m_last_floor_level;
    if (!m_level_tile_lines.empty()) {
      bool found_end = false;
      for (std::vector<Tile_Bitmap*>::reverse_iterator i = (m_level_tile_lines.back()).rbegin(); i != (m_level_tile_lines.back()).rend(); ++i) {
        if ((*i)->GetPosition()->y >= (m_tile_start_point_y + (m_tile_size * adjusted_floor_level))) {
          float position_y = m_tile_start_point_y + (m_tile_size * adjusted_floor_level);
          if ((*i)->GetPosition()->y == position_y) {
            found_end = true;
            (*i)->SetIsBotLeftWallEnd(true);
            (*i)->SetIsLeftFloorEnd(true);
          }
          else {
            (*i)->SetIsLeftWall(true);
          }
          ResetMiddlegroundTileTexture(*i);
          m_model->Update((*i));
          if ((*i)->IsFloor()) {
            break;
          }
        }
      }
      if (!found_end) {
        //for (float i = adjusted_floor_level; i <= adjusted_last_floor_level - 1; i++) {
        for (float i = adjusted_last_floor_level; i >= adjusted_floor_level; i--) {
          new_tile = CreateMiddlegroundTile(m_tile_size);
          float position_y = m_tile_start_point_y + (m_tile_size * i);
          float position_x = m_level_right;
          float position_z = 0;
          new_tile->SetPosition(D3DXVECTOR3(position_x, position_y, position_z));

          ((m_level_tile_lines.back()).back())->SetIsBotLeftWallEnd(false);
          ((m_level_tile_lines.back()).back())->SetIsLeftFloorEnd(false);
          ((m_level_tile_lines.back()).back())->SetIsLeftWall(true);
          ResetMiddlegroundTileTexture(((m_level_tile_lines.back()).back()));
          m_model->Update((m_level_tile_lines.back()).back());

          new_tile->SetIsBotLeftWallEnd(true);
          new_tile->SetIsLeftFloorEnd(true);
          m_model->Add(new_tile);
          m_level_tiles.push_back(new_tile);
          ResetMiddlegroundTileTexture(new_tile);
          (m_level_tile_lines.back()).push_back(new_tile);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::CloseTunnelBehindAvatar() {
  std::vector<Tile_Bitmap*> new_line;
  std::vector<Tile_Bitmap*>::iterator old_tile;
  for (old_tile = (*(m_level_tile_lines.begin())).begin(); old_tile != (*(m_level_tile_lines.begin())).end(); old_tile++) {
    Tile_Bitmap* new_tile = CreateMiddlegroundTile(m_tile_size);
    new_tile->SetPosition((*old_tile)->GetPosition()->x , (*old_tile)->GetPosition()->y, (*old_tile)->GetPosition()->z);
    (*old_tile)->GetTexture()->transparency = 0.0f;
    ResetMiddlegroundTileTexture((*old_tile));
    if ((*old_tile)->IsRoof() ||
        (*old_tile)->IsLeftRoofEnd() ||
        (*old_tile)->IsTopLeftWallEnd()) {
      new_tile->SetIsLeftRoofEnd(true);
      new_tile->SetIsTopLeftWallEnd(true);
      if (!new_line.empty()) {
        if (new_line.back()->IsLeftRoofEnd() ||
            new_line.back()->IsTopLeftWallEnd() ||
            new_line.back()->IsLeftWall()) {
          new_tile->SetIsLeftRoofEnd(false);
          new_tile->SetIsTopLeftWallEnd(false);
          new_tile->SetIsLeftWall(true);
        }
      }
    } else if ((*old_tile)->IsFloor() ||
               (*old_tile)->IsLeftFloorEnd() ||
               (*old_tile)->IsBotLeftWallEnd()) {
      new_tile->SetIsLeftFloorEnd(true);
      new_tile->SetIsBotLeftWallEnd(true);
      if (!new_line.empty()) {
        if (new_line.back()->IsLeftFloorEnd() ||
            new_line.back()->IsBotLeftWallEnd() ||
            new_line.back()->IsLeftWall()) {
          new_line.back()->SetIsLeftFloorEnd(false);
          new_line.back()->SetIsBotLeftWallEnd(false);
          new_line.back()->SetIsLeftWall(true);
          ResetMiddlegroundTileTexture(new_line.back());
        }
      }
    } else if ((*old_tile)->IsLeftWall()) {
      if (!new_line.empty()) {
        if (new_line.back()->IsLeftFloorEnd() ||
            new_line.back()->IsBotLeftWallEnd() ||
            new_line.back()->IsLeftWall()) {
          new_line.back()->SetIsLeftFloorEnd(false);
          new_line.back()->SetIsBotLeftWallEnd(false);
          new_line.back()->SetIsLeftWall(true);
          ResetMiddlegroundTileTexture(new_line.back());
        }
      }
      new_tile->SetIsLeftWall(true);
    } else if (!(*old_tile)->IsRightWall() &&
               !(*old_tile)->IsRightRoofEnd() &&
               !(*old_tile)->IsTopRightWallEnd() &&
               !(*old_tile)->IsRightFloorEnd() && 
               !(*old_tile)->IsBotRightWallEnd()) {
      new_tile->SetIsLeftWall(true);
    }
    m_model->Add(new_tile);
    m_level_tiles.push_back(new_tile);
    ResetMiddlegroundTileTexture(new_tile);
    new_line.push_back(new_tile);
 }
 (*(m_level_tile_lines.begin())).insert(((*m_level_tile_lines.begin())).begin(), new_line.begin(), new_line.end());
}

//------------------------------------------------------------------------------
void Procedural_Level_Tile_Controller::GenerateDefaultTunnel(std::vector<Tile_Bitmap*> &new_line) {
    RandomTheFloor();
    RandomTheRoof();
    EnsureAPath();
    EnsureMinTunnel();
    EnsureMaxTunnel();
    EnsureAClimbablePath();

    AddRoofTile(new_line);
    AddBackgroundTiles(new_line);
    AddFloorTile(new_line);

    AddRoofRightWalls(new_line);
    AddRoofLeftWalls();
    AddFloorRightWalls(new_line);
    AddFloorLeftWalls();
}


} // Tunnelour