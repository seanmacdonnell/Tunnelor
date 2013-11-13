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
#include "Background_Controller_Mutator.h"
#include "Camera_Component.h"
#include "Tileset_Helper.h"
#include "Level_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                background (Layer 0)
//-----------------------------------------------------------------------------
class Background_Controller: public Controller {
 public:
  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Background_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Background_Controller();

  //--------------------------------------------------------------------------
  // Description : Initialisation function for the Controller
  //---------------------------------------------------------------------------
  virtual bool Init(Component_Composite * const model);

  //---------------------------------------------------------------------------
  // Description : Controller Runner
  //---------------------------------------------------------------------------
  virtual bool Run();

 protected:

 private:
  void CreateLevel();

  std::vector<Tile_Bitmap*> GenerateTunnelFromMetadata(Level_Component::Level_Metadata level_metadata);
  std::vector<Tileset_Helper::Tileset_Metadata> m_tilesets;
  Tileset_Helper::Tileset_Metadata m_current_tileset;
  Tileset_Helper::Subset m_current_background_subset;
  void LoadTilesetMetadata();
  Tileset_Helper::Tileset_Metadata GetNamedTileset(std::string name);
  Tileset_Helper::Subset GetCurrentBackgroundSubset();
  Tile_Bitmap* CreateTile(float base_tile_size);
  Tileset_Helper::Line GetCurrentSizedLine(float size);
  
  //---------------------------------------------------------------------------
  // Description : Resets a tiles texture to 0 for switching tilesets.
  //---------------------------------------------------------------------------
  void ResetTileTexture(Tile_Bitmap *out_tile);
  std::vector<Tile_Bitmap*> m_left_edge_tiles;
  std::vector<Tile_Bitmap*> m_right_edge_tiles;
  std::vector<Tile_Bitmap*> m_bottom_edge_tiles;
  std::vector<Tile_Bitmap*> m_top_edge_tiles;
  std::vector<Tile_Bitmap*> m_tunnel_edge_tiles;

  //---------------------------------------------------------------------------
  // Description : Tiles up from the current background edge
  //---------------------------------------------------------------------------
  void TileUp(float camera_top, float background_top);

  //---------------------------------------------------------------------------
  // Description : Tiles down from the current background edge
  //---------------------------------------------------------------------------
  void TileDown(float camera_bottom, float background_bottom);

  //---------------------------------------------------------------------------
  // Description : Tiles right from the current background edge
  //---------------------------------------------------------------------------
  void TileRight(float camera_right, float background_right);

  //---------------------------------------------------------------------------
  // Description : Tiles left from the current background edge
  //---------------------------------------------------------------------------
  void TileLeft(float camera_left, float background_left);

  //---------------------------------------------------------------------------
  // Description : Switches the tileset from Debug to Dirt and vise versa
  //---------------------------------------------------------------------------
  void SwitchTileset();

  std::vector<Tile_Bitmap*> m_background_tiles;

  Game_Settings_Component* m_game_settings;

  Camera_Component *m_camera;
  Level_Component *m_level;
  std::string m_tileset_filename;
  bool m_is_debug_mode;
  
  std::string m_debug_metadata_file_path;
  std::string m_dirt_metadata_file_path;

};
}  // namespace Tunnelour
#endif  // TUNNELOUR_BACKGROUND_CONTROLLER_H_
