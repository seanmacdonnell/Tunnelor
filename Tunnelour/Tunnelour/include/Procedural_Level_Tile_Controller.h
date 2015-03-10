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

#ifndef TUNNELOUR_PROCEDURAL_LEVEL_TILE_CONTROLLER_H_
#define TUNNELOUR_PROCEDURAL_LEVEL_TILE_CONTROLLER_H_

#include "Level_Tile_Controller.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                middleground (Layer 0)
//-----------------------------------------------------------------------------
class Procedural_Level_Tile_Controller: public Tunnelour::Level_Tile_Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Procedural_Level_Tile_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Procedural_Level_Tile_Controller();

  //--------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

  virtual void CreateLevel();

  virtual void HandleEvent(Tunnelour::Component * const component);

  virtual void AddTilesToModel(std::vector<Tile_Bitmap*> tiles);
  virtual void RemoveTilesFromModel(int count);

 protected:
  void RandomTheRoof();
  void RandomTheFloor();
  void EnsureMinTunnel();
  void EnsureMaxTunnel();
  void EnsureAPath();
  void EnsureAClimbablePath();

 private:
  std::vector<Tile_Bitmap*> GenerateTunnel(Level_Component::Level_Metadata level_metadata);
  float m_floor_offset_x;
  float m_floor_offset_y;
  float m_roof_offset_x;
  float m_roof_offset_y;
  float m_back_offset_x;
  float m_back_offset_y;
  std::vector<Tile_Bitmap*> m_roof_tiles;
  std::vector<std::vector<Tile_Bitmap*>> m_level_tiles;
  float m_roof_level;
  float m_floor_level;
  float m_last_roof_level;
  float m_last_floor_level;
  float m_last_last_roof_level;
  float m_last_last_floor_level;
  float m_tile_start_point_x;
  float m_tile_start_point_y;
  float m_level_right;
  float m_level_count;
  float m_min_tunnel_size;
  float m_max_tunnel_size;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_PROCEDURAL_LEVEL_TILE_CONTROLLER_H_
