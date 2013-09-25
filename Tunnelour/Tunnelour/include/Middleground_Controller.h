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
#include "Tileset_Helper.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                middleground (Layer 0)
//-----------------------------------------------------------------------------
class Middleground_Controller: public Controller {
 public:
  /* Example
  Size 128;Type Middleground,Size 128;Type Tunnel,Size 128;Type Tunnel
  Size 128;Type Middleground,Size 128;Type Tunnel,Size 128;Type Tunnel
  */
  struct Tile_Metadata {
    std::string type;
    float size;
  };

  /* Example
  Level_Name "Level 0"
  Level_Blurb "Testing Walking"
  Level_CSV_Filename "Level_0.csv"
  Level_TilesetName "Dirt"
  Level_TunnelStartTopLeftX "0"
  Level_TunnelStartTopLeftY "0"
  Level_AvatarStartTopLeftY "64"
  Level_AvatarStartTopLeftX "-64"
  */
  struct Level_Metadata {
    std::string level_name;
    std::string blurb;
    std::string filename;
    std::string tileset_name;
    float tunnel_top_left_x;
    float tunnel_top_left_y;
    float avatar_top_left_x;
    float avatar_top_left_y;
    std::vector<std::vector<Tile_Metadata>> level;
  };

  //---------------------------------------------------------------------------
  // Description : Constructor
  //---------------------------------------------------------------------------
  Middleground_Controller();

  //---------------------------------------------------------------------------
  // Description : Deconstructor
  //---------------------------------------------------------------------------
  virtual ~Middleground_Controller();

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
  void LoadLevelMetadata();
  std::list<Level_Metadata> m_level_metadata;
  Level_Metadata LoadLevelMetadataIntoStruct(std::string metadata_path);
  void LoadLevelCSVIntoStruct(std::string metadata_path, Level_Metadata *out_struct);
  std::vector<Tile_Bitmap*> GenerateTunnelFromMetadata(Level_Metadata level_metadata);
  std::list<Tileset_Helper::Tileset_Metadata> m_tilesets;
  Tileset_Helper::Tileset_Metadata m_current_tileset;
  Tileset_Helper::Subset m_current_middleground_subset;
  void LoadTilesetMetadata();
  Tileset_Helper::Tileset_Metadata GetNamedTileset(std::string name);
  Tileset_Helper::Subset GetCurrentMiddlegroundSubset();
  Tile_Bitmap* CreateTile(float base_tile_size);
  Tileset_Helper::Line GetCurrentSizedLine(float size);
  Level_Metadata GetNamedLevel(std::string name);
  //---------------------------------------------------------------------------
  // Description : Resets a tiles texture to 0 for switching tilesets.
  //---------------------------------------------------------------------------
  void ResetTileTexture(Tile_Bitmap *out_tile);
  std::vector<Tile_Bitmap*> m_left_edge_tiles;
  std::vector<Tile_Bitmap*> m_right_edge_tiles;
  std::vector<Tile_Bitmap*> m_bottom_edge_tiles;
  std::vector<Tile_Bitmap*> m_top_edge_tiles;

  //---------------------------------------------------------------------------
  // Description : Tiles up from the current background edge
  //---------------------------------------------------------------------------
  void TileUp(float camera_top, float middleground_top);

  //---------------------------------------------------------------------------
  // Description : Tiles down from the current background edge
  //---------------------------------------------------------------------------
  void TileDown(float camera_bottom, float middleground_bottom);

  //---------------------------------------------------------------------------
  // Description : Tiles right from the current background edge
  //---------------------------------------------------------------------------
  void TileRight(float camera_right, float middleground_right);

  //---------------------------------------------------------------------------
  // Description : Tiles left from the current background edge
  //---------------------------------------------------------------------------
  void TileLeft(float camera_left, float middleground_left);

  //---------------------------------------------------------------------------
  // Description : Switches the tileset from Debug to Dirt and vise versa
  //---------------------------------------------------------------------------
  void SwitchTileset();


  std::vector<Tile_Bitmap*> m_middleground_tiles;

  Game_Settings_Component* m_game_settings;

  Camera_Component *m_camera;
  std::string m_tileset_filename;
  bool m_is_debug_mode;
  
  std::string m_debug_metadata_file_path;
  std::string m_dirt_metadata_file_path;

};
}  // namespace Tunnelour
#endif  // TUNNELOUR_MIDDLEGROUND_CONTROLLER_H_
