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

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"
#include "Camera_Component.h"
#include "Tileset_Helper.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                background (Layer 0)
//-----------------------------------------------------------------------------
class Background_Controller: public Tunnelour::Controller {
 public:
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
  //---------------------------------------------------------------------------
  // Description : Creates a new bitmap tile of the given tile size.
  //---------------------------------------------------------------------------
  Tunnelour::Tile_Bitmap* CreateTile(float base_tile_size);

  //---------------------------------------------------------------------------
  // Description : Loads the tileset metadata from file into the variables
  //---------------------------------------------------------------------------
  void LoadTilesetMetadata();

  Tileset_Helper::Tileset_Metadata GetNamedTileset(std::string name);

  Tileset_Helper::Subset GetCurrentBackgroundTilesetSubset();

  //---------------------------------------------------------------------------
  // Member Variables
  //---------------------------------------------------------------------------
  std::list<Tileset_Helper::Tileset_Metadata> m_tilesets;
  Tileset_Helper::Tileset_Metadata m_current_tileset;
  Tileset_Helper::Subset m_current_background_subset;

  std::vector<Tunnelour::Tile_Bitmap*> m_background_tiles;
  Tunnelour::Game_Settings_Component* m_game_settings;
  Tunnelour::Camera_Component* m_camera;
  bool m_has_init_background_been_generated;
  std::string m_metadata_file_path;
  float m_background_top;
  float m_background_bottom;
  float m_background_left;
  float m_background_right;
  std::string m_debug_metadata_file_path;
  std::string m_dirt_metadata_file_path;
  bool m_is_debug_mode;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_BACKGROUND_CONTROLLER_H_
