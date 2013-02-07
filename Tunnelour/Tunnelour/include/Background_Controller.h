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

#ifndef TUNNELOUR_BACKGROUND_CONTROLLER_H_
#define TUNNELOUR_BACKGROUND_CONTROLLER_H_

#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"
#include <vector>


namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                background (Layer 0)
//-----------------------------------------------------------------------------
class Background_Controller: public Tunnelour::Controller {
 public:
  struct Line {
    int line_number;
    float top_left_x, top_left_y;
    float size_x, size_y;
    int number_of_tiles;
  };

  struct Tileset {
    std::string type;
    float top_left_x, top_left_y;
    float size_x, size_y;
    int number_of_lines;
    std::list<Line> lines;
  };

  struct Tileset_Metadata {
    std::string name;
    std::string type;
    std::wstring filename;
    float top_left_x, top_left_y;
    float size_x, size_y;
    int number_of_subsets;
    std::list<Tileset> tilesets;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Background_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Background_Controller();

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
  void Load_Tilset_Metadata();
  Tunnelour::Tile_Bitmap* Create_Tile();
  std::wstring CharToWChar(const char* pstrSrc);

  Tunnelour::Tile_Bitmap *m_bitmap;
  Tileset_Metadata m_metadata;
  std::vector<std::vector<Tunnelour::Tile_Bitmap*>> m_background_tiles;

  Tunnelour::Game_Settings_Component* m_game_settings;
  bool m_has_init_background_been_generated;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_BACKGROUND_CONTROLLER_H_
