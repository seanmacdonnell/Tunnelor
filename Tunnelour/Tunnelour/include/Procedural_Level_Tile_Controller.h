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

#include <vector>
#include <string>
#include <list>
#include "Component_Composite.h"
#include "Controller.h"
#include "Tile_Bitmap.h"
#include "Game_Settings_Component.h"
#include "Level_Tile_Controller_Mutator.h"
#include "Camera_Component.h"
#include "Tileset_Helper.h"
#include "Level_Component.h"

namespace Tunnelour {
//-----------------------------------------------------------------------------
//  Author(s)   : Sean MacDonnell
//  Description : This controller is responsible for the generation of the
//                middleground (Layer 0)
//-----------------------------------------------------------------------------
class Procedural_Level_Tile_Controller: public Tunnelour::Level_Tile_Controller {
 public:
  struct Background_Tile_Type {
    bool found_right_exit;
    bool found_left_exit;

    Background_Tile_Type() {
      found_right_exit = false;
      found_left_exit = false;
    }

    // equality comparison. doesn't modify object. therefore const.
    bool operator==(const Background_Tile_Type& a) const {
      return (found_right_exit == a.found_right_exit &&
              found_left_exit == a.found_left_exit);
    }
  };

  struct Middleground_Tile_Type {
    bool found_roof;
    bool found_floor;
    bool found_right_wall;
    bool found_left_wall;

    bool found_roof_right_end;
    bool found_roof_left_end;
    bool found_floor_right_end;
    bool found_floor_left_end;
    bool found_right_wall_top_end;
    bool found_right_wall_bot_end;
    bool found_left_wall_top_end;
    bool found_left_wall_bot_end;

    Middleground_Tile_Type() {
      found_roof = false;
      found_floor = false;
      found_right_wall = false;
      found_left_wall = false;

      found_roof_right_end = false;
      found_roof_left_end = false;
      found_floor_right_end = false;
      found_floor_left_end = false;
      found_right_wall_top_end = false;
      found_right_wall_bot_end = false;
      found_left_wall_top_end = false;
      found_left_wall_bot_end = false;
    }

    // equality comparison. doesn't modify object. therefore const.
    bool operator==(const Middleground_Tile_Type& a) const {
      return (found_roof == a.found_roof &&
              found_floor == a.found_floor &&
              found_right_wall == a.found_right_wall &&
              found_left_wall == a.found_left_wall && 
              found_roof_right_end == a.found_roof_right_end &&
              found_roof_left_end == a.found_roof_left_end &&
              found_floor_right_end == a.found_floor_right_end &&
              found_floor_left_end == a.found_floor_left_end &&
              found_right_wall_top_end == a.found_right_wall_top_end &&
              found_right_wall_bot_end == a.found_right_wall_bot_end &&
              found_left_wall_top_end == a.found_left_wall_top_end &&
              found_left_wall_bot_end == a.found_left_wall_bot_end);
    }
  };
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

  //---------------------------------------------------------------------------
  // Description : Create the tiles for the current level
  //---------------------------------------------------------------------------
  void CreateLevel();
  void AddLevelToModel();
  void ShowLevel();
  void HideLevel();
  void DestroyLevel();

  std::vector<Tile_Bitmap*> GetExitTiles();

  virtual void HandleEvent(Tunnelour::Component * const component);

 protected:

 private:
  std::vector<Tile_Bitmap*> GenerateTunnelFromMetadata(Level_Component::Level_Metadata level_metadata);
  std::vector<Tile_Bitmap*> GenerateStartingTunnel(Level_Component::Level_Metadata level_metadata);
  std::vector<Tileset_Helper::Tileset_Metadata> m_tilesets;
  Tileset_Helper::Tileset_Metadata m_current_tileset;
  Tileset_Helper::Subset m_current_middleground_subset;
  Tileset_Helper::Subset m_current_background_subset;
  void LoadTilesetMetadata();
  Tileset_Helper::Tileset_Metadata GetNamedTileset(std::string name);
  Tileset_Helper::Subset GetCurrentMiddlegroundSubset();
  Tileset_Helper::Subset GetCurrentMiddlegroundSubsetType(Procedural_Level_Tile_Controller::Middleground_Tile_Type types);
  Tileset_Helper::Subset GetCurrentBackgroundSubsetType(Procedural_Level_Tile_Controller::Background_Tile_Type types);
  Middleground_Tile_Type ParseSubsetTypesFromString(std::string type);
  Background_Tile_Type ParseSubsetBackgroundTypesFromString(std::string type);
  Tileset_Helper::Subset GetCurrentNamedSubset(std::string name);
  Tileset_Helper::Subset GetCurrentBackgroundSubset();
  Tile_Bitmap* CreateMiddlegroundTile(float base_tile_size);
  Tile_Bitmap* CreateBackgroundTile(float base_tile_size);
  Tileset_Helper::Line GetCurrentSizedMiddlegroundLine(float size);
  Tileset_Helper::Line GetSizedNamedLine(Tileset_Helper::Subset subset, float size);
  Tileset_Helper::Line GetCurrentSizedBackgroundLine(float size);
  
  //---------------------------------------------------------------------------
  // Description : Resets a tiles texture to 0 for switching tilesets.
  //---------------------------------------------------------------------------
  void ResetMiddlegroundTileTexture(Tile_Bitmap *out_tile);
  void ResetBackgroundTileTexture(Tile_Bitmap *out_tile);
  std::vector<Tile_Bitmap*> m_left_edge_tiles;
  std::vector<Tile_Bitmap*> m_right_edge_tiles;
  std::vector<Tile_Bitmap*> m_bottom_edge_tiles;
  std::vector<Tile_Bitmap*> m_top_edge_tiles;
  std::vector<Tile_Bitmap*> m_tunnel_edge_tiles;
  std::vector<Tile_Bitmap*> m_exit_tiles;
  /* DEPRECIATED
  //---------------------------------------------------------------------------
  // Description : Tiles up from the current background edge
  //---------------------------------------------------------------------------
  void TileUp(camera_top, float middleground_top);

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
  */
  //---------------------------------------------------------------------------
  // Description : Switches the tileset from Debug to Dirt and vise versa
  //---------------------------------------------------------------------------
  void SwitchTileset();

  std::vector<Tile_Bitmap*> m_middleground_tiles;
  std::vector<Tile_Bitmap*> m_background_tiles;

  Game_Settings_Component* m_game_settings;

  Camera_Component *m_camera;
  Level_Component *m_level;
  std::string m_tileset_filename;
  bool m_is_debug_mode;
  
  std::string m_debug_metadata_file_path;
  std::string m_dirt_metadata_file_path;
  std::string m_blue_cave_metadata_file_path;
  std::string m_the_void_metadata_file_path;

  std::string m_current_level_name;
  std::vector<Tile_Bitmap*> m_created_tiles;

  float m_blocksize_x;
  float m_blocksize_y;
  std::vector<std::vector<Tile_Bitmap*>> m_block_tile_lines;
  std::vector<Tile_Bitmap*> m_block_tile_line;
  Tile_Bitmap* m_back_block;
};
}  // namespace Tunnelour
#endif  // TUNNELOUR_PROCEDURAL_LEVEL_TILE_CONTROLLER_H_
