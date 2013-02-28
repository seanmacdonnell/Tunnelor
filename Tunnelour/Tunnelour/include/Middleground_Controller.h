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
  void Load_Tilset_Metadata();
  Tunnelour::Tile_Bitmap* Create_Tile(int base_tile_size, int resised_tile_size);
  bool DoTheseTilesCollide(Tunnelour::Tile_Bitmap* TileA, Tunnelour::Tile_Bitmap* TileB, D3DXVECTOR2 &tile_a_output_colision);
  bool WhereDoThesePointsIntersect( D3DXVECTOR2 p1, D3DXVECTOR2 p2, D3DXVECTOR2 p3, D3DXVECTOR2 p4, D3DXVECTOR2 &output);

  Tunnelour::Tile_Bitmap *m_bitmap;
  Tileset_Metadata m_metadata;
  std::vector<std::vector<Tunnelour::Tile_Bitmap*>> m_middleground_tiles;
  std::vector<std::vector<Tunnelour::Tile_Bitmap*>> m_tunnel_tiles;

  Tunnelour::Game_Settings_Component* m_game_settings;
  bool m_has_init_middleground_been_generated, m_has_init_tunnel_been_generated;

  std::string m_metadata_file_path;
  int m_tunnel_x_size;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_MIDDLEGROUND_CONTROLLER_H_
