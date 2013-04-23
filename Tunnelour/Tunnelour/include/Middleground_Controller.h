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

#ifndef TUNNELOUR_MIDDLEGROUND_CONTROLLER_H_
#define TUNNELOUR_MIDDLEGROUND_CONTROLLER_H_

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"
#include "Middleground_Controller_Mutator.h"
#include "Camera_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                middleground (Layer 0)
//-----------------------------------------------------------------------------
class Middleground_Controller: public Tunnelour::Controller {
 public:
  struct Line {
    int line_number;
    int top_left_x, top_left_y;
    int tile_size_x, tile_size_y;
    int number_of_tiles;
  };

  struct Tileset {
    std::string name;
    std::string type;
    int top_left_x, top_left_y;
    int size_x, size_y;
    int number_of_lines;
    std::list<Line> lines;
  };

  struct Tileset_Metadata {
    std::string name;
    std::string type;
    std::string filename;
    int top_left_x, top_left_y;
    int size_x, size_y;
    int number_of_subsets;
    std::list<Tileset> tilesets;
  };

  struct Collision {
    Tunnelour::Tile_Bitmap* a_tile;
    Tunnelour::Tile_Bitmap* b_tile;
    D3DXVECTOR2 a_tile_collision_point;
    D3DXVECTOR2 b_tile_collision_point;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Middleground_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Middleground_Controller();

  //---------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual void Init(Tunnelour::Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual void Run();

 protected:

 private:
  void Tile_Tunnel();
  void Tile_Middleground();
  std::vector<Tunnelour::Tile_Bitmap*> GenerateTilesUpwards(Tunnelour::Tile_Bitmap* from_tile);
  std::vector<Tunnelour::Tile_Bitmap*> GenerateTilesDownwards(Tunnelour::Tile_Bitmap* from_tile);
  Tunnelour::Tile_Bitmap* Create_Tile(int base_tile_size);

  void Load_Tilset_Metadata();

  Tileset_Metadata m_metadata;
  std::vector<Tunnelour::Tile_Bitmap*> m_middleground_tiles;
  std::vector<Tunnelour::Tile_Bitmap*> m_tunnel_tiles;
  std::vector<Tunnelour::Tile_Bitmap*> m_left_edge_tiles;
  std::vector<Tunnelour::Tile_Bitmap*> m_right_edge_tiles;

  Tunnelour::Game_Settings_Component* m_game_settings;
  bool m_has_init_middleground_been_generated, m_has_init_tunnel_been_generated;
  int m_middleground_top, m_middleground_bottom, m_middleground_left, m_middleground_right;
  std::string m_metadata_file_path;
  int m_tunnel_x_size;
  Tunnelour::Camera_Component *m_camera;
  std::wstring m_tileset_filename;
  bool m_is_debug_mode;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_MIDDLEGROUND_CONTROLLER_H_
